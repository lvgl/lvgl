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

/**
 * Draw a polygon. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param clip_area polygon will be drawn only in this area
 * @param draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_polygon(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * clip_area,
                     const lv_draw_rect_dsc_t * draw_dsc)
{

    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->draw_polygon(points, point_cnt, clip_area, draw_dsc);
}

void lv_draw_triangle(const lv_point_t points[], const lv_area_t * clip_area,
                     const lv_draw_rect_dsc_t * draw_dsc)
{

    const lv_draw_backend_t * backend = lv_draw_backend_get();
    backend->draw_polygon(points, 3, clip_area, draw_dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
