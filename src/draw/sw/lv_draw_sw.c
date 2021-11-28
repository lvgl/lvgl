/**
 * @file lv_draw_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw.h"
#include "lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_init(void)
{
    static lv_draw_backend_t backend;
    lv_draw_backend_init(&backend);

    backend.draw_arc = lv_draw_sw_arc;
    backend.draw_rect = lv_draw_sw_rect;
    backend.draw_letter = lv_draw_sw_letter;
    backend.draw_img = lv_draw_sw_img;
    backend.draw_line = lv_draw_sw_line;
    backend.draw_polygon = lv_draw_sw_polygon;
    backend.blend_fill = lv_blend_sw_fill;
    backend.blend_map  = lv_blend_sw_map;

    lv_draw_backend_add(&backend);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
