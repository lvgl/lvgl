/**
 * @file lv_draw_sw_gradient_private.h
 *
 */

#ifndef LV_DRAW_SW_GRADIENT_PRIVATE_H
#define LV_DRAW_SW_GRADIENT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sw_gradient.h"

#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_grad_t {
    lv_color_t   *  color_map;
    lv_opa_t   *  opa_map;
    uint32_t size;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_SW */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_GRADIENT_PRIVATE_H*/
