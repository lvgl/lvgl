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
#include "../../misc/lv_color.h"
#include "../../misc/lv_area.h"
#include "../../misc/lv_style.h"
#include "../lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const lv_area_t * blend_area;   /**< The area with absolute coordinates to draw on `draw_ctx->buf`
                                     *   will be clipped to `draw_ctx->clip_area` */
    const lv_color_t * src_buf;     /**< Pointer to an image to blend. If set `fill_color` is ignored */
    lv_color_t color;               /**< Fill color*/
    lv_opa_t * mask_buf;            /**< NULL if ignored, or an alpha mask to apply on `blend_area`*/
    lv_draw_mask_res_t mask_res;    /**< The result of the previous mask operation */
    const lv_area_t * mask_area;    /**< The area of `mask_buf` with absolute coordinates*/
    lv_opa_t opa;                   /**< The overall opacity*/
    lv_blend_mode_t blend_mode;     /**< E.g. LV_BLEND_MODE_ADDITIVE*/
} lv_draw_sw_blend_dsc_t;

struct _lv_draw_ctx_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Call the blend function of the `draw_ctx`.
 * @param draw_ctx      pointer to a draw context
 * @param dsc           pointer to an initialized blend descriptor
 */
void lv_draw_sw_blend(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

/**
 * The basic blend function used with software rendering.
 * @param draw_ctx      pointer to a draw context
 * @param dsc           pointer to an initialized blend descriptor
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_blend_basic(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_H*/
