#pragma once
#include "audio.h"
#include <iostream>
#include <string>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

class REPL {
  AudioManager *am;
  lua_State *L = nullptr;

public:
  REPL() = default;
  ~REPL();

  void init();

  void exit();

  // Run a Lua string
  void run(const std::string &code);

  // REPL loop
  void loop();

  // Bind a C++ function to Lua
  void bind(const std::string &name, lua_CFunction func);

  lua_State *state();
};
