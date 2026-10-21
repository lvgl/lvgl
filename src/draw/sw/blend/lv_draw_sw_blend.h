/**
 * @file lv_draw_sw_blend.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_H
#define LV_DRAW_SW_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"

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

/**
 * Custom draw function for SW rendering.
 * @param layer         pointer to the target layer
 * @param clip_area     pointer to the current clip area
 * @param dsc           pointer to an initialized blend descriptor
 */
typedef void (*lv_draw_sw_blend_handler_t)(lv_layer_t * layer, const lv_area_t * clip_area,
                                           const lv_draw_sw_blend_dsc_t * dsc);

typedef struct {
    lv_color_format_t dest_cf;
    lv_draw_sw_blend_handler_t handler;
} lv_draw_sw_custom_blend_handler_t;

/**
 * Call the blend function of the `layer`.
 * @param layer         pointer to the target layer
 * @param clip_area     pointer to the current clip area
 * @param dsc           pointer to an initialized blend descriptor
 */
void lv_draw_sw_blend(lv_layer_t * layer, const lv_area_t * clip_area, const lv_draw_sw_blend_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_H*/
