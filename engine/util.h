#pragma once

#include <cglm/cglm.h>

typedef struct {
    float delta_time;
    float last_frame;
    float fps_counter;
    int frames;
    float fps;
} FPS;

int asprintf(char **, const char *, ...);

int vasprintf(char **, const char *, va_list);

char *load_string_from_file(const char *);

char *vec3_to_string(vec3);

void update_fps(float);

float get_fps();

float get_delta_time();