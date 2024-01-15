/**
 * @file lv_draw_sw_helium.h
 *
 */

#ifndef LV_DRAW_SW_HELIUM_H
#define LV_DRAW_SW_HELIUM_H

#ifdef __cplusplus
extern "C" {
#endif

/* *INDENT-OFF* */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

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
#include "lv_draw_sw_arm2d.h"

#endif /* defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_HELIUM_H*/
