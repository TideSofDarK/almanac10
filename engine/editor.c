#include "editor.h"

#include "config.h"
#include "game.h"
#include "util.h"
#include "input.h"
#include "terrain.h"
#include "mesh.h"
#include "camera.h"
#include "transform.h"

#define GIZMO_RAY 8.0f
#define GIZMO_SPHERE_RADIUS 0.1f
#define GIZMO_CYLINDER_RADIUS 0.02f

#define GIZMO_RED \
    (vec4) { 1.0f, 0.0f, 0.0f, 1.0f }
#define GIZMO_GREEN \
    (vec4) { 0.0f, 1.0f, 0.0f, 1.0f }
#define GIZMO_BLUE \
    (vec4) { 0.0f, 1.0f, 0.0f, 1.0f }

#define GIZMO_AXIS_COLOR(a) \
    (vec4) { (a) == 0 ? 1.0f : 0.0f, (a) == 1 ? 1.0f : 0.0f, (a) == 2 ? 1.0f : 0.0f, 1.0f }

#define GIZMO_WHITE \
    (vec4) { 1.0f, 1.0f, 1.0f, 1.0f }

static Editor editor = {};

static inline void clear_gizmos(Gizmo **gizmos)
{
    if (gizmos == NULL)
        return;
    if (*gizmos != NULL)
    {
        vector_free(*gizmos);
        *gizmos = NULL;
    }
}

static inline void clear_axis_gizmos()
{
    clear_gizmos(&editor.axis_gizmos);
}

static inline void gizmos_middle_point(Gizmo *gizmos, int *indices, vec3 out)
{
    glm_vec_copy(GLM_VEC3_ZERO, out);

    for (int i = 0; i < vector_size(indices); i++)
    {
        glm_vec_add(out, gizmos[indices[i]].transform.pos, out);
    }

    int count = vector_size(indices);

    out[0] /= count;
    out[1] /= count;
    out[2] /= count;
}

static inline void update_axis_gizmos_position()
{
    vec3 middle_point = {};
    gizmos_middle_point(editor.gizmos, editor.selected_gizmos, middle_point);

    for (int i = 0; i < 3; ++i)
    {
        glm_vec_copy(middle_point, editor.axis_gizmos[i].transform.pos);
    }
}

static inline void init_axis_gizmos()
{
    clear_axis_gizmos();

    for (int i = 0; i < 3; ++i)
    {
        Gizmo axis = {};
        axis.type = GT_ARROW;

        glm_vec4_copy(GIZMO_AXIS_COLOR(i), axis.color);

        init_transform(&axis.transform);

        if (i == 0)
        {
            transform_rotate_axis(&axis.transform, 2, -90.0f);
        }
        else if (i == 1)
        {
            transform_rotate_axis(&axis.transform, 1, 90.0f);
        }
        else if (i == 2)
        {
            transform_rotate_axis(&axis.transform, 0, 90.0f);
        }

        vector_push_back(editor.axis_gizmos, axis);
    }

    update_axis_gizmos_position();
}

static inline void select_gizmo(int index, bool add_to_selection)
{
    /* Unselect previous one */
    if (!add_to_selection)
    {
        if (vector_size(editor.selected_gizmos) > 0)
        {
            for (int i = 0; i < vector_size(editor.selected_gizmos); i++)
            {
                glm_vec4_copy(GIZMO_RED, editor.gizmos[editor.selected_gizmos[i]].color);
            }
            editor.selected_gizmos = NULL;

            clear_axis_gizmos();
        }
    }

    vector_push_back(editor.selected_gizmos, index);

    glm_vec4_copy(GIZMO_WHITE, editor.gizmos[index].color);
    init_axis_gizmos();
}

static inline void set_gizmo_position(Gizmo *gizmo, vec3 pos, float scale)
{
    init_transform(&gizmo->transform);
    glm_vec_copy(pos, gizmo->transform.pos);
    glm_vec_mul(gizmo->transform.pos, (vec3){scale, scale, scale}, gizmo->transform.pos);
}

static inline void rebuild_terrain_gizmos(Terrain *terrain)
{
    clear_gizmos(&editor.gizmos);

    for (size_t i = 0; i < vector_size(terrain->vertices); ++i)
    {
        Gizmo gizmo;

        gizmo.type = GT_SPHERE;

        glm_vec4_copy(GIZMO_RED, gizmo.color);

        set_gizmo_position(&gizmo, terrain->vertices[i].pos, (float)terrain->grid_size);

        vector_push_back(editor.gizmos, gizmo);
    }
}

static inline void editor_toolbar(struct nk_context *ctx)
{
    Config config = get_config();

    static const char *editor_modes[] = {"Terrain", "Props", "Creatures"};
    static const char *axis_modes[] = {"Translate", "Rotate", "Scale"};

    float font_size = 19;
    int w = config.w;
    int h = 32;
    int start_x = 0;
    int start_y = 0;
    if (nk_begin(ctx, "World Editor", nk_rect((float)start_x, (float)start_y, (float)w, (float)h),
                 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_begin(ctx, NK_STATIC, font_size, 5);
        nk_layout_row_push(ctx, 50);
        if (nk_button_label(ctx, "file"))
        {
        }
        nk_layout_row_push(ctx, 50);
        if (nk_button_label(ctx, "edit"))
        {
        }
        nk_layout_row_push(ctx, 50);
        if (nk_button_label(ctx, "view"))
        {
        }
        nk_layout_row_push(ctx, 100);
        editor.editor_mode = nk_combo(ctx, editor_modes, NK_LEN(editor_modes), editor.axis_mode, font_size, nk_vec2(100, 200));
        nk_layout_row_push(ctx, 100);
        editor.axis_mode = nk_combo(ctx, axis_modes, NK_LEN(axis_modes), editor.axis_mode, font_size, nk_vec2(100, 200));
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}

void init_editor()
{
    editor.gizmos = NULL;
    editor.axis_gizmos = NULL;
    editor.selected_gizmos = NULL;
    editor.drag.dragging = false;
    editor.editor_mode = EM_TERRAIN;
    editor.axis_mode = AM_TRANSLATE;
    editor.world = NULL;
    editor.previous_world = NULL;
}

void shutdown_editor()
{
    clear_gizmos(&editor.gizmos);

    editor.world = NULL;
    editor.previous_world = NULL;
}

void editor_ui(struct nk_context *ctx)
{
    if (editor.world == NULL)
        return;

    editor_toolbar(ctx);
}

void update_editor()
{
    Camera *camera = get_active_camera();
    if (camera == NULL)
        return;

    static float px, py;
    float cx, cy;
    cursor_position(&cx, &cy);

    /* Gizmo drag */
    if (editor.drag.dragging)
    {
        if (get_control_state(CT_LMB) == BS_RELEASED)
        {
            init_axis_gizmos();

            editor.drag.dragging = false;
            px = py = 0;
            return;
        }
        else if ((px == 0 && py == 0))
        {
            px = cx;
            py = cy;
        }
        else
        {
            float ox = cx - px;
            float oy = py - cy;
            px = cx;
            py = cy;

            vec3 r_origin = {}, r_dir = {}, new_pos = {}, offset = {};
            cursor_raycast_custom(camera, cx + editor.drag.offset[0], cy + editor.drag.offset[1], r_origin, r_dir);

            vec3 or_origin = {}, or_dir = {};
            cursor_raycast_custom(camera, ox, oy, or_origin, or_dir);

            vec3 dragging_gizmos_position = {};
            gizmos_middle_point(editor.gizmos, editor.selected_gizmos, dragging_gizmos_position);

            float distance = glm_vec_distance(dragging_gizmos_position, or_origin);
            glm_vec_copy(r_origin, new_pos);
            glm_vec_muladds(r_dir, distance, new_pos);

            float actual_offset = new_pos[editor.drag.axis] - dragging_gizmos_position[editor.drag.axis];

            for (int i = 0; i < vector_size(editor.selected_gizmos); i++)
            {
                editor.gizmos[editor.selected_gizmos[i]].transform.pos[editor.drag.axis] += actual_offset;
                editor.world->terrain->vertices[editor.selected_gizmos[i]].pos[editor.drag.axis] += actual_offset / (float)editor.world->terrain->grid_size;
            }

            rebuild_terrain(editor.world->terrain);

            update_axis_gizmos_position();
        }
    }
    else if (is_press_or_pressed(CT_LMB)) /* Gizmos hit test */
    {
        vec3 r_origin = {}, r_dir = {}, r_end = {};
        cursor_raycast(camera, r_origin, r_dir);
        glm_vec_copy(r_origin, r_end);
        glm_vec_muladds(r_dir, GIZMO_RAY, r_end);

        /* Single click (selecting) */
        /* Choosing closest to camera */
        int potential_gizmo = -1;
        if (get_control_state(CT_LMB) == BS_PRESSED)
        {
            for (size_t i = 0; i < vector_size(editor.gizmos); i++)
            {
                float scale = transform_distance(camera->transform, editor.gizmos[i].transform) * GIZMO_SCALE;
                switch (editor.gizmos[i].type)
                {
                case GT_SPHERE:
                    if (potential_gizmo == -1 ||
                        glm_vec_distance(camera->transform.pos,
                                         editor.gizmos[i].transform.pos) <
                            glm_vec_distance(camera->transform.pos,
                                             editor.gizmos[potential_gizmo].transform.pos))
                    {
                        if (intersect_ray_sphere(r_origin, r_end, editor.gizmos[i].transform.pos,
                                                 GIZMO_SPHERE_RADIUS * scale))
                        {
                            potential_gizmo = (int)i;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        if (potential_gizmo >= 0)
        {
            select_gizmo(potential_gizmo, get_control_state(CT_LEFT_SHIFT) == BS_PRESS);
        }
        else if (vector_size(editor.selected_gizmos) > 0 && is_pressed(CT_LMB))
        {
            for (size_t i = 0; i < vector_size(editor.axis_gizmos); i++) /* Axis gizmos hit test */
            {
                vec3 c_end = {};
                float scale = transform_distance(camera->transform, editor.axis_gizmos[i].transform) * GIZMO_SCALE;
                transform_front(editor.axis_gizmos[i].transform, c_end);
                glm_vec_add(editor.axis_gizmos[i].transform.pos, c_end, c_end);
                if (intersect_ray_cylinder(r_origin, r_end, editor.axis_gizmos[i].transform.pos, c_end,
                                           GIZMO_CYLINDER_RADIUS * scale))
                {
                    glm_vec4_copy(GIZMO_WHITE, editor.axis_gizmos[i].color);

                    editor.drag.dragging = true;
                    gizmos_middle_point(editor.gizmos, editor.selected_gizmos, editor.drag.start);
                    world_to_screen(camera, editor.drag.start, editor.drag.offset);
                    editor.drag.offset[0] -= cx;
                    editor.drag.offset[1] -= cy;
                    editor.drag.axis = (int)i;

                    break;
                }
            }
        }
    }

    editor_navigation(camera);
}

void toggle_editor()
{
    if (get_game_state() != GS_EDITOR)
    {
        editor.previous_world = get_active_world();

        if (editor.world == NULL)
        {
            construct_world(&editor.world, "new_world");
            construct_terrain(&editor.world->terrain, 16);

            insert_world(editor.world, true);
        }
        else
        {
            make_world_active(editor.world);
        }

        if (editor.editor_mode == EM_TERRAIN)
        {
            rebuild_terrain_gizmos(editor.world->terrain);
        }
    }
    else
    {
        make_world_active(editor.previous_world);
    }

    set_game_state(get_game_state() == GS_WORLD ? GS_EDITOR : GS_WORLD);
}

void editor_navigation(Camera *camera)
{
    float delta_time = get_delta_time();

    static float px, py;

    float rotatation_speed = 96.0f * delta_time;
    float speed = 5.0f * delta_time;

    float cx, cy;
    cursor_position(&cx, &cy);

    bool pressed = is_press_or_pressed(CT_RMB);

    /* Camera pitch */
    if ((px == 0 && py == 0) || !pressed)
    {
        px = cx;
        py = cy;
    }
    else
    {
        float ox = cx - px;
        float oy = py - cy;
        px = cx;
        py = cy;

        float sensitivity = 0.1f;
        ox *= sensitivity;
        oy *= sensitivity;

        transform_rotate_axis(&camera->transform, 0, ox);
        transform_rotate_axis(&camera->transform, 1, oy);
    }

    vec3 front = {};
    transform_front(camera->transform, front);

    /* WASD */
    if (is_press_or_pressed(CT_FORWARD))
    {
        vec3 offset = {};
        glm_vec_mul(front, (vec3){speed, speed, speed}, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_BACK))
    {
        vec3 offset = {};
        glm_vec_mul(front, (vec3){-speed, -speed, -speed}, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_LEFT))
    {
        if (get_control_state(CT_STRAFE) == BS_NONE)
        {
            vec3 offset = {};
            glm_vec_cross(front, (float *)get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3){-speed, -speed, -speed}, offset);
            transform_translate_vec3(&camera->transform, offset);
        }
        else
        {
            // Yaw
            transform_rotate_axis(&camera->transform, 0, -rotatation_speed);
        }
    }
    if (is_press_or_pressed(CT_RIGHT))
    {
        if (get_control_state(CT_STRAFE) == BS_NONE)
        {
            vec3 offset = {};
            glm_vec_cross(front, (float *)get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3){speed, speed, speed}, offset);
            transform_translate_vec3(&camera->transform, offset);
        }
        else
        {
            // Yaw
            transform_rotate_axis(&camera->transform, 0, rotatation_speed);
        }
    }
}

Editor *get_editor()
{
    return &editor;
}