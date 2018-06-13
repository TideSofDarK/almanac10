#pragma once

#include <cglm\cglm.h>

typedef struct
{
	vec3 pos;
	vec3 euler;
	vec3 scale;
} Transform;

Transform create_transform();

void euler_to_front(vec3, vec3);
float transform_distance(Transform, Transform);
void translate_pos_vec3(Transform*, vec3);
void translate_pos_axis(Transform*, unsigned int, float);
void translate_euler_axis(Transform*, unsigned int, float);