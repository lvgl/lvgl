/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_draw_triangle.h"
#include "../misc/lv_math.h"
#include "../misc/lv_mem.h"

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
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_polygon(struct _lv_draw_t * draw, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t points[], uint16_t point_cnt)
{
    draw->draw_polygon(draw, draw_dsc, points, point_cnt);
}

void lv_draw_triangle(struct _lv_draw_t * draw, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t points[])
{

    draw->draw_polygon(draw, draw_dsc, points, 3);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
