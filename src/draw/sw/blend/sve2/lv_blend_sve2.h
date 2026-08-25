/**
 * @file lv_blend_sve2.h
 *
 */

#ifndef LV_BLEND_SVE2_H
#define LV_BLEND_SVE2_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lvgl_public.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2

#ifdef LV_DRAW_SW_SVE2_CUSTOM_INCLUDE
#include LV_DRAW_SW_SVE2_CUSTOM_INCLUDE
#endif

#include "lv_draw_sw_blend_sve2_to_rgb888.h"

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
 *      MACROS
 **********************/

#endif /* #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_SVE2 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_SVE2_H*/
