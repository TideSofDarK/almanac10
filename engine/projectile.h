#pragma once

#include "transform.h"
#include "sprite.h"

#define PROJ_DEFAULT_RADIUS 0.25f
#define PROJ_DEFAULT_LIFETIME 1.0f
#define PROJ_DEFAULT_SPEED 15.0f

typedef struct {
    Transform transform;
    vec3 origin;
    vec3 direction;
    vec3 assumed_target;
    float radius;
    float lifetime;
    float speed;
    Sprite *sprite;
    clock_t begin;
    int dead;
} Projectile;

void construct_projectile(Projectile **, Sprite *);

void destruct_projectile(Projectile **);

void launch_projectile(Projectile *);

void kill_projectile(Projectile *);

void update_projectile(Projectile *);