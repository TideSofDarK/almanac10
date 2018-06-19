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
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

#include "nuklear.h"
#include "nuklear_init.h"
#include "object.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    /* TODO: attempt to fix memory allocation */
    Camera *camera = get_active_camera();
    if (camera == NULL)
        return;
    camera_projection(camera, get_config().fov, (float) width, (float) height);

    config_window_size((unsigned) width, (unsigned) height);
    resize_render_textures(width, height);
}

#ifndef __linux__
void set_window_icon(GLFWwindow *window) {
    int width, height, nr_channels;
    unsigned char *data = stbi_load("assets/icon.png", &width, &height, &nr_channels, STBI_rgb_alpha);

    /* TODO: Free later */
    GLFWimage *image = malloc(sizeof(GLFWimage));
    if (image != NULL) {
        image->height = height;
        image->width = width;
        image->pixels = data;

        glfwSetWindowIcon(window, 1, image);
    }
}
#endif

int main(int argc, char *argv[]) {
    /* Random seed */
    srand((unsigned int) time(NULL));

    /* TODO: configuration save/load */
    init_config();

    /* Create window */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(get_config().w, get_config().h, "", NULL, NULL);
    if (window == NULL) {
        printf("Failed to initialize window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
#ifndef __linux__
    set_window_icon(window);
#endif

    /* Check if GL functions are loaded */
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    /* Create nuklear context */
    struct nk_context *ctx;
    struct nk_colorf bg = {0.509f, 0.705f, 0.2f, 1.0f};
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        nk_glfw3_font_stash_end();
    }
    bg.r = 0.20f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    init_game(window);
    init_editor();
    init_renderers();
    init_input();

    World *world = NULL;
    construct_world(&world, "tatalia");
    insert_world(world, true);

    Object3D *spider = NULL;
    construct_object3d(&spider, get_prop_model("spider"));
    transform_scale(&spider->transform, 0.01f);
    transform_translate_vec3(&spider->transform, (vec3) {0.0f, 0.5f, 0.0f});
    transform_rotate_axis(&spider->transform, 1, 90.0f);

    insert_object3d(world, spider);

    Object3D *spider2 = NULL;
    construct_object3d(&spider2, get_prop_model("spider"));
    transform_scale(&spider2->transform, 0.01f);
    transform_translate_vec3(&spider2->transform, (vec3) {4.0f, 0.5f, 0.0f});
    transform_rotate_axis(&spider2->transform, 1, 90.0f);

    insert_object3d(world, spider2);

    spawn_creature(world, "minotaur", (vec3) {1.0f, 0.0f, 0.0f});
    spawn_creature(world, "minotaur_warrior", (vec3) {-1.0f, 0.0f, 0.0f});
    spawn_creature(world, "black_dragon", (vec3) {0.0f, 0.0f, 0.0f});

    /* Delta time calculations */
    char *title_string;
    while (!glfwWindowShouldClose(window)) {
        update_fps((float) glfwGetTime());
        asprintf(&title_string, "Almanac 10 | FPS: %.1f", get_fps());
        glfwSetWindowTitle(window, title_string);

        glfwPollEvents();
        update_input();

        if (is_pressed(CT_EXIT)) {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        if (is_pressed(CT_EDITOR)) {
            toggle_editor();
        }

        world = get_active_world();
        assert(world != NULL);

        if (get_game_state() == GS_EDITOR) {
            update_editor();
        }
        update_world(world);

        draw_world(world);

        /* UI, Editor, Debug */
        nk_glfw3_new_frame();
        if (get_game_state() == GS_EDITOR) {
            editor_ui(ctx);
        } else {
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