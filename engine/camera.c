#include "camera.h"

#include <GLFW/glfw3.h>

#include "config.h"
#include "input.h"

const float *DEFAULT_FRONT = (vec3){0.0f, 0.0f, -1.0f};
const float *DEFAULT_UP = (vec3){0.0f, 1.0f, 0.0f};

void construct_camera(Camera **_camera)
{
    *_camera = malloc(sizeof(Camera));
    Camera *camera = *_camera;

    /* Default transform values */
    init_transform(&camera->transform);

    /* Init position */
    glm_vec_copy((vec3){0.0f, 0.5f, 3.0f}, camera->transform.pos);

    /* Init vectors */
    glm_vec_copy((vec3){270.0f, 0.0f, 0.0f}, camera->transform.euler);
    glm_vec_copy(GLM_VEC3_ONE, camera->transform.scale);

    /* Init view */
    glm_mat4_copy(GLM_MAT4_IDENTITY, camera->view);

    /* Init projection */
    glm_mat4_copy(GLM_MAT4_IDENTITY, camera->projection);
    camera_projection(camera, get_config().fov, (float)get_config().w, (float)get_config().h);
}

void destruct_camera(Camera **_camera)
{
    Camera *camera = *_camera;

    free(*_camera);
    *_camera = NULL;
}

void world_to_screen(Camera *camera, vec3 world, vec3 screen)
{
    vec4 v = {};
    glm_vec4_copy((vec4){0.0f, 0.0f, (float)get_config().w, (float)get_config().h}, v);
    mat4 m = {};
    glm_mat4_mul(camera->projection, camera->view, m);
    glm_project(world, m, v, screen);
    screen[1] = (float)get_config().h - screen[1];
}

void camera_front(Camera *camera, vec3 dest)
{
    vec3 front = {};
    transform_front(camera->transform, front);
    glm_vec_sub(camera->transform.pos, front, dest);
    glm_normalize(dest);
}

void camera_right(Camera *camera, vec3 *dest)
{
    vec3 direction = {};
    camera_front(camera, direction);

    glm_vec_cross((float *)get_default_up(), direction, *dest);
    glm_normalize(*dest);
}

void camera_up(Camera *camera, vec3 *dest)
{
    vec3 direction = {};
    camera_front(camera, direction);

    vec3 right = {};
    camera_right(camera, &right);

    glm_vec_cross(direction, right, *dest);
}

void camera_projection(Camera *camera, float fov, float width, float height)
{
    glm_perspective(glm_rad(fov), width / height, 0.1f, 100.0f, camera->projection);
}

const float *get_default_front()
{
    return DEFAULT_FRONT;
}

const float *get_default_up()
{
    return DEFAULT_UP;
}

void update_camera(Camera *camera)
{
    vec3 front = {};
    euler_to_front(camera->transform.euler, front);
    vec3 camera_center = {};
    glm_vec_add(camera->transform.pos, front, camera_center);
    glm_lookat(camera->transform.pos, camera_center, (float *)get_default_up(), camera->view);
}