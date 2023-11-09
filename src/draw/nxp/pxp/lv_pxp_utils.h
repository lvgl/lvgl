/**
 * @file lv_pxp_utils.h
 *
 */

/**
 * Copyright 2023 NXP
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

#if LV_USE_DRAW_PXP
#include "fsl_pxp.h"
#include "../../../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

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

pxp_ps_pixel_format_t pxp_get_ps_px_format(lv_color_format_t cf);

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_PXP*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PXP_UTILS_H*/
