/**
 * @file lv_blend_neon.h
 *
 */

#ifndef LV_BLEND_NEON_H
#define LV_BLEND_NEON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON

#ifdef LV_DRAW_SW_NEON_CUSTOM_INCLUDE
#include LV_DRAW_SW_NEON_CUSTOM_INCLUDE
#endif

#include "lv_draw_sw_blend_neon_to_rgb565.h"

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

#endif /* #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_NEON_H*/
