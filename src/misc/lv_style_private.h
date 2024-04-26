/**
 * @file lv_style_private.h
 *
 */

#ifndef LV_STYLE_PRIVATE_H
#define LV_STYLE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Descriptor for style transitions
 */
struct lv_style_transition_dsc_t {
    const lv_style_prop_t * props; /**< An array with the properties to animate.*/
    void * user_data;              /**< A custom user data that will be passed to the animation's user_data */
    lv_anim_path_cb_t path_xcb;     /**< A path for the animation.*/
    uint32_t time;                 /**< Duration of the transition in [ms]*/
    uint32_t delay;                /**< Delay before the transition in [ms]*/
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

#endif /*LV_STYLE_PRIVATE_H*/
