/**
 * @file lv_draw_sw_blend_rgb565.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_RGB565_H
#define LV_DRAW_SW_BLEND_RGB565_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw_sw.h"
#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_blend_color_to_rgb565(_lv_draw_sw_blend_fill_dsc_t * dsc);

LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_blend_image_to_rgb565(_lv_draw_sw_blend_image_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_RGB565_H*/
