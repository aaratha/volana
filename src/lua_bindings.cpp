#include "lua_bindings.h"

int lua_create_oscillator(lua_State *L) {
  if (!lua_istable(L, 1)) {
    return luaL_error(L, "Expected table as argument");
  }

  // Default values
  float amp = 1.0f;
  float freq = 440.0f;

  // Check if table has 'amp'
  lua_getfield(L, 1, "amp");
  if (lua_isnumber(L, -1)) {
    amp = lua_tonumber(L, -1);
  }
  lua_pop(L, 1);

  // Check if table has 'freq'
  lua_getfield(L, 1, "freq");
  if (lua_isnumber(L, -1)) {
    freq = lua_tonumber(L, -1);
  }
  lua_pop(L, 1);

  // Create oscillator
  auto osc = Oscillator::init(amp, freq);

  // Store pointer as lightuserdata
  lua_pushlightuserdata(L, osc.get());

  // Keep the unique_ptr alive somewhere (AudioManager)
  am.addNode(std::move(osc));

  return 1;
}
