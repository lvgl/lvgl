/**
 * @file lv_pxp_utils.c
 *
 */

/**
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_pxp_utils.h"

#if LV_USE_DRAW_PXP

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
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

pxp_output_pixel_format_t pxp_get_out_px_format(lv_color_format_t cf)
{
    pxp_output_pixel_format_t out_px_format = kPXP_OutputPixelFormatRGB565;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
            out_px_format = kPXP_OutputPixelFormatRGB565;
            break;
        case LV_COLOR_FORMAT_RGB888:
            out_px_format = kPXP_OutputPixelFormatRGB888P;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            out_px_format = kPXP_OutputPixelFormatARGB8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            out_px_format = kPXP_OutputPixelFormatRGB888;
            break;

        default:
            PXP_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return out_px_format;
}

pxp_as_pixel_format_t pxp_get_as_px_format(lv_color_format_t cf)
{
    pxp_as_pixel_format_t as_px_format = kPXP_AsPixelFormatRGB565;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
            as_px_format = kPXP_AsPixelFormatRGB565;
            break;
        case LV_COLOR_FORMAT_RGB888:
            PXP_ASSERT_MSG(false, "Unsupported color format.");
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            as_px_format = kPXP_AsPixelFormatARGB8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            as_px_format = kPXP_AsPixelFormatRGB888;
            break;

        default:
            PXP_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return as_px_format;
}

pxp_ps_pixel_format_t pxp_get_ps_px_format(lv_color_format_t cf)
{
    pxp_ps_pixel_format_t ps_px_format = kPXP_PsPixelFormatRGB565;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
            ps_px_format = kPXP_PsPixelFormatRGB565;
            break;
        case LV_COLOR_FORMAT_RGB888:
            PXP_ASSERT_MSG(false, "Unsupported color format.");
            break;
        case LV_COLOR_FORMAT_ARGB8888:
#if (!(defined(FSL_FEATURE_PXP_HAS_NO_EXTEND_PIXEL_FORMAT) && FSL_FEATURE_PXP_HAS_NO_EXTEND_PIXEL_FORMAT)) && \
    (!(defined(FSL_FEATURE_PXP_V3) && FSL_FEATURE_PXP_V3))
            ps_px_format = kPXP_PsPixelFormatARGB8888;
#else
            PXP_ASSERT_MSG(false, "Unsupported color format.");
#endif
            break;
        case LV_COLOR_FORMAT_XRGB8888:
#if (!(defined(FSL_FEATURE_PXP_HAS_NO_EXTEND_PIXEL_FORMAT) && FSL_FEATURE_PXP_HAS_NO_EXTEND_PIXEL_FORMAT)) && \
    (!(defined(FSL_FEATURE_PXP_V3) && FSL_FEATURE_PXP_V3))
            ps_px_format = kPXP_PsPixelFormatARGB8888;
#else
            ps_px_format = kPXP_PsPixelFormatRGB888;
#endif
            break;

        default:
            PXP_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return ps_px_format;
}

bool pxp_buf_aligned(const void * buf, uint32_t stride)
{
    /* Test for pointer alignment */
    if((uintptr_t)buf % 64)
        return false;

    /* Test for invalid stride (no stride alignment required) */
    if(stride == 0)
        return false;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_PXP*/
