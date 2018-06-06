#pragma once

#include <lautoc.h>
#include <assimp/types.h>

#include "creature.h"

CreatureData parse_lua_creature(const char *);

void script_insert_creature(lua_State *, const char *, unsigned int);
void script_update_creature(lua_State *, Creature*, float);

void construct_world_lua_state (lua_State **);

void init_script();