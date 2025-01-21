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
#include "../lv_draw_sw_mask.h"
#if LV_USE_DRAW_SW

#include "../../../misc/lv_color.h"
#include "../../../misc/lv_area.h"
#include "../../../misc/lv_style.h"

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
 * Blend a color to a buffer
 * @param dsc pointer to a fill descriptor
 */
typedef void (*lv_draw_sw_blend_color_handler_t)(lv_draw_sw_blend_fill_dsc_t * dsc);

/**
 * Blend an image to a buffer
 * @param dsc pointer to an image blend descriptor
 */
typedef void (*lv_draw_sw_blend_image_handler_t)(lv_draw_sw_blend_image_dsc_t * dsc);

/**
 * Handler to blend colors and images
 */
typedef struct {
    lv_color_format_t dest_cf;
    lv_draw_sw_blend_color_handler_t color_blend_handler;
    lv_draw_sw_blend_image_handler_t image_blend_handler;
} lv_draw_sw_blend_handler_t;

/**
 * Call the blend function of the `layer`.
 * @param draw_unit     pointer to a draw unit
 * @param dsc           pointer to an initialized blend descriptor
 */
void lv_draw_sw_blend(lv_draw_unit_t * draw_unit, const lv_draw_sw_blend_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_H*/
