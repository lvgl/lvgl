/**
 * @file lv_draw_blend.h
 *
 */

#ifndef LV_DRAW_BLEND_H
#define LV_DRAW_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"
#include "lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area, lv_color_t color,
                                          lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode);


/**
 * Copy a map (image) to a display buffer.
 * @param clip_area clip the map to this area (absolute coordinates)
 * @param map_area area of the image  (absolute coordinates)
 * @param map_buf a pixels of the map (image)
 * @param mask a mask to apply on every pixel (uint8_t array with 0x00..0xff values).
 *                Relative to map area but its width is truncated to clip area.
 * @param mask_res LV_MASK_RES_COVER: the mask has only 0xff values (no mask),
 *                 LV_MASK_RES_TRANSP: the mask has only 0x00 values (full transparent),
 *                 LV_MASK_RES_CHANGED: the mask has mixed values
 * @param opa  overall opacity in 0x00..0xff range
 * @param mode blend mode from `lv_blend_mode_t`
 */
void lv_draw_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                                         const lv_color_t * map_buf,
                                         lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BLEND_H*/
