#include "lua_bindings.h"

int lua_create_oscillator(lua_State *L) {
  int nargs = lua_gettop(L);

  // Default values
  float amp = 1.0f;
  float freq = 440.0f;

  if (nargs >= 1) {
    if (lua_istable(L, 1)) {
      lua_getfield(L, 1, "amp");
      if (lua_isnumber(L, -1))
        amp = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 1, "freq");
      if (lua_isnumber(L, -1))
        freq = lua_tonumber(L, -1);
      lua_pop(L, 1);
    } else if (lua_isnumber(L, 1)) {
      amp = lua_tonumber(L, 1);
      if (nargs >= 2 && lua_isnumber(L, 2))
        freq = lua_tonumber(L, 2);
    } else {
      return luaL_error(L, "Expected table or numbers as arguments");
    }
  }

  auto osc = Oscillator::init(amp, freq);

  // Create full userdata
  Oscillator **udata = (Oscillator **)lua_newuserdata(L, sizeof(Oscillator *));
  *udata = osc.get();

  // Optional: set a metatable if you want to add methods later
  luaL_getmetatable(L, "Oscillator");
  lua_setmetatable(L, -2);

  // Store unique_ptr to keep alive
  am.addNode(std::move(osc));

  return 1;
}

int lua_create_lfo(lua_State *L) {
  int nargs = lua_gettop(L);

  float base = 440.0f;
  float amp = 40.0f;
  float freq = 3.0f;

  if (nargs >= 1) {
    if (lua_istable(L, 1)) {
      lua_getfield(L, 1, "base");
      if (lua_isnumber(L, -1))
        base = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 1, "amp");
      if (lua_isnumber(L, -1))
        amp = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 1, "freq");
      if (lua_isnumber(L, -1))
        freq = lua_tonumber(L, -1);
      lua_pop(L, 1);
    } else if (lua_isnumber(L, 1)) {
      base = lua_tonumber(L, 1);
      if (nargs >= 2 && lua_isnumber(L, 2)) {
        amp = lua_tonumber(L, 2);
        if (nargs >= 3 && lua_isnumber(L, 3))
          freq = lua_tonumber(L, 3);
      }
    } else {
      return luaL_error(L, "Expected table or numbers as arguments");
    }
  }

  auto lfo = LFO::init(amp, freq);

  LFO **udata = (LFO **)lua_newuserdata(L, sizeof(LFO *));
  *udata = lfo.get();

  luaL_getmetatable(L, "LFO");
  lua_setmetatable(L, -2);

  am.addNode(std::move(lfo));

  return 1;
}
