#pragma once

#include <GLFW/glfw3.h>

#include "camera.h"

#define BTN_DEFAULT_FORWARD GLFW_KEY_W
#define BTN_DEFAULT_BACK GLFW_KEY_S
#define BTN_DEFAULT_LEFT GLFW_KEY_A
#define BTN_DEFAULT_RIGHT GLFW_KEY_D

#define BTN_DEFAULT_STRAFE GLFW_KEY_LEFT_SHIFT

#define BTN_DEFAULT_CAMERA_UP GLFW_KEY_PAGE_UP
#define BTN_DEFAULT_CAMERA_DOWN GLFW_KEY_PAGE_DOWN
#define BTN_DEFAULT_CAMERA_HOME GLFW_KEY_HOME

#define BTN_DEFAULT_ATTACK GLFW_KEY_F

#define BTN_DEFAULT_EDITOR GLFW_KEY_F6

typedef enum
{
	CT_LMB,
	CT_RMB,

	CT_FORWARD,
	CT_BACK,
	CT_LEFT,
	CT_RIGHT,

	CT_STRAFE,

	CT_CAMERA_UP,
	CT_CAMERA_DOWN,
	CT_CAMERA_HOME,

	CT_ATTACK,

	CT_EDITOR,

	CT_LAST
} ControlType;

typedef enum
{
	BS_NONE,
	BS_PRESS,
	BS_PRESSED,
	BS_RELEASED
} ButtonState;

void init_input();

void update_input();

ButtonState get_control_state(ControlType btn);
bool is_press_or_pressed(ControlType btn);
bool is_pressed(ControlType btn);

void set_cursor_hidden(bool);
void cursor_position(float*, float*);

void cursor_raycast(Camera*, vec3);