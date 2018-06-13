#pragma once

#include "world.h"
#include "camera.h"
#include "vector.h"

typedef enum
{
	/* Main menu */
	GS_MENU,
	/* Party creation */
	GS_CREATION,
	/* Ingame; regular */
	GS_WORLD,
	/* Ingame; in house */
	GS_HOUSE,
	/* Ingame; inventory */
	GS_INVENTORY,
	/* Ingame; ESC-menu */
	GS_PAUSED
} GameState;

typedef struct
{
	GameState state;
	World* active_world;
	World** worlds;
} Game;

void init_game(GameState, World**);
void shutdown_game();

void insert_world(World**, int);
void make_world_active(World**);

World** get_all_worlds();
void active_world(World**);
Camera active_camera();