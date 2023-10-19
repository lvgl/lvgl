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
typedef struct {
    lv_draw_dsc_base_t base;

    lv_opa_t bg_opa;
    lv_color_t bg_color;
    lv_grad_dsc_t bg_grad;

    lv_point_t p[3];
} lv_draw_triangle_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_triangle_dsc_init(lv_draw_triangle_dsc_t * draw_dsc);

void lv_draw_triangle(struct _lv_layer_t * layer, const lv_draw_triangle_dsc_t * draw_dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_TRIANGLE_H*/
