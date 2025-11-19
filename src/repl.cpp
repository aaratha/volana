#include "repl.h"
#include "lua_bindings.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <system_error>

namespace {

std::string locate_runtime_script() {
  namespace fs = std::filesystem;

  static const fs::path sourceRoot =
      fs::path(__FILE__).parent_path().parent_path();

  const std::array<fs::path, 3> candidates = {
      fs::current_path() / "lua" / "runtime.lua",
      fs::current_path() / "runtime.lua", sourceRoot / "lua" / "runtime.lua"};

  for (const auto &candidate : candidates) {
    std::error_code ec;
    if (fs::exists(candidate, ec)) {
      return candidate.string();
    }
  }

  return {};
}

void load_runtime(lua_State *L) {
  auto runtimePath = locate_runtime_script();
  if (runtimePath.empty()) {
    std::cerr << "Warning: could not locate lua/runtime.lua" << std::endl;
    return;
  }

  if (luaL_loadfile(L, runtimePath.c_str()) || lua_pcall(L, 0, 0, 0)) {
    std::cerr << "Lua error while loading runtime (" << runtimePath
              << "): " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);
  }
}

} // namespace

REPL::~REPL() { exit(); }

void REPL::init() {
  L = luaL_newstate();
  luaL_openlibs(L);

  register_oscillator(L);
  register_lfo(L);
  register_filter(L);
  register_waveforms(L);
  register_param(L);

  load_runtime(L);
}

void REPL::exit() {
  if (L) {
    lua_close(L);
    L = nullptr;
  }
}

void REPL::run(const std::string &code) {
  if (luaL_loadstring(L, code.c_str()) || lua_pcall(L, 0, 0, 0)) {
    std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);
  }
}

void REPL::run_file(const std::string &filename) {
  std::ifstream in(filename);
  if (in) {
    std::cout << "--- " << filename << " ---\n";
    std::cout << in.rdbuf(); // echo the file
    std::cout << "\n--- end ---\n";
  }
  if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
    std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);
  }
}

// REPL loop
void REPL::loop() {
  std::string line;
  while (true) {
    std::cout << ">> ";
    if (!std::getline(std::cin, line))
      break;
    if (line == "exit")
      break;
    run(line);
  }
}

void REPL::bind(const std::string &name, lua_CFunction func) {
  lua_register(L, name.c_str(), func);
}

lua_State *REPL::state() { return L; }
