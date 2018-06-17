#include "object.h"

#include <malloc.h>

void construct_object3d(Object3D **_object3d, Model *model) {
    *_object3d = malloc(sizeof(Object3D));
    Object3D *object3d = *_object3d;

    object3d->model = model;

    init_transform(&object3d->transform);
}

void destruct_object3d(Object3D **_object3d) {
    Object3D *object3d = *_object3d;

    /* Model will be freed separately */

    free(*_object3d);
    *_object3d = NULL;
}