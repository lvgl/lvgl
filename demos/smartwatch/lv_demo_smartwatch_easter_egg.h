/**
 * @file lv_demo_smartwatch_easter_egg.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_EASTER_EGG_H
#define LV_DEMO_SMARTWATCH_EASTER_EGG_H

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
 * Create the easter_egg page. Called only once.
 */
void lv_demo_smartwatch_easter_egg_create(void);

/**
 * Load the easter egg screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_easter_egg_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
