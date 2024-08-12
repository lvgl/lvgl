/**
 * @file lv_slider_private.h
 *
 */

#ifndef LV_SLIDER_PRIVATE_H
#define LV_SLIDER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../bar/lv_bar_private.h"
#include "lv_slider.h"

#if LV_USE_SLIDER != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_slider_t {
    lv_bar_t bar;                 /**< Add the ancestor's type first */
    lv_area_t left_knob_area;
    lv_area_t right_knob_area;
    lv_point_t pressed_point;
    int32_t * value_to_set;       /**< Which bar value to set */
    uint8_t dragging : 1;         /**< 1: the slider is being dragged */
    uint8_t left_knob_focus : 1;  /**< 1: with encoder now the right knob can be adjusted */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SLIDER != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SLIDER_PRIVATE_H*/
