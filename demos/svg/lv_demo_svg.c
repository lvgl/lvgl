/**
 * @file lv_demo_svg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_svg.h"

#if LV_USE_DEMO_SVG

extern size_t svg_len;
extern char svg_data[];

/*********************
 *      DEFINES
 *********************/
#define WIDTH 640
#define HEIGHT 480

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_svg_render_obj_t * render_list;

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void event_cb(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_vector_dsc_t * dsc = lv_vector_dsc_create(layer);

    lv_area_t rect = {0, 0, WIDTH, HEIGHT};
    lv_vector_dsc_set_fill_color(dsc, lv_color_white());
    lv_vector_clear_area(dsc, &rect); // clear screen

    lv_vector_dsc_scale(dsc, 0.5f, 0.5f);

    lv_draw_svg_render(dsc, render_list);
    lv_draw_vector(dsc);
    lv_vector_dsc_delete(dsc);
}

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_svg(void)
{
    lv_svg_node_t * svg = lv_svg_load_data(svg_data, svg_len);
    render_list = lv_svg_render_create(svg);
    lv_obj_add_event_cb(lv_screen_active(), event_cb, LV_EVENT_DRAW_MAIN, NULL);
    lv_svg_node_delete(svg);
}

#else

void lv_demo_svg(void)
{
    /*fallback for online examples*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "SVG is not enabled");
    lv_obj_center(label);
}

#endif
