#pragma once

#include <time.h>

#include <cglm/cglm.h>

#include "shader.h"
#include "texture.h"
#include "camera.h"

#define CREATURE_WIDTH 256
#define CREATURE_HEIGHT 256

/* Sign will tell if animation should not be looped */
#define NOLOOP 5 * -1

/* Positive (or 0) animation ID */
#define ANIM_ID(A) (A + (NOLOOP * -1))

/* Animation slot is [positive animation state] + [direction] */
#define ANIM_SLOT(A, D) ((ANIM_ID(A) * 5) + D)

/* 5 possible directions of each sheet */
#define MAXANIM (((ANIM_ID(ANIM_CUSTOM2) + 1) * 5) - 1)

typedef enum
{
	ANIM_CAST1 = NOLOOP,
	ANIM_CAST2,
	ANIM_FLINCH,
	ANIM_DEATH,
	ANIM_CUSTOM1,
	ANIM_NONE,
	ANIM_IDLE,
	ANIM_MOVE,
	ANIM_ATTACK,
	ANIM_CUSTOM2
} AnimationState;

typedef enum
{
	DIR_N,
	DIR_NE,
	DIR_E,
	DIR_SE,
	DIR_S,
	DIR_SW,
	DIR_W,
	DIR_NW
} Direction;

/* Any renderable 2D entity */
typedef struct
{
	Texture* textures[MAXANIM];
	unsigned int w, h, sheet_position;
	AnimationState anim_state;
	float anim_speed;
	int animation_finished;
	clock_t begin;
} Sprite;

void destruct_sprite(Sprite**);

void sprite_creature(Sprite**, const char*);
void sprite_particle(Sprite**, const char*, int, int);

void update_sprite(Sprite*);

void play_sprite_animation(Sprite*, AnimationState);

int get_sheet_length(Sprite*);
int determine_orientation(Transform, Camera*);