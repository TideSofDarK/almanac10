#pragma once

#include <stdbool.h>

typedef struct {
    unsigned int ID;
    int w, h, channels;
    char *filename;
    bool loaded;
} Texture;

int free_precached_textures();

void free_precached_texture(Texture *);

Texture *get_texture(const char *);