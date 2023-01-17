/**
 * @file lv_draw_vglite_blend.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2023 NXP
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

#ifndef LV_DRAW_VGLITE_BLEND_H
#define LV_DRAW_VGLITE_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_NXP_VG_LITE
#include "lv_vglite_utils.h"

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
 * Fill area, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] color Color
 * @param[in] opa Opacity (255 = full, 128 = 50% background/50% color, 0 = no fill)
 *
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_fill(const lv_area_t * dest_area, lv_color_t color, lv_opa_t opa);

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity.
 *
 * @param[in/out] dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] opa Opacity
 *
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_blit(lv_color_t * dest_buf, lv_area_t * dest_area, lv_coord_t dest_stride,
                                const lv_color_t * src_buf, lv_area_t * src_area, lv_coord_t src_stride,
                                lv_opa_t opa);

/**
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with transformation.
 * By default, image is copied directly, with optional opacity.
 *
 * @param[in/out] dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] dsc Image descriptor
 *
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_blit_transform(lv_color_t * dest_buf, lv_area_t * dest_area, lv_coord_t dest_stride,
                                          const lv_color_t * src_buf, lv_area_t * src_area, lv_coord_t src_stride,
                                          const lv_draw_img_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_NXP_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_BLEND_H*/
