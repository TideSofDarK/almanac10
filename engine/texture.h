#pragma once

#include <stdbool.h>

typedef struct
{
	unsigned int ID;
	int w, h, channels;
	char* filename;
	bool loaded;
} Texture;

int free_cached_textures();
void free_cached_texture(Texture**);

void cached_texture(const char*, Texture**);