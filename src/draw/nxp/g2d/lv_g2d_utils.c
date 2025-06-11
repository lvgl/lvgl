/**
 * @file lv_g2d_utils.c
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_g2d_utils.h"

#if LV_USE_DRAW_G2D
#include "lv_g2d_buf_map.h"
#include "lv_draw_g2d.h"

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

g2d_format_t g2d_get_buf_format(lv_color_format_t cf)
{
    g2d_format_t color_f = G2D_RGB565;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
            color_f = G2D_RGB565;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            color_f = G2D_BGRA8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            color_f = G2D_BGRX8888;
            break;
        case LV_COLOR_FORMAT_RGB888:
            color_f = G2D_BGR888;
            break;
        case LV_COLOR_FORMAT_NV12:
            color_f = G2D_NV12;
            break;
        case LV_COLOR_FORMAT_I420:
            color_f = G2D_I420;
            break;
        case LV_COLOR_FORMAT_NV21:
            color_f = G2D_NV21;
            break;
        case LV_COLOR_FORMAT_YUY2:
            color_f = G2D_YUYV;
            break;
        case LV_COLOR_FORMAT_UYVY:
            color_f = G2D_UYVY;
            break;
        default:
            G2D_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }
    return color_f;
}

uint32_t g2d_rgba_to_u32(lv_color_t color)
{
    return (uint32_t)((color.red) + (color.green << 8) + (color.blue << 16) + ((uint32_t)0xff << 24));
}

int32_t g2d_get_buf_fd(const lv_draw_buf_t * draw_buf)
{
    struct g2d_buf * buf = g2d_search_buf_map(draw_buf->data);
    G2D_ASSERT_MSG(buf, "Failed to find buffer in map.");
    return g2d_buf_export_fd(buf);
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /*LV_USE_DRAW_G2D*/