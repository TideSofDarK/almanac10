#include "world.h"

#include <GLFW/glfw3.h>

#include "vertices.h"
#include "grid.h"
#include "model.h"
#include "config.h"
#include "renderer.h"
#include "input.h"
#include "script.h"
#include "creature.h"

/* Macros to clean up creatures, projectiles and so on */
#define clean_up(to_remove, things, deconstruct, all)					\
do {																	\
	if (all == 1)														\
	{																	\
		vector_free(to_remove);											\
		if (things != NULL && !vector_empty(things))					\
		{																\
			for (size_t i = 0; i < vector_size(things); ++i) {			\
				vector_push_back(to_remove, i);							\
			}															\
		}																\
	}																	\
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
} while(0)

void construct_world(World ** _world, const char * name)
{
	*_world = malloc(sizeof(World));
	World *world = *_world;

	world->creatures = NULL;
	world->creatures_to_remove = NULL;

	world->projectiles = NULL;
	world->projectiles_to_remove = NULL;

	world->objects3d = NULL;
	world->objects3d_to_remove = NULL;

	world->camera = create_camera();

	world->name = _strdup(name);

	construct_world_lua_state(&world->L);
}

void destruct_world(World ** _world)
{
	World *world = *_world;

	clean_up(world->projectiles_to_remove, world->projectiles, destruct_projectile, 1);
	clean_up(world->creatures_to_remove, world->creatures, destruct_creature, 1);
	clean_up(world->objects3d_to_remove, world->objects3d, destruct_object3d, 1);

	free(world->name);
	world->name = NULL;

	free(*_world);
	*_world = NULL;
}

void insert_object3d(World* world, Object3D* object3d)
{
	vector_push_back(world->objects3d, object3d);
}

void insert_projectile(World* world, Projectile* projectile)
{
	vector_push_back(world->projectiles, projectile);
}

/* TODO: maybe we should move this code somewhere else and leave only insert_creature thingy */
Creature* spawn_creature(World* world, const char * name, vec3 pos)
{
	static unsigned int creature_index;
	creature_index++;

	CreatureData creature_data = get_precached_creature_data(name);

	Creature* creature = NULL;
	construct_creature(&creature, creature_data, pos);
	creature->index = creature_index;
	vector_push_back(world->creatures, creature);

	script_insert_creature(world->L, creature_data.script_name, creature_index);

	return creature;
}

void creature_by_index(Creature** _creature, World* world, unsigned int index)
{
	if (world->creatures != NULL && !vector_empty(world->creatures))
	{
		for (size_t i = 0; i < vector_size(world->creatures); ++i) {
			Creature* creature = world->creatures[i];
			if (creature != NULL)
			{
				if (creature->index == index)
				{
					*_creature = world->creatures[i];
					return;
				}
			}
		}
	}
}

void update_world(World* world, float delta_time)
{
	if (world == NULL)
		return;

	/* Clean up things using macro */
	clean_up(world->projectiles_to_remove, world->projectiles, destruct_projectile, 0);
	clean_up(world->creatures_to_remove, world->creatures, destruct_creature, 0);
	clean_up(world->objects3d_to_remove, world->objects3d, destruct_object3d, 0);

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
				update_creature(creature);
				script_update_creature(world->L, creature, delta_time);
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