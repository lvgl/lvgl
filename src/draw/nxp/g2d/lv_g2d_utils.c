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

#if LV_USE_G2D
#if LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D
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
static void * g2d_handle;
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

void g2d_set_handle(void * handle)
{
    g2d_handle = handle;
}
void * g2d_get_handle(void)
{
    return g2d_handle;
}

#if LV_USE_ROTATE_G2D
void g2d_rotate(lv_draw_buf_t * buf1, lv_draw_buf_t * buf2, int32_t width, int32_t height, uint32_t rotation,
                lv_color_format_t cf)
{
    struct g2d_surface src_surf, dst_surf;
    struct g2d_buf * src_buf = g2d_search_buf_map(buf1->data);
    struct g2d_buf * dst_buf = g2d_search_buf_map(buf2->data);
    bool has_rotation = (rotation != 0);

    int32_t src_width = width;
    int32_t src_height = height;
    if(has_rotation) {
        if(rotation == LV_DISPLAY_ROTATION_90 || rotation == LV_DISPLAY_ROTATION_270) {
            src_width = height;
            src_height = width;
        }
    }

    src_surf.format = g2d_get_buf_format(cf);

    src_surf.left   = 0;
    src_surf.top    = 0;
    src_surf.right  = src_width;
    src_surf.bottom = src_height;
    src_surf.stride = src_width;
    src_surf.width  = src_width;
    src_surf.height = src_height;

    src_surf.planes[0] = src_buf->buf_paddr;
    src_surf.rot = G2D_ROTATION_0;

    enum g2d_rotation g2d_angle = G2D_ROTATION_0;
    if(has_rotation) {
        switch(rotation) {
            case LV_DISPLAY_ROTATION_0:
                g2d_angle = G2D_ROTATION_0;
                break;
            case LV_DISPLAY_ROTATION_90:
                g2d_angle = G2D_ROTATION_90;
                break;
            case LV_DISPLAY_ROTATION_180:
                g2d_angle = G2D_ROTATION_180;
                break;
            case LV_DISPLAY_ROTATION_270:
                g2d_angle = G2D_ROTATION_270;
                break;
            default:
                g2d_angle = G2D_ROTATION_0;
        }
    }

    dst_surf.format = g2d_get_buf_format(cf);

    dst_surf.left   = 0;
    dst_surf.top    = 0;
    dst_surf.right  = width;
    dst_surf.bottom = height;
    dst_surf.stride = width;
    dst_surf.width  = width;
    dst_surf.height = height;

    dst_surf.planes[0] = dst_buf->buf_paddr;
    dst_surf.rot = g2d_angle;

    void * handle = g2d_get_handle();
    g2d_blit(handle, &src_surf, &dst_surf);
    g2d_finish(handle);
}
#endif

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /*LV_USE_DRAW_G2D || LV_USE_ROTATE_G2D*/
#endif /*LV_USE_G2D*/
