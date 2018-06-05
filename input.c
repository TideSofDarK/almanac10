#include "input.h"

#include <stdio.h>

#include "config.h"

ButtonState input[GLFW_KEY_LAST];
unsigned int input_map[CT_LAST];

double cursorX, cursorY = 0.0;

void init_input()
{
	for (int i = 0; i < GLFW_KEY_LAST; i++)
	{
		input[i] = BS_NONE;
	}

	/* TODO: Input map save/load */
	input_map[CT_FORWARD] = BTN_DEFAULT_FORWARD;
	input_map[CT_BACK] = BTN_DEFAULT_BACK;
	input_map[CT_LEFT] = BTN_DEFAULT_LEFT;
	input_map[CT_RIGHT] = BTN_DEFAULT_RIGHT;

	input_map[CT_STRAFE] = BTN_DEFAULT_STRAFE;

	input_map[CT_CAMERA_UP] = BTN_DEFAULT_CAMERA_UP;
	input_map[CT_CAMERA_DOWN] = BTN_DEFAULT_CAMERA_DOWN;
	input_map[CT_CAMERA_HOME] = BTN_DEFAULT_CAMERA_HOME;

	input_map[CT_ATTACK] = BTN_DEFAULT_ATTACK;
}

void update_input(GLFWwindow* window)
{
	for (int i = 0; i < (int)CT_LAST; i++)
	{
		int press = glfwGetKey(window, input_map[i]) == GLFW_PRESS;
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

	glfwGetCursorPos(window, &cursorX, &cursorY);
}

ButtonState get_button_state(int btn)
{
	return input[btn];
}

ButtonState get_control_state(ControlType btn)
{
	return get_button_state(input_map[btn]);
}

int is_press_or_pressed(ControlType btn)
{
	return get_control_state(btn) == BS_PRESS || get_control_state(btn) == BS_PRESSED;
}

void cursor_position(float* _cursorX, float* _cursorY)
{
	*_cursorX = (float)cursorX;
	*_cursorY = (float)cursorY;
}

void cursor_raycast(Camera camera, vec3 direction)
{
	float cursorX, cursorY;
	cursor_position(&cursorX, &cursorY);

	float w = (float)get_config().w;
	float h = (float)get_config().h;

	mat4 world_to_camera;
	glm_mat4_mul(camera.projection, camera.view, world_to_camera);
	glm_mat4_inv(world_to_camera, world_to_camera);

	vec4 in;
	glm_vec4_copy((vec4) { ((2.0f*(cursorX / w)) - 1.0f), ((2.0f*(cursorY / h)) - 1.0f) * -1.0f, 1.0f, 1.0f }, in);
	glm_mat4_mulv(world_to_camera, in, in);
	glm_vec4_copy((vec4) { in[0] / in[3], in[1] / in[3], in[2] / in[3], 1.0f / in[3] }, in);
	glm_vec3(in, direction);
	glm_normalize(direction);
}