#include <math.h>
#include <stdio.h>

#include "../onlay.h"

#define FRAMES 90
#define CANVAS_W 800
#define CANVAS_H 450
#define PI 3.14159265f

enum { WINDOW, BAR, LOGO, TITLE, PILL, SIDEBAR, ENTRY, CONTENT, PANEL, CHIP, DOCK, KEY };

static lay_context ctx;
static int kind[512], tint[512];

static lay_id add(lay_id parent, int k, int t, lay_scalar w, lay_scalar h,
                  uint32_t contain, uint32_t behave)
{
    lay_id it = lay_item(&ctx);

    kind[it] = k;
    tint[it] = t;
    lay_set_size_xy(&ctx, it, w, h);
    lay_set_contain(&ctx, it, contain);
    lay_set_behave(&ctx, it, behave);
    if (parent != LAY_INVALID_ID) lay_insert(&ctx, parent, it);
    return it;
}

static float wave(float t, float cycles)
{
    return 0.5f - 0.5f * cosf(2.0f * PI * cycles * t);
}

static void frame(int f)
{
    static const lay_scalar chip_w[] = { 64, 92, 48, 118, 76, 56, 104, 70,
                                         88, 52, 124, 60, 96, 72, 58, 110 };
    static const lay_scalar entry_h[] = { 26, 26, 36, 26, 26, 36, 26, 26 };
    float t = (float)f / FRAMES;
    lay_scalar win_w = 600.0f + 160.0f * wave(t, 1.0f);
    lay_scalar side_w = 120.0f + 80.0f * wave(t + 0.25f, 2.0f);
    lay_id root, bar, body, side, content, head, grid, dock;
    lay_id i;
    int k;

    lay_reset_context(&ctx);
    root = add(LAY_INVALID_ID, WINDOW, 0, win_w, 410, LAY_COLUMN, 0);
    lay_set_paddings_ltrb(&ctx, root, 12, 12, 12, 12);
    lay_set_gap(&ctx, root, 10);

    bar = add(root, BAR, 0, 0, 40, LAY_ROW, LAY_HFILL);
    lay_set_paddings_ltrb(&ctx, bar, 8, 8, 8, 8);
    lay_set_gap(&ctx, bar, 8);
    add(bar, LOGO, 0, 24, 24, 0, 0);
    add(bar, TITLE, 0, 0, 24, 0, LAY_HFILL);
    for (k = 0; k < 3; k++) add(bar, PILL, k, 52, 24, 0, 0);

    body = add(root, 0, 0, 0, 0, LAY_ROW, LAY_FILL);
    lay_set_gap(&ctx, body, 10);

    side = add(body, SIDEBAR, 0, side_w, 0, LAY_COLUMN | LAY_START, LAY_VFILL);
    lay_set_paddings_ltrb(&ctx, side, 8, 8, 8, 8);
    lay_set_gap(&ctx, side, 6);
    for (k = 0; k < 8; k++) add(side, ENTRY, k == 2, 0, entry_h[k], 0, LAY_HFILL);

    content = add(body, CONTENT, 0, 0, 0, LAY_COLUMN | LAY_START, LAY_FILL);
    lay_set_paddings_ltrb(&ctx, content, 10, 10, 10, 10);
    lay_set_gap(&ctx, content, 10);

    head = add(content, 0, 0, 0, 64, LAY_ROW, LAY_HFILL);
    lay_set_gap(&ctx, head, 10);
    i = add(head, PANEL, 0, 0, 0, 0, LAY_FILL);
    lay_set_weight(&ctx, i, 2.0f);
    add(head, PANEL, 1, 0, 0, 0, LAY_FILL);

    grid = add(content, 0, 0, 0, 0, LAY_ROW | LAY_WRAP | LAY_START, LAY_HFILL);
    lay_set_gap(&ctx, grid, 8);
    for (k = 0; k < (int)(sizeof chip_w / sizeof chip_w[0]); k++)
        add(grid, CHIP, k % 5, chip_w[k], 26, 0, 0);

    dock = add(root, DOCK, 0, 0, 36, LAY_ROW, LAY_HFILL);
    lay_set_paddings_ltrb(&ctx, dock, 6, 6, 6, 6);
    lay_set_gap(&ctx, dock, 6);
    for (k = 0; k < 7; k++) add(dock, KEY, k == 3, 32, 24, 0, 0);

    lay_run_context(&ctx);
    for (i = 0; i < lay_items_count(&ctx); i++) {
        lay_vec4 r = lay_get_rect(&ctx, i);

        if (i && !kind[i]) continue;
        printf("%d %d %d %.2f %.2f %.2f %.2f\n", f, kind[i], tint[i],
               r.v[0] + (CANVAS_W - win_w) / 2.0f, r.v[1] + (CANVAS_H - 410) / 2.0f,
               r.v[2], r.v[3]);
    }
}

int main(void)
{
    int f;

    lay_init_context(&ctx);
    lay_reserve_items_capacity(&ctx, 128);
    for (f = 0; f < FRAMES; f++) frame(f);
    lay_destroy_context(&ctx);
    return 0;
}
