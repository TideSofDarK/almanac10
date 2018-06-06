#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <lautoc.h>

#include "vector.h"
#include "camera.h"
#include "sprite.h"
#include "creature.h"
#include "projectile.h"

typedef struct
{
	//unsigned int ID;
	Camera camera;

	Projectile** projectiles;
	unsigned int* projectiles_to_remove;

	Creature** creatures;
	unsigned int* creatures_to_remove;

	char * name;

	lua_State * L;
} World;

void construct_world(World**, const char*);
void destruct_world(World**);

void insert_projectile(World*, Projectile*);
Creature* spawn_creature(World*, const char *, vec3);
void creature_by_index(Creature**, World*, unsigned int);

void update_world(World*, float);