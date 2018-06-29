#include "transform.h"

void init_transform(Transform *transform)
{
    glm_vec_copy(GLM_VEC3_ZERO, transform->pos);
    glm_vec_copy(GLM_VEC3_ZERO, transform->euler);
    glm_vec_copy(GLM_VEC3_ONE, transform->scale);
}

void euler_to_front(vec3 euler, vec3 dest)
{
    float x = cosf(glm_rad(euler[1])) * cosf(glm_rad(euler[0]));
    float y = sinf(glm_rad(euler[1]));
    float z = cosf(glm_rad(euler[1])) * sinf(glm_rad(euler[0]));
    glm_vec_copy((vec3){x, y, z}, dest);
    glm_normalize(dest);
}

void transform_to_mat4(Transform transform, mat4 dest)
{
    glm_mat4_identity(dest);

    mat4 rot = {};
    glm_euler((vec3){glm_rad(transform.euler[0]), glm_rad(transform.euler[1]), glm_rad(transform.euler[2])}, rot);

    glm_translate(dest, transform.pos);
    glm_scale(dest, transform.scale);
    glm_mul_rot(dest, rot, dest);
}

void transform_front(Transform transform, vec3 dest)
{
    euler_to_front(transform.euler, dest);
}

float transform_distance(Transform t1, Transform t2)
{
    return glm_vec_distance(t1.pos, t2.pos);
}

void transform_scale(Transform *transform, float scale)
{
    glm_vec_copy((vec3){scale, scale, scale}, transform->scale);
}

void transform_translate_vec3(Transform *transform, vec3 offset)
{
    glm_vec_add(transform->pos, offset, transform->pos);
}

void transform_translate_axis(Transform *transform, unsigned int i, float offset)
{
    transform->pos[i] += offset;
}

void transform_rotate_axis(Transform *transform, unsigned int i, float offset)
{
    float value = transform->euler[i];
    if (value + offset > 0.0f)
    {
        transform->euler[i] = fmodf(value + offset, 360.0f);
    }
    else
    {
        transform->euler[i] = 360.0f - ((value + offset) * -1.0f);
    }
}
