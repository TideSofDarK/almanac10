#pragma once

#include "party.h"
#include "camera.h"

typedef struct
{
    Party party;
} Player;

void construct_player(Player**);
void destruct_player(Player**);

void update_player(Player *, float delta_time);

void ground_navigation(Camera *, float);
void fly_navigation(Camera *, float);