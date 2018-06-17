#pragma once

#include "party.h"
#include "camera.h"

typedef struct {
    Party party;
} Player;

void construct_player(Player **);

void destruct_player(Player **);

void update_player(Player *);

void ground_navigation(Camera *);

void fly_navigation(Camera *);