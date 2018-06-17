#pragma once

#include "transform.h"

typedef struct
{
	Transform transform;
	mat4 view;
	mat4 projection;
} Camera;

void construct_camera(Camera **);
void destruct_camera(Camera **);

void camera_direction(Camera *, vec3 *);
void camera_right(Camera *, vec3 *);
void camera_up(Camera *, vec3 *);

void camera_projection(Camera *, float, float, float);

const float* get_default_front();
const float* get_default_up();

void update_camera(Camera *);