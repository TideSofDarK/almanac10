#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "stb_image.h"

#include "util.h"
#include "world.h"
#include "config.h"
#include "grid.h"
#include "renderer.h"
#include "ui.h"
#include "debug.h"
#include "game.h"
#include "creature.h"
#include "input.h"
#include "script.h"

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		printf("LMB pressed");
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	/* TODO: attempt to fix memory allocation */
	World* world = NULL;
	active_world(&world);
	camera_projection(&world->camera, get_config().fov, (float)width, (float)height);

	config_window_size(width, height);
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
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

	init_game(GS_WORLD, &world1);
	init_renderers();
	init_input();

/*    for (int i = 0; i < 10; i++)
    {
        spawn_creature(world1, minotaur_data, (vec3) {(6.0f / 25.0f * (float)i) - 3.0f, 0.0f, 0.0f });
    }*/

	Model* model = NULL;
	construct_model("props/spider/spider", &model);

	Object3D * spider = NULL;
	construct_object3d(&spider, model);
    insert_object3d(world1, spider);

	translate_pos_vec3((Transform*)spider, (vec3) { 0.0f, 0.5f, 0.0f });

	spawn_creature(world1, "minotaur", (vec3) { 1.0f, 0.0f, 0.0f });
    spawn_creature(world1, "black_dragon", (vec3) { 0.0f, 0.0f, 0.0f });

	/* Delta time calculations */
	float last_frame	= 0.0f;
	float fps_counter	= (float)glfwGetTime();
	int frames			= 0;
	char title_string[30];
	while (!glfwWindowShouldClose(window))
	{
		float current_frame = (float)glfwGetTime();
		float delta_time = current_frame - last_frame;
		last_frame = current_frame;

		if ((current_frame - fps_counter) > 1.0 || frames == 0)
		{
			float fps = (float)frames / (current_frame - fps_counter);
			sprintf_s(title_string, 30, "Almanac 10 | FPS: %.1f", fps);
			glfwSetWindowTitle(window, title_string);
			fps_counter = current_frame;
			frames = 0;
		}
		frames++;

		World* world = NULL;
		active_world(&world);
		if (world == NULL)
			break;

		/* Determine cursor target */
		double cx, cy;
		glfwGetCursorPos(window, &cx, &cy);
		int sprite_under_cursor = get_sprite_under_cursor(world, (int)cx, (int)cy);

		/* Process input and update world */
		glfwPollEvents();
		update_input(window);
		update_world(world, delta_time);
		
		/* Terrain, Props, Sprites,.. */
		draw_world(world);

		/* UI, Debug */
		nk_glfw3_new_frame();
		ui(ctx, window, (UIState)
		{
			sprite_under_cursor
		});
		nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);

		glfwSwapBuffers(window);
	}

	nk_glfw3_shutdown();

	shutdown_renderers();
	shutdown_game();
	destruct_grid();

	glfwTerminate();

	return 0;
}