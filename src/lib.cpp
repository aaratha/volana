#include "audio.h"
#include "lua_bindings.h"
#include "miniaudio.h"
#include "nodes.h"
#include "repl.h"
#include "utils.h"

// A simple function that Lua can call
int cpp_print(lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  std::cout << "[Lua] " << msg << std::endl;
  return 0; // number of return values
}

int main(int argc, char **argv) {
  REPL repl;
  am.init();
  repl.init();

  // ---------Example using C++----------
  // // Create nodes
  // auto osc = Oscillator::init(0.2f, 440.0f);
  // auto *oscNode = am.addNode(std::move(osc));
  // auto lfo = LFO::init(440.0f, 50.0f, 100.0f);
  // auto *lfoNode = am.addNode(std::move(lfo));
  //
  // // Wire: Envelope → Oscillator amplitude
  // auto *oscData = static_cast<Oscillator *>(oscNode->data.get());
  // auto *lfoData = static_cast<LFO *>(lfoNode->data.get());
  // lfoData->targets.push_back(&oscData->freq);
  //
  // // Add dependency in graph (optional, for topological sort)
  // am.addEdge(lfoNode, oscNode);

  repl.bind("cpp_print", cpp_print);
  repl.bind("Oscillator", lua_create_oscillator);

  if (argc > 1) {
    // A file path was provided, so run it and exit
    std::string filename = argv[1];
    repl.run_file(filename);
  }
  repl.loop();

  am.exit();
  repl.exit();
}
