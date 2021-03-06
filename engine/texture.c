#include "texture.h"

#include <malloc.h>
#include <string.h>

#include <glad/glad.h>
#include "stb_image.h"

#include "vector.h"

/* Global state */
static Texture **precached_textures = NULL;

void construct_texture(const char *filename, Texture **_texture)
{
    *_texture = malloc(sizeof(Texture));
    Texture *texture = *_texture;

    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filename, &texture->w, &texture->h, &texture->channels, 0);

    if (data == NULL)
    {
        free(texture);
        *_texture = NULL;
        return;
    }

    texture->filename = strdup(filename);

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

    vector_push_back(precached_textures, texture);
}

void destruct_texture(Texture **_texture)
{
    Texture *texture = *_texture;

    printf("[Engine] Freeing texture: %s\n", texture->filename);

    glDeleteTextures(1, &texture->ID);
    free(texture->filename);

    free(*_texture);
    *_texture = NULL;
}

int free_precached_textures()
{
    int count = 0;
    for (int i = 0; i < (int)vector_size(precached_textures); i++)
    {
        Texture *texture = precached_textures[i];
        if (texture != NULL)
        {
            destruct_texture(&texture);
            count++;
        }
    }
    return count;
}

void free_precached_texture(Texture *texture)
{
    if (texture == NULL)
        return;

    for (int i = 0; i < (int)vector_size(precached_textures); i++)
    {
        if (strcmp(texture->filename, precached_textures[i]->filename) == 0)
        {
            destruct_texture(&texture);

            vector_erase(precached_textures, i);

            break;
        }
    }
}

/* TODO: maybe we should return it instead? */
Texture *get_texture(const char *filename)
{
    for (int i = 0; i < (int)vector_size(precached_textures); i++)
    {
        if (strcmp(precached_textures[i]->filename, filename) == 0)
        {
            return precached_textures[i];
        }
    }

    Texture *texture = NULL;
    construct_texture(filename, &texture);

    return texture;
}