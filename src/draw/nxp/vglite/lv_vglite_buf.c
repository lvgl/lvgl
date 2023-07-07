/**
 * @file lv_vglite_buf.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */


/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_buf.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_utils.h"

#include "../../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void _set_vgbuf_ptr(vg_lite_buffer_t * vgbuf, const uint8_t * buf);

static inline uint32_t _get_bits_per_pixel(vg_lite_buffer_format_t format);

/**********************
 *  STATIC VARIABLES
 **********************/

static vg_lite_buffer_t _dest_vgbuf;
static vg_lite_buffer_t _src_vgbuf;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

vg_lite_buffer_t * vglite_get_dest_buf(void)
{
    return &_dest_vgbuf;
}

vg_lite_buffer_t * vglite_get_src_buf(void)
{
    return &_src_vgbuf;
}

void vglite_set_dest_buf_ptr(const uint8_t * buf)
{
    _set_vgbuf_ptr(&_dest_vgbuf, buf);
}

void vglite_set_src_buf_ptr(const uint8_t * buf)
{
    _set_vgbuf_ptr(&_src_vgbuf, buf);
}

void vglite_set_dest_buf(const uint8_t * buf, const lv_area_t * area, lv_coord_t stride,
                         lv_color_format_t cf)
{
    vg_lite_buffer_format_t vgformat = vglite_get_buf_format(cf);

    vglite_set_buf(&_dest_vgbuf, buf, area, stride, vgformat);
}

void vglite_set_src_buf(const uint8_t * buf, const lv_area_t * area, lv_coord_t stride,
                        lv_color_format_t cf)
{
    vg_lite_buffer_format_t vgformat = vglite_get_buf_format(cf);

    vglite_set_buf(&_src_vgbuf, buf, area, stride, vgformat);
}

void vglite_set_buf(vg_lite_buffer_t * vgbuf, const uint8_t * buf,
                    const lv_area_t * area, lv_coord_t stride,
                    vg_lite_buffer_format_t vgformat)
{
    int32_t bpp = _get_bits_per_pixel(vgformat);

    vgbuf->format = vgformat;
    vgbuf->tiled = VG_LITE_LINEAR;
    vgbuf->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    vgbuf->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    vgbuf->width = (int32_t)lv_area_get_width(area);
    vgbuf->height = (int32_t)lv_area_get_height(area);
    vgbuf->stride = (int32_t)(stride) * bpp / 8;

    lv_memset(&vgbuf->yuv, 0, sizeof(vgbuf->yuv));

    vgbuf->memory = (void *)buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
    vgbuf->handle = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline uint32_t _get_bits_per_pixel(vg_lite_buffer_format_t format)
{
    uint32_t bpp = 32;

    switch(format) {
        case VG_LITE_INDEX_1:
            bpp = 1;
            break;
        case VG_LITE_INDEX_2:
            bpp = 2;
            break;
        case VG_LITE_INDEX_4:
            bpp = 4;
            break;
        case VG_LITE_INDEX_8:
        case VG_LITE_A8:
        case VG_LITE_L8:
            bpp = 8;
            break;
        case VG_LITE_BGR565:
            bpp = 16;
            break;
        case VG_LITE_BGRA8888:
        case VG_LITE_BGRX8888:
            bpp = 32;
            break;

        default:
            LV_ASSERT_MSG(false, "Unsupported buffer format.");
            break;
    }

    return bpp;
}

static inline void _set_vgbuf_ptr(vg_lite_buffer_t * vgbuf, const uint8_t * buf)
{
    vgbuf->memory = (void *)buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
}

#endif /*LV_USE_DRAW_VGLITE*/
