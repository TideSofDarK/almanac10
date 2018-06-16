#include "renderer.h"

#include <glad/glad.h>

#include "shader.h"
#include "vertices.h"
#include "config.h"
#include "grid.h"
#include "object.h"
#include "world.h"
#include "game.h"
#include "terrain.h"
#include "editor.h"
#include "input.h"

/* Global state */
static SpriteRenderer sprite_renderer;
static ModelRenderer model_renderer;

static Model * primitive_sphere = NULL;

RenderData create_quad_render_data(int buffer_size, const float* vertices)
{
	RenderData render_data;

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &render_data.VAO);
	glBindVertexArray(render_data.VAO);

	glGenBuffers(1, &render_data.VBO);
	glBindBuffer(GL_ARRAY_BUFFER,render_data.VBO);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &render_data.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_data.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	return render_data;
}

FrameBufferRenderData create_frame_buffer_render_data(float scale)
{
	FrameBufferRenderData frame_buffer_render_data;
	frame_buffer_render_data.scale = scale;

	glGenFramebuffers(1, &frame_buffer_render_data.FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_render_data.FBO);

	frame_buffer_render_data.texture_id = 0;
	glGenTextures(1, &frame_buffer_render_data.texture_id);
	glBindTexture(GL_TEXTURE_2D, frame_buffer_render_data.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)(get_config().w * scale), (GLsizei)(get_config().h * scale), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	frame_buffer_render_data.depth_id = 0;
	glGenTextures(1, &frame_buffer_render_data.depth_id);
	glBindTexture(GL_TEXTURE_2D, frame_buffer_render_data.depth_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, (GLsizei)(get_config().w * scale), (GLsizei)(get_config().h * scale), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frame_buffer_render_data.texture_id, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, frame_buffer_render_data.depth_id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return frame_buffer_render_data;
}

void init_renderers()
{
	/* Sprite renderer; requires a render data struct for each origin mode */
	construct_shader(&sprite_renderer.shader, "sprite.vert", "sprite.frag", NULL);
	for (SpriteOriginMode i = (SpriteOriginMode)0; i < OM_LAST; i++)
	{
		int buffer_size = 0;
		const float* sprite_vertices = get_sprite_vertices_and_tex_coords(&buffer_size, i);
		sprite_renderer.render_data[i] = create_quad_render_data(buffer_size, sprite_vertices);
	}
	sprite_renderer.fb_render_data = create_frame_buffer_render_data(SPRITE_RENDERER_SCALE);

	/* Model renderer; also includes render texture renderer to merge 2D and 3D buffers */
	construct_shader(&model_renderer.shader, "mesh.vert", "mesh.frag", NULL);
	construct_shader(&model_renderer.render_texture_shader, "render_texture.vert", "render_texture.frag", NULL);
	int buffer_size = 0;
	const float* render_texture_vertices = get_render_texture_vertices_and_tex_coords(&buffer_size);
	model_renderer.render_data = create_quad_render_data(buffer_size, render_texture_vertices); /* Simillar VBO is used for rendering render texture */
	model_renderer.fb_render_data = create_frame_buffer_render_data(MODEL_RENDERER_SCALE);

	construct_model(&primitive_sphere, "assets/models/tools/sphere/", "sphere");

	/* TODO: Remove later */
	if (is_grid_constructed() == 0)
	{
		construct_grid();
	}
}

void shutdown_renderers()
{
	destruct_shader(&sprite_renderer.shader);
	glDeleteTextures(1, &sprite_renderer.fb_render_data.texture_id);
	glDeleteTextures(1, &sprite_renderer.fb_render_data.depth_id);

	destruct_shader(&model_renderer.shader);
	destruct_shader(&model_renderer.render_texture_shader);

	glDeleteTextures(1, &model_renderer.fb_render_data.texture_id);
	glDeleteTextures(1, &model_renderer.fb_render_data.depth_id);
}

void resize_render_textures(int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, sprite_renderer.fb_render_data.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
		(GLsizei)(width * sprite_renderer.fb_render_data.scale),
		(GLsizei)(height * sprite_renderer.fb_render_data.scale),
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, model_renderer.fb_render_data.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
		(GLsizei)(width * model_renderer.fb_render_data.scale),
		(GLsizei)(height * model_renderer.fb_render_data.scale),
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, sprite_renderer.fb_render_data.depth_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		(GLsizei)(width * sprite_renderer.fb_render_data.scale),
		(GLsizei)(height * sprite_renderer.fb_render_data.scale),
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, model_renderer.fb_render_data.depth_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		(GLsizei)(width * model_renderer.fb_render_data.scale),
		(GLsizei)(height * model_renderer.fb_render_data.scale),
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
}

int get_sprite_under_cursor(World* world, int cx, int cy) /* Still, pretty hacky way to do it */
{
	if (!is_cursor_inside_window())
		return -1;

	Camera * camera = NULL;
	active_camera(&camera);
	if (camera == NULL)
		return -1;

	start_sprite_rendering();

	set_sprite_origin_mode(OM_BOTTOM);
	set_uniform_mat4(sprite_renderer.shader, "view", camera->view);
	set_uniform_mat4(sprite_renderer.shader, "projection", camera->projection);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < vector_size(world->creatures); i++)
	{
		Creature* creature = world->creatures[i];
		if (creature != NULL)
		{
			int seed = (int)i + 1;
			int r = (seed & 0x000000FF) >> 0;
			int g = (seed & 0x0000FF00) >> 8;
			int b = (seed & 0x00FF0000) >> 16;
			set_uniform_vec4(sprite_renderer.shader, "solidColor", (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
			draw_creature(creature);
		}
	}

	glReadBuffer(GL_COLOR_ATTACHMENT0);

	unsigned char data[4];
	glReadPixels(cx, (int)get_config().h - cy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	int picked_id =
			(data[0] +
			 data[1] * 256 +
			 data[2] * 256 * 256) - 1;

	//printf("%c %c %c %c %i\n", data[0], data[1], data[2], data[3], picked_id);

	set_uniform_vec4(sprite_renderer.shader, "solidColor", 0.0f, 0.0f, 0.0f, 0.0f);

	finish_sprite_rendering();

	return (int)fmax(picked_id, 0);
}

static inline void draw_sprite(Sprite* sprite, vec3 pos, int invert, int direction)
{
	glBindVertexArray(sprite_renderer.render_data[sprite_renderer.mode].VAO);

	glActiveTexture(GL_TEXTURE0);	
	set_uniform_int(sprite_renderer.shader, "texture1", 0);
	int ID = 0;
	if (!sprite->textures[ANIM_SLOT(sprite->anim_state, direction)])
	{
		/* Fallback to 0 (North) direction */
		ID = sprite->textures[ANIM_SLOT(sprite->anim_state, 0)]->ID;
	}
	else
	{
		ID = sprite->textures[ANIM_SLOT(sprite->anim_state, direction)]->ID;
	}
	glBindTexture(GL_TEXTURE_2D, ID);

	set_uniform_vec3(sprite_renderer.shader, "pos", pos);
	set_uniform_float(sprite_renderer.shader, "aspectRatio", (float)sprite->w / (float)sprite->h);
	set_uniform_int(sprite_renderer.shader, "sheetPosition", sprite->sheet_position);
	set_uniform_int(sprite_renderer.shader, "sheetLength", get_sheet_length(sprite));
	set_uniform_int(sprite_renderer.shader, "sheetInvert", invert);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void draw_creature(Creature* creature)
{
	assert(creature != NULL);

	Transform sprite_transform = creature->transform;

	Camera * camera = NULL;
	active_camera(&camera);
	int index = 0;
	if (camera != NULL)
		index = determine_orientation(creature->transform, camera);

	draw_sprite(creature->sprite, sprite_transform.pos, index > 0, abs(index));
}

void draw_projectile(Projectile* projectile)
{
	assert(projectile != NULL);

	Transform projectile_transform = projectile->transform;

	draw_sprite(projectile->sprite, projectile_transform.pos, 0, 0);
}

static inline void draw_terrain(Terrain * terrain)
{
	mat4 model;
	transform_to_mat4(terrain->transform, model);

	glActiveTexture(GL_TEXTURE0);
	set_uniform_int(model_renderer.shader, "texture_diffuse1", 0);
	/* TODO: Terrain textures */
	glBindTexture(GL_TEXTURE_2D, 7);

	set_uniform_mat4(model_renderer.shader, "model", model);

	glBindVertexArray(terrain->render_data.VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)vector_size(terrain->indices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static inline void draw_sphere(Transform transform, float scale)
{
	mat4 model;
	transform_to_mat4(transform, model);

	glActiveTexture(GL_TEXTURE0);
	set_uniform_int(model_renderer.shader, "texture_diffuse1", 0);
	/* TODO: Terrain textures */
	glBindTexture(GL_TEXTURE_2D, 3);

	set_uniform_mat4(model_renderer.shader, "model", model);

	glBindVertexArray(primitive_sphere->meshes[0]->render_data.VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)vector_size(primitive_sphere->meshes[0]->indices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static inline void draw_mesh(Mesh* mesh)
{
	glActiveTexture(GL_TEXTURE0);
	set_uniform_int(model_renderer.shader, "texture_diffuse1", 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture->ID);

	glBindVertexArray(mesh->render_data.VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)vector_size(mesh->indices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static inline void draw_model(Model* model)
{
	for (unsigned int i = 0; i < vector_size(model->meshes); i++)
	{
		if (model->meshes[i])
		{
			draw_mesh(model->meshes[i]);
		}
	}
}

void draw_object3d(Object3D* object3d)
{
	assert(object3d != NULL);

	mat4 model;
	transform_to_mat4(object3d->transform, model);
	set_uniform_mat4(model_renderer.shader, "model", model);

	draw_model(object3d->model);
}

void draw_world(World* world)
{
	Camera * camera = NULL;
	active_camera(&camera);
	if (camera == NULL)
		return;

	reset_opengl_settings();
	glViewport(0, 0, (GLuint)(get_config().w), (GLuint)(get_config().h));

	/* Draw 2D stuff to buffer; default origin mode is OM_CENTER */
	start_sprite_rendering();

	set_uniform_mat4(sprite_renderer.shader, "view", camera->view);
	set_uniform_mat4(sprite_renderer.shader, "projection", camera->projection);

	set_sprite_origin_mode(OM_CENTER);

	for (size_t i = 0; i < vector_size(world->projectiles); i++)
	{
		draw_projectile(world->projectiles[i]);
	}

	set_sprite_origin_mode(OM_BOTTOM);

	for (size_t i = 0; i < vector_size(world->creatures); i++)
	{
		draw_creature(world->creatures[i]);
	}

	finish_sprite_rendering();

	/* Draw 3D stuff to buffer */
	start_model_rendering();

	/* TODO: Refactor grid things */
	draw_grid();

	use_shader(model_renderer.shader);
	set_uniform_mat4(model_renderer.shader, "view", camera->view);
	set_uniform_mat4(model_renderer.shader, "projection", camera->projection);

	if (world->terrain != NULL)
		draw_terrain(world->terrain);

	for (size_t i = 0; i < vector_size(world->objects3d); i++)
	{
		draw_object3d(world->objects3d[i]);
	}

	/* Draw gizmos */
	if (get_game_state() == GS_EDITOR)
	{
		Gizmo * gizmos = get_gizmos();
		for (size_t i = 0; i < vector_size(gizmos); i++)
		{
			Transform transform;
			init_transform(&transform);
			glm_vec_copy(*gizmos[i].value, transform.pos);
			glm_vec_mul(transform.pos, (vec3){gizmos[i].scale,gizmos[i].scale,gizmos[i].scale}, transform.pos);
			draw_sphere(transform, 1.0f);
		}
	}

	finish_model_rendering();

	/* Merge them using depth buffers */
	display_everything();
}

void start_model_rendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, model_renderer.fb_render_data.FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLuint)(get_config().w * model_renderer.fb_render_data.scale), (GLuint)(get_config().h * model_renderer.fb_render_data.scale));
}

void finish_model_rendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void start_sprite_rendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, sprite_renderer.fb_render_data.FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLuint)(get_config().w * sprite_renderer.fb_render_data.scale), (GLuint)(get_config().h * sprite_renderer.fb_render_data.scale));

	use_shader(sprite_renderer.shader);
}

void set_sprite_origin_mode(SpriteOriginMode origin_mode)
{
	sprite_renderer.mode = origin_mode;
}

void finish_sprite_rendering()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
}

void display_everything()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLuint)(get_config().w), (GLuint)(get_config().h));

	use_shader(model_renderer.render_texture_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model_renderer.fb_render_data.texture_id);
	set_uniform_int(model_renderer.render_texture_shader, "texture0", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sprite_renderer.fb_render_data.texture_id);
	set_uniform_int(model_renderer.render_texture_shader, "texture1", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, model_renderer.fb_render_data.depth_id);
	set_uniform_int(model_renderer.render_texture_shader, "depth0", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sprite_renderer.fb_render_data.depth_id);
	set_uniform_int(model_renderer.render_texture_shader, "depth1", 3);

	set_uniform_float(model_renderer.render_texture_shader, "scale0", model_renderer.fb_render_data.scale);
	set_uniform_float(model_renderer.render_texture_shader, "scale1", sprite_renderer.fb_render_data.scale);

	glBindVertexArray(model_renderer.render_data.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void reset_opengl_settings()
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_TRUE);
	glDepthRange(0, 1);
	glDepthFunc(GL_LEQUAL);

	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glClearStencil(0);
	glClearDepth(1.0f);
	glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void unbind_render_buffers()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}