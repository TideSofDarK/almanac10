#pragma once

#include <cglm\cglm.h>

typedef struct
{
	vec3 pos;
	vec3 euler;
	vec3 scale;
} Transform;

void init_transform(Transform *);

void euler_to_front(vec3, vec3);
void transform_to_mat4(Transform, mat4);
float transform_distance(Transform, Transform);
void scale_by_float(Transform*, float);
void translate_pos_vec3(Transform*, vec3);
void translate_pos_axis(Transform*, unsigned int, float);
void translate_euler_axis(Transform*, unsigned int, float);