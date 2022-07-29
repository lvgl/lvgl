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
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity. This function can also
 * rotate the display output buffer to a specified angle (90x step).
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_area destination area
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] src_buf source buffer
 * @param[in] src_area source area with absolute coordinates to draw on destination buffer
 * @param[in] opa opacity of the result
 * @param[in] angle display rotation angle (90x)
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_pxp_blit(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                             const lv_color_t * src_buf, const lv_area_t * src_area, lv_opa_t opa, lv_disp_rot_t angle);

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with transformation.
 *
 *
 * @param[in/out] dest_buf destination buffer
 * @param[in] dest_area destination area
 * @param[in] dest_stride width (stride) of destination buffer in pixels
 * @param[in] src_buf source buffer
 * @param[in] src_area source area with absolute coordinates to draw on destination buffer
 * @param[in] dsc image descriptor
 * @param[in] cf color format
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_PXP_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_pxp_blit_transform(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, const lv_area_t * src_area, const lv_draw_img_dsc_t * dsc, lv_img_cf_t cf);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_NXP_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PXP_BLEND_H*/
