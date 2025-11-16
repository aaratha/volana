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

  auto lfo = LFO::init(base, amp, freq);

  LFO **udata = (LFO **)lua_newuserdata(L, sizeof(LFO *));
  *udata = lfo.get();

  luaL_getmetatable(L, "LFO");
  lua_setmetatable(L, -2);

  am.addNode(std::move(lfo));

  return 1;
}

int lua_osc_index(lua_State *L) {
  Oscillator **osc_ud = (Oscillator **)luaL_checkudata(L, 1, "Oscillator");
  Oscillator *osc = *osc_ud;

  const char *key = lua_tostring(L, 2);

  if (strcmp(key, "freq") == 0) {
    std::atomic<float> *ptr = &osc->freq;
    lua_pushlightuserdata(L, ptr); // push as lightuserdata
    return 1;
  }

  if (strcmp(key, "amp") == 0) {
    std::atomic<float> *ptr = &osc->amp;
    lua_pushlightuserdata(L, ptr);
    return 1;
  }

  return 0;
}

int lua_lfo_index(lua_State *L) {
  LFO **lfo_ud = (LFO **)luaL_checkudata(L, 1, "LFO");
  LFO *lfo = *lfo_ud;

  const char *key = lua_tostring(L, 2);

  // field access

  if (strcmp(key, "base") == 0) {
    lua_pushlightuserdata(L, &lfo->base); // return atomic pointer
    return 1;
  }

  if (strcmp(key, "amp") == 0) {
    lua_pushlightuserdata(L, &lfo->amp); // return atomic pointer
    return 1;
  }

  if (strcmp(key, "freq") == 0) {
    lua_pushlightuserdata(L, &lfo->freq); // return atomic pointer
    return 1;
  }

  // method access
  if (strcmp(key, "connect") == 0) {
    lua_pushcfunction(L, lua_lfo_connect);
    return 1;
  }

  return 0; // key not found
}

int lua_lfo_connect(lua_State *L) {
  LFO **lfo_ud = (LFO **)luaL_checkudata(L, 1, "LFO");
  LFO *lfo = *lfo_ud;

  std::atomic<float> *target = (std::atomic<float> *)lua_touserdata(L, 2);
  if (!target)
    return luaL_error(L, "Expected atomic<float>* target to connect");

  lfo->targets.push_back(target);
  return 0;
}

int lua_osc_newindex(lua_State *L) {
  LFO **osc_ud = (LFO **)luaL_checkudata(L, 1, "Oscillator");
  LFO *osc = *osc_ud;

  const char *key = lua_tostring(L, 2);

  if (strcmp(key, "freq") == 0) {
    std::atomic<float> *ptr = &osc->freq;
    float value = luaL_checknumber(L, 3);
    ptr->store(value);
    return 0;
  }

  if (strcmp(key, "amp") == 0) {
    std::atomic<float> *ptr = &osc->amp;
    float value = luaL_checknumber(L, 3);
    ptr->store(value);
    return 0;
  }

  return luaL_error(L, "Unknown field %s", key);
}

int lua_lfo_newindex(lua_State *L) {
  LFO **lfo_ud = (LFO **)luaL_checkudata(L, 1, "LFO");
  LFO *lfo = *lfo_ud;

  const char *key = lua_tostring(L, 2);

  if (strcmp(key, "freq") == 0) {
    std::atomic<float> *ptr = &lfo->freq;
    float value = luaL_checknumber(L, 3);
    ptr->store(value);
    return 0;
  }

  if (strcmp(key, "amp") == 0) {
    std::atomic<float> *ptr = &lfo->amp;
    float value = luaL_checknumber(L, 3);
    ptr->store(value);
    return 0;
  }

  if (strcmp(key, "base") == 0) {
    lfo->base = luaL_checknumber(L, 3);
    return 0;
  }

  return luaL_error(L, "Unknown field %s", key);
}

void register_oscillator(lua_State *L) {
  luaL_newmetatable(L, "Oscillator");

  lua_pushcfunction(L, lua_osc_index);
  lua_setfield(L, -2, "__index");

  // __newindex for real-time updates
  lua_pushcfunction(L, lua_osc_newindex);
  lua_setfield(L, -2, "__newindex");

  lua_pop(L, 1);
}

void register_lfo(lua_State *L) {
  luaL_newmetatable(L, "LFO");

  // set __index to our function
  lua_pushcfunction(L, lua_lfo_index);
  lua_setfield(L, -2, "__index");

  // __newindex for real-time updates
  lua_pushcfunction(L, lua_lfo_newindex);
  lua_setfield(L, -2, "__newindex");

  lua_pop(L, 1); // pop metatable
}
