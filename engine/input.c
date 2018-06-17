#include "input.h"

#include <stdio.h>

#include "config.h"
#include "game.h"
#include "camera.h"

ButtonState input[GLFW_KEY_LAST];
unsigned int input_map[CT_LAST];

int cx = 0, cy = 0;
bool cursor_inside_window = false;

void init_input()
{
	for (int i = 0; i < GLFW_KEY_LAST; i++)
	{
		input[i] = BS_NONE;
	}

	/* TODO: Input map save/load */
	input_map[CT_LMB] = GLFW_MOUSE_BUTTON_LEFT;
	input_map[CT_RMB] = GLFW_MOUSE_BUTTON_RIGHT;

	input_map[CT_FORWARD] = BTN_DEFAULT_FORWARD;
	input_map[CT_BACK] = BTN_DEFAULT_BACK;
	input_map[CT_LEFT] = BTN_DEFAULT_LEFT;
	input_map[CT_RIGHT] = BTN_DEFAULT_RIGHT;

	input_map[CT_STRAFE] = BTN_DEFAULT_STRAFE;

	input_map[CT_CAMERA_UP] = BTN_DEFAULT_CAMERA_UP;
	input_map[CT_CAMERA_DOWN] = BTN_DEFAULT_CAMERA_DOWN;
	input_map[CT_CAMERA_HOME] = BTN_DEFAULT_CAMERA_HOME;

	input_map[CT_ATTACK] = BTN_DEFAULT_ATTACK;

	input_map[CT_EDITOR] = BTN_DEFAULT_EDITOR;

	input_map[CT_EXIT] = BTN_DEFAULT_EXIT;
}

void update_input()
{
	GLFWwindow * window = get_active_window();
	assert(window != NULL);

	for (int i = 0; i < (int)CT_LAST; i++)
	{
		int press;
		if (i == CT_LMB || i == CT_RMB)
		{
			press = glfwGetMouseButton(window, input_map[i]) == GLFW_PRESS;
		}
		else
		{
			press = glfwGetKey(window, input_map[i]) == GLFW_PRESS;
		}
		if (press)
		{
			if (input[input_map[i]] == BS_NONE)
			{
				input[input_map[i]] = BS_PRESSED;
			}
			else
			{
				input[input_map[i]] = BS_PRESS;
			}
		}
		else if (input[input_map[i]] == BS_PRESS)
		{
			input[input_map[i]] = BS_RELEASED;
		}
		else
		{
			input[input_map[i]] = BS_NONE;
		}
	}

	double dcx, dcy;
	glfwGetCursorPos(window, &dcx, &dcy);
	cx = (int)dcx;
	cy = (int)dcy;

	cursor_inside_window = (get_config().w > cx > 0) && (get_config().h > cy > 0);
}

ButtonState get_button_state(int btn)
{
	return input[btn];
}

ButtonState get_control_state(ControlType btn)
{
	return get_button_state(input_map[btn]);
}

bool is_press_or_pressed(ControlType btn)
{
	return get_control_state(btn) == BS_PRESS || is_pressed(btn);
}

bool is_pressed(ControlType btn)
{
	return get_control_state(btn) == BS_PRESSED;
}

bool is_cursor_inside_window()
{
	return cursor_inside_window;
}

void set_cursor_hidden(bool hidden)
{
	glfwSetInputMode(get_active_window(), GLFW_CURSOR, hidden ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}

void cursor_position(float* _cx, float* _cy)
{
	*_cx = (float)cx;
	*_cy = (float)cy;
}

void cursor_raycast(Camera * camera, vec3 origin, vec3 direction)
{
	float fcx, fcy;
	cursor_position(&fcx, &fcy);

	float w = (float)get_config().w;
	float h = (float)get_config().h;

	mat4 world_to_camera;
	glm_mat4_mul(camera->projection, camera->view, world_to_camera);
	glm_mat4_inv(world_to_camera, world_to_camera);

	vec4 ray_start_ndc = {
			(fcx / w - 0.5f) * 2.0f,
			(fcy / h - 0.5f) * -2.0f,
			-1.0f,
			1.0f
	};
	vec4 ray_end_ndc = {
			(fcx / w - 0.5f) * 2.0f,
			(fcy / h - 0.5f) * -2.0f,
			0.0,
			1.0f
	};

	vec4 ray_start_world;
	glm_mat4_mulv(world_to_camera, ray_start_ndc, ray_start_world);
	glm_vec4_divs(ray_start_world, ray_start_world[3], ray_start_world);

	vec4 ray_end_world;
	glm_mat4_mulv(world_to_camera, ray_end_ndc, ray_end_world);
	glm_vec4_divs(ray_end_world, ray_end_world[3], ray_end_world);

	vec4 d;
	glm_vec4_sub(ray_end_world, ray_start_world, d);
	glm_vec3(d, direction);
	glm_normalize(direction);

	glm_vec3(ray_start_world, origin);
}