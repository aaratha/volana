#include "repl.h"
#include "lua_bindings.h"

REPL::~REPL() { exit(); }

void REPL::init() {
  L = luaL_newstate();
  luaL_openlibs(L);

  register_oscillator(L);
  register_lfo(L);
  register_filter(L);
  register_osc_types(L);
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
