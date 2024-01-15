/**
 * @file lv_blend_arm2d.h
 *
 */

#ifndef LV_BLEND_HELIUM_H
#define LV_BLEND_HELIUM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"

/* detect whether helium is available based on arm compilers' standard */
#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE

#ifdef LV_DRAW_SW_HELIUM_CUSTOM_INCLUDE
#include LV_DRAW_SW_HELIUM_CUSTOM_INCLUDE
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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*********************
 *   POST INCLUDES
 *********************/
/* use arm-2d as the default helium acceleration */
#include "lv_blend_arm2d.h"

#endif /* defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_HELIUM_H*/
