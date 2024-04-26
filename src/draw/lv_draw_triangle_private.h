/**
 * @file lv_draw_triangle_private.h
 *
 */

#ifndef LV_DRAW_TRIANGLE_PRIVATE_H
#define LV_DRAW_TRIANGLE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_triangle.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/
struct lv_draw_triangle_dsc_t {
    lv_draw_dsc_base_t base;

    lv_opa_t bg_opa;
    lv_color_t bg_color;
    lv_grad_dsc_t bg_grad;

    lv_point_precise_t p[3];
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_TRIANGLE_PRIVATE_H*/
