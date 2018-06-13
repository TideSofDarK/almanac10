#pragma once

typedef struct
{
	unsigned int w, h;
	float fov;
} Config;

void init_config();

void config_window_size(unsigned int, unsigned int);
void config_fov(float);

Config get_config();