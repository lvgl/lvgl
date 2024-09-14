/**
 * @file lv_bar_private.h
 *
 */

#ifndef LV_BAR_PRIVATE_H
#define LV_BAR_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_bar.h"

#if LV_USE_BAR != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_bar_anim_t {
    lv_obj_t * bar;
    int32_t anim_start;
    int32_t anim_end;
    int32_t anim_state;
};

struct lv_bar_t {
    lv_obj_t obj;
    int32_t cur_value;          /**< Current value of the bar*/
    int32_t min_value;          /**< Minimum value of the bar*/
    int32_t max_value;          /**< Maximum value of the bar*/
    int32_t start_value;        /**< Start value of the bar*/
    lv_area_t indic_area;       /**< Save the indicator area. Might be used by derived types*/
    bool val_reversed;          /**< Whether value been reversed */
    lv_bar_anim_t cur_value_anim;
    lv_bar_anim_t start_value_anim;
    lv_bar_mode_t mode : 3;     /**< Type of bar*/
    lv_bar_orientation_t orientation : 3;  /**< Orientation of bar*/
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_BAR != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BAR_PRIVATE_H*/
