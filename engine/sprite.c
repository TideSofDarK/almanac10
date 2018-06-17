#include "sprite.h"

#include <malloc.h>

#include "util.h"
#include "vertices.h"
#include "world.h"
#include "renderer.h"
#include "game.h"

/* Used to flip sprite to match direction */
static const int DIR_INDICES[] = {
        DIR_N,
        DIR_NE,
        DIR_E,
        DIR_SE,
        DIR_S,
        -DIR_SE,
        -DIR_E,
        -DIR_NE
};

static inline void particle_path(char **dest, const char *particle) {
    asprintf(dest, "assets/particles/%s.png", particle);
}

static inline void creature_sheet_path(char **dest, const char *folder, const char *state, int dir) {
    asprintf(dest, "assets/creatures/%s/%s%s%s%s%i.png", folder, folder, "_", state, "_", dir);
}

/* Internal; use sprite_creature, sprite_particle, etc */
void construct_sprite(Sprite **_sprite, unsigned int w, unsigned int h) {
    *_sprite = malloc(sizeof(Sprite));
    Sprite *sprite = *_sprite;

    for (size_t i = 0; i < MAXANIM; i++) {
        sprite->textures[i] = NULL;
    }

    sprite->w = w;
    sprite->h = h;
    sprite->sheet_position = 0;
    sprite->anim_state = ANIM_NONE;
    sprite->anim_speed = 2.0f;
    sprite->animation_finished = 0;
    sprite->begin = clock();
}

void destruct_sprite(Sprite **_sprite) {
    Sprite *sprite = *_sprite;

    free(*_sprite);
    *_sprite = NULL;
}

void sprite_creature(Sprite **_sprite, const char *folder) {
    construct_sprite(_sprite, CREATURE_WIDTH, CREATURE_HEIGHT);
    Sprite *sprite = *_sprite;

    sprite->anim_state = ANIM_MOVE;

    char *path = NULL;

    for (unsigned int i = 0; i < 5; i++) {
        creature_sheet_path(&path, folder, "move", i);
        sprite->textures[ANIM_SLOT(ANIM_MOVE, i)] = get_texture(path);
    }

    creature_sheet_path(&path, folder, "death", 0);
    sprite->textures[ANIM_SLOT(ANIM_DEATH, 0)] = get_texture(path);

    free(path);
}

void sprite_particle(Sprite **_sprite, const char *particle, int w, int h) {
    construct_sprite(_sprite, w, h);
    Sprite *sprite = *_sprite;

    /* TODO: maybe we should have separate variable for animation stop/play */
    sprite->anim_state = ANIM_DEATH;
    sprite->begin = 0;

    Texture *texture = NULL;

    char *path = NULL;
    particle_path(&path, particle);
    sprite->textures[ANIM_SLOT(ANIM_DEATH, 0)] = get_texture(path);

    free(path);
}

void update_sprite(Sprite *sprite) {
    float lifetime = (float) (clock() - sprite->begin) / CLOCKS_PER_SEC;

    if (sprite->anim_state != ANIM_NONE && sprite->begin) {
        /* Check if animation should not be looped */
        int sheet_length = get_sheet_length(sprite);
        if (sprite->anim_state >= 0 || sprite->sheet_position < sheet_length - 1) {
            sprite->sheet_position = (unsigned int) (floorf(
                    fmodf(lifetime * sprite->anim_speed, 1.0) * (float) sheet_length));
        } else {
            sprite->animation_finished = 1;
        }
    }
}

void play_sprite_animation(Sprite *sprite, AnimationState animation) {
    sprite->anim_state = animation;
    sprite->sheet_position = 0;
    sprite->begin = clock();
}

int get_sheet_length(Sprite *sprite) {
    /* Assuming there is 0 (North) direction */
    /* TODO: maybe we need better animation system */
    return sprite->textures[ANIM_SLOT(sprite->anim_state, 0)]->w /
           sprite->textures[ANIM_SLOT(sprite->anim_state, 0)]->h;
}

int determine_orientation(Transform transform, Camera *camera) {
    vec3 diff;
    glm_vec_sub(camera->transform.pos, transform.pos, diff);

    glm_vec_rotate(diff, glm_rad(transform.euler[0]), (vec3) {0.0f, 1.0f, 0.0f});

    int directions = sizeof(DIR_INDICES) / sizeof(int) / 2;

    float id = roundf(atan2f(diff[0], diff[2]) * (float) directions / (float) M_PI);

    if ((int) id == directions) {
        id = (float) -directions;
    }

    return DIR_INDICES[4 + (int) id];
}