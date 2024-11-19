/**
 * @file lv_draw_buf_pxp.c
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

#include "lv_draw_pxp.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP
#include "../../lv_draw_buf_private.h"
#include "lv_pxp_cfg.h"
#include "lv_pxp_utils.h"

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

static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_pxp_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_buf_handlers_t * font_handlers = lv_draw_buf_get_font_handlers();
    lv_draw_buf_handlers_t * image_handlers = lv_draw_buf_get_image_handlers();

    handlers->invalidate_cache_cb = _invalidate_cache;
    font_handlers->invalidate_cache_cb = _invalidate_cache;
    image_handlers->invalidate_cache_cb = _invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    const lv_image_header_t * header = &draw_buf->header;
    uint32_t stride = header->stride;
    lv_color_format_t cf = header->cf;

    if(area->y1 == 0) {
        uint32_t size = stride * lv_area_get_height(area);

        /* Invalidate full buffer. */
        DEMO_CleanInvalidateCacheByAddr((void *)draw_buf->data, size);
        return;
    }

    const uint8_t * buf_u8 = draw_buf->data;
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

#endif /*LV_USE_DRAW_PXP*/
#endif /*LV_USE_PXP*/
