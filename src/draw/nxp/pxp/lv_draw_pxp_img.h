/**
 * @file lv_draw_pxp_img.h
 *
 */

/**
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */


#ifndef LV_DRAW_PXP_IMG_H
#define LV_DRAW_PXP_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_PXP

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
 * BLock Image Transfer - copy rectangular image from src_buf to dst_buf with effects.
 * By default, image is copied directly, with optional opacity. This function can also
 * rotate the display output buffer to a specified angle (90x step).
 *
 * @param dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] opa Opacity
 * @param[in] angle Display rotation angle (90x)
 */
void pxp_blit(uint8_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
              const uint8_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride,
              lv_opa_t opa, lv_disp_rotation_t angle);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_PXP_IMG_H*/
