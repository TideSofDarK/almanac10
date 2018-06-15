#include "editor.h"

#include "config.h"
#include "game.h"
#include "util.h"
#include "input.h"
#include "terrain.h"
#include "world.h"
#include "mesh.h"

static World * world = NULL;
static World * previous_world = NULL;

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

void update_editor(float delta_time)
{
    Camera * camera = NULL;
    active_camera(&camera);
    if (camera == NULL)
        return;

    bool pressed = is_press_or_pressed(CT_LMB);
    if (pressed)
    {
        world->terrain->vertices[rand() % vector_size(world->terrain->vertices)].pos[1] = ((float)rand() / (float)(RAND_MAX / (0.25f))) - 0.0125f;
        rebuild_terrain(world->terrain);
    }

    editor_navigation(camera, delta_time);
}

void toggle_editor()
{
    if (get_game_state() != GS_EDITOR)
    {
        active_world(&previous_world);

        if (world == NULL)
        {
            construct_world(&world, "new_world");
            construct_terrain(&world->terrain, 16);

            insert_world(&world, true);
        }
        else
        {
            make_world_active(&world);
        }
    }
    else
    {
        make_world_active(&previous_world);
    }

    set_game_state(get_game_state() == GS_WORLD ? GS_EDITOR : GS_WORLD);
}

void editor_navigation(Camera * camera, float delta_time)
{
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

        translate_euler_axis(&camera->transform, 0, offsetX);
        translate_euler_axis(&camera->transform, 1, offsetY);
    }

    vec3 front;
    euler_to_front(camera->transform.euler, front);

    /* WASD */
    if (is_press_or_pressed(CT_FORWARD))
    {
        vec3 offset;
        glm_vec_mul(front, (vec3) { speed, speed, speed }, offset);
        translate_pos_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_BACK))
    {
        vec3 offset;
        glm_vec_mul(front, (vec3) { -speed, -speed, -speed }, offset);
        translate_pos_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_LEFT))
    {
        if (get_control_state(CT_STRAFE) == BS_NONE)
        {
            vec3 offset;
            glm_vec_cross(front, (float*)get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3) { -speed, -speed, -speed }, offset);
            translate_pos_vec3(&camera->transform, offset);
        }
        else
        {
            // Yaw
            translate_euler_axis(&camera->transform, 0, -rotatation_speed);
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
            translate_pos_vec3(&camera->transform, offset);
        }
        else
        {
            // Yaw
            translate_euler_axis(&camera->transform, 0, rotatation_speed);
        }
    }

    vec3 camera_center;
    glm_vec_add(camera->transform.pos, front, camera_center);
    glm_lookat(camera->transform.pos, camera_center, (float*)get_default_up(), camera->view);
}