/**
 * @file lv_demo_multilang.h
 *
 */

#ifndef LV_DEMO_MULTILANG_H
#define LV_DEMO_MULTILANG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

#if LV_USE_DEMO_MULTILANG

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

#if LV_USE_FLEX == 0
#error "LV_USE_FLEX needs to be enabled"
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
void lv_demo_multilang(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_MULTILANG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_MULTILANG_H*/
