#pragma once
#include "miniaudio.h"
#include "nodes.h"
#include "utils.h" // your non-templated DAG

class AudioManager {
  Graph graph;
  ma_device_config deviceConfig{};
  ma_device device{};
  std::atomic<bool> running{false};
  bool audioInitialized;

  // Audio callback called by miniaudio
  static void dataCallback(ma_device *pDevice, void *pOutput,
                           const void * /*pInput*/, ma_uint32 frameCount);

public:
  AudioManager();

  ~AudioManager();

  // Initialize audio device
  void init();

  // Shut down audio
  void exit();

  // Add a node to the graph
  GraphNode *addNode(std::unique_ptr<Node> node);

  // Add a dependency edge
  void addEdge(GraphNode *parent, GraphNode *child);
};

inline AudioManager am;
