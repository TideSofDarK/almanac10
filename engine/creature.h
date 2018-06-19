#pragma once

#include <cglm/cglm.h>

#include "transform.h"
#include "sprite.h"

typedef enum {
    MOVCAP_NONE,
    MOVCAP_GROUND,
    MOVCAP_FLY
} MovementCapability;

typedef enum {
    /* No AI */
            AISTATE_NONE,
    /* Rare use; just stands still */
            AISTATE_IDLE,
    /* Default NPC behavior */
            AISTATE_ROAM,
    /* Fleeing from attack_target, will return to chasing/attack after some time */
            AISTATE_FLEE,
    /* Stands still; attacks if possible or starts chasing */
            AISTATE_ATTACKING,
    /* Chasing the attack_target */
            AISTATE_CHASING
} AIState;

/* Used to store parsed LUA data */
typedef struct {
    char *script_name;
    char *name;
    int health, mana;
    int attack_dice_count, attack_dice, attack_bonus;
    MovementCapability movement_capability;
    char *sprite_sheet_folder;
} CreatureData;

/* Anything that can move, attack, cast spells, etc */
typedef struct Creature {
    Transform transform;
    vec3 start_pos;

    CreatureData const *data;

    Sprite *sprite;
    struct Creature *attack_target;

    AIState ai_state;
    vec3 roam_start_pos;
    vec3 roam_pos;
    clock_t roam_clock;

    int health, max_health;
    int mana, max_mana;
    int attack_dice_count, attack_dice, attack_bonus;

    const char *name;
    MovementCapability movement_capability;

    int dead;
    int index;
    // TODO: Abilities, loot, etc...
    clock_t begin;
} Creature;

CreatureData const *get_precached_creature_data(const char *);

void free_precached_creatures();

void construct_creature(Creature **, CreatureData const *, vec3);

void destruct_creature(Creature **);

void reset_creature_stats(Creature *);

void kill_creature(Creature *);

void update_creature(Creature *);