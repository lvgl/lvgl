/**
 * @file lv_draw_triangle.h
 *
 */

#ifndef LV_DRAW_TRIANGLE_H
#define LV_DRAW_TRIANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Draw a triangle
 * @param points pointer to an array with 3 points
 * @param clip_area the triangle will be drawn only in this area
 * @param draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_triangle(const lv_point_t points[], const lv_area_t * clip, const lv_draw_rect_dsc_t * draw_dsc);

/**
 * Draw a polygon. Only convex polygons are supported.
 * @param points an array of points
 * @param point_cnt number of points
 * @param clip_area polygon will be drawn only in this area
 * @param draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_polygon(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * mask,
                     const lv_draw_rect_dsc_t * draw_dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_TRIANGLE_H*/
