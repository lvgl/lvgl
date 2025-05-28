/**
 * @file lv_demo_render.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_render.h"

#if LV_USE_DEMO_RENDER

#include "../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

#define COL_CNT 8
#define ROW_CNT 8
#define DEF_WIDTH   55
#define DEF_HEIGHT  30

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const char * name;
    void (*create_cb)(lv_obj_t * parent);
} scene_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_opa_t opa_saved;
static void add_to_cell(lv_obj_t * obj, int32_t col, int32_t row);

static lv_obj_t * fill_obj_create(lv_obj_t * parent, int32_t col, int32_t row)
{
    lv_color_t colors[] = {lv_color_hex3(0x000),
                           lv_color_hex3(0xfff),
                           lv_color_hex3(0xf00),
                           lv_color_hex3(0x0f0),
                           lv_color_hex3(0x00f),
                           lv_color_hex3(0xff0),
                           lv_color_hex3(0x0ff),
                           lv_color_hex3(0xf0f),
                          };

    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, colors[col], 0);
    lv_obj_set_size(obj, DEF_WIDTH, DEF_HEIGHT);
    add_to_cell(obj, col, row);

    return obj;

}

static void fill_cb(lv_obj_t * parent)
{

    uint32_t i;
    for(i = 0; i < COL_CNT; i++) {
        fill_obj_create(parent, i, 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = fill_obj_create(parent, i, 1);
        lv_obj_set_style_radius(obj, 10, 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = fill_obj_create(parent, i, 2);
        lv_obj_set_style_radius(obj, 100, 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = fill_obj_create(parent, i, 3);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, 0);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0x888), 0);
        lv_obj_set_style_bg_grad_stop(obj, 200, 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = fill_obj_create(parent, i, 4);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0x888), 0);
        lv_obj_set_style_bg_grad_stop(obj, 200, 0);
    }

    for(i = 0; i < COL_CNT; i++) {

        lv_obj_t * obj = fill_obj_create(parent, i, 5);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, 0);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0x888), 0);
        lv_obj_set_style_bg_grad_opa(obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_grad_stop(obj, 200, 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = fill_obj_create(parent, i, 6);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0x888), 0);
        lv_obj_set_style_bg_grad_opa(obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_bg_grad_stop(obj, 200, 0);
    }
}

static lv_obj_t * border_obj_create(lv_obj_t * parent, int32_t col, int32_t row)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_border_color(obj, lv_color_hex3(0x000), 0);
    lv_obj_set_style_border_width(obj, 3, 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    lv_obj_set_size(obj, DEF_WIDTH, DEF_HEIGHT);
    add_to_cell(obj, col, row);

    return obj;

}

static void border_cb(lv_obj_t * parent)
{
    lv_border_side_t sides[] = {
        LV_BORDER_SIDE_NONE,
        LV_BORDER_SIDE_FULL,
        LV_BORDER_SIDE_LEFT,
        LV_BORDER_SIDE_RIGHT,
        LV_BORDER_SIDE_TOP,
        LV_BORDER_SIDE_BOTTOM,
        LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_RIGHT,
        LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM,
        LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM,
        LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP,
        LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT,
        LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM,
        LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_BOTTOM,
        LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_TOP,
        LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_TOP,
        LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT | LV_BORDER_SIDE_TOP,
    };

    uint32_t i;
    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 0);
        lv_obj_set_style_radius(obj, 0, 0);
        lv_obj_set_style_border_side(obj, sides[i], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0xf00), 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 1);
        lv_obj_set_style_radius(obj, 0, 0);
        lv_obj_set_style_border_side(obj, sides[i + 8], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0xf00), 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 2);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_border_side(obj, sides[i], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0x0f0), 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 3);
        lv_obj_set_style_radius(obj, 10, 0);
        lv_obj_set_style_border_side(obj, sides[i + 8], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0x0f0), 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 4);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_border_side(obj, sides[i], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0x00f), 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 5);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_border_side(obj, sides[i + 8], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0x00f), 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 6);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_border_side(obj, sides[i], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0x888), 0);
        lv_obj_set_style_border_width(obj, 10, 0);
    }

    for(i = 0; i < COL_CNT; i++) {
        lv_obj_t * obj = border_obj_create(parent, i, 7);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_border_side(obj, sides[i + 8], 0);
        lv_obj_set_style_border_color(obj, lv_color_hex3(0x888), 0);
        lv_obj_set_style_border_width(obj, 10, 0);
    }
}

static lv_obj_t * box_shadow_obj_create(lv_obj_t * parent, int32_t col, int32_t row)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_20, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_shadow_color(obj, lv_color_hex3(0xf00), 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    lv_obj_set_size(obj, DEF_WIDTH - 20, DEF_HEIGHT - 5);
    add_to_cell(obj, col, row);

    return obj;
}

static void box_shadow_cb(lv_obj_t * parent)
{

    static const int32_t grid_rows[] = {45, 45, 45, 45, 45, 45, LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_cols[] = {68, 68, 68, 68, 68, 68, 68, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_cols, grid_rows);

    lv_point_t ofs[] = {
        {0, 0},
        {5, 5},
        {5, -5},
        {-5, 5},
        {-5, -5},
        {10, 0},
        {0, 10},
    };

    uint32_t i;
    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 0);
        lv_obj_set_style_radius(obj, 0, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_offset_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_offset_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 1);
        lv_obj_set_style_radius(obj, 5, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_offset_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_offset_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 2);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_offset_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_offset_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 3);
        lv_obj_set_style_radius(obj, 5, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_spread(obj, 3, 0);
        lv_obj_set_style_shadow_offset_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_offset_y(obj, ofs[i].y, 0);
    }
}

static lv_obj_t * text_obj_create(lv_obj_t * parent, int32_t col, int32_t row)
{

    lv_obj_t * obj = lv_label_create(parent);
    lv_obj_remove_style_all(obj);
    lv_label_set_text(obj, "Hello LVGL! It should be a placeholder: ű. Looks good?");
    lv_obj_set_style_opa(obj, opa_saved, 0);
    add_to_cell(obj, col, row);

    return obj;

}

static void text_cb(lv_obj_t * parent)
{
    lv_obj_t * obj;

    obj = text_obj_create(parent, 3, 0);

    obj = text_obj_create(parent, 3, 1);
    lv_obj_set_style_text_color(obj, lv_color_hex3(0xff0), 0);

    obj = text_obj_create(parent, 3, 2);
    lv_label_set_text_selection_start(obj, 12);
    lv_label_set_text_selection_end(obj, 21);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0x0ff), LV_PART_SELECTED);

    obj = text_obj_create(parent, 3, 3);
    lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_UNDERLINE, 0);

    obj = text_obj_create(parent, 3, 4);
    lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_STRIKETHROUGH, 0);

    obj = text_obj_create(parent, 3, 5);
    lv_obj_set_style_text_decor(obj, LV_TEXT_DECOR_UNDERLINE | LV_TEXT_DECOR_STRIKETHROUGH, 0);

}

static lv_obj_t * image_obj_create(lv_obj_t * parent, int32_t col, int32_t row, bool recolor)
{
    lv_obj_t * obj = lv_image_create(parent);
    lv_obj_remove_style_all(obj);
    if(recolor) {
        lv_obj_set_style_image_recolor_opa(obj, LV_OPA_50, 0);
        lv_obj_set_style_image_recolor(obj, lv_color_hex3(0x0f0), 0);
    }

    lv_obj_set_style_opa(obj, opa_saved, 0);
    add_to_cell(obj, col, row);

    return obj;

}
#include <stdio.h>

static void image_core_cb(lv_obj_t * parent, bool recolor, uint32_t startAt)
{
    LV_IMAGE_DECLARE(img_render_lvgl_logo_xrgb8888);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_rgb888);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_rgb565);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_rgb565_swapped);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_argb8888);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_argb8888_premultiplied);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_l8);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_i1);
    LV_IMAGE_DECLARE(img_render_lvgl_logo_rgb565a8);

    const void * srcs[] = {
        &img_render_lvgl_logo_argb8888,
        &img_render_lvgl_logo_argb8888_premultiplied,
        &img_render_lvgl_logo_xrgb8888,
        &img_render_lvgl_logo_rgb888,
        &img_render_lvgl_logo_rgb565,
        &img_render_lvgl_logo_rgb565_swapped,
        &img_render_lvgl_logo_l8,
        NULL,
        &img_render_lvgl_logo_rgb565a8,
        &img_render_lvgl_logo_i1,
    };

    const void * names[] = {
        "ARGB\n8888",
        "ARGB\n8888\nPM",
        "XRGB\n8888",
        "RGB\n888",
        "RGB\n565",
        "RGB\n565\nSWAP",
        "L8",
        "", /*Make sure that RGB565A8 and I1 are on the same page.
              Both are disabled in VGLite as they are not supported*/
        "RGB\n565A8",
        "I1",
    };

    uint32_t stopAt = startAt + LV_MIN(sizeof(srcs) / sizeof(void *) - startAt, 4);
    uint32_t i;
    for(i = startAt; i < stopAt; i++) {
        lv_obj_t * obj;
        uint32_t row = i - startAt;

        obj = lv_label_create(parent);
        lv_label_set_text(obj, names[i]);
        add_to_cell(obj, 0, row * 2);

        obj = image_obj_create(parent, 1, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);

        obj = image_obj_create(parent, 2, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_rotation(obj, 300);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 3, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 4, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale_x(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 5, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale_y(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 6, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_rotation(obj, 300);
        lv_image_set_scale(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 7, row * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale_y(obj, 400);
        lv_image_set_rotation(obj, 300);
        lv_image_set_pivot(obj, 0, 0);
    }
}

static void image_normal_1_cb(lv_obj_t * parent)
{
    image_core_cb(parent, false, 0);
}

static void image_recolored_1_cb(lv_obj_t * parent)
{
    image_core_cb(parent, true, 0);
}

static void image_normal_2_cb(lv_obj_t * parent)
{
    image_core_cb(parent, false, 4);
}

static void image_recolored_2_cb(lv_obj_t * parent)
{
    image_core_cb(parent, true, 4);
}

static void image_normal_3_cb(lv_obj_t * parent)
{
    image_core_cb(parent, false, 8);
}

static void image_recolored_3_cb(lv_obj_t * parent)
{
    image_core_cb(parent, true, 8);
}

static lv_obj_t * line_obj_create(lv_obj_t * parent, int32_t col, int32_t row, lv_point_precise_t p[])
{
    lv_obj_t * obj = lv_line_create(parent);
    lv_obj_remove_style_all(obj);
    lv_line_set_points(obj, p, 2);
    lv_obj_set_size(obj, DEF_WIDTH, DEF_HEIGHT);
    lv_obj_set_style_line_color(obj, lv_color_hex3(0xff0), 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    add_to_cell(obj, col, row);

    return obj;
}

static void line_cb(lv_obj_t * parent)
{

    static lv_point_precise_t points[][2] = {
        {{5, DEF_HEIGHT / 2}, {DEF_WIDTH - 5, DEF_HEIGHT / 2}}, /* - */
        {{5, DEF_HEIGHT / 2}, {DEF_WIDTH - 5, DEF_HEIGHT / 2 + 1}}, /* - */
        {{5, DEF_HEIGHT / 2}, {DEF_WIDTH - 5, DEF_HEIGHT / 2 - 1}}, /* - */
        {{DEF_WIDTH / 2, 5}, {DEF_WIDTH / 2, DEF_HEIGHT - 5}},  /* | */
        {{DEF_WIDTH / 2, 5}, {DEF_WIDTH / 2 + 1, DEF_HEIGHT - 5}},  /* | */
        {{DEF_WIDTH / 2, 5}, {DEF_WIDTH / 2 - 1, DEF_HEIGHT - 5}},  /* | */
        {{5, 5}, {DEF_WIDTH - 5, DEF_HEIGHT - 5}},               /* \ */
        {{DEF_WIDTH - 5, 5}, {5, DEF_HEIGHT - 5}},               /* / */
    };

    int32_t widths[] = {1, 3, 5, 10};

    uint32_t r;
    for(r = 0; r < 2; r++) {
        uint32_t w;
        for(w = 0; w < 4; w++) {
            uint32_t i;
            for(i = 0; i < COL_CNT; i++) {
                lv_obj_t * obj = line_obj_create(parent, i, w + 4 * r, points[i]);
                lv_obj_set_style_line_width(obj, widths[w], 0);
                lv_obj_set_style_line_rounded(obj, r, 0);
            }
        }
    }
}

static lv_obj_t * arc_obj_create(lv_obj_t * parent, int32_t col, int32_t row, int32_t w,
                                 lv_value_precise_t start, lv_value_precise_t end)
{
    lv_obj_t * obj = lv_arc_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_arc_width(obj, w, 0);
    lv_obj_set_style_arc_color(obj, lv_color_white(), 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    lv_arc_set_bg_angles(obj, start, end);
    lv_obj_set_size(obj, DEF_HEIGHT, DEF_HEIGHT);
    lv_obj_set_style_line_color(obj, lv_color_hex3(0xff0), 0);
    add_to_cell(obj, col, row);

    return obj;
}

static void arc_core_cb(lv_obj_t * parent, const void * img_src)
{
    static lv_value_precise_t angles[][2] = {
        {355, 5},
        {85, 95},
        {175, 185},
        {265, 275},
        {30, 330},
        {120, 60},
        {0, 180},
        {0, 360},
    };

    int32_t widths[] = {1, 5, 10, 100};

    uint32_t r;
    for(r = 0; r < 2; r++) {
        uint32_t w;
        for(w = 0; w < 4; w++) {
            uint32_t i;
            for(i = 0; i < COL_CNT; i++) {
                lv_obj_t * obj = arc_obj_create(parent, i, w + 4 * r, widths[w], angles[i][0], angles[i][1]);
                lv_obj_set_style_arc_rounded(obj, r, 0);
                lv_obj_set_style_arc_image_src(obj, img_src, 0);
            }
        }
    }
}

static void arc_normal_cb(lv_obj_t * parent)
{
    arc_core_cb(parent, NULL);
}

static void arc_image_cb(lv_obj_t * parent)
{
    LV_IMAGE_DECLARE(img_render_arc_bg);
    arc_core_cb(parent, &img_render_arc_bg);
}

static void triangle_draw_event_cb(lv_event_t * e)
{
    lv_draw_triangle_dsc_t dsc;
    lv_draw_triangle_dsc_init(&dsc);

    lv_obj_t * obj = lv_event_get_target(e);

    lv_point_t * p_rel = lv_event_get_user_data(e);

    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);
    dsc.p[0].x = p_rel[0].x + coords.x1 + 8;
    dsc.p[0].y = p_rel[0].y + coords.y1 + 2;
    dsc.p[1].x = p_rel[1].x + coords.x1 + 8;
    dsc.p[1].y = p_rel[1].y + coords.y1 + 2;
    dsc.p[2].x = p_rel[2].x + coords.x1 + 8;
    dsc.p[2].y = p_rel[2].y + coords.y1 + 2;

    lv_opa_t opa = lv_obj_get_style_opa(obj, 0);
    dsc.grad.dir = lv_obj_get_style_bg_grad_dir(obj, 0);
    dsc.grad.stops[0].color = lv_obj_get_style_bg_color(obj, 0);
    dsc.grad.stops[0].frac = lv_obj_get_style_bg_main_stop(obj, 0);
    dsc.grad.stops[0].opa = LV_OPA_MIX2(lv_obj_get_style_bg_main_opa(obj, 0), opa);
    dsc.grad.stops[1].color = lv_obj_get_style_bg_grad_color(obj, 0);
    dsc.grad.stops[1].frac = lv_obj_get_style_bg_grad_stop(obj, 0);
    dsc.grad.stops[1].opa = LV_OPA_MIX2(lv_obj_get_style_bg_grad_opa(obj, 0), opa);
    dsc.grad.stops_count = 2;

    dsc.color = dsc.grad.stops[0].color;
    dsc.opa = dsc.grad.stops[0].opa;

    lv_draw_triangle(lv_event_get_layer(e), &dsc);
}

static lv_obj_t * triangle_obj_create(lv_obj_t * parent, int32_t col, int32_t row, lv_point_t p[])
{
    lv_obj_t * obj = lv_arc_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, DEF_WIDTH, DEF_HEIGHT);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0xff0), 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    lv_obj_add_event_cb(obj, triangle_draw_event_cb, LV_EVENT_DRAW_MAIN, p);
    add_to_cell(obj, col, row);

    return obj;
}

static void triangle_cb(lv_obj_t * parent)
{
    static lv_point_t points[16][3] = {

        /*Right angle triangles*/
        {{0, 0},  {0, 26}, {26, 26}},   /* |\ */
        {{0, 26}, {26, 0}, {26, 26}},   /* /| */
        {{0, 0},  {26, 0}, {26, 26}},   /* \| */
        {{0, 0},  {0, 26}, {26, 0}},    /* |/ */

        /*One side vertical or horizontal triangles*/
        {{0, 0},  {26, 0}, {13, 26}},   /* \/ */
        {{0, 26}, {26, 26}, {13, 0 }},  /* /\ */
        {{0, 0},  {0, 26}, {26, 13}},   /* > */
        {{0, 13}, {26, 0}, {26, 26}},   /* < */

        /*Thin triangles*/
        {{0, 0},  {26, 26}, {13, 18}},  /* \ */
        {{0, 0},  {26, 26}, {13, 8}},   /* \ */
        {{26, 0},  {0, 26}, {13, 18}},  /* / */
        {{26, 0},  {0, 26}, {13, 8}},   /* / */

        /*General triangles with various point orders*/
        {{0, 1},  {26, 6},  {13, 26}},  /*ABC*/
        {{0, 1},  {13, 26}, {26, 6}},   /*ACB*/
        {{26, 6}, {0, 1},   {13, 26}},  /*BAC*/
        {{13, 26}, {26, 6},  {0, 1}},   /*CBA*/
    };

    uint32_t i;
    for(i = 0; i < 16; i++) {
        triangle_obj_create(parent, i % 8, i / 8, points[i]);
    }

    for(i = 0; i < 16; i++) {
        lv_obj_t * obj = triangle_obj_create(parent, i % 8, 2 + i / 8, points[i]);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0xf00), 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
    }

    for(i = 0; i < 16; i++) {
        lv_obj_t * obj = triangle_obj_create(parent, i % 8, 4 + i / 8, points[i]);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0xf00), 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, 0);
    }

    for(i = 0; i < 8; i++) {
        lv_obj_t * obj = triangle_obj_create(parent, i % 8, 6 + i / 8, points[i]);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0xf00), 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_bg_grad_opa(obj, LV_OPA_TRANSP, 0);
    }

    for(i = 0; i < 8; i++) {
        lv_obj_t * obj = triangle_obj_create(parent, i % 8, 7 + i / 8, points[i]);
        lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0xf00), 0);
        lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_HOR, 0);
        lv_obj_set_style_bg_grad_opa(obj, LV_OPA_TRANSP, 0);
    }
}

static lv_obj_t * layer_obj_create(lv_obj_t * parent, int32_t col, int32_t row, lv_blend_mode_t blend_mode)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, DEF_WIDTH - 10, DEF_HEIGHT);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0xff0), 0);
    lv_obj_set_style_bg_grad_color(obj, lv_color_hex3(0xf00), 0);
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_width(obj, 3, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex3(0x000), 0);
    lv_obj_set_style_transform_pivot_x(obj, 0, 0);
    lv_obj_set_style_transform_pivot_y(obj, 0, 0);
    lv_obj_set_style_blend_mode(obj, blend_mode, 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    add_to_cell(obj, col, row);

    lv_obj_t * label = lv_label_create(obj);
    lv_label_set_text(label, "ABC");
    lv_obj_center(label);

    return obj;
}

static void layer_core_cb(lv_obj_t * parent, lv_blend_mode_t blend_mode)
{

    uint32_t i;
    for(i = 0; i < 2; i++) {
        int32_t row = 4 * i;
        lv_obj_t * obj;

        obj = layer_obj_create(parent, 0, row, blend_mode);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 1, row, blend_mode);
        lv_obj_set_style_transform_rotation(obj, 300, 0);
        lv_obj_set_style_translate_x(obj, 10, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 2, row, blend_mode);
        lv_obj_set_style_transform_scale(obj, 400, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 4, row, blend_mode);
        lv_obj_set_style_transform_rotation(obj, 300, 0);
        lv_obj_set_style_transform_scale(obj, 400, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 5, row, blend_mode);
        lv_obj_set_style_transform_scale_x(obj, 400, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 7, row, blend_mode);
        lv_obj_set_style_transform_scale_y(obj, 400, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 0, row + 2, blend_mode);
        lv_obj_set_style_transform_rotation(obj, 300, 0);
        lv_obj_set_style_transform_scale_x(obj, 400, 0);
        lv_obj_set_style_translate_x(obj, 10, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 2, row + 2, blend_mode);
        lv_obj_set_style_transform_rotation(obj, 300, 0);
        lv_obj_set_style_transform_scale_y(obj, 400, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 5, row + 2, blend_mode);
        lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);
        lv_obj_set_style_translate_y(obj, 10, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);

        obj = layer_obj_create(parent, 7, row + 2, blend_mode);
        lv_obj_set_style_opa_layered(obj, LV_OPA_50, 0);
        lv_obj_set_style_transform_rotation(obj, 300, 0);
        lv_obj_set_style_translate_y(obj, 10, 0);
        lv_obj_set_style_radius(obj, 8 * i, 0);
    }
}

static void layer_normal_cb(lv_obj_t * parent)
{
    layer_core_cb(parent, LV_BLEND_MODE_NORMAL);
}

static void create_blend_mode_image_buffer(lv_obj_t * canvas)
{
    lv_canvas_fill_bg(canvas, lv_color_hex3(0x844), LV_OPA_COVER);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.color = lv_color_hex(0xff0000);
    dsc.text = "R";

    lv_area_t coords = {0, 0, 100, 60};

    lv_draw_label(&layer, &dsc, &coords);
    dsc.color = lv_color_hex(0x00ff00);
    dsc.text = "G";
    coords.x1 = 11;
    lv_draw_label(&layer, &dsc, &coords);

    dsc.color = lv_color_hex(0x0000ff);
    dsc.text = "B";
    coords.x1 = 23;
    lv_draw_label(&layer, &dsc, &coords);

    dsc.color = lv_color_hex(0xffffff);
    dsc.text = "W";
    coords.y1 = 14;
    coords.x1 = 4;
    lv_draw_label(&layer, &dsc, &coords);

    dsc.color = lv_color_hex(0x000000);
    dsc.text = "K";
    coords.x1 = 20;
    lv_draw_label(&layer, &dsc, &coords);

    lv_canvas_finish_layer(canvas, &layer);
}

static lv_obj_t * create_blend_mode_obj(lv_obj_t * parent, int32_t col, int32_t row, const void * src,
                                        lv_blend_mode_t blend_mode)
{
    lv_obj_t * obj = lv_image_create(parent);
    lv_image_set_src(obj, src);
    lv_image_set_blend_mode(obj, blend_mode);
    lv_obj_set_style_image_opa(obj, opa_saved, 0);
    lv_obj_set_style_image_recolor(obj, lv_color_hex(0x00ff00), 0);

    add_to_cell(obj, col, row);

    return obj;
}

static void canvas_draw_buf_reshape(lv_draw_buf_t * draw_buf)
{
#if LV_USE_DRAW_VG_LITE
    /* VG-Lite requires automatic stride calculation */
    lv_draw_buf_t * buf = lv_draw_buf_reshape(draw_buf,
                                              draw_buf->header.cf,
                                              draw_buf->header.w,
                                              draw_buf->header.h,
                                              LV_STRIDE_AUTO);
    LV_ASSERT_MSG(buf == draw_buf, "Reshape failed");
#else
    LV_UNUSED(draw_buf);
#endif
}

static void blend_mode_cb(lv_obj_t * parent)
{

    static const int32_t grid_cols[] = {53, 53, 53, 53, 53, 53, 53, 53, 53, LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_rows[] = {32, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, grid_cols, grid_rows);

    /*Make the parent darker for additive blending*/
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x808080), 0);

    LV_DRAW_BUF_DEFINE_STATIC(buf_rgb565, 36, 30, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_DEFINE_STATIC(buf_rgb888, 36, 30, LV_COLOR_FORMAT_RGB888);
    LV_DRAW_BUF_DEFINE_STATIC(buf_xrgb8888, 36, 30, LV_COLOR_FORMAT_XRGB8888);
    LV_DRAW_BUF_DEFINE_STATIC(buf_argb8888, 36, 30, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_DEFINE_STATIC(buf_argb8888_premul, 36, 30, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);

    /*The canvas will stay in the top left corner to show the original image*/
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());

    const char * cf_txt[] = {"RGB565", "RGB888.", "XRGB8888", "ARGB8888", "ARGB8888_PREMUL"};
    lv_draw_buf_t * cf_bufs[] = {&buf_rgb565, &buf_rgb888, &buf_xrgb8888, &buf_argb8888, &buf_argb8888_premul};
    static lv_draw_buf_t image_dscs[4];

    const char * mode_txt[] = {"Add.", "Sub.", "Mul."};
    lv_blend_mode_t mode_values[] = {LV_BLEND_MODE_ADDITIVE, LV_BLEND_MODE_SUBTRACTIVE, LV_BLEND_MODE_MULTIPLY};

    uint32_t m;
    for(m = 0; m < 3; m++) {
        lv_obj_t * mode_label = lv_label_create(parent);
        lv_label_set_text(mode_label, mode_txt[m]);
        lv_obj_set_grid_cell(mode_label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1 + m * 2, 2);
    }

    uint32_t cf;
    for(cf = 0; cf < 4; cf++) {
        lv_obj_t * cf_label = lv_label_create(parent);
        lv_label_set_text(cf_label, cf_txt[cf]);
        lv_obj_set_grid_cell(cf_label, LV_GRID_ALIGN_CENTER, 1 + cf * 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        canvas_draw_buf_reshape(cf_bufs[cf]);
        lv_canvas_set_draw_buf(canvas, cf_bufs[cf]);
        create_blend_mode_image_buffer(canvas);
        lv_draw_buf_t * img_src = lv_canvas_get_draw_buf(canvas);
        image_dscs[cf] = *img_src;

        for(m = 0; m < 3; m++) {
            lv_obj_t * img;
            img = create_blend_mode_obj(parent, 1 + cf * 2, 1 + m * 2, &image_dscs[cf], mode_values[m]);

            img = create_blend_mode_obj(parent, 2 + cf * 2, 1 + m * 2, &image_dscs[cf], mode_values[m]);
            lv_image_set_rotation(img, 200);

            img = create_blend_mode_obj(parent, 1 + cf * 2, 2 + m * 2, &image_dscs[cf], mode_values[m]);
            lv_obj_set_style_image_recolor_opa(img, LV_OPA_50, 0);

            img = create_blend_mode_obj(parent, 2 + cf * 2, 2 + m * 2, &image_dscs[cf], mode_values[m]);
            lv_image_set_rotation(img, 200);
            lv_obj_set_style_image_recolor_opa(img, LV_OPA_50, 0);
        }
    }

    /*Show the recolored image to show the original image*/
    lv_obj_t * img_recolored = lv_image_create(parent);
    lv_image_set_src(img_recolored, lv_canvas_get_image(canvas));
    lv_obj_set_style_image_recolor(img_recolored, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_image_recolor_opa(img_recolored, LV_OPA_50, 0);
    lv_obj_set_y(img_recolored, 30);
    lv_obj_add_flag(img_recolored, LV_OBJ_FLAG_IGNORE_LAYOUT);

}

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

static lv_obj_t * create_linear_gradient_obj(lv_obj_t * parent, int32_t col, int32_t row, lv_grad_dsc_t * grad,
                                             int32_t x1, int32_t y1, lv_grad_extend_t extend, bool use_opa_map, int32_t radius)
{
    const lv_color_t grad_color[2] = {
        LV_COLOR_MAKE(0xd5, 0x03, 0x47),
        LV_COLOR_MAKE(0x00, 0x00, 0x00),
    };

    const lv_opa_t grad_opa[2] = {
        LV_OPA_100, LV_OPA_0,
    };

    /*init gradient color map*/
    lv_grad_init_stops(grad, grad_color, use_opa_map ? grad_opa : NULL, NULL, sizeof(grad_color) / sizeof(lv_color_t));

    /*init gradient parameters*/
    grad->dir = LV_GRAD_DIR_LINEAR;
    grad->params.linear.start.x = 0;                           /*vector start x position*/
    grad->params.linear.start.y = 0;                           /*vector start y position*/
    grad->params.linear.end.x = x1;                            /*vector end x position*/
    grad->params.linear.end.y = y1;                            /*vector end y position*/
    grad->extend = extend;                              /*color pattern outside the vector*/

    /*create rectangle*/
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 70, 50);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);

    /*set gradient as background*/
    lv_obj_set_style_bg_grad(obj, grad, 0);

    add_to_cell(obj, col, row);

    return obj;
}

static void linear_gradient_cb(lv_obj_t * parent)
{
    static const int32_t grid_cols[] = { 53, 53, 53, 53, 53, 53, 53, 53, 53, LV_GRID_TEMPLATE_LAST };
    static const int32_t grid_rows[] = { 32, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST };
    lv_obj_set_grid_dsc_array(parent, grid_cols, grid_rows);

    const char * opa_txt[] = { "no opa", "no opa round", "stop opa", "stop opa round" };
    int32_t radius_values[] = { 0, 20, 0, 20 };
    bool opa_map_values[] = { false, false, true, true };

    const char * offs_txt[] = { "pad", "repeat", "reflect" };
    int32_t x1_values[] = { lv_pct(100), lv_pct(15), lv_pct(30)};
    int32_t y1_values[] = { lv_pct(100), lv_pct(30), lv_pct(15) };
    lv_grad_extend_t extend_values[] = { LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT, LV_GRAD_EXTEND_REFLECT };

    static lv_grad_dsc_t grad_values[3][4];

    uint32_t y;
    for(y = 0; y < 3; y++) {
        lv_obj_t * offs_label = lv_label_create(parent);
        lv_label_set_text(offs_label, offs_txt[y]);
        lv_obj_set_grid_cell(offs_label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1 + y * 2, 2);
    }

    uint32_t x;
    for(x = 0; x < 4; x++) {
        lv_obj_t * op_label = lv_label_create(parent);
        lv_label_set_text(op_label, opa_txt[x]);
        lv_obj_set_grid_cell(op_label, LV_GRID_ALIGN_CENTER, 1 + x * 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        for(y = 0; y < 3; y++) {
            create_linear_gradient_obj(parent, 1 + x * 2, 1 + y * 2, &grad_values[y][x], x1_values[y], y1_values[y],
                                       extend_values[y], opa_map_values[x], radius_values[x]);
        }
    }
}

static lv_obj_t * create_radial_gradient_obj(lv_obj_t * parent, int32_t col, int32_t row, lv_grad_dsc_t * grad,
                                             int32_t offs, int32_t r0, lv_grad_extend_t extend, bool use_opa_map, int32_t radius)
{
    const lv_color_t grad_color[2] = {
        LV_COLOR_MAKE(0xd5, 0x03, 0x47),
        LV_COLOR_MAKE(0x00, 0x00, 0x00),
    };

    const lv_opa_t grad_opa[2] = {
        LV_OPA_100, LV_OPA_0,
    };

    /*init gradient color map*/
    lv_grad_init_stops(grad, grad_color, use_opa_map ? grad_opa : NULL, NULL, sizeof(grad_color) / sizeof(lv_color_t));

    /*init gradient parameters*/
    grad->dir = LV_GRAD_DIR_RADIAL;
    grad->params.radial.focal.x = lv_pct(50);                          /*start circle center x position*/
    grad->params.radial.focal.y = lv_pct(50);                          /*start circle center y position*/
    grad->params.radial.focal_extent.x = grad->params.radial.focal.x + r0;    /*start circle point x coordinate*/
    grad->params.radial.focal_extent.y = grad->params.radial.focal.y;         /*start circle point y coordinate*/
    grad->params.radial.end.x = grad->params.radial.focal.x + offs;           /*end circle center x position*/
    grad->params.radial.end.y = grad->params.radial.focal.y + offs;           /*end circle center y position*/
    grad->params.radial.end_extent.x = grad->params.radial.end.x;             /*end circle point x coordinate*/
    grad->params.radial.end_extent.y = lv_pct(85);                    /*end circle point y coordinate*/
    grad->extend = extend;                                      /*color pattern outside the border circles*/

    /*create rectangle*/
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 70, 50);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);

    /*set gradient as background*/
    lv_obj_set_style_bg_grad(obj, grad, 0);

    add_to_cell(obj, col, row);

    return obj;
}

static void radial_gradient_cb(lv_obj_t * parent)
{
    static const int32_t grid_cols[] = { 53, 53, 53, 53, 53, 53, 53, 53, 53, LV_GRID_TEMPLATE_LAST };
    static const int32_t grid_rows[] = { 32, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST };
    lv_obj_set_grid_dsc_array(parent, grid_cols, grid_rows);

    const char * opa_txt[] = { "no opa", "no opa round", "stop opa", "stop opa round" };
    int32_t radius_values[] = { 0, 20, 0, 20 };
    bool opa_map_values[] = { false, false, true, true };

    const char * offs_txt[] = { "pad", "repeat", "reflect" };
    lv_grad_extend_t extend_values[] = { LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT, LV_GRAD_EXTEND_REFLECT};

    static lv_grad_dsc_t grad_values[3][4];

    uint32_t y;
    for(y = 0; y < 3; y++) {
        lv_obj_t * offs_label = lv_label_create(parent);
        lv_label_set_text(offs_label, offs_txt[y]);
        lv_obj_set_grid_cell(offs_label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1 + y * 2, 2);
    }

    uint32_t x;
    for(x = 0; x < 4; x++) {
        lv_obj_t * op_label = lv_label_create(parent);
        lv_label_set_text(op_label, opa_txt[x]);
        lv_obj_set_grid_cell(op_label, LV_GRID_ALIGN_CENTER, 1 + x * 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        for(y = 0; y < 3; y++) {
            create_radial_gradient_obj(parent, 1 + x * 2, 1 + y * 2, &grad_values[y][x], y * 5, 0, extend_values[y],
                                       opa_map_values[x], radius_values[x]);
        }
    }
}

static lv_obj_t * create_conical_gradient_obj(lv_obj_t * parent, int32_t col, int32_t row, lv_grad_dsc_t * grad,
                                              int32_t a0, int32_t a1, lv_grad_extend_t extend, bool use_opa_map, int32_t radius)
{
    const lv_color_t grad_color[2] = {
        LV_COLOR_MAKE(0xd5, 0x03, 0x47),
        LV_COLOR_MAKE(0x00, 0x00, 0x00),
    };

    const lv_opa_t grad_opa[2] = {
        LV_OPA_100, LV_OPA_0,
    };

    /*init gradient color map*/
    lv_grad_init_stops(grad, grad_color, use_opa_map ? grad_opa : NULL, NULL, sizeof(grad_color) / sizeof(lv_color_t));

    /*init gradient parameters*/
    grad->dir = LV_GRAD_DIR_CONICAL;
    grad->params.conical.center.x = lv_pct(50);                /*center x position*/
    grad->params.conical.center.y = lv_pct(50);                /*center y position*/
    grad->params.conical.start_angle = a0;                     /*start angle*/
    grad->params.conical.end_angle = a1;                       /*end angle*/
    grad->extend = extend;                              /*color pattern outside the vector*/

    /*create rectangle*/
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 70, 50);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);

    /*set gradient as background*/
    lv_obj_set_style_bg_grad(obj, grad, 0);

    add_to_cell(obj, col, row);

    return obj;
}

static void conical_gradient_cb(lv_obj_t * parent)
{
    static const int32_t grid_cols[] = { 53, 53, 53, 53, 53, 53, 53, 53, 53, LV_GRID_TEMPLATE_LAST };
    static const int32_t grid_rows[] = { 32, 40, 40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST };
    lv_obj_set_grid_dsc_array(parent, grid_cols, grid_rows);

    const char * opa_txt[] = { "no opa", "no opa round", "stop opa", "stop opa round" };
    int32_t radius_values[] = { 0, 20, 0, 20 };
    bool opa_map_values[] = { false, false, true, true };

    const char * offs_txt[] = { "pad", "repeat", "reflect" };
    lv_grad_extend_t extend_values[] = { LV_GRAD_EXTEND_PAD, LV_GRAD_EXTEND_REPEAT, LV_GRAD_EXTEND_REFLECT };

    static lv_grad_dsc_t grad_values[3][4];

    uint32_t y;
    for(y = 0; y < 3; y++) {
        lv_obj_t * offs_label = lv_label_create(parent);
        lv_label_set_text(offs_label, offs_txt[y]);
        lv_obj_set_grid_cell(offs_label, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1 + y * 2, 2);
    }

    uint32_t x;
    for(x = 0; x < 4; x++) {
        lv_obj_t * op_label = lv_label_create(parent);
        lv_label_set_text(op_label, opa_txt[x]);
        lv_obj_set_grid_cell(op_label, LV_GRID_ALIGN_CENTER, 1 + x * 2, 2, LV_GRID_ALIGN_CENTER, 0, 1);

        for(y = 0; y < 3; y++) {
            create_conical_gradient_obj(parent, 1 + x * 2, 1 + y * 2, &grad_values[y][x], 10, 100, extend_values[y],
                                        opa_map_values[x], radius_values[x]);
        }
    }
}

#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static scene_dsc_t scenes[] = {
    {.name = "fill",                .create_cb = fill_cb},
    {.name = "border",              .create_cb = border_cb},
    {.name = "box_shadow",          .create_cb = box_shadow_cb},
    {.name = "text",                .create_cb = text_cb},
    {.name = "image_normal_1",      .create_cb = image_normal_1_cb},
    {.name = "image_recolor_1",     .create_cb = image_recolored_1_cb},
    {.name = "image_normal_2",      .create_cb = image_normal_2_cb},
    {.name = "image_recolor_2",     .create_cb = image_recolored_2_cb},
    {.name = "image_normal_3",      .create_cb = image_normal_3_cb},
    {.name = "image_recolor_3",     .create_cb = image_recolored_3_cb},
    {.name = "line",                .create_cb = line_cb},
    {.name = "arc_normal",          .create_cb = arc_normal_cb},
    {.name = "arc_image",           .create_cb = arc_image_cb},
    {.name = "triangle",            .create_cb = triangle_cb},
    {.name = "layer_normal",        .create_cb = layer_normal_cb},
    {.name = "blend_mode",          .create_cb = blend_mode_cb},

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    {.name = "linear_gradient",     .create_cb = linear_gradient_cb},
    {.name = "radial_gradient",     .create_cb = radial_gradient_cb},
    {.name = "conical_gradient",    .create_cb = conical_gradient_cb},
#endif

    {.name = "", .create_cb = NULL}
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_render(lv_demo_render_scene_t id, lv_opa_t opa)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);

    lv_obj_t * main_parent = lv_obj_create(scr);
    lv_obj_remove_style_all(main_parent);
    lv_obj_set_style_bg_opa(main_parent, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(main_parent, lv_color_hex3(0xaaf), 0);
    lv_obj_set_size(main_parent, 480, 272);

    static const int32_t grid_cols[] = {60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_rows[] = {34, 34, 34, 34, 34, 34, 34, 34, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(main_parent, grid_cols, grid_rows);

    opa_saved = opa;

    if(scenes[id].create_cb) scenes[id].create_cb(main_parent);
}

const char * lv_demo_render_get_scene_name(lv_demo_render_scene_t id)
{
    if(id > LV_DEMO_RENDER_SCENE_NUM) return NULL;
    return scenes[id].name;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void add_to_cell(lv_obj_t * obj, int32_t col, int32_t row)
{
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
}

#endif
