/**
 * @file lv_draw_sw_blend_to_l8.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_TO_L8_H
#define LV_DRAW_SW_BLEND_TO_L8_H

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

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_blend_color_to_l8(lv_draw_sw_blend_fill_dsc_t * dsc);

void /* LV_ATTRIBUTE_FAST_MEM */ lv_draw_sw_blend_image_to_l8(lv_draw_sw_blend_image_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_TO_L8_H*/
