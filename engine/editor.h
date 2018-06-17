#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "nuklear.h"

#include "camera.h"

typedef enum {
    EM_TERRAIN,
    EM_PROPS,
    EM_CREATURES
} EditorMode;

typedef enum {
    GT_SPHERE,
    GT_ARROW
} GizmoType;

typedef struct Gizmo {
    Transform transform;
    vec3 *value;
    vec4 color;
    GizmoType type;
    struct Gizmo *children;
} Gizmo;

void shutdown_editor();

void editor_ui(struct nk_context *);

void update_editor();

void toggle_editor();

void editor_navigation(Camera *);

Gizmo *get_gizmos();