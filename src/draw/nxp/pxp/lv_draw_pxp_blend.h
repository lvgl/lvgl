/**
 * @file lv_draw_pxp_blend.h
 *
 */

/**
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#if 0
#ifndef LV_DRAW_PXP_BLEND_H
#define LV_DRAW_PXP_BLEND_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_PXP
#include "lv_gpu_nxp_pxp.h"
#include "../../sw/lv_draw_sw.h"

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
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with transformation.
 *
 * @param dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf Source buffer
 * @param[in] src_area Area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] dsc Image descriptor
 * @param[in] cf Color format
 */
void lv_gpu_nxp_pxp_blit_transform(lv_color_t * dest_buf, lv_area_t * dest_area, lv_coord_t dest_stride,
                                   const lv_color_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride,
                                   const lv_draw_img_dsc_t * dsc, lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PXP_BLEND_H*/
#endif
