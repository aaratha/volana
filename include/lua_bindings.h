#pragma once

#include "repl.h"

// Lua binding function declarations
int lua_create_oscillator(lua_State *L);

int lua_create_lfo(lua_State *L);

int lua_create_filter(lua_State *L);

int lua_osc_index(lua_State *L);

int lua_lfo_index(lua_State *L);

int lua_filter_index(lua_State *L);

int lua_node_pipe(lua_State *L);

int lua_node_set_audio_out(lua_State *L);

int lua_node_set_processing(lua_State *L);

int lua_osc_newindex(lua_State *L);

int lua_lfo_newindex(lua_State *L);

int lua_filter_newindex(lua_State *L);

int lua_param_mod(lua_State *L);

int lua_param_chain(lua_State *L);

int lua_param_effect(lua_State *L);

void register_oscillator(lua_State *L);

void register_lfo(lua_State *L);

void register_filter(lua_State *L);

void register_waveforms(lua_State *L);

void register_param(lua_State *L);
