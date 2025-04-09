/**
 * @file lv_demo_smartwatch.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_H
#define LV_DEMO_SMARTWATCH_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"


#if LV_USE_DEMO_SMARTWATCH

#if LV_USE_LOTTIE == 0
#error "lv_demo_smartwatch requires LV_USE_LOTTIE"
#endif

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
 * Create a smartwatch demo.
 * Recommended screen size 384x384.
 */
void lv_demo_smartwatch(void);

/**********************
 * GLOBAL VARIABLES
 **********************/


#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_H*/
