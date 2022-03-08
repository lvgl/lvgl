/**
 * @file lv_draw_pxp_blend.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2022 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef LV_DRAW_PXP_BLEND_H
#define LV_DRAW_PXP_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_NXP_PXP
#include "lv_gpu_nxp_pxp.h"
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT
/** Minimum area (in pixels) for image copy with 100% opacity to be handled by PXP*/
#define LV_GPU_NXP_PXP_BLIT_SIZE_LIMIT 5000
#endif

#ifndef LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT
/** Minimum area (in pixels) for image copy with transparency to be handled by PXP*/
#define LV_GPU_NXP_PXP_BLIT_OPA_SIZE_LIMIT 5000
#endif

#ifndef LV_GPU_NXP_PXP_BUFF_SYNC_BLIT_SIZE_LIMIT
/** Minimum invalidated area (in pixels) to be synchronized by PXP during buffer sync */
#define LV_GPU_NXP_PXP_BUFF_SYNC_BLIT_SIZE_LIMIT 5000
#endif

#ifndef LV_GPU_NXP_PXP_FILL_SIZE_LIMIT
/** Minimum area (in pixels) to be filled by PXP with 100% opacity*/
#define LV_GPU_NXP_PXP_FILL_SIZE_LIMIT 5000
#endif

#ifndef LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT
/** Minimum area (in pixels) to be filled by PXP with transparency*/
#define LV_GPU_NXP_PXP_FILL_OPA_SIZE_LIMIT 5000
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Fill area, with optional opacity.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] fill_area area to fill
 * @param[in] color color
 * @param[in] opa transparency of the color
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_pxp_fill(lv_color_t * dest_buf, lv_coord_t dest_stride, const lv_area_t * fill_area,
                             lv_color_t color, lv_opa_t opa);

/**
 * @brief BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 *
 * By default, image is copied directly, with optional opacity configured by \p opa.
 * Color keying can be enabled by calling lv_gpu_nxp_pxp_enable_color_key() before calling this function.
 * Recoloring can be enabled by calling  lv_gpu_nxp_pxp_enable_recolor() before calling this function.
 * Note that color keying and recoloring at the same time is not supported and black rectangle is rendered.
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_area destination area
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] src_buf source buffer
 * @param[in] src_area source area with absolute coordinates to draw on destination buffer
 * @param[in] opa opacity of the result
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_pxp_blit(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                             const lv_color_t * src_buf, const lv_area_t * src_area, lv_opa_t opa);

/**
 * @brief Set color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * Color key is defined by LV_COLOR_TRANSP symbol in lv_conf.h
 */
void lv_gpu_nxp_pxp_set_color_key(void);

/**
 * @brief Enable color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * Color key is defined by LV_COLOR_TRANSP symbol in lv_conf.h
 */
void lv_gpu_nxp_pxp_enable_color_key(void);

/**
 * @brief Disable color keying for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 */
void lv_gpu_nxp_pxp_disable_color_key(void);

/**
 * @brief Enable recolor feature for subsequent calls to lv_gpu_nxp_pxp_blit()
 *
 * @param[in] color recolor value
 * @param[in] opa effect opacity
 */
void lv_gpu_nxp_pxp_enable_recolor(lv_color_t color, lv_opa_t opa);

/**
 * @brief Disable recolor feature for subsequent calls to lv_gpu_nxp_pxp_blit()
 */
void lv_gpu_nxp_pxp_disable_recolor(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_NXP_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PXP_BLEND_H*/
