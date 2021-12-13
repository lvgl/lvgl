/**raw
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

void lv_draw_sw_init_ctx(lv_disp_drv_t * drv, lv_draw_t * draw)
{
    lv_draw_sw_t * draw_sw = (lv_draw_sw_t *) draw;
    lv_memset_00(draw_sw, sizeof(lv_draw_sw_t));

    draw_sw->base_draw.draw_arc = lv_draw_sw_arc;
    draw_sw->base_draw.draw_rect = lv_draw_sw_rect;
    draw_sw->base_draw.draw_letter = lv_draw_sw_letter;
    draw_sw->base_draw.draw_img = lv_draw_sw_img;
    draw_sw->base_draw.draw_line = lv_draw_sw_line;
    draw_sw->base_draw.draw_polygon = lv_draw_sw_polygon;
    draw_sw->blend = lv_draw_sw_blend;
}

void lv_draw_sw_deinit_ctx(lv_disp_drv_t * drv, lv_draw_t * draw)
{
    lv_draw_sw_t * draw_sw = (lv_draw_sw_t *) draw;
    lv_memset_00(draw_sw, sizeof(lv_draw_sw_t));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
