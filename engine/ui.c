#include "ui.h"

#include "vector.h"
#include "config.h"
#include "debug.h"
#include "input.h"

static UIState ui_state;

void tooltip_position(int *start_x, int *start_y, int w, int h, int cx, int cy)
{
    Config config = get_config();

    int hw = w / 2;
    int hh = h / 2;

    *start_x = cx - hw;
    *start_y = cy - hh;

    if (*start_x + w > (int)config.w)
    {
        *start_x = config.w - w;
    }
    else if (*start_x < 0)
    {
        *start_x = 0;
    }

    if (*start_y + h > (int)config.h)
    {
        *start_y = config.h - h;
    }
    else if (*start_y < 0)
    {
        *start_y = 0;
    }
}

void cursor_target_extended_info(struct nk_context *ctx, int cx, int cy, int creature_under_cursor)
{
    World *world = get_active_world();

    int w = 150;
    int h = 250;
    int start_x, start_y;
    tooltip_position(&start_x, &start_y, w, h, cx, cy);
    if (nk_begin(ctx, "Cursor Target Extended Info", nk_rect((float)start_x, (float)start_y, (float)w, (float)h),
                 NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_MOVABLE | NK_WINDOW_BORDER))
    {
        nk_layout_row_begin(ctx, NK_DYNAMIC, (float)h, 1);
        nk_layout_row_push(ctx, 1.0f);
        nk_label(ctx, world->creatures[creature_under_cursor]->name, NK_TEXT_ALIGN_CENTERED);
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}

void cursor_target_info(struct nk_context *ctx, int creature_under_cursor)
{
    World *world = get_active_world();

    if (world->creatures == NULL)
        return;

    Config config = get_config();
    int w = 150;
    int h = 25;
    int y_offset = 10;
    int start_x = config.w / 2 - w / 2;
    int start_y = config.h - h - y_offset;
    if (nk_begin(ctx, "Cursor Target Info", nk_rect((float)start_x, (float)start_y, (float)w, (float)h),
                 NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_begin(ctx, NK_DYNAMIC, (float)h, 1);
        nk_layout_row_push(ctx, 1.0f);

        const char *s = "nothing";
        if (creature_under_cursor != -1)
        {
            s = (int)vector_size(world->creatures) > creature_under_cursor && world->creatures[creature_under_cursor] ? world->creatures[creature_under_cursor]->name : "nothing";
        }

        nk_label(ctx, s, NK_TEXT_ALIGN_CENTERED);
        nk_layout_row_end(ctx);
    }
    nk_end(ctx);
}

void ui(struct nk_context *ctx)
{
    World *world = get_active_world();

    debug_world(ctx);

    if (world->creatures == NULL)
        return;

    cursor_target_info(ctx, ui_state.creature_under_cursor);

    int rmb_press = is_press_or_pressed(CT_RMB);
    float dcx, dcy;
    int cx, cy;
    cursor_position(&dcx, &dcy);
    cx = (int)dcx;
    cy = (int)dcy;

    int valid_sprite = (int)vector_size(world->creatures) - 1 >= ui_state.creature_under_cursor &&
                       world->creatures[ui_state.creature_under_cursor];
    if (rmb_press && valid_sprite && ui_state.creature_under_cursor >= 0)
    {
        cursor_target_extended_info(ctx, cx, cy, ui_state.creature_under_cursor);
        set_cursor_hidden(true);
    }
    else
    {
        set_cursor_hidden(false);
    }
}

void set_creature_under_cursor(int creature_under_cursor)
{
    ui_state.creature_under_cursor = creature_under_cursor;
}