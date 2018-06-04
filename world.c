#include "world.h"

#include <GLFW/glfw3.h>

#include "vertices.h"
#include "grid.h"
#include "model.h"
#include "config.h"
#include "renderer.h"
#include "input.h"

#define clean_up(to_remove, things, deconstruct)						\
do {																	\
	if (to_remove != NULL && !vector_empty(to_remove))					\
	{																	\
		for (size_t i = 0; i < vector_size(to_remove); ++i) {			\
			int index_to_remove = to_remove[i];							\
			deconstruct(&things[index_to_remove]);						\
			vector_erase(things, index_to_remove);						\
		}																\
		vector_free(to_remove);											\
		to_remove = NULL;												\
	}																	\
} while(0);

/* TO REMOVE */
Model* model = NULL;

void construct_world(World ** _world, const char * name)
{
	*_world = malloc(sizeof(World));
	World *world = *_world;

	world->creatures = NULL;
	world->creatures_to_remove = NULL;

	world->projectiles = NULL;
	world->projectiles_to_remove = NULL;

	world->camera = create_camera();

	world->name = _strdup(name);

	//construct_model("spider.obj", &model);
}

void destruct_world(World ** _world)
{
	World *world = *_world;

	for (size_t i = 0; i < vector_size(world->creatures); i++)
	{
		/* TODO: improve texture freeing */
		destruct_creature(&world->creatures[i]);
	}

	vector_free(world->creatures);
	vector_free(world->creatures_to_remove);

	free(world->name);
	world->name = NULL;

	/* TO REMOVE */
	if (model != NULL)
	{
		destruct_model(&model);
		model = NULL;
	}

	free(*_world);
	*_world = NULL;
}

void insert_projectile(World* world, Projectile* projectile)
{
	vector_push_back(world->projectiles, projectile);
}

Creature* spawn_creature(World* world, CreatureData creature_data, vec3 pos)
{
	Creature* creature = NULL;
	construct_creature(&creature, creature_data, pos);
	vector_push_back(world->creatures, creature);

	return creature;
}

void update_world(World* world, float delta_time)
{
	if (world == NULL)
		return;

	/* Clean up things using macro */
	clean_up(world->projectiles_to_remove, world->projectiles, destruct_projectile)
	clean_up(world->creatures_to_remove, world->creatures, destruct_creature)

	/* Update projectiles */
	if (world->projectiles != NULL && !vector_empty(world->projectiles))
	{
		for (size_t i = 0; i < vector_size(world->projectiles); ++i) {
			Projectile* projectile = world->projectiles[i];
			if (projectile != NULL)
			{
				update_projectile(projectile);

				if (projectile->dead)
				{
					if (projectile->sprite->animation_finished)
						vector_push_back(world->projectiles_to_remove, (unsigned int)i);
				}
				else
				{
					/* TODO: proper collision checking/whatever */
					if (projectile->transform.pos[1] <= 0.0f)
					{
						kill_projectile(projectile);
						continue;
					}

					for (size_t i = 0; i < vector_size(world->creatures); ++i) {
						Creature* creature = world->creatures[i];
						if (creature != NULL)
						{
							if (!creature->dead && transform_distance(projectile->transform, creature->transform) <= projectile->radius)
							{
								kill_projectile(projectile);
								kill_creature(creature);
								break;
							}
						}
					}
				}
			}
		}
	}

	/* Update creatures */
	if (world->creatures != NULL && !vector_empty(world->creatures))
	{
		for (size_t i = 0; i < vector_size(world->creatures); ++i) {
			Creature* creature = world->creatures[i];
			if (creature != NULL)
			{
				if (strcmp(creature->name, "Dragon") == 0)
				{
					float lifetime = (float)(clock() - creature->begin) / CLOCKS_PER_SEC;
					float normalized = fmodf(lifetime, (float)M_PI * 2.0f);

					glm_vec3((vec3) {
						cosf(normalized) / 1.0f,
						cosf(normalized) / 4.0f,
						sinf(normalized) / 1.0f
					}, creature->transform.pos);

					glm_vec3((vec3) {
						fmodf(90.0f + 360.0f - (glm_deg((atan2f(creature->transform.pos[0], creature->transform.pos[2]))) + 180.0f), 360.0f), 0, 0
					}, creature->transform.euler);
				}

				update_creature(creature);
			}
		}
	}

	/* Party controls */
	update_camera(&world->camera, delta_time);

	/* Projectile test */
	if (get_control_state(CT_ATTACK) == BS_PRESSED)
	{
		Sprite* explosion = NULL;
		sprite_particle(&explosion, "explosion", 64, 64);

		Projectile* projectile = NULL;
		construct_projectile(&projectile, explosion);

		glm_vec_copy(world->camera.transform.pos, projectile->origin);
		cursor_raycast(world->camera, projectile->direction);

		insert_projectile(world, projectile);
		launch_projectile(projectile);
	}
}