/**
 * @file lv_demo_render.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_render.h"

#if LV_USE_DEMO_RENDER
#include "../../src/display/lv_display_private.h"
#include "../../src/core/lv_global.h"

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

static void image_core_cb(lv_obj_t * parent, bool recolor)
{
    LV_IMG_DECLARE(img_render_lvgl_logo_xrgb8888);
    LV_IMG_DECLARE(img_render_lvgl_logo_rgb888);
    LV_IMG_DECLARE(img_render_lvgl_logo_rgb565);
    LV_IMG_DECLARE(img_render_lvgl_logo_argb8888);
    const void * srcs[] = {
        &img_render_lvgl_logo_argb8888,
        &img_render_lvgl_logo_xrgb8888,
        &img_render_lvgl_logo_rgb888,
        &img_render_lvgl_logo_rgb565,
    };

    const void * names[] = {
        "ARGB\n8888",
        "XRGB\n8888",
        "RGB\n888",
        "RGB\n565",
    };

    uint32_t i;
    for(i = 0; i < 4; i++) {
        lv_obj_t * obj;

        obj = lv_label_create(parent);
        lv_label_set_text(obj, names[i]);
        add_to_cell(obj, 0, i * 2);

        obj = image_obj_create(parent, 1, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);

        obj = image_obj_create(parent, 2, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_rotation(obj, 300);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 3, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 4, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale_x(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 5, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale_y(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 6, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_rotation(obj, 300);
        lv_image_set_scale(obj, 400);
        lv_image_set_pivot(obj, 0, 0);

        obj = image_obj_create(parent, 7, i * 2, recolor);
        lv_image_set_src(obj, srcs[i]);
        lv_image_set_scale_y(obj, 400);
        lv_image_set_rotation(obj, 300);
        lv_image_set_pivot(obj, 0, 0);
    }
}

static void image_normal_cb(lv_obj_t * parent)
{
    image_core_cb(parent, false);
}

static void image_recolored_cb(lv_obj_t * parent)
{
    image_core_cb(parent, true);
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

    dsc.p[0].x = p_rel[0].x + obj->coords.x1 + 8;
    dsc.p[0].y = p_rel[0].y + obj->coords.y1 + 2;
    dsc.p[1].x = p_rel[1].x + obj->coords.x1 + 8;
    dsc.p[1].y = p_rel[1].y + obj->coords.y1 + 2;
    dsc.p[2].x = p_rel[2].x + obj->coords.x1 + 8;
    dsc.p[2].y = p_rel[2].y + obj->coords.y1 + 2;

    lv_opa_t opa = lv_obj_get_style_opa(obj, 0);
    dsc.bg_grad.dir = lv_obj_get_style_bg_grad_dir(obj, 0);
    dsc.bg_grad.stops[0].color = lv_obj_get_style_bg_color(obj, 0);
    dsc.bg_grad.stops[0].frac = lv_obj_get_style_bg_main_stop(obj, 0);
    dsc.bg_grad.stops[0].opa = LV_OPA_MIX2(lv_obj_get_style_bg_main_opa(obj, 0), opa);
    dsc.bg_grad.stops[1].color = lv_obj_get_style_bg_grad_color(obj, 0);
    dsc.bg_grad.stops[1].frac = lv_obj_get_style_bg_grad_stop(obj, 0);
    dsc.bg_grad.stops[1].opa = LV_OPA_MIX2(lv_obj_get_style_bg_grad_opa(obj, 0), opa);
    dsc.bg_grad.stops_count = 2;

    dsc.bg_color = dsc.bg_grad.stops[0].color;
    dsc.bg_opa = dsc.bg_grad.stops[0].opa;

    lv_draw_triangle(lv_event_get_layer(e), &dsc);
}

static lv_obj_t * triangle_obj_create(lv_obj_t * parent, int32_t col, int32_t row, lv_point_t p[])
{
    lv_obj_t * obj = lv_arc_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, DEF_WIDTH, DEF_HEIGHT);
    lv_obj_set_style_bg_color(obj, lv_color_hex3(0xff0), 0);
    lv_obj_set_style_opa(obj, opa_saved, 0);
    lv_obj_add_event(obj, triangle_draw_event_cb, LV_EVENT_DRAW_MAIN, p);
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

static void layer_additive_cb(lv_obj_t * parent)
{
    return; /*Not working*/

    /*Make the parent darker for additive blending*/
    lv_obj_set_style_bg_color(parent, lv_color_hex3(0x008), 0);
    layer_core_cb(parent, LV_BLEND_MODE_ADDITIVE);
}

/**********************
 *  STATIC VARIABLES
 **********************/

static scene_dsc_t scenes[] = {
    {.name = "fill",                .create_cb = fill_cb},
    {.name = "border",              .create_cb = border_cb},
    {.name = "box_shadow",          .create_cb = box_shadow_cb},
    {.name = "text",                .create_cb = text_cb},
    {.name = "image normal",        .create_cb = image_normal_cb},
    {.name = "image_recolor",       .create_cb = image_recolored_cb},
    {.name = "line",                .create_cb = line_cb},
    {.name = "arc_normal",          .create_cb = arc_normal_cb},
    {.name = "arc_image",           .create_cb = arc_image_cb},
    {.name = "triangle",            .create_cb = triangle_cb},
    {.name = "layer_normal",        .create_cb = layer_normal_cb},
    {.name = "layer_additive",      .create_cb = layer_additive_cb},

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
    if(id > _LV_DEMO_RENDER_SCENE_NUM) return NULL;
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
