#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#include "nuklear.h"

#include "camera.h"
#include "world.h"

#define GIZMO_SCALE 0.15f

typedef enum
{
    EM_TERRAIN,
    EM_PROPS,
    EM_CREATURES
} EditorMode;

typedef enum
{
    AM_TRANSLATE,
    AM_ROTATE,
    AM_SCALE
} AxisMode;

typedef enum
{
    GT_SPHERE,
    GT_ARROW,
    GT_CYLINDER
} GizmoType;

typedef struct Gizmo
{
    Transform transform;
    //void *value;
    vec4 color;
    GizmoType type;
} Gizmo;

typedef struct
{
    Gizmo *gizmos;
    Gizmo *axis_gizmos;

    int *selected_gizmos;
    struct Drag
    {
        bool dragging;
        vec3 offset;
        vec3 start;
        int axis;
    } drag;

    EditorMode editor_mode;
    AxisMode axis_mode;

    World *world;
    World *previous_world;
} Editor;

void init_editor();

void shutdown_editor();

void editor_ui(struct nk_context *);

void update_editor();

void toggle_editor();

void editor_navigation(Camera *);

Editor *get_editor();