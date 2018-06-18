#include "editor.h"

#include "config.h"
#include "game.h"
#include "util.h"
#include "input.h"
#include "terrain.h"
#include "world.h"
#include "mesh.h"
#include "camera.h"
#include "transform.h"

#define GIZMO_RAY 8.0f
#define GIZMO_SPHERE_RADIUS 0.1f
#define GIZMO_CYLINDER_RADIUS 0.02f

#define GIZMO_RED (vec4){1.0f,0.0f,0.0f,1.0f}
#define GIZMO_GREEN (vec4){0.0f,1.0f,0.0f,1.0f}
#define GIZMO_BLUE (vec4){0.0f,1.0f,0.0f,1.0f}

#define GIZMO_AXIS_COLOR(a) (vec4){(a) == 0 ? 1.0f : 0.0f, (a) == 1 ? 1.0f : 0.0f, (a) == 2 ? 1.0f : 0.0f, 1.0f}

#define GIZMO_WHITE (vec4){1.0f,1.0f,1.0f,1.0f}

static EditorMode editor_mode = EM_TERRAIN;

static Gizmo *all_gizmos = NULL;
static Gizmo *drag = NULL;
static int selected_gizmo = -1;

static World *world = NULL;
static World *previous_world = NULL;

static inline void clear_gizmos(Gizmo **gizmos) {
    if (gizmos == NULL) return;
    if (*gizmos != NULL) {
        vector_free(*gizmos);
        *gizmos = NULL;
    }
}

static inline void clear_axis_gizmos() {
    /* TODO: Needs better solution */
    vector_pop_back(all_gizmos);
    vector_pop_back(all_gizmos);
    vector_pop_back(all_gizmos);
}

static inline void init_axis_gizmos(Gizmo *gizmo) {
    for (int i = 0; i < 3; ++i) {
        Gizmo axis = {};

        axis.value = &((float *) gizmo->value)[i];
        axis.type = GT_ARROW;

        glm_vec4_copy(GIZMO_AXIS_COLOR(i), axis.color);

        init_transform(&axis.transform);
        glm_vec_copy(gizmo->transform.pos, axis.transform.pos);

        if (i == 0) {
            transform_rotate_axis(&axis.transform, 2, -90.0f);
        } else if (i == 1) {
            transform_rotate_axis(&axis.transform, 1, 90.0f);
        } else if (i == 2) {
            transform_rotate_axis(&axis.transform, 0, 90.0f);
        }

        vector_push_back(all_gizmos, axis);
    }
}

static inline void select_gizmo(int index) {
    /* Unselect previous one */
    if (selected_gizmo != -1) {
        glm_vec4_copy(GIZMO_RED, all_gizmos[selected_gizmo].color);

        clear_axis_gizmos();
    }

    selected_gizmo = index;
    glm_vec4_copy(GIZMO_WHITE, all_gizmos[selected_gizmo].color);
    init_axis_gizmos(&all_gizmos[selected_gizmo]);
}

void shutdown_editor() {
    clear_gizmos(&all_gizmos);

    world = NULL;
    previous_world = NULL;
}

static inline void init_gizmo_position(Gizmo *gizmo, float scale) {
    init_transform(&gizmo->transform);
    glm_vec_copy(*(vec3 *) gizmo->value, gizmo->transform.pos);
    glm_vec_mul(gizmo->transform.pos, (vec3) {scale, scale, scale}, gizmo->transform.pos);
}

static inline void rebuild_terrain_gizmos(Terrain *terrain) {
    clear_gizmos(&all_gizmos);

    for (size_t i = 0; i < vector_size(terrain->vertices); ++i) {
        Gizmo gizmo;

        gizmo.value = terrain->vertices[i].pos;
        gizmo.type = GT_SPHERE;

        glm_vec4_copy(GIZMO_RED, gizmo.color);

        init_gizmo_position(&gizmo, (float) terrain->grid_size);

        vector_push_back(all_gizmos, gizmo);
    }
}

static inline void editor_toolbar(struct nk_context *ctx) {
    Config config = get_config();

    float font_size = 19;
    int w = config.w;
    int h = 32;
    int start_x = 0;
    int start_y = 0;
    if (nk_begin(ctx, "World Editor", nk_rect((float) start_x, (float) start_y, (float) w, (float) h),
                 NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_begin(ctx, NK_STATIC, font_size, 3);
        nk_layout_row_push(ctx, 50);
        if (nk_button_label(ctx, "file")) {

        }
        nk_layout_row_push(ctx, 50);
        if (nk_button_label(ctx, "edit")) {

        }
        nk_layout_row_push(ctx, 50);
        if (nk_button_label(ctx, "view")) {

        }
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}

void editor_ui(struct nk_context *ctx) {
    if (world == NULL)
        return;

    editor_toolbar(ctx);
}

void update_editor() {
    Camera *camera = get_active_camera();
    if (camera == NULL)
        return;

    static float px, py;
    float cx, cy;
    cursor_position(&cx, &cy);

    /* Gizmo drag */
    if (drag != NULL) {
        if (get_control_state(CT_LMB) == BS_RELEASED) {
            select_gizmo(selected_gizmo);

            drag = NULL;
            px = py = 0;
            return;
        } else if ((px == 0 && py == 0)) {
            px = cx;
            py = cy;
        } else {
            float ox = cx - px;
            float oy = py - cy;
            px = cx;
            py = cy;

            const float sensitivity = 0.0005f;
            ox *= sensitivity;
            oy *= sensitivity;

            *(float *) drag->value += ox + oy;
            rebuild_terrain(world->terrain);

            init_gizmo_position(&all_gizmos[selected_gizmo], (float) world->terrain->grid_size);
            select_gizmo(selected_gizmo); /* Rebuild axis gizmos positions */

            glm_vec4_copy(GIZMO_WHITE, drag->color);
        }
    } else if (is_press_or_pressed(CT_LMB)) { /* Gizmos hit test */
        vec3 r_origin = {}, r_dir = {}, r_end = {};
        cursor_raycast(camera, r_origin, r_dir);
        glm_vec_copy(r_origin, r_end);
        glm_vec_muladds(r_dir, GIZMO_RAY, r_end);

        if (get_control_state(CT_LMB) == BS_PRESSED) { /* Single click (selecting) */
            for (size_t i = 0; i < vector_size(all_gizmos); i++) {
                bool exit = false;
                switch (all_gizmos[i].type) {
                    case GT_SPHERE:
                        if (intersect_ray_sphere(r_origin, r_end, all_gizmos[i].transform.pos, GIZMO_SPHERE_RADIUS)) {
                            select_gizmo((int) i);
                            exit = true;
                        }
                        break;
                    default:
                        break;
                }
                if (exit) break;
            }
        } else if (get_control_state(CT_LMB) == BS_PRESS) { /* Axis movement */
            for (size_t i = 0; i < vector_size(all_gizmos); i++) {
                bool exit = false;
                vec3 target = {};
                switch (all_gizmos[i].type) {
                    case GT_ARROW:
                        euler_to_front(all_gizmos[i].transform.euler, target);
                        glm_vec_add(all_gizmos[i].transform.pos, target, target);
                        if (intersect_ray_cylinder(r_origin, r_end, all_gizmos[i].transform.pos, target,
                                                   GIZMO_CYLINDER_RADIUS)) {
                            drag = &all_gizmos[i];
                            exit = true;
                        }
                        break;
                    default:
                        break;
                }
                if (exit) break;
            }
        }
    }

    editor_navigation(camera);
}

void toggle_editor() {
    if (get_game_state() != GS_EDITOR) {
        previous_world = get_active_world();

        if (world == NULL) {
            construct_world(&world, "new_world");
            construct_terrain(&world->terrain, 16);

            insert_world(world, true);
        } else {
            make_world_active(world);
        }

        if (editor_mode == EM_TERRAIN) {
            rebuild_terrain_gizmos(world->terrain);
        }
    } else {
        make_world_active(previous_world);
    }

    set_game_state(get_game_state() == GS_WORLD ? GS_EDITOR : GS_WORLD);
}

void editor_navigation(Camera *camera) {
    float delta_time = get_delta_time();

    static float px, py;

    float rotatation_speed = 96.0f * delta_time;
    float speed = 5.0f * delta_time;

    float cx, cy;
    cursor_position(&cx, &cy);

    bool pressed = is_press_or_pressed(CT_RMB);

    /* Camera pitch */
    if ((px == 0 && py == 0) || !pressed) {
        px = cx;
        py = cy;
    } else {
        float ox = cx - px;
        float oy = py - cy;
        px = cx;
        py = cy;

        float sensitivity = 0.1f;
        ox *= sensitivity;
        oy *= sensitivity;

        transform_rotate_axis(&camera->transform, 0, ox);
        transform_rotate_axis(&camera->transform, 1, oy);
    }

    vec3 front = {};
    euler_to_front(camera->transform.euler, front);

    /* WASD */
    if (is_press_or_pressed(CT_FORWARD)) {
        vec3 offset = {};
        glm_vec_mul(front, (vec3) {speed, speed, speed}, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_BACK)) {
        vec3 offset = {};
        glm_vec_mul(front, (vec3) {-speed, -speed, -speed}, offset);
        transform_translate_vec3(&camera->transform, offset);
    }
    if (is_press_or_pressed(CT_LEFT)) {
        if (get_control_state(CT_STRAFE) == BS_NONE) {
            vec3 offset = {};
            glm_vec_cross(front, (float *) get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3) {-speed, -speed, -speed}, offset);
            transform_translate_vec3(&camera->transform, offset);
        } else {
            // Yaw
            transform_rotate_axis(&camera->transform, 0, -rotatation_speed);
        }
    }
    if (is_press_or_pressed(CT_RIGHT)) {
        if (get_control_state(CT_STRAFE) == BS_NONE) {
            vec3 offset = {};
            glm_vec_cross(front, (float *) get_default_up(), offset);
            glm_normalize(offset);
            glm_vec_mul(offset, (vec3) {speed, speed, speed}, offset);
            transform_translate_vec3(&camera->transform, offset);
        } else {
            // Yaw
            transform_rotate_axis(&camera->transform, 0, rotatation_speed);
        }
    }
}

Gizmo *get_gizmos() {
    return all_gizmos;
}