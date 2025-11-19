#include "lua_bindings.h"

#include "audio.h"
#include "nodes.h"

namespace {

Node *lua_check_node(lua_State *L, int index) {
  if (luaL_testudata(L, index, "Oscillator")) {
    auto **osc = (Oscillator **)luaL_checkudata(L, index, "Oscillator");
    return *osc;
  }

  if (luaL_testudata(L, index, "Filter")) {
    auto **filter = (Filter **)luaL_checkudata(L, index, "Filter");
    return *filter;
  }

  if (luaL_testudata(L, index, "LFO")) {
    auto **lfo = (LFO **)luaL_checkudata(L, index, "LFO");
    return *lfo;
  }

  return nullptr;
}

ControlNode *lua_check_control_node(lua_State *L, int index) {
  Node *node = lua_check_node(L, index);
  if (auto *control = dynamic_cast<ControlNode *>(node))
    return control;
  return nullptr;
}

EffectNode *lua_check_effect_node(lua_State *L, int index) {
  Node *node = lua_check_node(L, index);
  if (auto *effect = dynamic_cast<EffectNode *>(node))
    return effect;
  return nullptr;
}

} // namespace

int lua_create_oscillator(lua_State *L) {
  int nargs = lua_gettop(L);

  // Default values
  float amp = 1.0f;
  float freq = 440.0f;
  Waveform type = Waveform::Sine;

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
        type = static_cast<Waveform>(lua_tointeger(L, -1)); // cast to OscType
      }
      lua_pop(L, 1);

    } else if (lua_isnumber(L, 1)) {
      amp = lua_tonumber(L, 1);
      if (nargs >= 2 && lua_isnumber(L, 2))
        freq = lua_tonumber(L, 2);
      if (nargs >= 3 && lua_isnumber(L, 3))
        type = static_cast<Waveform>(lua_tointeger(L, 3));
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
  float shift = 0.0f;
  Waveform type = Waveform::Sine;

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

      lua_getfield(L, 1, "shift");
      if (lua_isnumber(L, -1))
        shift = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, 1, "type"); // get table field "type"
      if (lua_isnumber(L, -1)) {
        type = static_cast<Waveform>(lua_tointeger(L, -1)); // cast to OscType
      }
      lua_pop(L, 1);

    } else if (lua_isnumber(L, 1)) {
      base = lua_tonumber(L, 1);
      if (nargs >= 2 && lua_isnumber(L, 2))
        amp = lua_tonumber(L, 2);
      if (nargs >= 3 && lua_isnumber(L, 3))
        freq = lua_tonumber(L, 3);
      if (nargs >= 4 && lua_isnumber(L, 4))
        shift = lua_tonumber(L, 4);
      if (nargs >= 5 && lua_isnumber(L, 5))
        type = static_cast<Waveform>(lua_tointeger(L, 5));

    } else {
      return luaL_error(L, "Expected table or numbers as arguments");
    }
  }

  auto lfo = LFO::init(base, amp, freq, shift, type);

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
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &osc->freq;
    p->owner = osc;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "amp") == 0) {
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &osc->amp;
    p->owner = osc;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "type") == 0) {
    lua_pushinteger(L, static_cast<int>(osc->type));
    return 1;
  }

  // method access
  if (strcmp(key, "pipe") == 0) {
    lua_pushcfunction(L, lua_node_pipe);
    return 1;
  }

  if (strcmp(key, "set_audio_out") == 0) {
    lua_pushcfunction(L, lua_node_set_audio_out);
    return 1;
  }

  if (strcmp(key, "set_processing") == 0) {
    lua_pushcfunction(L, lua_node_set_processing);
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
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &lfo->base;
    p->owner = lfo;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "amp") == 0) {
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &lfo->amp;
    p->owner = lfo;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "freq") == 0) {
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &lfo->freq;
    p->owner = lfo;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "shift") == 0) {
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &lfo->shift;
    p->owner = lfo;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "type") == 0) {
    lua_pushinteger(L, static_cast<int>(lfo->type));
    return 1;
  }

  if (strcmp(key, "type") == 0) {
    lua_pushinteger(L, static_cast<int>(lfo->type));
    return 1;
  }

  if (strcmp(key, "set_processing") == 0) {
    lua_pushcfunction(L, lua_node_set_processing);
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
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &filter->cutoff;
    p->owner = filter;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "q") == 0) {
    Param *p = (Param *)lua_newuserdata(L, sizeof(Param));
    p->ptr = &filter->q;
    p->owner = filter;

    luaL_getmetatable(L, "Param");
    lua_setmetatable(L, -2);
    return 1;
  }

  if (strcmp(key, "pipe") == 0) {
    lua_pushcfunction(L, lua_node_pipe);
    return 1;
  }

  if (strcmp(key, "set_audio_out") == 0) {
    lua_pushcfunction(L, lua_node_set_audio_out);
    return 1;
  }

  if (strcmp(key, "set_processing") == 0) {
    lua_pushcfunction(L, lua_node_set_processing);
    return 1;
  }

  return 0; // key not found
}

int lua_node_pipe(lua_State *L) {
  Node *source = lua_check_node(L, 1);
  if (!source)
    return luaL_error(L, "pipe() expects an audio node source");

  EffectNode *effect = lua_check_effect_node(L, 2);
  if (!effect)
    return luaL_error(L, "pipe() requires an effect node destination");

  effect->addInput(&source->out);
  source->audioOut = false;

  if (source->owner && effect->owner)
    am.addEdge(source->owner, effect->owner);

  lua_pushvalue(L, 2);
  return 1;
}

int lua_node_set_audio_out(lua_State *L) {
  Node *node = lua_check_node(L, 1);
  if (!node)
    return luaL_error(L, "set_audio_out expects a node");

  bool enabled = lua_toboolean(L, 2);
  node->audioOut = enabled;
  return 0;
}

int lua_node_set_processing(lua_State *L) {
  Node *node = lua_check_node(L, 1);
  if (!node)
    return luaL_error(L, "set_processing expects a node");

  bool enabled = lua_toboolean(L, 2);
  node->active.store(enabled, std::memory_order_relaxed);
  if (!enabled) {
    node->out.store(0.0f, std::memory_order_relaxed);

    if (auto *osc = dynamic_cast<Oscillator *>(node)) {
      osc->phase.store(0.0f, std::memory_order_relaxed);
    } else if (auto *filter = dynamic_cast<Filter *>(node)) {
      filter->x1 = filter->x2 = 0.0f;
      filter->y1 = filter->y2 = 0.0f;
    }
  }

  return 0;
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
    osc->type = static_cast<Waveform>(t);
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

  if (strcmp(key, "shift") == 0) {
    lfo->shift = luaL_checknumber(L, 3);
    return 0;
  }

  if (strcmp(key, "type") == 0) {
    int t = luaL_checkinteger(L, 3);
    lfo->type = static_cast<Waveform>(t);
    return 0;
  }

  if (strcmp(key, "type") == 0) {
    int t = luaL_checkinteger(L, 3);
    lfo->type = static_cast<Waveform>(t);
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

int lua_param_mod(lua_State *L) {
  Param *param = (Param *)luaL_checkudata(L, 1, "Param");

  auto resolveControl = [&](int index) -> ControlNode * {
    if (luaL_testudata(L, index, "Param")) {
      Param *controllerParam = (Param *)luaL_checkudata(L, index, "Param");
      if (!controllerParam->owner)
        luaL_error(L, "Param argument %d has no owner", index);

      if (auto *control = dynamic_cast<ControlNode *>(controllerParam->owner))
        return control;

      luaL_error(L, "Param argument %d is not owned by a control node", index);
    }

    if (auto *control = lua_check_control_node(L, index))
      return control;

    luaL_error(L, "Expected control node or Param as controller");
    return nullptr;
  };

  ControlNode *control = resolveControl(2);

  control->addTarget(param->ptr);

  if (control->owner && param->owner && param->owner->owner)
    am.addEdge(control->owner, param->owner->owner);

  lua_pushvalue(L, 1); // return param so you can do .freq:m(l1):m(l2)
  return 1;
}

int lua_param_chain(lua_State *L) { return lua_param_mod(L); }

int lua_param_effect(lua_State *L) {
  Param *param = (Param *)luaL_checkudata(L, 1, "Param");
  EffectNode *effect = lua_check_effect_node(L, 2);
  if (!effect)
    return luaL_error(L, "Expected effect node (e.g. Filter)");

  effect->addInput(param->ptr);

  if (effect->owner && param->owner && param->owner->owner)
    am.addEdge(param->owner->owner, effect->owner);

  lua_pushvalue(L, 1);
  return 1;
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

void register_waveforms(lua_State *L) {
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

  lua_pushinteger(L, static_cast<int>(Waveform::Sine));
  lua_setglobal(L, "Sine");

  lua_pushinteger(L, static_cast<int>(Waveform::Saw));
  lua_setglobal(L, "Saw");

  lua_pushinteger(L, static_cast<int>(Waveform::InvSaw));
  lua_setglobal(L, "InvSaw");

  lua_pushinteger(L, static_cast<int>(Waveform::Square));
  lua_setglobal(L, "Square");

  lua_pushinteger(L, static_cast<int>(Waveform::Triangle));
  lua_setglobal(L, "Triangle");
}

void register_param(lua_State *L) {
  luaL_newmetatable(L, "Param");

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pushcfunction(L, lua_param_mod);
  lua_setfield(L, -2, "m");
  lua_pushcfunction(L, lua_param_mod);
  lua_setfield(L, -2, "mod");

  lua_pushcfunction(L, lua_param_chain);
  lua_setfield(L, -2, "c");
  lua_pushcfunction(L, lua_param_chain);
  lua_setfield(L, -2, "chain");

  lua_pushcfunction(L, lua_param_effect);
  lua_setfield(L, -2, "e");
  lua_pushcfunction(L, lua_param_effect);
  lua_setfield(L, -2, "effect");

  lua_pop(L, 1);
}
