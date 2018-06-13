#include "game.h"

/* Global state */
static Game* game = NULL;

void init_game(GameState state, World** world)
{
	game = malloc(sizeof(Game));

	game->worlds = NULL;
	game->state = state;

	if (world)
	{
		insert_world(world, 1);
	}
}

void shutdown_game()
{
	game->state = GS_WORLD;

	if (game->worlds)
	{
		for (size_t i = 0; i < vector_size(game->worlds); i++)
		{
			World* world = game->worlds[i];
			if (world != NULL)
				destruct_world(&world);
			world = NULL;
		}

		vector_free(game->worlds);
		game->worlds = NULL;
	}

	game->active_world = NULL;

	free_precached_creatures();
	free_cached_textures();

	free(game);
	game = NULL;
}

void insert_world(World** world, int make_active)
{
	vector_push_back(game->worlds, *world);
	if (make_active)
	{
		make_world_active(world);
	}
}

void make_world_active(World** world)
{
	game->active_world = *world;
}

void active_world(World** dest)
{
	if (game && game->active_world)
	{
		*dest = game->active_world;
	}
	else
	{
		*dest = NULL;
	}
}

World** get_all_worlds()
{
	return game->worlds;
}

Camera active_camera()
{
	return game->active_world->camera;
}