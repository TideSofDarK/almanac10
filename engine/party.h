#pragma once

#include "transform.h"

typedef enum {
    C_LIZARD,
    C_TROLL,
    C_ORC,
    C_DROW,
    C_WEREWOLF,
    C_ACOLYTE,
    C_FANATIC
} Class;

typedef enum {
    HS_NONE,
    HS_CURSED,
    HS_DEAD
} Status;

typedef struct {
    int health, max_health;
    int mana, max_mana;
    int attack_dice_count, attack_dice, attack_bonus;
    Status status;
} Stats;

typedef struct {
    Class class;
    Stats stats;
    char *name;
} Hero;

typedef struct {
    Hero heroes[4];
    Transform transform;
    /* TODO: Movement speed calculations? */
} Party;

Party create_random_party();