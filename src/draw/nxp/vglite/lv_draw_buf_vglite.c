/**
 * @file lv_draw_buf_vglite.c
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

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_utils.h"

#include "lvgl_support.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * _buf_malloc(size_t size_bytes, lv_color_format_t cf);

static void * _buf_align(void * buf, lv_color_format_t cf);

static void _invalidate_cache(const void * buf, uint32_t stride, lv_color_format_t cf, const lv_area_t * area);

static uint32_t _width_to_stride(uint32_t w, lv_color_format_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_vglite_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();

    handlers->buf_malloc_cb = _buf_malloc;
    handlers->align_pointer_cb = _buf_align;
    handlers->invalidate_cache_cb = _invalidate_cache;
    handlers->width_to_stride_cb = _width_to_stride;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * _buf_malloc(size_t size_bytes, lv_color_format_t cf)
{
    uint8_t align_bytes = vglite_get_alignment(cf);

    /*Allocate larger memory to be sure it can be aligned as needed*/
    size_bytes += align_bytes - 1;

    return lv_malloc(size_bytes);
}

static void * _buf_align(void * buf, lv_color_format_t cf)
{
    uint8_t align_bytes = vglite_get_alignment(cf);

    uint8_t * buf_u8 = buf;
    if(buf_u8) {
        buf_u8 += align_bytes - 1;
        buf_u8 = (uint8_t *)((lv_uintptr_t)buf_u8 & ~(align_bytes - 1));
    }

    return buf_u8;
}

static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;
    lv_color_format_t cf = header->cf;

    if(area->y1 == 0) {
        uint16_t size = stride * lv_area_get_height(area);

        /* Invalidate full buffer. */
        DEMO_CleanInvalidateCacheByAddr((void *)buf, size);
        return;
    }

    const uint8_t * buf_u8 = buf;
    /* ARM require a 32 byte aligned address. */
    uint8_t align_bytes = 32;
    uint8_t bits_per_pixel = lv_color_format_get_bpp(cf);

    uint16_t align_pixels = align_bytes * 8 / bits_per_pixel;
    uint16_t offset_x = 0;

    if(area->x1 >= (int32_t)(area->x1 % align_pixels)) {
        uint16_t shift_x = area->x1 - (area->x1 % align_pixels);

        offset_x = area->x1 - shift_x;
        buf_u8 += (shift_x * bits_per_pixel) / 8;
    }

    if(area->y1) {
        uint16_t shift_y = area->y1;

        buf_u8 += shift_y * stride;
    }

    /* Area to clear can start from a different offset in buffer.
     * Invalidate the area line by line.
     */
    uint16_t line_pixels = offset_x + lv_area_get_width(area);
    uint16_t line_size = (line_pixels * bits_per_pixel) / 8;
    uint16_t area_height = lv_area_get_height(area);

    for(uint16_t y = 0; y < area_height; y++) {
        const void * line_addr = buf_u8 + y * stride;

        DEMO_CleanInvalidateCacheByAddr((void *)line_addr, line_size);
    }
}

static uint32_t _width_to_stride(uint32_t w, lv_color_format_t cf)
{
    uint8_t bits_per_pixel = lv_color_format_get_bpp(cf);
    uint32_t width_bits = (w * bits_per_pixel + 7) & ~7;
    uint32_t width_bytes = width_bits / 8;
    uint8_t align_bytes = vglite_get_alignment(cf);

    return (width_bytes + align_bytes - 1) & ~(align_bytes - 1);
}

#endif /*LV_USE_DRAW_VGLITE*/
