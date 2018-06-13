#pragma once

#include <cglm/cglm.h>

#include "projectile.h"
#include "sprite.h"
#include "shader.h"
#include "model.h"
#include "vertices.h"
#include "world.h"
#include "object.h"

#define SPRITE_RENDERER_SCALE 1.0f
#define MODEL_RENDERER_SCALE 1.0f

typedef struct
{
	unsigned int texture_id, depth_id, FBO;
	float scale;
} FrameBufferRenderData;

typedef struct
{
	RenderData render_data;
	FrameBufferRenderData fb_render_data;
	Shader* shader;
	Shader* render_texture_shader;
} ModelRenderer;

typedef struct
{
	RenderData render_data[2];
	SpriteOriginMode mode;
	FrameBufferRenderData fb_render_data;
	Shader* shader;
} SpriteRenderer;

void init_renderers();
void shutdown_renderers();

void resize_render_textures(int, int);

int get_sprite_under_cursor(World*, int, int);

void draw_creature(Creature*, Camera);
void draw_projectile(Projectile*, Camera);
void draw_object3d(Object3D*, Camera);
void draw_world(World*);

void start_model_rendering();
void finish_model_rendering();

void start_sprite_rendering();
void set_sprite_origin_mode(SpriteOriginMode);
void finish_sprite_rendering();

void display_everything();

void reset_opengl_settings();
void unbind_render_buffers();