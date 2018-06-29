#pragma once

typedef enum
{
    OM_CENTER, /* particles and projectiles */
    OM_BOTTOM, /* mostly for creatures */
    OM_LAST
} SpriteOriginMode;

typedef struct
{
    unsigned int VBO, VAO, EBO;
} RenderData;

const float *get_quad_vertices_and_tex_coords(int *);

const float *get_sprite_vertices_and_tex_coords(int *, SpriteOriginMode);

const float *get_render_texture_vertices_and_tex_coords(int *);

const float *get_cross_vertices(int *);