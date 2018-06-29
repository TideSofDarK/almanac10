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
int asprintf(char **str, const char *fmt, ...)
{
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
int vasprintf(char **str, const char *fmt, va_list args)
{
    int size = 0;
    va_list tmpa;
    va_copy(tmpa, args);
    size = vsnprintf(NULL, size, fmt, tmpa);
    va_end(tmpa);
    if (size < 0)
    {
        return -1;
    }
    *str = (char *)malloc(size + 1);
    if (NULL == *str)
    {
        return -1;
    }
    size = vsprintf(*str, fmt, args);
    return size;
}

char *load_string_from_file(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    char *buf = NULL;
    if (!f)
    {
        fprintf(stderr, "Could not open text file: %s\n", filename);
        return NULL;
    }
    else
    {
        fseek(f, 0, SEEK_END);
        unsigned int len = ftell(f);
        buf = (char *)malloc(1 + len * sizeof(char));
        fseek(f, 0, SEEK_SET);
        if (buf)
        {
            fread(buf, sizeof(char), len, f);
        }
        else
        {
            fprintf(stderr, "Could not allocate memory for string\n");
            return NULL;
        }
        buf[len] = '\0';
    }
    return buf;
}

char *vec3_to_string(vec3 v)
{
    char *buffer = NULL;
    asprintf(&buffer, "%f %f %f", v[0], v[1], v[2]);

    return buffer;
}

void update_fps(float time)
{
    float current_frame = time;
    fps.delta_time = current_frame - fps.last_frame;
    fps.last_frame = current_frame;

    if ((current_frame - fps.fps_counter) > 1.0 || fps.frames == 0)
    {
        fps.fps = (float)fps.frames / (current_frame - fps.fps_counter);
        fps.fps_counter = current_frame;
        fps.frames = 0;
    }
    fps.frames++;
}

float get_fps()
{
    return fps.fps;
}

float get_delta_time()
{
    return fps.delta_time;
}

bool intersect_ray_sphere(vec3 r_origin, vec3 r_end, vec3 s_origin, float r)
{
    float x1 = r_origin[0];
    float y1 = r_origin[1];
    float z1 = r_origin[2];
    float x2 = r_end[0];
    float y2 = r_end[1];
    float z2 = r_end[2];
    float x3 = s_origin[0];
    float y3 = s_origin[1];
    float z3 = s_origin[2];

    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;

    float a = dx * dx + dy * dy + dz * dz;
    float b = 2.0f * (dx * (x1 - x3) + dy * (y1 - y3) + dz * (z1 - z3));
    float c = x3 * x3 + y3 * y3 + z3 * z3 + x1 * x1 + y1 * y1 + z1 * z1 - 2.0f * (x3 * x1 + y3 * y1 + z3 * z1) -
              r * r;

    return b * b - 4.0f * a * c >= 0.0f;
}

bool intersect_ray_cylinder(vec3 r_origin, vec3 r_end, vec3 c_origin, vec3 c_end, float c_r)
{
    vec3 r_norm = {};
    glm_vec_sub(r_end, r_origin, r_norm);
    glm_normalize(r_norm);

    vec3 A = {};
    glm_vec_copy(c_origin, A);
    vec3 B = {};
    glm_vec_copy(c_end, B);

    //    Quat.rotateVec3(cap.rotation,A); //Apply Rotation first
    //    Quat.rotateVec3(cap.rotation,B);
    //    A.add(cap.position);	//Then Translation
    //    B.add(cap.position);

    vec3 AB = {};
    glm_vec_sub(B, A, AB);
    vec3 AO = {};
    glm_vec_sub(r_origin, A, AO);
    vec3 AOxAB = {};
    glm_vec_cross(AO, AB, AOxAB);
    vec3 VxAB = {};
    glm_vec_cross(r_norm, AB, VxAB);

    float ab2 = glm_vec_dot(AB, AB);
    float a = glm_vec_dot(VxAB, VxAB);
    float b = 2.0f * glm_vec_dot(VxAB, AOxAB);
    float c = glm_vec_dot(AOxAB, AOxAB) - (c_r * c_r * ab2);
    float d = b * b - 4.0f * a * c;

    if (d < 0)
    {
        return false;
    }

    float t = (-b - sqrtf(d)) / (2.0f * a);
    if (t < 0)
    {
        float aLen2 = glm_vec_distance(A, r_origin);
        float bLen2 = glm_vec_distance(B, r_origin);

        if (aLen2 < bLen2)
        {
            //printf("t is parallel, test top cap\n");
        }
        else
        {
            //printf("t is parallel, test bottom cap\n");
        }

        return false;
    }

    vec3 r_vec = {};
    glm_vec_copy(r_norm, r_vec);
    r_vec[0] = r_vec[0] * t;
    r_vec[1] = r_vec[1] * t;
    r_vec[2] = r_vec[2] * t;

    vec3 intersection = {};
    glm_vec_add(r_vec, r_origin, intersection);

    vec3 intersection_length = {};
    glm_vec_sub(intersection, A, intersection_length);

    float t_limit = glm_vec_dot(intersection_length, AB) / ab2;
    if (t_limit >= 0 && t_limit <= 1)
        return true;
    else if (t_limit < 0)
        return false;
    else if (t_limit > 1)
        return false;

    return false;
}