/**
 * @file lv_pxp_utils.h
 *
 */

/**
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_PXP_UTILS_H
#define LV_PXP_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP
#include "fsl_pxp.h"
#include "../../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_PXP_ASSERT
#define PXP_ASSERT(expr) LV_ASSERT(expr)
#else
#define PXP_ASSERT(expr)
#endif

#define PXP_ASSERT_MSG(expr, msg)                                    \
    do {                                                             \
        if(!(expr)) {                                                \
            LV_LOG_ERROR(msg);                                       \
            PXP_ASSERT(false);                                       \
        }                                                            \
    } while(0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

pxp_output_pixel_format_t pxp_get_out_px_format(lv_color_format_t cf);

pxp_as_pixel_format_t pxp_get_as_px_format(lv_color_format_t cf);

#if LV_USE_DRAW_PXP
pxp_ps_pixel_format_t pxp_get_ps_px_format(lv_color_format_t cf);

bool pxp_buf_aligned(const void * buf, uint32_t stride);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_PXP*/
#endif /*LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP*/
#endif /*LV_USE_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PXP_UTILS_H*/
