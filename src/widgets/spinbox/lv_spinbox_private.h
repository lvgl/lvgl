/**
 * @file lv_spinbox_private.h
 *
 */

#ifndef LV_SPINBOX_PRIVATE_H
#define LV_SPINBOX_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../textarea/lv_textarea_private.h"
#include "lv_spinbox.h"

#if LV_USE_SPINBOX

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of spinbox */
struct _lv_spinbox_t {
    lv_textarea_t ta;             /**< Ext. of ancestor */
    /*New data for this type*/
    int32_t value;
    int32_t range_max;
    int32_t range_min;
    int32_t step;
    uint32_t digit_count : 4;
    uint32_t dec_point_pos : 4;   /**< if 0, there is no separator and the number is an integer */
    uint32_t rollover : 1;        /**< Set to true for rollover functionality */
    uint32_t digit_step_dir : 2;  /**< the direction the digit will step on encoder button press when editing */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SPINBOX */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SPINBOX_PRIVATE_H*/
