#include "script.h"

#include <cglm/cglm.h>

#include "config.h"

// static lua_State *L;

#define SCRIPTS_PATH ".\\scripts\\?.lua"

//#define constname(name) #name
//#define pushenum(L, v)				\
//do {								\
//	lua_pushnumber(L, v);			\
//	lua_setglobal(L, constname(v)); \
//} while (0);						\
//
//inline void luaerr(lua_State *L, int e)
//{
//	if (e != 0)
//	{
//		fprintf(stderr, "%s\n", lua_tostring(L, -1));
//	}
//}
//
//int append_path(lua_State *L)
//{
//	lua_getglobal(L, "package");
//	lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
//	char* npath = malloc(1024);
//	const char * s2 = lua_tostring(L, -1);
//	snprintf(npath, 1024, "%s;%s", SCRIPTS_PATH, lua_tostring(L, -1)); // grab path string from top of stack
//	lua_pop(L, 1);
//	lua_pushstring(L, npath);
//	lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
//	lua_pop(L, 1); // get rid of package table from top of stack
//	return 0;
//}
//
//int require(lua_State *L, const char *name)
//{
//	lua_getglobal(L, "require");
//	lua_pushstring(L, name);
//	int r = lua_pcall(L, 1, 1, 0);
//	if (!r)
//		lua_pop(L, 1);
//	luaerr(L, r);
//	return r;
//}
//
//const char * get_field_string(lua_State *L, const char *key)
//{
//	lua_pushstring(L, key);
//	lua_gettable(L, -2);
//	// if (!lua_isnumber(L, -1))
//	const char * result = _strdup(lua_tostring(L, -1));
//	lua_pop(L, 1);
//	return result;
//}
//
//int get_field_int(lua_State *L, const char *key)
//{
//	int result;
//	lua_pushstring(L, key);
//	lua_gettable(L, -2);
//	// if (!lua_isnumber(L, -1))
//	result = (int)lua_tonumber(L, -1);
//	lua_pop(L, 1);
//	return result;
//}
//
//static int script_config_get_window_size(lua_State *L)
//{
//	lua_pushnumber(L, get_config().w);
//	lua_pushnumber(L, get_config().h);
//
//	return 2;
//}
//
//static int script_config_get_fov(lua_State *L)
//{
//	lua_pushnumber(L, get_config().fov);
//
//	return 1;
//}
//
//void push_config_functions(lua_State *L)
//{
//	lua_pushcfunction(L, script_config_get_fov);
//	lua_setglobal(L, "config_get_fov");
//
//	lua_pushcfunction(L, script_config_get_window_size);
//	lua_setglobal(L, "config_get_window_size");
//}
//
//void push_creature_enums(lua_State *L)
//{
//	pushenum(L, MOVCAP_NONE);
//	pushenum(L, MOVCAP_GROUND);
//	pushenum(L, MOVCAP_FLY);
//
//	pushenum(L, AISTATE_NONE);
//	pushenum(L, AISTATE_IDLE);
//	pushenum(L, AISTATE_ROAM);
//	pushenum(L, AISTATE_FLEE);
//	pushenum(L, AISTATE_ATTACKING);
//	pushenum(L, AISTATE_CHASING);
//}
//
//int fib(int n)
//{
//	if (n == 0) { return 1; }
//	if (n == 1) { return 1; }
//	return fib(n - 1) + fib(n - 2);
//}
//
//CreatureData parse_lua_creature(const char * filename)
//{
//	static CreatureData creature_data;
//
//	int status;
//	lua_State *L = luaL_newstate();
//	if (!L)
//		return creature_data;
//
//	luaL_openlibs(L);
//	status = luaL_loadfile(L, filename);
//
//	append_path(L);
//
//	push_creature_enums(L);
//
//	luaA_function(L, fib, int, int);
//
//	int ret = lua_pcall(L, 0, 0, 0);
//	if (ret != 0)
//	{
//		fprintf(stderr, "%s\n", lua_tostring(L, -1));
//		return creature_data;
//	}
//
//	//int len = _scprintf("%s_data", filename);
//	//char * creature_data_table = malloc(len + 1);
//	//snprintf(creature_data_table, len + 1, "%s_data", filename);
//
//	lua_getglobal(L, "c_minotaur_data");
//
//	//lua_getglobal(L, "CREATURE_NAME");
//	//lua_getglobal(L, "CREATURE_HEALTH");
//	//lua_getglobal(L, "CREATURE_MANA");
//
//	//lua_getglobal(L, "CREATURE_ATTACK_DICE_COUNT");
//	//lua_getglobal(L, "CREATURE_ATTACK_DICE");
//	//lua_getglobal(L, "CREATURE_ATTACK_BONUS");
//
//	//lua_getglobal(L, "CREATURE_MOVEMENT_CAPABILITY");
//	//lua_getglobal(L, "CREATURE_SPRITE_SHEET_FOLDER");
//
//	creature_data.name = get_field_string(L, "name");
//	creature_data.health = get_field_int(L, "health");
//	creature_data.mana = get_field_int(L, "mana");
//
//	creature_data.attack_dice_count		= get_field_int(L, "attack_dice_count");
//	creature_data.attack_dice			= get_field_int(L, "attack_dice");
//	creature_data.attack_bonus			= get_field_int(L, "attack_bonus");
//
//	creature_data.movement_capability	= (MovementСapability)get_field_int(L, "movement_capability");
//
//	creature_data.sprite_sheet_folder	= get_field_string(L, "sprite_sheet_folder");
//
//	lua_settop(L, 0);
//	lua_close(L);
//
//	return creature_data;
//}

void init_script()
{
	//int status;
	//lua_State *L;

	//L = luaL_newstate();
	//if (!L)
	//{
	//	return;
	//}

	//luaL_openlibs(L);
	//status = luaL_loadfile(L, "scripts/c_minotaur.lua");

	//lua_pushcfunction(L, script_config_get_fov);
	//lua_setglobal(L, "config_get_fov");

	//lua_pushcfunction(L, script_config_get_window_size);
	//lua_setglobal(L, "config_get_window_size");

	//int ret = lua_pcall(L, 0, 0, 0);
	//if (ret != 0)
	//{
	//	fprintf(stderr, "%s\n", lua_tostring(L, -1));
	//	return;
	//}
	//
	//lua_getglobal(L, "CREATURE_ATTACK_DICE_COUNT");
	//lua_getglobal(L, "CREATURE_ATTACK_DICE");
	//lua_getglobal(L, "CREATURE_ATTACK_BONUS");
	//printf("Attack is %id%i + %i\n",
	//	(int)luaL_checkinteger(L, -3),
	//	(int)luaL_checkinteger(L, -2),
	//	(int)luaL_checkinteger(L, -1));
	//lua_settop(L, 0);

	//lua_close(L);
}