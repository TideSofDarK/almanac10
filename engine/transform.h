#pragma once

#include <cglm/cglm.h>

typedef struct {
    vec3 pos;
    vec3 euler;
    vec3 scale;
} Transform;

void init_transform(Transform *);

void euler_to_front(vec3, vec3);

void transform_front(Transform transform, vec3 dest);

void transform_to_mat4(Transform, mat4);

float transform_distance(Transform, Transform);

void transform_scale(Transform *, float);

void transform_translate_vec3(Transform *, vec3);

void transform_translate_axis(Transform *, unsigned int, float);

void transform_rotate_axis(Transform *, unsigned int, float);