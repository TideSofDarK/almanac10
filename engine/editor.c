#include "editor.h"

#include "config.h"
#include "game.h"
#include "util.h"
#include "input.h"
#include "terrain.h"
#include "world.h"
#include "mesh.h"
#include "camera.h"

#define GIZMO_RED (vec4){1.0f,0.0f,0.0f,1.0f}
#define GIZMO_GREEN (vec4){0.0f,1.0f,0.0f,1.0f}

static EditorMode editor_mode = EM_TERRAIN;

static Gizmo * gizmos = NULL;
static Gizmo * selected_gizmo = NULL;

static World * world = NULL;
static World * previous_world = NULL;

void shutdown_editor()
{
    if (gizmos != NULL)
        vector_free(gizmos);

    world = NULL;
    previous_world = NULL;
}

static inline void select_gizmo(int index)
{
    if (selected_gizmo != NULL)
    {
        glm_vec4_copy(GIZMO_RED, selected_gizmo->color);
    }

    selected_gizmo = &gizmos[index];

    glm_vec4_copy(GIZMO_GREEN, selected_gizmo->color);
}

static inline void clear_gizmos()
{
    if (gizmos != NULL)
    {
        vector_free(gizmos);
        gizmos = NULL;
        selected_gizmo = NULL;
    }
}

static inline void rebuild_terrain_gizmos(Terrain * terrain)
{
    clear_gizmos();

    float scale = (float)terrain->grid_size;

    for (size_t i = 0; i < vector_size(terrain->vertices); ++i)
    {
        Gizmo gizmo;

        gizmo.value = &terrain->vertices[i].pos;

        glm_vec4_copy(GIZMO_RED, gizmo.color);

        init_transform(&gizmo.transform);
        glm_vec_copy(*gizmo.value, gizmo.transform.pos);
        glm_vec_mul(gizmo.transform.pos, (vec3){scale,scale,scale}, gizmo.transform.pos);

        vector_push_back(gizmos, gizmo);
    }
}

static inline void editor_toolbar(struct nk_context * ctx)
{
    Config config = get_config();

    float font_size = 19;
    int w = config.w;
    int h = 32;
    int start_x = 0;
    int start_y = 0;
    if (nk_begin(ctx, "World Editor", nk_rect((float)start_x, (float)start_y, (float)w, (float)h), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_begin(ctx, NK_STATIC, font_size, 3);
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
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}

void editor_ui(struct nk_context *ctx)
{
    if (world == NULL)
        return;

    editor_toolbar(ctx);
}

void update_editor()
{
    Camera * camera = get_active_camera();
    if (camera == NULL)
        return;

    /* Gizmos hit test */
    if (get_control_state(CT_LMB) == BS_PRESSED)
    {
        vec3 origin, direction;
        cursor_raycast(camera, origin, direction);

        vec3 line;
        glm_vec_copy(direction, line);

        line[0] *= 8.0f;
        line[1] *= 8.0f;
        line[2] *= 8.0f;

        glm_vec_add(origin, line, line);

        for (size_t i = 0; i < vector_size(gizmos); i++)
        {
            vec3 p1;
            glm_vec_copy(origin, p1);
            vec3 p2;
            glm_vec_copy(line, p2);
            vec3 p3;
            glm_vec_copy(gizmos[i].transform.pos, p3);
            float r = 0.1f;

            float x1 = p1[0]; float y1 = p1[1]; float z1 = p1[2];
            float x2 = p2[0]; float y2 = p2[1]; float z2 = p2[2];
            float x3 = p3[0]; float y3 = p3[1]; float z3 = p3[2];

            float dx = x2 - x1;
            float dy = y2 - y1;
            float dz = z2 - z1;

            float a = dx*dx + dy*dy + dz*dz;
            float b = 2.0f * (dx * (x1 - x3) + dy * (y1 - y3) + dz * (z1 - z3));
            float c = x3*x3 + y3*y3 + z3*z3 + x1*x1 + y1*y1 + z1*z1 - 2.0f * (x3*x1 + y3*y1 + z3*z1) - r*r;

            float test = b*b - 4.0f*a*c;

            if (test >= 0.0f)
            {
                select_gizmo((int)i);
            }
        }
    }

    editor_navigation(camera);
}

void toggle_editor()
{
    if (get_game_state() != GS_EDITOR)
    {
        previous_world = get_active_world();

        if (world == NULL)
        {
            construct_world(&world, "new_world");
            construct_terrain(&world->terrain, 16);

            insert_world(world, true);
        }
        else
        {
            make_world_active(world);
        }

        if (editor_mode == EM_TERRAIN)
        {
            rebuild_terrain_gizmos(world->terrain);
        }
    }
    else
    {
        make_world_active(previous_world);
    }

    set_game_state(get_game_state() == GS_WORLD ? GS_EDITOR : GS_WORLD);
}

void editor_navigation(Camera * camera)
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
        float offsetX = cx - px;
        float offsetY = py - cy;
        px = cx;
        py = cy;

        float sensitivity = 0.1f;
        offsetX *= sensitivity;
        offsetY *= sensitivity;

        transform_rotate_axis(&camera->transform, 0, offsetX);
        transform_rotate_axis(&camera->transform, 1, offsetY);
    }

    vec3 front;
    euler_to_front(camera->transform.euler, front);

    /* WASD */
    if (is_press_or_pressed(CT_FORWARD))
    {
        vec3 offset;
        glm_vec_mul(front, (vec3) { speed, speed, speed }, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_BACK))
    {
        vec3 offset;
        glm_vec_mul(front, (vec3) { -speed, -speed, -speed }, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_LEFT))
    {
        if (get_control_state(CT_STRAFE) == BS_NONE)
        {
            vec3 offset;
            glm_vec_cross(front, (float*)get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3) { -speed, -speed, -speed }, offset);
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
            vec3 offset;
            glm_vec_cross(front, (float*)get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3) { speed, speed, speed }, offset);
            transform_translate_vec3(&camera->transform, offset);
        }
        else
        {
            // Yaw
            transform_rotate_axis(&camera->transform, 0, rotatation_speed);
        }
    }
}

Gizmo * get_gizmos()
{
    return gizmos;
}