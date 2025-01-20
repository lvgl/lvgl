/**
 * @file lv_demo_smartwatch_list.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_LIST_H
#define LV_DEMO_SMARTWATCH_LIST_H

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
 * Create the list page. Called only once.
 * @param       parent  tileview object
 */
void lv_demo_smartwatch_list_create(lv_obj_t * parent);

/**
 * Load the app list screen
 * @param anim_type screen load animation to use
 * @param time      animation time
 * @param delay     delay time before loading the screen
 */
void lv_demo_smartwatch_list_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * External apps call this function to return to the app list
 */
void lv_demo_smartwatch_app_close(void);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
