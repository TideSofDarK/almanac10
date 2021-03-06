#pragma once

#include <cglm/cglm.h>

typedef struct
{
    unsigned int ID;
    char *vertex_shader_source;
    char *fragment_shader_source;
    char *geometry_shader_source;
} Shader;

void construct_shader(Shader **, const char *, const char *, const char *);

void destruct_shader(Shader **);

void use_shader(Shader *);

void set_uniform_vec2(Shader *, const char *, vec2);

void set_uniform_vec3(Shader *, const char *, vec3);

void set_uniform_vec4(Shader *, const char *, float, float, float, float);

void set_uniform_float(Shader *, const char *, float);

void set_uniform_int(Shader *, const char *, int);

void set_uniform_mat4(Shader *, const char *, mat4);