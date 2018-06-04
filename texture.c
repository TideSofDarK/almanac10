#include "texture.h"

#include <malloc.h>
#include <string.h>

#include "stb_image.h"

#include "vector.h"
#include "util.h"

/* Global state */
static Texture** cached_textures = NULL;

void load_texture(const char* filename, Texture** _texture)
{
	*_texture = malloc(sizeof(Texture));
	Texture * texture = *_texture;

	texture->filename = _strdup(filename);

	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(filename, &texture->w, &texture->h, &texture->channels, 0);

	texture->ID = 0;
	glGenTextures(1, &texture->ID);
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texture->channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->w, texture->h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (texture->channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->w, texture->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	texture->loaded = true;

	vector_push_back(cached_textures, texture);
}

void free_texture(Texture** _texture)
{
	Texture* texture = *_texture;

	glDeleteTextures(1, &texture->ID);
	free(texture->filename);

	free(*_texture);
	*_texture = NULL;
}

int free_cached_textures()
{
	int count = 0;
	for (int i = 0; i < (int)vector_size(cached_textures); i++)
	{
		Texture* texture = cached_textures[i];
		if (texture != NULL)
		{
			free_texture(&texture);
			count++;
		}
	}
	return count;
}

void free_cached_texture(Texture** _texture)
{
	if (!(*_texture))
		return;

	for (int i = 0; i < (int)vector_size(cached_textures); i++)
	{
		Texture* texture = cached_textures[i];
		if (texture != NULL && strcmp(texture->filename, (*_texture)->filename) == 0)
		{
			glDeleteTextures(1, &texture->ID);
			free(texture->filename);

			free(*_texture);
			*_texture = NULL;

			vector_erase(cached_textures, i);

			break;
		}
	}
}

/* TODO: maybe we should return it instead? */
void cached_texture(const char* filename, Texture** _texture)
{
	for (int i = 0; i < (int)vector_size(cached_textures); i++)
	{
		if (strcmp(cached_textures[i]->filename, filename) == 0)
		{
			*_texture = cached_textures[i];

			return;
		}
	}

	load_texture(filename, _texture);
}