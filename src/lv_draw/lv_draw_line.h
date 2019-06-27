/**
 * @file lv_draw_line.h
 *
 */

#ifndef LV_DRAW_LINE_H
#define LV_DRAW_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

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
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param mask the line will be drawn only on this area
 * @param style pointer to a line's style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * mask,
                  const lv_style_t * style, lv_opa_t opa_scale);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_LINE_H*/
