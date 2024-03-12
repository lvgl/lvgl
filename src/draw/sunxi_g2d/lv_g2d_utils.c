/**
 * @file lv_g2d_utils.c
 *
 */

/**
 * Copyright 2023，2024 G2D
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_g2d_utils.h"

#if LV_USE_DRAW_G2D

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

int32_t sunxifb_g2d_get_limit(sunxi_g2d_limit limit)
{
    /* The following data is tested when the cpu frequency is 1.2GHz */
    switch(limit) {
        case SUNXI_G2D_LIMIT_FILL:
            /* In the case of buffer with cache, it will never be as fast as cpu */
            return 2073600;
        case SUNXI_G2D_LIMIT_OPA_FILL:
            /* 110x110=12100, g2d is 311us~357us, cpu is 338us~669us */
            return 12100;
        case SUNXI_G2D_LIMIT_BLIT:
            /* 220x220=48400, g2d is 373us~577us, cpu is 438us~751us */
            return 48400;
        case SUNXI_G2D_LIMIT_BLEND:
            /* 100x100=10000, g2d is 176us~326us, cpu is 196us~400us */
            return 10000;
        case SUNXI_G2D_LIMIT_SCALE:
            /* 50x50=2500, g2d is 606us~870us, cpu is 777us~949us */
            return 2500;
        default:
            return 0;
    }
}

g2d_fmt_enh g2d_get_px_format(lv_color_format_t cf)
{
    g2d_fmt_enh out_px_format = G2D_FORMAT_RGB565;

    switch(cf) {
        /*2 byte (+alpha) formats*/
        case LV_COLOR_FORMAT_RGB565:
            out_px_format = G2D_FORMAT_RGB565;
            break;
        case LV_COLOR_FORMAT_RGB565A8:
            LV_ASSERT_MSG(false, "Unsupported color format.");
            break;

        /*3 byte (+alpha) formats*/
        case LV_COLOR_FORMAT_RGB888:
            out_px_format = G2D_FORMAT_RGB888;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            out_px_format = G2D_FORMAT_ARGB8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            out_px_format = G2D_FORMAT_XRGB8888;
            break;

        default:
            LV_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return out_px_format;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_G2D*/
