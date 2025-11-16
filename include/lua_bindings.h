#include "audio.h"
#include "nodes.h"
#include "repl.h"

// Lua binding function declarations
int lua_create_oscillator(lua_State *L);

int lua_create_lfo(lua_State *L);

int lua_osc_index(lua_State *L);

int lua_lfo_index(lua_State *L);

int lua_lfo_connect(lua_State *L);

int lua_osc_newindex(lua_State *L);

int lua_lfo_newindex(lua_State *L);

void register_oscillator(lua_State *L);

void register_lfo(lua_State *L);
