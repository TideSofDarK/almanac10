#include "game.h"

/* Global state */
static Game* game = NULL;

void init_game(GLFWwindow * window)
{
	game = malloc(sizeof(Game));

	game->window = window;

	game->state = GS_WORLD;

	game->player = NULL;
	construct_player(&game->player);

	game->camera = NULL;
	construct_camera(&game->camera);

	game->worlds = NULL;
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

	destruct_player(&game->player);

	free(game);
	game = NULL;
}

GameState get_game_state()
{
	return game->state;
}

void set_game_state(GameState state)
{
	game->state = state;
}

void insert_world(World * world, bool make_active)
{
	vector_push_back(game->worlds, world);
	if (make_active)
	{
		make_world_active(world);
	}
}

void make_world_active(World * world)
{
	game->active_world = world;
}

World ** get_all_worlds()
{
	return game->worlds;
}

GLFWwindow * get_active_window()
{
	assert(game != NULL);
	return game->window;
}

World * get_active_world()
{
	assert(game != NULL);
	return game->active_world;
}

Player * get_active_player()
{
	assert(game != NULL);
	return game->player;
}

Camera * get_active_camera()
{
	assert(game != NULL);
	return game->camera;
}