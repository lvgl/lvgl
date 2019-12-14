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
#include "../lv_core/lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_color_t bg_color;
    lv_color_t bg_grad_color;
    lv_style_value_t border_width;
    lv_color_t border_color;
    lv_blend_mode_t border_blend_mode;
    lv_opa_t bg_opa;
    lv_opa_t border_opa;
    lv_style_value_t radius;
    lv_style_value_t border_part;
    lv_style_value_t bg_grad_dir;
    lv_style_value_t bg_blend_mode;
}lv_draw_rect_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc);

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * mask, lv_draw_rect_dsc_t * dsc, lv_opa_t opa_scale);

/**
 * Draw a pixel
 * @param point the coordinates of the point to draw
 * @param mask the pixel will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down the opacity by the factor
 */
void lv_draw_px(const lv_point_t * point, const lv_area_t * clip_area, const lv_style_t * style, lv_opa_t opa_scale);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_RECT_H*/
