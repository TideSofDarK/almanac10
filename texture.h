#pragma once

#include <stdbool.h>

#include <glad/glad.h>

typedef struct
{
	GLuint ID;
	int w, h, channels;
	char* filename;
	bool loaded;
} Texture;

int free_cached_textures();
void free_cached_texture(Texture**);

void cached_texture(const char*, Texture**);