/**
 * @file lv_demo_smartwatch_control.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_CONTROL_H
#define LV_DEMO_SMARTWATCH_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"

#if LV_USE_DEMO_SMARTWATCH

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
 * Create the control page. Called only once.
 * @param       parent  tileview object
 */
void lv_demo_smartwatch_control_create(lv_obj_t * parent);

/**
 * Load the control panel screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_control_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Set the callback funtion for music control
 * @param cb callback function
 */
void lv_demo_smartwatch_set_music_control_cb(lv_smartwatch_music_control_cb_t cb);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
