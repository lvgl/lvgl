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

lv_draw_t * lv_draw_sw_create(void)
{
    lv_draw_sw_t * draw = lv_mem_alloc(sizeof(lv_draw_sw_t));
    lv_memset_00(draw, sizeof(lv_draw_sw_t));

//    draw->base_draw.draw_arc = lv_draw_sw_arc;
    draw->base_draw.draw_rect = lv_draw_sw_rect;
//    draw->base_draw.draw_letter = lv_draw_sw_letter;
//    draw->base_draw.draw_img = lv_draw_sw_img;
//    draw->base_draw.draw_line = lv_draw_sw_line;
//    draw->base_draw.draw_polygon = lv_draw_sw_polygon;
    draw->blend = lv_draw_sw_blend;

    return (lv_draw_t *)draw;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
