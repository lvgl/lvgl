/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_triangle.h"
#include "../lv_misc/lv_math.h"

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
 *
 * @param points pointer to an array with 3 points
 * @param mask the triangle will be drawn only in this mask
 * @param style style for of the triangle
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{

}

/**
 * Draw a polygon from triangles. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param mask polygon will be drawn only in this mask
 * @param style style of the polygon
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_polygon(const lv_point_t * points, uint32_t point_cnt, const lv_area_t * mask, const lv_style_t * style,
                     lv_opa_t opa_scale)
{

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
