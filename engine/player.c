#include "player.h"

#include "game.h"
#include "input.h"
#include "util.h"

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

void update_player(Player *player)
{
    Camera * camera = get_active_camera();
    if (camera == NULL)
        return;

    GameState game_state = get_game_state();

    if (game_state == GS_WORLD)
    {
        ground_navigation(camera);
    }
}

void ground_navigation(Camera * camera)
{
    float delta_time = get_delta_time();

    float rotatation_speed = 96.0f * delta_time;
    float speed = 5.0f * delta_time;

    /* Camera pitch */
    if (is_press_or_pressed(CT_CAMERA_UP))
    {
        transform_rotate_axis(&camera->transform, 1, rotatation_speed);
    }
    if (is_press_or_pressed(CT_CAMERA_DOWN))
    {
        transform_rotate_axis(&camera->transform, 1, -rotatation_speed);
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
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_BACK))
    {
        vec3 offset;
        glm_vec_mul(moving_front, (vec3) { -speed, -speed, -speed }, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_LEFT))
    {
        if (get_control_state(CT_STRAFE) == BS_PRESS)
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
        if (get_control_state(CT_STRAFE) == BS_PRESS)
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

void fly_navigation(Camera * camera)
{

}