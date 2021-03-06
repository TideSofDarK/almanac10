#include "debug.h"

#include "config.h"
#include "game.h"
#include "util.h"

void debug_world(struct nk_context *ctx)
{
    World *world = get_active_world();
    Camera *camera = get_active_camera();
    if (world == NULL || camera == NULL)
        return;

    float font_size = 17;
    int w = 300;
    int h = 300;
    int y_offset = 10;
    int start_x = 50;
    int start_y = 50;
    if (nk_begin(ctx, "World Debug", nk_rect((float)start_x, (float)start_y, (float)w, (float)h),
                 NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE))
    {
        /* Player, camera info */
        nk_layout_row_dynamic(ctx, 90, 1);
        if (nk_group_begin(ctx, "Player/Camera",
                           NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_DYNAMIC))
        {
            char *b = vec3_to_string(camera->transform.pos);
            nk_layout_row_dynamic(ctx, font_size, 1);
            nk_label(ctx, b, NK_TEXT_LEFT);
            free(b);

            b = vec3_to_string(camera->transform.euler);
            nk_layout_row_dynamic(ctx, font_size, 1);
            nk_label(ctx, b, NK_TEXT_LEFT);
            free(b);

            nk_group_end(ctx);
        }

        /* World list */
        nk_layout_row_dynamic(ctx, 150, 1);
        if (nk_group_begin(ctx, "Worlds",
                           NK_WINDOW_BORDER | NK_WINDOW_TITLE))
        {
            World **worlds = get_all_worlds();
            for (int i = 0; i < vector_size(worlds); i++)
            {
                if (worlds[i] != NULL)
                {
                    char buf[BUFSIZ];
                    snprintf(buf, sizeof(buf), "%d", i);

                    nk_layout_row_begin(ctx, NK_DYNAMIC, font_size, 4);
                    nk_layout_row_push(ctx, 0.1f);
                    nk_label(ctx, buf, NK_TEXT_ALIGN_LEFT);
                    nk_layout_row_push(ctx, 0.65f);
                    nk_label(ctx, worlds[i]->name, NK_TEXT_ALIGN_LEFT);
                    nk_layout_row_push(ctx, 0.25f);
                    if (nk_button_label(ctx, worlds[i] == world ? "active" : "switch"))
                    {
                        make_world_active(worlds[i]);
                    }
                    //nk_layout_row_push(ctx, 0.3f);
                    //if (nk_button_label(ctx, "destroy"))
                    //{
                    //}
                    nk_layout_row_end(ctx);
                }
            }
            nk_group_end(ctx);
        }

        /* Creature list */
        nk_layout_row_dynamic(ctx, 150, 1);
        if (nk_group_begin(ctx, "Creatures",
                           NK_WINDOW_BORDER | NK_WINDOW_TITLE))
        {
            for (int i = 0; i < vector_size(world->creatures); i++)
            {
                Creature *creature = world->creatures[i];
                if (creature != NULL)
                {
                    char buf[BUFSIZ];
                    snprintf(buf, sizeof(buf), "%d", i);

                    nk_layout_row_begin(ctx, NK_DYNAMIC, font_size, 3);
                    nk_layout_row_push(ctx, 0.1f);
                    nk_label(ctx, buf, NK_TEXT_ALIGN_LEFT);
                    nk_layout_row_push(ctx, 0.7f);
                    nk_label(ctx, creature->name, NK_TEXT_ALIGN_LEFT);
                    nk_layout_row_push(ctx, 0.2f);
                    if (nk_button_label(ctx, "remove"))
                    {
                        vector_push_back(world->creatures_to_remove, i);
                    }
                    nk_layout_row_end(ctx);
                }
            }
            nk_group_end(ctx);
        }
    }
    nk_end(ctx);
}