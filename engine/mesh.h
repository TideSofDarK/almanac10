#pragma once

#include <cglm/cglm.h>

#include "vertices.h"
#include "vector.h"
#include "texture.h"
#include "shader.h"

typedef struct {
    vec3 pos;
    vec3 normal;
    vec2 tex_coords;
} Vertex;

typedef struct {
    RenderData render_data;
    Vertex *vertices;
    int *indices;
    Texture *texture;
} Mesh;

void construct_mesh(Vertex *, int *, Texture *, Mesh **);

void destruct_mesh(Mesh **);