#include "script.h"

#include <cglm/cglm.h>

#include "config.h"
#include "creature.h"
#include "util.h"
#include "game.h"
#include "transform.h"

#define SCRIPTS_PATH ".\\assets\\scripts\\?.lua"

static inline void require_api(lua_State*);

static lua_State *global_lua_state;

static inline void luaerr(lua_State *L, int e)
{
	if (e != 0)
	{
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}
}

static inline void luadbg(lua_State *L)
{
	printf("%s\n", lua_typename(L, lua_type(L, -1)));
}

/* Makes require() bypass '/assets/scripts/' path */
static inline int append_path(lua_State *L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");
	char* npath = malloc(1024);
	const char * s2 = lua_tostring(L, -1);
	snprintf(npath, 1024, "%s;%s", SCRIPTS_PATH, lua_tostring(L, -1));
	lua_pop(L, 1);
	lua_pushstring(L, npath);
	lua_setfield(L, -2, "path");
	lua_pop(L, 1);
	return 0;
}

static inline int require(lua_State *L, const char *name)
{
	lua_getglobal(L, "require");
	lua_pushstring(L, name);
	int r = lua_pcall(L, 1, 1, 0);
	if (!r)
		lua_pop(L, 1);
	luaerr(L, r);
	return r;
}

static inline const char * get_field_string(lua_State *L, const char *key)
{
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	// if (!lua_isnumber(L, -1))
	const char * result = _strdup(lua_tostring(L, -1));
	lua_pop(L, 1);
	return result;
}

static inline int get_field_int(lua_State *L, const char *key)
{
	int result;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	// if (!lua_isnumber(L, -1))
	result = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return result;
}

static int script_config_get_window_size(lua_State *L)
{
	lua_pushnumber(L, get_config().w);
	lua_pushnumber(L, get_config().h);

	return 2;
}

static int script_config_get_fov(lua_State *L)
{
	lua_pushnumber(L, get_config().fov);

	return 1;
}

void push_config_functions(lua_State *L)
{
	lua_register(L, "config_get_fov", script_config_get_fov);
	lua_register(L, "config_get_window_size", script_config_get_window_size);
}

static inline void push_creature_enums(lua_State *L)
{
//    luaA_enum(L, MovementCapability);
//    luaA_enum_value(L, MovementCapability, MOVCAP_NONE);
//    luaA_enum_value(L, MovementCapability, MOVCAP_GROUND);
//    luaA_enum_value(L, MovementCapability, MOVCAP_FLY);
//
//    luaA_enum(L, AIState);
//    luaA_enum_value(L, AIState, AISTATE_NONE);
//    luaA_enum_value(L, AIState, AISTATE_IDLE);
//    luaA_enum_value(L, AIState, AISTATE_ROAM);
//    luaA_enum_value(L, AIState, AISTATE_FLEE);
//    luaA_enum_value(L, AIState, AISTATE_ATTACKING);
//    luaA_enum_value(L, AIState, AISTATE_CHASING);

	lua_pushinteger(L, MOVCAP_NONE); lua_setglobal(L, "MOVCAP_NONE");
	lua_pushinteger(L, MOVCAP_GROUND); lua_setglobal(L, "MOVCAP_GROUND");
	lua_pushinteger(L, MOVCAP_FLY); lua_setglobal(L, "MOVCAP_FLY");
}

CreatureData parse_lua_creature(const char * script_name)
{
	CreatureData creature_data;

	creature_data.script_name = strdup(script_name);

	char * creature_data_table = malloc(MAXLEN);
	creature_data_table[0] = '\0';
	strcat(creature_data_table, creature_data.script_name);
	strcat(creature_data_table, "_data");

	int status;
	lua_State *L = luaL_newstate();
	if (!L)
		return creature_data;
    luaA_open(L);
	luaL_openlibs(L);

	require_api(L);

	require(L, creature_data.script_name);

	lua_getglobal(L, creature_data_table);

	creature_data.name = (char*)get_field_string(L, "name");
	creature_data.health = get_field_int(L, "health");
	creature_data.mana = get_field_int(L, "mana");

	creature_data.attack_dice_count		= get_field_int(L, "attack_dice_count");
	creature_data.attack_dice			= get_field_int(L, "attack_dice");
	creature_data.attack_bonus			= get_field_int(L, "attack_bonus");

	creature_data.movement_capability	= (MovementCapability)get_field_int(L, "movement_capability");
	//luaA_to(L, MovementCapability, &creature_data.movement_capability, -1);

	creature_data.sprite_sheet_folder	= (char*)get_field_string(L, "sprite_sheet_folder");

	lua_settop(L, 0);
	lua_close(L);

	free(creature_data_table);

	return creature_data;
}

static int script_get_creature_position(lua_State * L)
{
	unsigned int creature_index = (unsigned int)luaL_checknumber(L, 1);

	World* world = NULL;
	active_world(&world);
	assert(world != NULL);

	Creature* creature = NULL;
	creature_by_index(&creature, world, creature_index);
	assert(creature != NULL);

	lua_createtable(L, 3, 0);

	lua_pushnumber(L, 1);
	lua_pushnumber(L, creature->transform.pos[0]);
	lua_settable(L, -3);

	lua_pushnumber(L, 2);
	lua_pushnumber(L, creature->transform.pos[1]);
	lua_settable(L, -3);

	lua_pushnumber(L, 3);
	lua_pushnumber(L, creature->transform.pos[2]);
	lua_settable(L, -3);

	return 1;
}

void script_insert_creature(lua_State * L, const char * script_name, unsigned int creature_index)
{
	char i[10];
	sprintf(i, "%i", creature_index);

	lua_getglobal(L, "creatures");
	lua_getglobal(L, script_name);
	luaerr(L, lua_pcall(L, 0, 1, 0));
	lua_pushstring(L, i);
	lua_setfield(L, -2, "__creature_index");
	lua_setfield(L, -2, i);
	lua_settop(L, 0);
}

void script_update_creature(lua_State * L, Creature* creature, float delta_time)
{
	char i[10];
	sprintf(i, "%i", creature->index);

    lua_getglobal(L, "creatures");
	lua_getfield(L, -1, i);
	lua_getfield(L, -1, "update");
    lua_pushvalue(L, -2);
    lua_pushnumber(L, delta_time);
    luaerr(L, lua_pcall(L, 2, 0, 0));
    lua_settop(L, 0);
}

void construct_world_lua_state (lua_State ** _L)
{
	int status;
	lua_State *L = *_L = luaL_newstate();
	if (!L)
		return;
	luaA_open(L);
	luaL_openlibs(L);

	require_api(L);

	/* TODO: require all creatures */
	require(L, "c_minotaur");

	lua_newtable (L);
	lua_setglobal(L, "creatures");

	lua_settop(L, 0);
//	lua_close(L);
}

void init_script()
{

}

static inline void require_api(lua_State *L)
{
	append_path(L);
	require(L, "u_tools");
	//require(L, "u_class");
	push_config_functions(L);
	push_creature_enums(L);

	lua_register(L, "script_get_creature_position", script_get_creature_position);
}