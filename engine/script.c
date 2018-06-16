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
 * Utility functions
 *
 */

static inline void append_path(lua_State *L) /* Makes require() skip '/assets/scripts/' */
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");
	char* path = NULL;
    asprintf (&path, "%s;%s", SCRIPTS_PATH, lua_tostring(L, -1));
	lua_pop(L, 1);
	lua_pushstring(L, path);
	lua_setfield(L, -2, "path");
	lua_pop(L, 1);
	free(path);
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

static inline void call_method(lua_State * L, const char * method)
{
    lua_getfield(L, -1, method);
    if (!lua_isfunction(L, -1))
        return;
    lua_pushvalue(L, -2);
    lua_print_error(L, lua_pcall(L, 1, 0, 0));
}

static inline void call_method_1f(lua_State * L, const char * method, float arg1)
{
    lua_getfield(L, -1, method);
    if (!lua_isfunction(L, -1))
        return;
    lua_pushvalue(L, -2);
    lua_pushnumber(L, arg1);
    lua_print_error(L, lua_pcall(L, 2, 0, 0));
}

static inline void push_vector(lua_State * L, vec3 v)
{
	lua_getglobal(L, "Vector");

	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);

	lua_print_error(L, lua_pcall(L, 3, 1, 0));
}

static inline void get_vector(lua_State * L, vec3 v) /* assumes the Vector table is on top */
{
    float x, y, z;

    lua_getfield(L, -1, "x");
    x = (float)luaL_checknumber(L, -1);

    lua_getfield(L, -2, "y");
    y = (float)luaL_checknumber(L, -1);

    lua_getfield(L, -3, "z");
    z = (float)luaL_checknumber(L, -1);

    glm_vec_copy((vec3) { x, y, z }, v);
}

#define api_integer_getter(n, t, f) static int n(lua_State * L) \
{\
    lua_getfield(L, 1, "__self");\
    void **bp = lua_touserdata(L, -1);\
    void *p = *bp;\
    t thing = (t)p;\
    assert(thing != NULL);\
    lua_pushinteger(L, thing->f);\
    return 1;\
}

#define api_integer_setter(n, t, f) static int n(lua_State * L) \
{\
    lua_getfield(L, 1, "__self");\
    void **bp = lua_touserdata(L, -1);\
    void *p = *bp;\
    t thing = (t)p;\
    assert(thing != NULL);\
    thing->f = (int)luaL_checkinteger(L, 2);\
    return 0;\
}

#define api_vector_getter(n, t, f) static int n(lua_State * L) \
{\
    lua_getfield(L, 1, "__self");\
    void **bp = lua_touserdata(L, -1);\
    void *p = *bp;\
    t thing = (t)p;\
    assert(thing != NULL);\
    push_vector(L, thing->f);\
    return 1;\
}

#define api_vector_setter(n, t, f) static int n(lua_State * L) \
{\
    lua_getfield(L, 1, "__self");\
    void **bp = lua_touserdata(L, -1);\
    void *p = *bp;\
    t thing = (t)p;\
    assert(thing != NULL);\
    lua_settop(L, 2);\
    get_vector(L, thing->f);\
    return 0;\
}

#define api_integer_getter_setter(n, t, f) api_integer_getter(api_get_##n, t, f) api_integer_setter(api_set_##n, t, f)

#define api_vector_getter_setter(n, t, f) api_vector_getter(api_get_##n, t, f) api_vector_setter(api_set_##n, t, f)

#define api_methods_getter_setter(p, f) {"get_" #f, api_get_##p##f}, {"set_" #f, api_set_##p##f},

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

/*
 *
 * Creature API
 *
 */

CreatureData parse_lua_creature(const char * script_name)
{
    CreatureData creature_data;

    creature_data.script_name = strdup(script_name);

    char * creature_data_table = NULL;
    asprintf(&creature_data_table, "%s_data", creature_data.script_name);

    int status;
    lua_State *L = luaL_newstate();
    if (!L)
        return creature_data;
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

static inline void push_creature(lua_State * L, int creature_index)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, creature_index);
}

static int api_get_creature(lua_State * L)
{
    int creature_index = (int)luaL_checknumber(L, 1);

    push_creature(L, creature_index);

    return 1;
}

static int api_spawn_creature(lua_State * L)
{
    World* world = NULL;
    active_world(&world);
    assert(world != NULL);

    const char * creature_name = luaL_checkstring(L, 1);
    vec3 pos;

    lua_settop(L, 2);
    get_vector(L, pos);

    Creature* creature = spawn_creature(world, creature_name, pos);

    push_creature(L, creature->index);

    return 1;
}

static int api_kill_creature(lua_State * L)
{
    void **bp = lua_touserdata(L, 1);
    void *p = *bp;
    Creature* creature = (Creature*)p;
    assert(creature != NULL);

    kill_creature(creature);

    return 0;
}

api_integer_getter(api_get_creature_index, Creature*, index)
api_vector_getter_setter(creature_position, Creature*, transform.pos)
api_integer_getter_setter(creature_health, Creature*, health)
api_integer_getter_setter(creature_max_health, Creature*, max_health)
api_integer_getter_setter(creature_mana, Creature*, mana)
api_integer_getter_setter(creature_max_mana, Creature*, max_mana)
api_integer_getter_setter(creature_attack_dice, Creature*, attack_dice)
api_integer_getter_setter(creature_attack_dice_count, Creature*, attack_dice_count)
api_integer_getter_setter(creature_attack_bonus, Creature*, attack_bonus)
api_integer_getter_setter(creature_movement_capability, Creature*, movement_capability)

static const luaL_Reg api_creature_methods[] = {
        {"kill", api_kill_creature},
        {"get_index", api_get_creature_index},
        api_methods_getter_setter(creature_, position)
        api_methods_getter_setter(creature_, health)
        api_methods_getter_setter(creature_, max_health)
        api_methods_getter_setter(creature_, mana)
        api_methods_getter_setter(creature_, max_mana)
        api_methods_getter_setter(creature_, attack_dice)
        api_methods_getter_setter(creature_, attack_dice_count)
        api_methods_getter_setter(creature_, attack_bonus)
        api_methods_getter_setter(creature_, movement_capability)
        {0, 0}
};

int script_spawn_creature(lua_State *L, void *_creature)
{
    void **bp = lua_newuserdata(L, sizeof(_creature));
    *bp = _creature;
    Creature * creature = (Creature*)*bp;

    lua_getglobal(L, creature->data->script_name);
    lua_getfield(L, -1, "new");
    lua_pushvalue(L, -2);
    lua_call (L, 1, 1);

    lua_pushliteral(L, "__self");
    lua_pushvalue(L, -4);
    lua_rawset(L, -3);

    creature->index = luaL_ref(L, LUA_REGISTRYINDEX);

    push_creature(L, creature->index);
    call_method(L, "on_spawn");

    lua_settop(L, 0);
    return creature->index;
}

void script_destroy_creature(lua_State * L, Creature* creature)
{
    push_creature(L, creature->index);

    call_method(L, "on_destroy");
    luaL_unref(L, LUA_REGISTRYINDEX, creature->index);

    lua_settop(L, 0);
}

void script_kill_creature(lua_State * L, Creature * creature)
{
    push_creature(L, creature->index);
    call_method(L, "on_death");

    lua_settop(L, 0);
}

void script_update_creature(lua_State * L, Creature* creature)
{
    push_creature(L, creature->index);
    call_method_1f(L, "update", get_delta_time());

    lua_settop(L, 0);
}

static void register_creatures(lua_State * L)
{
    require(L, "c_base");

    lua_getglobal(L, "c_base");
    luaL_newlib(L, api_creature_methods);
    lua_setfield(L, -2, "_");

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
	luaL_openlibs(L);

	require_api(L);

	lua_settop(L, 0);
}

static inline void require_api(lua_State *L)
{
	append_path(L);
	require(L, "u_tools");
    require(L, "u_class");
    require(L, "u_vector");

    /* API: Config */
    lua_register(L, "config_get_fov", script_config_get_fov);
    lua_register(L, "config_get_window_size", script_config_get_window_size);

    /* API: Creature */
    lua_pushinteger(L, MOVCAP_NONE); lua_setglobal(L, "MOVCAP_NONE");
    lua_pushinteger(L, MOVCAP_GROUND); lua_setglobal(L, "MOVCAP_GROUND");
    lua_pushinteger(L, MOVCAP_FLY); lua_setglobal(L, "MOVCAP_FLY");

    lua_pushinteger(L, AISTATE_NONE); lua_setglobal(L, "AISTATE_NONE");
    lua_pushinteger(L, AISTATE_IDLE); lua_setglobal(L, "AISTATE_IDLE");
    lua_pushinteger(L, AISTATE_ROAM); lua_setglobal(L, "AISTATE_ROAM");
    lua_pushinteger(L, AISTATE_FLEE); lua_setglobal(L, "AISTATE_FLEE");
    lua_pushinteger(L, AISTATE_ATTACKING); lua_setglobal(L, "AISTATE_ATTACKING");
    lua_pushinteger(L, AISTATE_CHASING); lua_setglobal(L, "AISTATE_CHASING");

	lua_register(L, "get_creature", api_get_creature);
    lua_register(L, "spawn_creature", api_spawn_creature);
    lua_register(L, "kill_creature", api_kill_creature);

    /* TODO: require all creatures */

    register_creatures(L);

    require(L, "c_minotaur");
    require(L, "c_minotaur_warrior");
    require(L, "c_black_dragon");
}