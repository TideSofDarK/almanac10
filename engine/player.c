#include "player.h"

#include "game.h"
#include "input.h"

#include <malloc.h>

void construct_player(Player ** _player)
{
    *_player = malloc(sizeof(Player));
    Player * player = *_player;

    /* TODO: Proper party creation */
    player->party = create_random_party();
}

void destruct_player(Player ** _player)
{
    Player * player = *_player;

    free(*_player);
    *_player = NULL;
}

void update_player(Player *player, float delta_time)
{
    Camera * camera = NULL;
    active_camera(&camera);
    if (camera == NULL)
        return;

    GameState game_state = get_game_state();

    switch (game_state)
    {
        case GS_MENU:break;
        case GS_CREATION:break;
        case GS_WORLD:
            ground_navigation(camera, delta_time);
            break;
        case GS_HOUSE:break;
        case GS_INVENTORY:break;
        case GS_PAUSED:break;
        case GS_EDITOR:break;
    }
}

void ground_navigation(Camera * camera, float delta_time)
{
    float rotatation_speed = 96.0f * delta_time;
    float speed = 5.0f * delta_time;

    /* Camera pitch */
    if (is_press_or_pressed(CT_CAMERA_UP))
    {
        translate_euler_axis(&camera->transform, 1, rotatation_speed);
    }
    if (is_press_or_pressed(CT_CAMERA_DOWN))
    {
        translate_euler_axis(&camera->transform, 1, -rotatation_speed);
    }
    if (get_control_state(CT_CAMERA_HOME) == BS_PRESSED)
    {
        camera->transform.euler[1] = 0.0f;
    }

    vec3 front;
    euler_to_front(camera->transform.euler, front);
    vec3 moving_front;
    glm_vec_mul(front, (vec3) { 1.0f, 0.0f, 1.0f }, moving_front);

    /* WASD */
    if (is_press_or_pressed(CT_FORWARD))
    {
        vec3 offset;
        glm_vec_mul(moving_front, (vec3) { speed, speed, speed }, offset);
        translate_pos_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_BACK))
    {
        vec3 offset;
        glm_vec_mul(moving_front, (vec3) { -speed, -speed, -speed }, offset);
        translate_pos_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_LEFT))
    {
        if (get_control_state(CT_STRAFE) == BS_PRESS)
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
        if (get_control_state(CT_STRAFE) == BS_PRESS)
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

void fly_navigation(Camera * camera, float delta_time)
{

}