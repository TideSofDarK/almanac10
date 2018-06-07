#include "script.h"

#include <cglm/cglm.h>

#include "config.h"
#include "creature.h"
#include "util.h"
#include "game.h"
#include "transform.h"

#define SCRIPTS_PATH ".\\assets\\scripts\\?.lua"

static inline void require_api(lua_State*);

/*
 *
 * LUA utility debug functions
 *
 */

static inline void lua_print_error(lua_State *L, int e)
{
	if (e != 0)
	{
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}
}

static inline void lua_toptype(lua_State *L)
{
	printf("%s\n", lua_typename(L, lua_type(L, -1)));
}

/*
 *
 * LUA table functions imported from ltablib.c
 *
 */

#define TAB_R	1			/* read */
#define TAB_W	2			/* write */
#define TAB_L	4			/* length */
#define TAB_RW	(TAB_R | TAB_W)		/* read/write */
#define aux_getn(L,n,w)	(checktab(L, n, (w) | TAB_L), luaL_len(L, n))

static inline int checkfield (lua_State *L, const char *key, int n) {
	lua_pushstring(L, key);
	return (lua_rawget(L, -n) != LUA_TNIL);
}

static inline void checktab (lua_State *L, int arg, int what) {
	if (lua_type(L, arg) != LUA_TTABLE) {  /* is it not a table? */
		int n = 1;  /* number of elements to pop */
		if (lua_getmetatable(L, arg) &&  /* must have metatable */
			(!(what & TAB_R) || checkfield(L, "__index", ++n)) &&
			(!(what & TAB_W) || checkfield(L, "__newindex", ++n)) &&
			(!(what & TAB_L) || checkfield(L, "__len", ++n))) {
			lua_pop(L, n);  /* pop metatable and tested metamethods */
		}
		else
			luaL_checktype(L, arg, LUA_TTABLE);  /* force an error */
	}
}

static inline int tinsert (lua_State *L) {
	lua_Integer e = aux_getn(L, 1, TAB_RW) + 1;  /* first empty element */
	lua_Integer pos;  /* where to insert new element */
	switch (lua_gettop(L)) {
		case 2: {  /* called with only 2 arguments */
			pos = e;  /* insert new element at the end */
			break;
		}
		case 3: {
			lua_Integer i;
			pos = luaL_checkinteger(L, 2);  /* 2nd argument is the position */
			luaL_argcheck(L, 1 <= pos && pos <= e, 2, "position out of bounds");
			for (i = e; i > pos; i--) {  /* move up elements */
				lua_geti(L, 1, i - 1);
				lua_seti(L, 1, i);  /* t[i] = t[i - 1] */
			}
			break;
		}
		default: {
			return luaL_error(L, "wrong number of arguments to 'insert'");
		}
	}
	lua_seti(L, 1, pos);  /* t[pos] = v */
	return 0;
}

static inline int tremove (lua_State *L) {
	lua_Integer size = aux_getn(L, 1, TAB_RW);
	lua_Integer pos = luaL_optinteger(L, 2, size);
	if (pos != size)  /* validate 'pos' if given */
		luaL_argcheck(L, 1 <= pos && pos <= size + 1, 1, "position out of bounds");
	lua_geti(L, 1, pos);  /* result = t[pos] */
	for ( ; pos < size; pos++) {
		lua_geti(L, 1, pos + 1);
		lua_seti(L, 1, pos);  /* t[pos] = t[pos + 1] */
	}
	lua_pushnil(L);
	lua_seti(L, 1, pos);  /* t[pos] = nil */
	return 1;
}

/*
 *
 * Utility functions
 *
 */

static inline int append_path(lua_State *L) /* Makes require() skip '/assets/scripts/' */
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
	lua_print_error(L, r);
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

static inline void push_vector(lua_State * L, vec3 v)
{
	lua_getglobal(L, "Vector");

	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);

	lua_print_error(L, lua_pcall(L, 3, 1, 0));
}

/*
 *
 * Config API
 *
 */

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

/*
 *
 * Creature API
 *
 */

static inline void push_creature_enums(lua_State *L)
{
	lua_pushinteger(L, MOVCAP_NONE); lua_setglobal(L, "MOVCAP_NONE");
	lua_pushinteger(L, MOVCAP_GROUND); lua_setglobal(L, "MOVCAP_GROUND");
	lua_pushinteger(L, MOVCAP_FLY); lua_setglobal(L, "MOVCAP_FLY");

	lua_pushinteger(L, AISTATE_NONE); lua_setglobal(L, "AISTATE_NONE");
	lua_pushinteger(L, AISTATE_IDLE); lua_setglobal(L, "AISTATE_IDLE");
	lua_pushinteger(L, AISTATE_ROAM); lua_setglobal(L, "AISTATE_ROAM");
	lua_pushinteger(L, AISTATE_FLEE); lua_setglobal(L, "AISTATE_FLEE");
	lua_pushinteger(L, AISTATE_ATTACKING); lua_setglobal(L, "AISTATE_ATTACKING");
	lua_pushinteger(L, AISTATE_CHASING); lua_setglobal(L, "AISTATE_CHASING");
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

	creature_data.name                  = (char*)get_field_string(L, "name");
	creature_data.health                = get_field_int(L, "max_health");
	creature_data.mana                  = get_field_int(L, "max_mana");

	creature_data.attack_dice_count		= get_field_int(L, "attack_dice_count");
	creature_data.attack_dice			= get_field_int(L, "attack_dice");
	creature_data.attack_bonus			= get_field_int(L, "attack_bonus");

	creature_data.movement_capability	= (MovementCapability)get_field_int(L, "movement_capability");

	creature_data.sprite_sheet_folder	= (char*)get_field_string(L, "sprite_sheet_folder");

	lua_settop(L, 0);
	lua_close(L);

	free(creature_data_table);

	return creature_data;
}

static inline void push_creature(lua_State * L, unsigned int creature_index)
{
    lua_getglobal(L, "creatures");
    lua_pushinteger(L, creature_index);
	lua_gettable (L, -2);
}

static int api_get_creature(lua_State * L)
{
	unsigned int creature_index = (unsigned int)luaL_checknumber(L, 1);

	push_creature(L, creature_index);

	return 1;
}

static int api_get_creature_position(lua_State * L)
{
	unsigned int creature_index = (unsigned int)luaL_checknumber(L, 1);

	World* world = NULL;
	active_world(&world);
	assert(world != NULL);

	Creature* creature = NULL;
	creature_by_index(&creature, world, creature_index);
	assert(creature != NULL);

    push_vector(L, creature->transform.pos);

	return 1;
}

void script_insert_creature(lua_State * L, const char * script_name, unsigned int creature_index)
{
	lua_getglobal(L, "creatures");
	lua_getglobal(L, script_name);
	lua_print_error(L, lua_pcall(L, 0, 1, 0));
	lua_pushstring(L, "__creature_index");
	lua_pushinteger(L, creature_index);
	lua_settable(L, -3);
	lua_pushinteger(L, creature_index);
	lua_pushvalue(L, -2);
	lua_settable(L, -4);
	lua_settop(L, 0);
}

void script_destroy_creature(lua_State * L, Creature* creature)
{
    push_creature(L, creature->index);

    lua_getfield(L, -1, "on_destroy");
    lua_pushvalue(L, -2);
	lua_print_error(L, lua_pcall(L, 1, 0, 0));

	lua_getglobal(L, "creatures");
	lua_pushinteger(L, creature->index);
	lua_pushnil(L);
	lua_settable(L, -3);

	lua_settop(L, 0);
}

void script_kill_creature(lua_State * L, Creature * creature)
{
	push_creature(L, creature->index);

	lua_getfield(L, -1, "on_death");
	lua_pushvalue(L, -2);
	lua_print_error(L, lua_pcall(L, 1, 0, 0));

	lua_settop(L, 0);
}

void script_update_creature(lua_State * L, Creature* creature, float delta_time)
{
    push_creature(L, creature->index);

	lua_getfield(L, -1, "update");
    lua_pushvalue(L, -2);
    lua_pushnumber(L, delta_time);
	lua_print_error(L, lua_pcall(L, 2, 0, 0));
    lua_settop(L, 0);
}

/*
 *
 * World API
 *
 */

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
    require(L, "c_black_dragon");

	lua_newtable (L);
	lua_setglobal(L, "creatures");

	lua_settop(L, 0);
}

static inline void require_api(lua_State *L)
{
	append_path(L);
	require(L, "u_tools");
    require(L, "u_vector");
	//require(L, "u_class");
	push_config_functions(L);
	push_creature_enums(L);

	lua_register(L, "get_creature", api_get_creature);
	lua_register(L, "get_creature_position", api_get_creature_position);
}