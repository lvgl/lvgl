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

typedef struct {
    const lv_area_t * blend_area;   /**< The area with absolute coordinates to draw on `layer->buf`
                                     *   will be clipped to `layer->clip_area` */
    const void * src_buf;     /**< Pointer to an image to blend. If set `fill_color` is ignored */
    uint32_t src_stride;
    lv_color_format_t src_color_format;
    const lv_area_t * src_area;
    lv_opa_t opa;                   /**< The overall opacity*/
    lv_color_t color;               /**< Fill color*/
    const lv_opa_t * mask_buf;      /**< NULL if ignored, or an alpha mask to apply on `blend_area`*/
    lv_draw_sw_mask_res_t mask_res;    /**< The result of the previous mask operation */
    const lv_area_t * mask_area;    /**< The area of `mask_buf` with absolute coordinates*/
    int32_t mask_stride;
    lv_blend_mode_t blend_mode;     /**< E.g. LV_BLEND_MODE_ADDITIVE*/
} lv_draw_sw_blend_dsc_t;

typedef struct {
    void * dest_buf;
    int32_t dest_w;
    int32_t dest_h;
    int32_t dest_stride;
    const lv_opa_t * mask_buf;
    int32_t mask_stride;
    lv_color_t color;
    lv_opa_t opa;
} _lv_draw_sw_blend_fill_dsc_t;

typedef struct {
    void * dest_buf;
    int32_t dest_w;
    int32_t dest_h;
    int32_t dest_stride;
    const lv_opa_t * mask_buf;
    int32_t mask_stride;
    const void * src_buf;
    int32_t src_stride;
    lv_color_format_t src_color_format;
    lv_opa_t opa;
    lv_blend_mode_t blend_mode;
} _lv_draw_sw_blend_image_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

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
