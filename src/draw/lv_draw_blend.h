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

//! @cond Doxygen_Suppress
LV_ATTRIBUTE_FAST_MEM void _lv_blend_fill(const lv_area_t * clip_area, const lv_area_t * fill_area, lv_color_t color,
                                          lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode);

LV_ATTRIBUTE_FAST_MEM void _lv_blend_map(const lv_area_t * clip_area, const lv_area_t * map_area,
                                         const lv_color_t * map_buf,
                                         lv_opa_t * mask, lv_draw_mask_res_t mask_res, lv_opa_t opa, lv_blend_mode_t mode);

//! @endcond
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_BLEND_H*/
