#include "game.h"

/* Global state */
static Game* game = NULL;

void init_game(GLFWwindow * window, World** world)
{
	game = malloc(sizeof(Game));

	game->window = window;

	game->state = GS_WORLD;

	game->player = NULL;
	construct_player(&game->player);

	game->camera = NULL;
	construct_camera(&game->camera);

	game->worlds = NULL;

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

void insert_world(World** world, bool make_active)
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

World** get_all_worlds()
{
	return game->worlds;
}

void active_window(GLFWwindow ** _window)
{
	assert(game != NULL);
	*_window = game->window;
}

void active_world(World** _world)
{
	assert(game != NULL);
	if (game->active_world)
	{
		*_world = game->active_world;
	}
	else
	{
		*_world = NULL;
	}
}

void active_player(Player ** _player)
{
	assert(game != NULL);
	*_player = game->player;
}

void active_camera(Camera ** _camera)
{
	assert(game != NULL);
	*_camera = game->camera;
}