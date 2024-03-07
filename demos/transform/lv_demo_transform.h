/**
 * @file lv_demo_transform.h
 *
 */

#ifndef LV_DEMO_TRANSFORM_H
#define LV_DEMO_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

#if LV_USE_DEMO_TRANSFORM

#if LV_FONT_MONTSERRAT_18 == 0
#error "LV_FONT_MONTSERRAT_18 is required for lv_demo_transform. Enable it in lv_conf.h."
#endif

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
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
void lv_demo_transform(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_TRANSFORM*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_TRANSFORM_H*/
