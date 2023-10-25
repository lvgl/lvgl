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

/**********************
 *  STATIC VARIABLES
 **********************/
//box shadow (offset, size, spread)
//text (normal text + underline/strike through, placeholder)
//triangle (just some rectangles)
//image (various formats + transformation)
//line (various angles + line caps)
//arc (some arcs + caps)
//vector (later)
//layer (blend mode, transformation)
//mask bitmap (not implemented SW render yet)
//mask rectangle

static scene_dsc_t scenes[] = {
    {.name = "Fill ",               .create_cb = fill_cb},
    {.name = "Border",              .create_cb = border_cb},

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
