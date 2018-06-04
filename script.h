#pragma once

//#include <lua.h>
//#include <lualib.h>
//#include <lauxlib.h>
//#include <lautoc.h>

#include "creature.h"

CreatureData parse_lua_creature(const char *);

void init_script();