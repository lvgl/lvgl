/**
 * @file lv_draw_buf_vglite.c
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

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_matrix.h"
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

static void * _align_buf(void * buf, lv_color_format_t cf);

static void _invalidate_cache(lv_draw_buf_t * draw_buf, const char * area);

static uint32_t _width_to_stride(uint32_t w, lv_color_format_t cf);

static void * _go_to_xy(lv_draw_buf_t * draw_buf, int32_t x, int32_t y);

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
    handlers->align_pointer_cb = _align_buf;
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

static void * _align_buf(void * buf, lv_color_format_t cf)
{
    uint8_t align_bytes = vglite_get_alignment(cf);

    uint8_t * buf_u8 = buf;
    if(buf_u8) {
        buf_u8 += align_bytes - 1;
        buf_u8 = (uint8_t *)((lv_uintptr_t)buf_u8 & ~(align_bytes - 1));
    }

    return buf_u8;
}

static void _invalidate_cache(lv_draw_buf_t * draw_buf, const char * area)
{
    LV_UNUSED(draw_buf);
    LV_UNUSED(area);

    DEMO_CleanInvalidateCache();
}

static uint32_t _width_to_stride(uint32_t w, lv_color_format_t cf)
{
    uint8_t bits_per_pixel = vglite_get_px_size(cf);
    uint32_t width_bits = (w * bits_per_pixel + 7) & ~7;
    uint32_t width_bytes = width_bits / 8;
    uint8_t align_bytes = vglite_get_alignment(cf);

    return (width_bytes + align_bytes - 1) & ~(align_bytes - 1);
}

/**
 * @todo
 * LVGL needs to use hardware acceleration for buf_copy and do not affect GPU rendering.
 */
#if 0

static void _vglite_buf_clear(lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    uint32_t stride = lv_draw_buf_get_stride(draw_buf);

    /* Set vgbuf structure. */
    vg_lite_buffer_t vgbuf;
    vglite_set_buf(&vgbuf, draw_buf->buf, draw_buf->width, draw_buf->height, stride, draw_buf->color_format);

    vg_lite_color_t vgcol = 0;

    vg_lite_rectangle_t rect = {
        .x = area->x1,
        .y = area->y1,
        .width = lv_area_get_width(area),
        .height = lv_area_get_height(area)
    };

    vg_lite_error_t err = vg_lite_clear(&vgbuf, &rect, vgcol);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Clear failed.");

    vglite_run();
}

static void _vglite_buf_copy(void * dest_buf, uint32_t dest_stride, const lv_area_t * dest_area,
                             void * src_buf, uint32_t src_stride, const lv_area_t * src_area,
                             lv_color_format_t cf)
{
    /* Set src_vgbuf structure. */
    vg_lite_buffer_t src_vgbuf;
    vglite_set_buf(&src_vgbuf, src_buf, lv_area_get_width(src_area), lv_area_get_height(src_area), src_stride, cf);

    /* Set dest_vgbuf structure. */
    vg_lite_buffer_t dest_vgbuf;
    vglite_set_buf(&dest_vgbuf, dest_buf, lv_area_get_width(dest_area), lv_area_get_height(dest_area), dest_stride, cf);

    uint32_t rect[] = {
        (uint32_t)src_area->x1, /* start x */
        (uint32_t)src_area->y1, /* start y */
        (uint32_t)lv_area_get_width(src_area), /* width */
        (uint32_t)lv_area_get_height(src_area) /* height */
    };

    /* Set scissor. */
    vglite_set_scissor(dest_area);

    /* Set vgmatrix. */
    vglite_set_translation_matrix(dest_area);
    vg_lite_matrix_t * vgmatrix = vglite_get_matrix();

    vg_lite_error_t err = vg_lite_blit_rect(&dest_vgbuf, &src_vgbuf, rect, vgmatrix,
                                            VG_LITE_BLEND_NONE, 0xFFFFFFFFU, VG_LITE_FILTER_POINT);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Blit rectangle failed.");

    vglite_run();

    /* Disable scissor. */
    vglite_disable_scissor();
}
#endif

#endif /*LV_USE_DRAW_VGLITE*/
