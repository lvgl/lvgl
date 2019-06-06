/**
 * @file lv_draw_rect.h
 *
 */

#ifndef LV_DRAW_RECT_H
#define LV_DRAW_RECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"

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
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_RECT_H*/
