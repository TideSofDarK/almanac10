#pragma once

#include <cglm/cglm.h>

#include "vertices.h"
#include "vector.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

typedef struct
{
    RenderData render_data;
    Vertex * vertices;
    int* indices;
    int grid_size;
} Terrain;

void construct_terrain(Terrain**, int);
void destruct_terrain(Terrain**);

void rebuild_terrain(Terrain*);