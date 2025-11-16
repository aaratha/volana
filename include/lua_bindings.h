#pragma once

#include "repl.h"

// Lua binding function declarations
int lua_create_oscillator(lua_State *L);

int lua_create_lfo(lua_State *L);

int lua_create_filter(lua_State *L);

int lua_osc_index(lua_State *L);

int lua_lfo_index(lua_State *L);

int lua_filter_index(lua_State *L);

int lua_lfo_mod(lua_State *L);

int lua_osc_pipe(lua_State *L);

int lua_osc_newindex(lua_State *L);

int lua_lfo_newindex(lua_State *L);

int lua_filter_newindex(lua_State *L);

void register_oscillator(lua_State *L);

void register_lfo(lua_State *L);

void register_filter(lua_State *L);

void register_osc_types(lua_State *L);
