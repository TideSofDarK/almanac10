#pragma once

#include "transform.h"
#include "model.h"

/* Clickable wells, houses, etc... */
typedef struct
{
    Transform transform;
    Model * model;

    unsigned int index;
} Object3D;

void construct_object3d(Object3D**, Model*);
void destruct_object3d(Object3D**);