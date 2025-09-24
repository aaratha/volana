#pragma once
#include "miniaudio.h"
#include "nodes.h"
#include "utils.h" // your non-templated DAG

class AudioManager {
public:
  AudioManager() : audioInitialized(false) {}

  ~AudioManager() { exit(); }

  // Initialize audio device
  void init() {
    if (audioInitialized)
      return;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback = AudioManager::dataCallback;
    deviceConfig.pUserData = this;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
      return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
      ma_device_uninit(&device);
      return;
    }

    audioInitialized = true;
    running.store(true);
  }

  // Shut down audio
  void exit() {
    if (audioInitialized) {
      ma_device_uninit(&device);
      audioInitialized = false;
      running.store(false);
    }
  }

  // Add a node to the graph
  GraphNode *addNode(std::unique_ptr<Node> node) {
    return graph.addNode(std::move(node));
  }

  // Add a dependency edge
  void addEdge(GraphNode *parent, GraphNode *child) {
    graph.addEdge(parent, child);
  }

private:
  Graph graph;
  ma_device_config deviceConfig{};
  ma_device device{};
  std::atomic<bool> running{false};
  bool audioInitialized;

  // Audio callback called by miniaudio
  static void dataCallback(ma_device *pDevice, void *pOutput,
                           const void * /*pInput*/, ma_uint32 frameCount) {
    auto *manager = static_cast<AudioManager *>(pDevice->pUserData);
    float *out = static_cast<float *>(pOutput);

    // Get topological order of nodes
    auto sortedNodes = manager->graph.sort();

    for (ma_uint32 i = 0; i < frameCount; ++i) {
      // Update each node in topo order
      for (auto *node : sortedNodes) {
        node->data->update();
      }

      // Mix or collect samples here
      float sample = 0.0f;
      for (auto *node : sortedNodes) {
        if (auto *osc = dynamic_cast<Oscillator *>(node->data.get())) {
          sample += osc->out.load(std::memory_order_relaxed);
        }
      }

      // Write same sample to all channels (stereo)
      *out++ = sample;
      *out++ = sample;
    }
  }
};
