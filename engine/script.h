#pragma once

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "assimp/types.h"

#include "creature.h"

CreatureData parse_lua_creature(const char *);

int script_spawn_creature(lua_State *, void *);

void script_destroy_creature(lua_State *, Creature *);

void script_kill_creature(lua_State *, Creature *);

void script_update_creature(lua_State *, Creature *);

void construct_world_lua_state(lua_State **);