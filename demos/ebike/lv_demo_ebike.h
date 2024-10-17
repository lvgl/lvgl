/**
 * @file lv_demo_ebike.h
 *
 */

#ifndef LV_DEMO_EBIKE_H
#define LV_DEMO_EBIKE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"

#if LV_USE_DEMO_EBIKE

/*Testing of dependencies*/
#if LV_USE_BIDI == 0
#error "BiDirectional text support is required. Enable it in lv_conf.h (LV_USE_BIDI 1)"
#endif

#if LV_USE_ARABIC_PERSIAN_CHARS == 0
#error "Arabic/Perisan character processing is required. Enable it in lv_conf.h (LV_USE_ARABIC_PERSIAN_CHARS 1)"
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
 * Create an ebike demo.
 * If LV_DEMO_EBIKE_PORTRAIT is 0: the supported resolution as are: 320x240... 480x320
 * If LV_DEMO_EBIKE_PORTRAIT is 1: the supported resolution as are: 480x600... 480x858
 */
void lv_demo_ebike(void);


/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_EBIKE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_EBIKE_H*/
