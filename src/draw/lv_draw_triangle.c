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

void lv_draw_polygon(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t points[],
                     uint16_t point_cnt)
{
    LV_PROFILER_BEGIN;
    draw_ctx->draw_polygon(draw_ctx, draw_dsc, points, point_cnt);
    LV_PROFILER_END;
}

void lv_draw_triangle(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t points[])
{
    LV_PROFILER_BEGIN;
    draw_ctx->draw_polygon(draw_ctx, draw_dsc, points, 3);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
