#include "audio.h"
#include "miniaudio.h"
#include "nodes.h"
#include "utils.h"
#include <thread>

int main() {
  AudioManager am;
  am.init();

  // Create nodes
  auto osc = Oscillator::init(0.2f, 440.0f);
  auto *oscNode = am.addNode(std::move(osc));
  auto lfo = LFO::init(440.0f, 50.0f, 100.0f);
  auto *lfoNode = am.addNode(std::move(lfo));

  // Wire: Envelope → Oscillator amplitude
  auto *oscData = static_cast<Oscillator *>(oscNode->data.get());
  auto *lfoData = static_cast<LFO *>(lfoNode->data.get());
  lfoData->targets.push_back(&oscData->freq);

  // Add dependency in graph (optional, for topological sort)
  am.addEdge(lfoNode, oscNode);

  std::this_thread::sleep_for(std::chrono::seconds(5));
  am.exit();
}
