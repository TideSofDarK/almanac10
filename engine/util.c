#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

static FPS fps;

/*
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */
int asprintf(char **str, const char *fmt, ...) {
    int size = 0;
    va_list args;
    va_start(args, fmt);
    size = vasprintf(str, fmt, args);
    va_end(args);
    return size;
}

/*
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */
int vasprintf(char **str, const char *fmt, va_list args) {
    int size = 0;
    va_list tmpa;
    va_copy(tmpa, args);
    size = vsnprintf(NULL, size, fmt, tmpa);
    va_end(tmpa);
    if (size < 0) { return -1; }
    *str = (char *) malloc(size + 1);
    if (NULL == *str) { return -1; }
    size = vsprintf(*str, fmt, args);
    return size;
}

char *load_string_from_file(const char *filename) {
    FILE *f;
    fopen_s(&f, filename, "rb");
    char *buf = NULL;
    if (!f) {
        fprintf(stderr, "Could not open text file: %s\n", filename);
        return NULL;
    } else {
        fseek(f, 0, SEEK_END);
        unsigned int len = ftell(f);
        buf = (char *) malloc(1 + len * sizeof(char));
        fseek(f, 0, SEEK_SET);
        if (buf) {
            fread(buf, sizeof(char), len, f);
        } else {
            fprintf(stderr, "Could not allocate memory for string\n");
            return NULL;
        }
        buf[len] = '\0';
    }
    return buf;
}

char *vec3_to_string(vec3 v) {
    char *buffer = NULL;
    asprintf(&buffer, "%f %f %f", v[0], v[1], v[2]);

    return buffer;
}

void update_fps(float time) {
    float current_frame = time;
    fps.delta_time = current_frame - fps.last_frame;
    fps.last_frame = current_frame;

    if ((current_frame - fps.fps_counter) > 1.0 || fps.frames == 0) {
        fps.fps = (float) fps.frames / (current_frame - fps.fps_counter);
        fps.fps_counter = current_frame;
        fps.frames = 0;
    }
    fps.frames++;
}

float get_fps() {
    return fps.fps;
}

float get_delta_time() {
    return fps.delta_time;
}

float intersect_ray_sphere(vec3 p1, vec3 p2, vec3 p3, float r) {
    float x1 = p1[0];
    float y1 = p1[1];
    float z1 = p1[2];
    float x2 = p2[0];
    float y2 = p2[1];
    float z2 = p2[2];
    float x3 = p3[0];
    float y3 = p3[1];
    float z3 = p3[2];

    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    float a = dx * dx + dy * dy + dz * dz;
    float b = 2.0f * (dx * (x1 - x3) + dy * (y1 - y3) + dz * (z1 - z3));
    float c = x3 * x3 + y3 * y3 + z3 * z3 + x1 * x1 + y1 * y1 + z1 * z1 - 2.0f * (x3 * x1 + y3 * y1 + z3 * z1) -
              r * r;

    float test = b * b - 4.0f * a * c;

    return test;
}