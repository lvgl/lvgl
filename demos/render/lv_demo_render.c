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

#define SCENE_TIME_DEF  2000

typedef struct {
    const char * name;
    void (*create_cb)(lv_obj_t * parent);
} scene_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void add_to_cell(lv_obj_t * obj, lv_coord_t col, lv_coord_t row);


static lv_obj_t * fill_obj_create(lv_obj_t * parent, lv_coord_t col, lv_coord_t row)
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

static lv_obj_t * border_obj_create(lv_obj_t * parent, lv_coord_t col, lv_coord_t row)
{

    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_border_color(obj, lv_color_hex3(0x000), 0);
    lv_obj_set_style_border_width(obj, 3, 0);
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

static lv_obj_t * box_shadow_obj_create(lv_obj_t * parent, lv_coord_t col, lv_coord_t row)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_20, 0);
    lv_obj_set_style_shadow_color(obj, lv_color_hex3(0xf00), 0);
    lv_obj_set_size(obj, DEF_WIDTH - 20, DEF_HEIGHT - 5);
    add_to_cell(obj, col, row);

    return obj;
}

static void box_shadow_cb(lv_obj_t * parent)
{

    static const lv_coord_t grid_rows[] = {45, 45, 45, 45, 45, 45, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t grid_cols[] = {68, 68, 68, 68, 68, 68, 68, LV_GRID_TEMPLATE_LAST};
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
        lv_obj_set_style_shadow_ofs_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_ofs_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 1);
        lv_obj_set_style_radius(obj, 5, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_ofs_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_ofs_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 2);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_ofs_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_ofs_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 3);
        lv_obj_set_style_radius(obj, 5, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_spread(obj, 3, 0);
        lv_obj_set_style_shadow_ofs_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_ofs_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 4);
        lv_obj_set_style_radius(obj, 5, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_50, 0);
        lv_obj_set_style_shadow_ofs_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_ofs_y(obj, ofs[i].y, 0);
    }

    for(i = 0; i < 7; i++) {
        lv_obj_t * obj = box_shadow_obj_create(parent, i, 5);
        lv_obj_set_style_radius(obj, 100, 0);
        lv_obj_set_style_shadow_width(obj, 10, 0);
        lv_obj_set_style_shadow_opa(obj, LV_OPA_50, 0);
        lv_obj_set_style_shadow_ofs_x(obj, ofs[i].x, 0);
        lv_obj_set_style_shadow_ofs_y(obj, ofs[i].y, 0);
    }
}


static lv_obj_t * text_obj_create(lv_obj_t * parent, lv_coord_t col, lv_coord_t row)
{

    lv_obj_t * obj = lv_label_create(parent);
    lv_obj_remove_style_all(obj);
    lv_label_set_text(obj, "Hello LVGL! It should be a placeholder: ű. Looks good?");
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

static lv_obj_t * image_obj_create(lv_obj_t * parent, lv_coord_t col, lv_coord_t row, bool recolor)
{
    lv_obj_t * obj = lv_image_create(parent);
    lv_obj_remove_style_all(obj);
    if(recolor) {
        lv_obj_set_style_image_recolor_opa(obj, LV_OPA_50, 0);
        lv_obj_set_style_image_recolor(obj, lv_color_hex3(0x0f0), 0);
    }

    add_to_cell(obj, col, row);

    return obj;

}

static void image_cb_core(lv_obj_t * parent, bool recolor)
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


static void image_cb_normal(lv_obj_t * parent)
{
    image_cb_core(parent, false);
}

static void image_cb_recolored(lv_obj_t * parent)
{
    image_cb_core(parent, true);
}

/**********************
 *  STATIC VARIABLES
 **********************/

//line (various angles + line caps)
//arc (some arcs + caps)
//triangle (just some rectangles)
//vector (later)
//layer (blend mode, transformation)
//mask bitmap (not implemented SW render yet)
//mask rectangle

static scene_dsc_t scenes[] = {
    {.name = "Fill ",               .create_cb = fill_cb},
    {.name = "Border",              .create_cb = border_cb},
    {.name = "Box shadow",          .create_cb = box_shadow_cb},
    {.name = "Text",                .create_cb = text_cb},
    {.name = "Image normal",        .create_cb = image_cb_normal},
    {.name = "Image recolor",       .create_cb = image_cb_recolored},

    {.name = "", .create_cb = NULL}
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_render(uint32_t idx)
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

    static const lv_coord_t grid_cols[] = {60, 60, 60, 60, 60, 60, 60, 60, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t grid_rows[] = {34, 34, 34, 34, 34, 34, 34, 34, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(main_parent, grid_cols, grid_rows);


    if(scenes[idx].create_cb) scenes[idx].create_cb(main_parent);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void add_to_cell(lv_obj_t * obj, lv_coord_t col, lv_coord_t row)
{
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
}


#endif
