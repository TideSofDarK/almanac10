#include "config.h"

/* Global state */
static Config config;

const unsigned int DEFAULT_WIDTH = 1280;
const unsigned int DEFAULT_HEIGHT = 720;

const float DEFAULT_FOV = 55.0f;

void init_config() {
    config.w = DEFAULT_WIDTH;
    config.h = DEFAULT_HEIGHT;
    config.fov = DEFAULT_FOV;
}

void config_window_size(unsigned int w, unsigned int h) {
    config.w = w;
    config.h = h;
}

void config_fov(float fov) {
    config.fov = fov;
}

Config get_config() {
    return config;
}