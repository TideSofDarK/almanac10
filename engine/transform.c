#include "transform.h"

Transform create_transform()
{
	Transform transform;

	glm_vec_copy(GLM_VEC3_ZERO, transform.pos);
	glm_vec_copy(GLM_VEC3_ZERO, transform.euler);
	glm_vec_copy(GLM_VEC3_ONE, transform.scale);

	return transform;
}

void euler_to_front(vec3 euler, vec3 dest)
{
	float x = (float)cos(glm_rad(euler[1])) * (float)cos(glm_rad(euler[0]));
	float y = (float)sin(glm_rad(euler[1]));
	float z = (float)cos(glm_rad(euler[1])) * (float)sin(glm_rad(euler[0]));
	glm_vec_copy((vec3) { x, y, z }, dest);
	glm_normalize(dest);
}

float transform_distance(Transform t1, Transform t2)
{
	return glm_vec_distance(t1.pos, t2.pos);
}

void translate_pos_vec3(Transform* transform, vec3 offset)
{
	glm_vec_add(transform->pos, offset, transform->pos);
}

void translate_pos_axis(Transform* transform, unsigned int i, float offset)
{
	transform->pos[i] += offset;
}

void translate_euler_axis(Transform* transform, unsigned int i, float offset)
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

