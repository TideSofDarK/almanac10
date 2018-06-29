#pragma once

#include "shader.h"
#include "world.h"

typedef struct
{
    mat4 model;
    Shader *shader;
    unsigned int VBO, VAO;
} Grid;

void construct_grid();

void destruct_grid();

int is_grid_constructed();

void draw_grid();