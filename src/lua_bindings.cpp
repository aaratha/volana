#include "lua_bindings.h"

#include "audio.h"
#include "nodes.h"

int lua_create_oscillator(lua_State *L) {
  int nargs = lua_gettop(L);

  // Default values
  float amp = 1.0f;
  float freq = 440.0f;
  OscType type = OscType::Sine;

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

      lua_getfield(L, 1, "type"); // get table field "type"
      if (lua_isnumber(L, -1)) {
        type = static_cast<OscType>(lua_tointeger(L, -1)); // cast to OscType
      }
      lua_pop(L, 1);

    } else if (lua_isnumber(L, 1)) {
      amp = lua_tonumber(L, 1);
      if (nargs >= 2 && lua_isnumber(L, 2))
        freq = lua_tonumber(L, 2);
      if (nargs >= 3 && lua_isnumber(L, 3))
        type = static_cast<OscType>(lua_tointeger(L, 3));
    } else {
      return luaL_error(L, "Expected table or numbers as arguments");
    }
  }

  auto osc = Oscillator::init(amp, freq, type);

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

int lua_create_filter(lua_State *L) {
  int nargs = lua_gettop(L);

  float cutoff = 1000.0f;
  float q = 1.0f;

  if (nargs >= 1) {
    if (lua_istable(L, 1)) {
      lua_getfield(L, 1, "cutoff");
      if (lua_isnumber(L, -1))
        cutoff = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 1, "q");
      if (lua_isnumber(L, -1))
        q = lua_tonumber(L, -1);
      lua_pop(L, 1);

    } else if (lua_isnumber(L, 1)) {
      cutoff = lua_tonumber(L, 1);
      if (nargs >= 2 && lua_isnumber(L, 2)) {
        q = lua_tonumber(L, 2);
      }
    } else {
      return luaL_error(L, "Expected table or numbers as arguments");
    }
  }

  auto filter = Filter::init(cutoff, q);

  Filter **udata = (Filter **)lua_newuserdata(L, sizeof(Filter *));
  *udata = filter.get();

  luaL_getmetatable(L, "Filter");
  lua_setmetatable(L, -2);

  am.addNode(std::move(filter));

  return 1;
}

int lua_osc_index(lua_State *L) {
  Oscillator **osc_ud = (Oscillator **)luaL_checkudata(L, 1, "Oscillator");
  Oscillator *osc = *osc_ud;

  const char *key = lua_tostring(L, 2);

  if (strcmp(key, "freq") == 0) {
    lua_pushlightuserdata(L, &osc->freq); // push as lightuserdata
    return 1;
  }

  if (strcmp(key, "amp") == 0) {
    lua_pushlightuserdata(L, &osc->amp);
    return 1;
  }

  if (strcmp(key, "type") == 0) {
    lua_pushinteger(L, static_cast<int>(osc->type));
    return 1;
  }

  // method access
  if (strcmp(key, "pipe") == 0) {
    lua_pushcfunction(L, lua_osc_pipe);
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
  if (strcmp(key, "mod") == 0) {
    lua_pushcfunction(L, lua_lfo_mod);
    return 1;
  }

  return 0; // key not found
}

int lua_filter_index(lua_State *L) {
  Filter **filter_ud = (Filter **)luaL_checkudata(L, 1, "Filter");
  Filter *filter = *filter_ud;

  const char *key = lua_tostring(L, 2);

  // field access

  if (strcmp(key, "cutoff") == 0) {
    lua_pushlightuserdata(L, &filter->cutoff); // return atomic pointer
    return 1;
  }

  if (strcmp(key, "q") == 0) {
    lua_pushlightuserdata(L, &filter->q); // return atomic pointer
    return 1;
  }

  return 0; // key not found
}

int lua_lfo_mod(lua_State *L) {
  // 1. Get LFO
  LFO **lfo_ud = (LFO **)luaL_checkudata(L, 1, "LFO");
  LFO *lfo = *lfo_ud;

  // 2. Get target Node (Oscillator, LFO, or anything with atomic<float>
  // members)
  Node *node = nullptr;

  if (luaL_testudata(L, 2, "Oscillator")) {
    Oscillator **osc_ud = (Oscillator **)luaL_checkudata(L, 2, "Oscillator");
    node = *osc_ud;
  } else if (luaL_testudata(L, 2, "LFO")) {
    LFO **lfo_ud = (LFO **)luaL_checkudata(L, 2, "LFO");
    node = *lfo_ud;
  } else if (luaL_testudata(L, 2, "Filter")) {
    Filter **filter_ud = (Filter **)luaL_checkudata(L, 2, "Filter");
    node = *filter_ud;
  } else {
    return luaL_error(L, "Expected Oscillator or LFO");
  }

  // 3. Get parameter name
  const char *paramName = luaL_checkstring(L, 3);

  // 4. Find the target parameter
  std::atomic<float> *target = nullptr;

  // Check for known parameters; expand as needed
  if (auto *osc = dynamic_cast<Oscillator *>(node)) {
    if (strcmp(paramName, "freq") == 0)
      target = &osc->freq;
    else if (strcmp(paramName, "amp") == 0)
      target = &osc->amp;
  } else if (auto *otherLFO = dynamic_cast<LFO *>(node)) {
    if (strcmp(paramName, "freq") == 0)
      target = &otherLFO->freq;
    else if (strcmp(paramName, "amp") == 0)
      target = &otherLFO->amp;
    else if (strcmp(paramName, "base") == 0)
      target = &otherLFO->base;
  } else if (auto *filter = dynamic_cast<Filter *>(node)) {
    if (strcmp(paramName, "cutoff") == 0)
      target = &filter->cutoff;
    else if (strcmp(paramName, "q") == 0)
      target = &filter->q;
  }

  if (!target)
    return luaL_error(L, "Unknown parameter '%s' for this node", paramName);

  // 5. Add to LFO's target list
  lfo->targets.push_back(target);

  // 6. Add edge in the graph for topological update order
  if (lfo->owner && node->owner)
    am.addEdge(lfo->owner, node->owner);

  return 0; // no return values
}

int lua_osc_pipe(lua_State *L) {
  // 1. Get the oscillator userdata
  Oscillator **osc_ud = (Oscillator **)luaL_checkudata(L, 1, "Oscillator");
  Oscillator *osc = *osc_ud;

  // 2. Get the filter userdata
  Filter **filter_ud = (Filter **)luaL_checkudata(L, 2, "Filter");
  Filter *filter = *filter_ud;

  if (!osc || !filter)
    return luaL_error(L, "Expected Oscillator and Filter");

  // 3. Add oscillator output to filter inputs
  filter->inputs.push_back(&osc->out);

  // 4. Optionally mute oscillator direct output
  osc->audioOut = false;

  // 5. Add dependency in graph for topological order
  am.addEdge(osc->owner, filter->owner);

  // 6. Return the filter so you can chain pipes in Lua: osc:pipe(f1):pipe(f2)
  lua_pushvalue(L, 2);
  return 1;
}

int lua_osc_newindex(lua_State *L) {
  Oscillator **osc_ud = (Oscillator **)luaL_checkudata(L, 1, "Oscillator");
  Oscillator *osc = *osc_ud;

  const char *key = lua_tostring(L, 2);

  if (strcmp(key, "freq") == 0) {
    float value = luaL_checknumber(L, 3);
    osc->freq.store(value);
    return 0;
  }

  if (strcmp(key, "amp") == 0) {
    float value = luaL_checknumber(L, 3);
    osc->amp.store(value);
    return 0;
  }

  if (strcmp(key, "type") == 0) {
    int t = luaL_checkinteger(L, 3);
    osc->type = static_cast<OscType>(t);
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

int lua_filter_newindex(lua_State *L) {
  Filter **filter_ud = (Filter **)luaL_checkudata(L, 1, "Filter");
  Filter *filter = *filter_ud;

  const char *key = lua_tostring(L, 2);

  if (strcmp(key, "cutoff") == 0) {
    std::atomic<float> *ptr = &filter->cutoff;
    float value = luaL_checknumber(L, 3);
    ptr->store(value);
    return 0;
  }

  if (strcmp(key, "q") == 0) {
    std::atomic<float> *ptr = &filter->q;
    float value = luaL_checknumber(L, 3);
    ptr->store(value);
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

void register_filter(lua_State *L) {
  luaL_newmetatable(L, "Filter");

  // set __index to our function
  lua_pushcfunction(L, lua_filter_index);
  lua_setfield(L, -2, "__index");

  // __newindex for real-time updates
  lua_pushcfunction(L, lua_filter_newindex);
  lua_setfield(L, -2, "__newindex");

  lua_pop(L, 1); // pop metatable
}

void register_osc_types(lua_State *L) {
  lua_newtable(L); // create table

  // if i want to force OscType::...
  // lua_pushinteger(L, static_cast<int>(OscType::Sine));
  // lua_setfield(L, -2, "Sine");
  //
  // lua_pushinteger(L, static_cast<int>(OscType::Saw));
  // lua_setfield(L, -2, "Saw");
  //
  // lua_pushinteger(L, static_cast<int>(OscType::Square));
  // lua_setfield(L, -2, "Square");
  //
  // lua_pushinteger(L, static_cast<int>(OscType::Triangle));
  // lua_setfield(L, -2, "Triangle");
  //
  // lua_setglobal(L, "OscType"); // assign table to global `OscType`

  lua_pushinteger(L, static_cast<int>(OscType::Sine));
  lua_setglobal(L, "Sine");

  lua_pushinteger(L, static_cast<int>(OscType::Saw));
  lua_setglobal(L, "Saw");

  lua_pushinteger(L, static_cast<int>(OscType::Square));
  lua_setglobal(L, "Square");

  lua_pushinteger(L, static_cast<int>(OscType::Triangle));
  lua_setglobal(L, "Triangle");
}
