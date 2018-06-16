#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "stb_image.h"

#include "game.h"
#include "util.h"
#include "world.h"
#include "config.h"
#include "grid.h"
#include "renderer.h"
#include "ui.h"
#include "debug.h"
#include "editor.h"
#include "creature.h"
#include "input.h"
#include "script.h"
#include "terrain.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_init.h"
#include "object.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	/* TODO: attempt to fix memory allocation */
	Camera * camera = NULL;
	active_camera(&camera);
	if (camera == NULL)
		return;
	camera_projection(camera, get_config().fov, (float)width, (float)height);

	config_window_size((unsigned)width, (unsigned)height);
	resize_render_textures(width, height);
}

void set_window_icon(GLFWwindow *window)
{
	int width, height, nr_channels;
	unsigned char *data = stbi_load("assets/icon.png", &width, &height, &nr_channels, STBI_rgb_alpha);

	GLFWimage* image = malloc(sizeof(GLFWimage));
	if (image != NULL)
	{
		image->height = height;
		image->width = width;
		image->pixels = data;

		glfwSetWindowIcon(window, 1, image);
	}
}

int main(int argc, char *argv[])
{
	/* Random seed */
	srand((unsigned int)time(NULL));

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	/* TODO: configuration save/load */
	init_config();

	/* Create window */
	GLFWwindow* window = glfwCreateWindow(get_config().w, get_config().h, "", NULL, NULL);
	if (window == NULL) { printf("Failed to initialize window\n"); glfwTerminate(); return -1; }
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1);
	set_window_icon(window);

	/* Check if GL functions are loaded */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { printf("Failed to initialize GLAD\n"); return -1; }

	/* Create nuklear context */
	struct nk_context *ctx;
	struct nk_colorf bg = { 0.509f, 0.705f, 0.2f, 1.0f };
	ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
	{
		struct nk_font_atlas *atlas;
		nk_glfw3_font_stash_begin(&atlas);
		/*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
		nk_glfw3_font_stash_end();
	}
	bg.r = 0.20f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

	/* Init game structure and add some worlds */
	World* world1 = NULL;
	construct_world(&world1, "tatalia");

	init_game(window, &world1);
	init_renderers();
	init_input();

	Model* model = NULL;
	construct_prop(&model, "spider");

	Object3D * spider = NULL;
	construct_object3d(&spider, model);
	scale_by_float((Transform*)spider, 0.01f);
	translate_pos_vec3((Transform*)spider, (vec3) { 0.0f, 0.5f, 0.0f });
	translate_euler_axis((Transform*)spider, 1, 90.0f);

    insert_object3d(world1, spider);

	spawn_creature(world1, "minotaur", (vec3) { 1.0f, 0.0f, 0.0f });
    spawn_creature(world1, "minotaur_warrior", (vec3) { -1.0f, 0.0f, 0.0f });
    spawn_creature(world1, "black_dragon", (vec3) { 0.0f, 0.0f, 0.0f });

    World* world = NULL;

	/* Delta time calculations */
	char title_string[30];
	while (!glfwWindowShouldClose(window))
	{
		update_fps((float)glfwGetTime());
		sprintf_s(title_string, 30, "Almanac 10 | FPS: %.1f", get_fps());
		glfwSetWindowTitle(window, title_string);

		glfwPollEvents();
        update_input();

        if (is_pressed(CT_EDITOR))
        {
			toggle_editor();
        }

		active_world(&world);
		if (world == NULL)
			break;

		if (get_game_state() == GS_EDITOR)
        {
            update_editor();
        }

		/* Process input and update world */
		update_world(world);
		
		/* Terrain, Props, Sprites,.. */
		draw_world(world);

		/* UI, Editor, Debug */
		nk_glfw3_new_frame();
		if (get_game_state() == GS_EDITOR)
		{
			editor_ui(ctx);
		}
		else
		{
			ui(ctx);
		}
		nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);

		glfwSwapBuffers(window);
	}

	shutdown_nuklear();

	shutdown_renderers();
	shutdown_game();
	shutdown_editor();
	destruct_grid();

	glfwTerminate();

	return 0;
}