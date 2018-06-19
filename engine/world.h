#pragma once

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "vector.h"
#include "camera.h"
#include "sprite.h"
#include "creature.h"
#include "projectile.h"
#include "object.h"
#include "terrain.h"

typedef struct {
    char *name;

    Projectile **projectiles;
    unsigned int *projectiles_to_remove;

    Creature **creatures;
    unsigned int *creatures_to_remove;

    Object3D **objects3d;
    unsigned int *objects3d_to_remove;

    Terrain *terrain;

    lua_State *L;
} World;

void construct_world(World **, const char *);

void destruct_world(World **);

void insert_object3d(World *, Object3D *);

void insert_projectile(World *, Projectile *);

Creature *spawn_creature(World *, const char *, vec3);

void update_world(World *);