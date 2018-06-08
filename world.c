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
#include "sprite.h"

/* Macros to clean up creatures, projectiles and so on */
/* TODO: possible refactoring of this */
#define remove_entities(world, to_remove, things, func, all)        \
do {                                                                \
    if (all == 1)                                                   \
    {                                                               \
        for (size_t i = 0; i < vector_size(world->things); ++i)     \
        {                                                           \
            func(&world->things[i], world);                         \
        }                                                           \
        vector_free(world->things);                                 \
        world->things = NULL;                                       \
    }                                                               \
    else                                                            \
    {                                                               \
        for (size_t i = 0; i < vector_size(world->to_remove); ++i)  \
        {                                                           \
            int index_to_remove = world->to_remove[i];              \
            func(&world->things[index_to_remove], world);           \
            vector_erase(world->things, index_to_remove);           \
        }                                                           \
    }                                                               \
    vector_free(world->to_remove);                                  \
    world->to_remove = NULL;                                        \
} while(0);

static inline void destruct_world_projectile(Projectile** _projectile, World * world)
{
	destruct_projectile(_projectile);
}

static inline void destruct_world_creature(Creature** _creature, World * world)
{
	script_destroy_creature(world->L, *_creature);
	destruct_creature(_creature);
}

static inline void destruct_world_object3d(Object3D** _object3d, World * world)
{
	destruct_object3d(_object3d);
}

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

	remove_entities(world, projectiles_to_remove, projectiles, destruct_world_projectile, 1);
	remove_entities(world, creatures_to_remove, creatures, destruct_world_creature, 1);
	remove_entities(world, objects3d_to_remove, objects3d, destruct_world_object3d, 1);

    lua_close(world->L);

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
	CreatureData creature_data = get_precached_creature_data(name);

	Creature* creature = NULL;
	construct_creature(&creature, creature_data, pos);
    int creature_index = script_insert_creature(world->L, creature_data.script_name);
    creature->index = creature_index;

    vector_push_back(world->creatures, creature);

	return creature;
}

void creature_by_index(Creature** _creature, World* world, int index)
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
	remove_entities(world, projectiles_to_remove, projectiles, destruct_world_projectile, 0);
	remove_entities(world, creatures_to_remove, creatures, destruct_world_creature, 0);
	remove_entities(world, objects3d_to_remove, objects3d, destruct_world_object3d, 0);

	/* Update projectiles */
	if (world->projectiles != NULL && !vector_empty(world->projectiles))
	{
		for (size_t i = 0; i < vector_size(world->projectiles); ++i)
		{
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
								creature->health = 0;
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
		for (size_t i = 0; i < vector_size(world->creatures); ++i)
		{
			Creature* creature = world->creatures[i];
			if (creature != NULL)
			{
				if (!creature->dead && creature->health <= 0)
				{
					kill_creature(creature);
					script_kill_creature(world->L, creature);
				}
				else
				{
					update_creature(creature);
					script_update_creature(world->L, creature, delta_time);
				}
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