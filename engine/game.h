#pragma once

#include <GLFW/glfw3.h>

#include "world.h"
#include "camera.h"
#include "vector.h"
#include "player.h"

typedef enum
{
        /* Main menu */
        GS_MENU,
        /* Party creation */
        GS_CREATION,
        /* Ingame; regular */
        GS_WORLD,
        /* Ingame; in house */
        GS_HOUSE,
        /* Ingame; inventory */
        GS_INVENTORY,
        /* Ingame; ESC-menu */
        GS_PAUSED,
        /* Editor */
        GS_EDITOR
} GameState;

typedef struct
{
        GLFWwindow *window;

        GameState state;

        Player *player;
        Camera *camera;

        World *active_world;
        World **worlds;
} Game;

void init_game(GLFWwindow *);

void shutdown_game();

GameState get_game_state();

void set_game_state(GameState);

void insert_world(World *, bool);

void make_world_active(World *);

World **get_all_worlds();

GLFWwindow *get_active_window();

World *get_active_world();

Player *get_active_player();

Camera *get_active_camera();