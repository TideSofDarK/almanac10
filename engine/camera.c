#include "camera.h"

#include <GLFW/glfw3.h>

#include "config.h"
#include "input.h"

const float* DEFAULT_FRONT	= (vec3) { 0.0f, 0.0f, -1.0f };
const float* DEFAULT_UP		= (vec3) { 0.0f, 1.0f, 0.0f };

Camera create_camera()
{
	Camera camera;

	/* Default transform values */
	camera.transform = create_transform();

	/* Init position */
	glm_vec_copy((vec3) { 0.0f, 0.5f, 3.0f }, camera.transform.pos);

	/* Init vectors */
	glm_vec_copy((vec3) { 270.0f, 0.0f, 0.0f }, camera.transform.euler);
	glm_vec_copy(GLM_VEC3_ONE, camera.transform.scale);

	/* Init view */
	glm_mat4_copy(GLM_MAT4_IDENTITY, camera.view);

	/* Init projection */
	glm_mat4_copy(GLM_MAT4_IDENTITY, camera.projection);
	Config config = get_config();
	camera_projection(&camera, config.fov, (float)config.w, (float)config.h);

	return camera;
}

void camera_direction(Camera* camera, vec3 dest)
{
	vec3 front;
	euler_to_front(camera->transform.euler, front);
	glm_vec_sub(camera->transform.pos, front, dest);
	glm_normalize(dest);
}

void camera_right(Camera* camera, vec3 dest)
{
	vec3 direction;
	camera_direction(camera, direction);

	glm_vec_cross((float*)get_default_up(), direction, dest);
	glm_normalize(dest);
}

void camera_up(Camera* camera, vec3 dest)
{
	vec3 direction;
	camera_direction(camera, direction);

	vec3 right;
	camera_right(camera, right);

	glm_vec_cross(direction, right, dest);
}

void camera_projection(Camera* camera, float fov, float width, float height)
{
	glm_perspective(glm_rad(fov), width / height, 0.1f, 100.0f, camera->projection);
}

const float* get_default_front()
{
	return DEFAULT_FRONT;
}

const float* get_default_up()
{
	return DEFAULT_UP;
}

void update_camera(Camera* camera, float delta_time)
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