/**
 * @file lv_draw_eve5_image_download.c
 *
 * EVE5 (BT820) Image Download from RAM_G to CPU Memory
 *
 * Reverse format conversion for VRAM download: reads pixel data from
 * GPU memory (EVE format) and converts to the LVGL color format stored
 * in the draw_buf header. Mirrors the upload conversions in
 * lv_draw_eve5_image_upload.c.
 *
 * Primary use case: vram_download_cb called by lv_draw_buf_ensure_resident
 * when migrating a buffer from VRAM to CPU.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"
#include "../../drivers/display/eve5/lv_eve5_image_private.h"

#if LV_USE_DRAW_EVE5

/**********************
 * STATIC PROTOTYPES
 **********************/

static void convert_rgb8_to_xrgb8888(const uint8_t *src, uint8_t *dst, uint32_t w);
static void convert_argb8_to_rgb565a8(const uint8_t *src, uint8_t *rgb_dst, uint8_t *alpha_dst, uint32_t w);
static void convert_rgb565_to_swapped(const uint8_t *src, uint8_t *dst, uint32_t w);

/**********************
 * FORMAT CONVERSIONS
 **********************/

/** Convert RGB8 (BGR, 3 bpp) to XRGB8888 (BGRX, 4 bpp). Inserts alpha=0xFF. */
static void convert_rgb8_to_xrgb8888(const uint8_t *src, uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        dst[4 * x + 0] = src[3 * x + 0];  /* B */
        dst[4 * x + 1] = src[3 * x + 1];  /* G */
        dst[4 * x + 2] = src[3 * x + 2];  /* R */
        dst[4 * x + 3] = 0xFF;            /* X */
    }
}

/** Convert ARGB8 (BGRA, 4 bpp) to RGB565 + separate A8 plane. */
static void convert_argb8_to_rgb565a8(const uint8_t *src, uint8_t *rgb_dst, uint8_t *alpha_dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        uint8_t b = src[4 * x + 0];
        uint8_t g = src[4 * x + 1];
        uint8_t r = src[4 * x + 2];
        uint8_t a = src[4 * x + 3];

        uint16_t rgb565 = ((uint16_t)(r >> 3) << 11) |
                           ((uint16_t)(g >> 2) << 5) |
                           ((uint16_t)(b >> 3));
        ((uint16_t *)rgb_dst)[x] = rgb565;
        alpha_dst[x] = a;
    }
}

/** Byte-swap RGB565 pixels. */
static void convert_rgb565_to_swapped(const uint8_t *src, uint8_t *dst, uint32_t w)
{
    const uint16_t *s = (const uint16_t *)src;
    uint16_t *d = (uint16_t *)dst;
    for(uint32_t x = 0; x < w; x++) {
        uint16_t v = s[x];
        d[x] = (v >> 8) | (v << 8);
    }
}

/**********************
 * DOWNLOAD API
 **********************/

/**
 * Download image data from GPU RAM_G to a CPU draw_buf.
 *
 * Reads from the GPU allocation described by `vr` and writes to `buf->data`
 * in the LVGL color format specified by `buf->header.cf`. Handles all format
 * conversions that were applied during upload (in reverse).
 *
 * LVGL pre-allocates `buf->data` with sufficient size before calling this.
 */
bool lv_draw_eve5_download_image(lv_draw_eve5_unit_t *u,
                                  lv_draw_buf_t *buf,
                                  const lv_eve5_vram_res_t *vr)
{
    if(buf->data == NULL) return false;

    uint32_t gpu_addr = Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle);
    if(gpu_addr == GA_INVALID) return false;
    gpu_addr += vr->source_offset;

    int32_t w = buf->header.w;
    int32_t h = buf->header.h;
    lv_color_format_t lv_cf = (lv_color_format_t)buf->header.cf;
    uint16_t eve_fmt = vr->eve_format;
    uint32_t eve_stride = vr->stride;
    uint32_t lv_stride = buf->header.stride;
    if(lv_stride == 0) lv_stride = lv_draw_buf_width_to_stride(w, lv_cf);

    /* Check if format conversion is needed */
    bool needs_conversion = false;
    switch(lv_cf) {
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_I8:
            needs_conversion = true;
            break;
        default:
            {
                uint16_t expected_eve_fmt;
                uint8_t expected_bpp;
                bool upload_converts;
                if(lv_draw_eve5_get_eve_format_info(lv_cf, &expected_eve_fmt,
                                                     &expected_bpp, &upload_converts)) {
                    if(eve_fmt != expected_eve_fmt) needs_conversion = true;
                }
            }
            break;
    }

    if(!needs_conversion) {
        uint32_t lv_bpp = lv_color_format_get_bpp(lv_cf);
        uint32_t row_bytes = (w * lv_bpp + 7) / 8;

        if(eve_stride == lv_stride) {
            EVE_Hal_rdMem(u->hal, buf->data, gpu_addr, lv_stride * h);
        }
        else {
            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, buf->data + y * lv_stride,
                              gpu_addr + y * eve_stride, row_bytes);
            }
        }
        return true;
    }

    /* Conversion path: read into temp buffer, convert per row */
    uint8_t *row_buf = lv_malloc(eve_stride);
    if(row_buf == NULL) {
        LV_LOG_ERROR("EVE5: Failed to allocate download conversion buffer");
        return false;
    }

    switch(lv_cf) {
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, row_buf, gpu_addr + y * eve_stride, w * 2);
                convert_rgb565_to_swapped(row_buf, buf->data + y * lv_stride, w);
            }
            break;

        case LV_COLOR_FORMAT_XRGB8888:
            /* EVE stores RGB8 (3 bpp) → expand to XRGB8888 (4 bpp) */
            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, row_buf, gpu_addr + y * eve_stride, w * 3);
                convert_rgb8_to_xrgb8888(row_buf, buf->data + y * lv_stride, w);
            }
            break;

        case LV_COLOR_FORMAT_RGB565A8:
            /* EVE stores ARGB8 → split into RGB565 plane + A8 plane.
             * LVGL layout: [RGB565 rows][A8 rows] */
        {
            uint32_t rgb_stride = (w * 16 + 7) / 8;
            if(lv_stride > 0) rgb_stride = lv_stride;
            uint32_t alpha_stride = rgb_stride / 2;
            uint8_t *alpha_plane = buf->data + rgb_stride * h;

            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, row_buf, gpu_addr + y * eve_stride, w * 4);
                convert_argb8_to_rgb565a8(row_buf,
                                           buf->data + y * rgb_stride,
                                           alpha_plane + y * alpha_stride, w);
            }
            break;
        }

        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        {
            /* EVE stores PALETTEDARGB8 (8-bit indices + 256-entry palette).
             * LVGL layout: [palette N×ARGB8888][packed sub-byte index data] */
            if(eve_fmt != PALETTEDARGB8) {
                LV_LOG_WARN("EVE5: Cannot download %d format to indexed I%d (no quantization)",
                            eve_fmt, lv_color_format_get_bpp(lv_cf));
                lv_free(row_buf);
                return false;
            }
            uint32_t src_palette_entries = LV_COLOR_INDEXED_PALETTE_SIZE(lv_cf);
            uint32_t src_palette_bytes = src_palette_entries * sizeof(lv_color32_t);
            uint32_t bpp = lv_color_format_get_bpp(lv_cf);
            uint32_t pixels_per_byte = 8 / bpp;

            /* Download palette */
            if(vr->palette_offset != GA_INVALID) {
                uint32_t pal_addr = Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle) + vr->palette_offset;
                EVE_Hal_rdMem(u->hal, buf->data, pal_addr, src_palette_bytes);
            }
            else {
                lv_memzero(buf->data, src_palette_bytes);
            }

            /* Pack 8-bit indices to sub-byte, MSB-first */
            uint8_t *index_dst = buf->data + src_palette_bytes;
            uint32_t dst_row_bytes = (w * bpp + 7) / 8;

            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, row_buf, gpu_addr + y * eve_stride, w);

                uint8_t *dst_row = index_dst + y * lv_stride;
                lv_memzero(dst_row, dst_row_bytes);
                for(int32_t x = 0; x < w; x++) {
                    uint32_t byte_idx = x / pixels_per_byte;
                    uint32_t bit_shift = (pixels_per_byte - 1 - (x % pixels_per_byte)) * bpp;
                    uint8_t index_val = row_buf[x] & ((1u << bpp) - 1u);
                    dst_row[byte_idx] |= index_val << bit_shift;
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_I8:
        {
            /* EVE stores PALETTEDARGB8 (8-bit indices + 256-entry palette).
             * LVGL layout: [palette 256×ARGB8888][8-bit index data] */
            if(eve_fmt != PALETTEDARGB8) {
                LV_LOG_WARN("EVE5: Cannot download %d format to indexed I8 (no quantization)",
                            eve_fmt);
                lv_free(row_buf);
                return false;
            }
            uint32_t palette_bytes = 256 * sizeof(lv_color32_t);

            if(vr->palette_offset != GA_INVALID) {
                uint32_t pal_addr = Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle) + vr->palette_offset;
                EVE_Hal_rdMem(u->hal, buf->data, pal_addr, palette_bytes);
            }
            else {
                lv_memzero(buf->data, palette_bytes);
            }

            uint8_t *index_dst = buf->data + palette_bytes;
            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, index_dst + y * lv_stride,
                              gpu_addr + y * eve_stride, w);
            }
            break;
        }

        default:
        {
            /* Generic mismatch: expand EVE pixels to ARGB8, then pack to LVGL format */
            int32_t eve_bpp = eve5_format_bpp(eve_fmt);
            uint32_t eve_row_bytes = (uint32_t)(w * eve_bpp);

            uint8_t *argb_row = lv_malloc(w * 4);
            if(argb_row == NULL) {
                LV_LOG_ERROR("EVE5: Failed to allocate ARGB8 conversion buffer");
                lv_free(row_buf);
                return false;
            }

            for(int32_t y = 0; y < h; y++) {
                EVE_Hal_rdMem(u->hal, row_buf, gpu_addr + y * eve_stride, eve_row_bytes);

                /* Expand EVE format to ARGB8 (B,G,R,A in memory) */
                for(int32_t x = 0; x < w; x++) {
                    uint8_t r = 0, g = 0, b = 0, a = 255;
                    switch(eve_fmt) {
                        case ARGB8:
                            b = row_buf[4 * x + 0];
                            g = row_buf[4 * x + 1];
                            r = row_buf[4 * x + 2];
                            a = row_buf[4 * x + 3];
                            break;
                        case RGB8:
                            b = row_buf[3 * x + 0];
                            g = row_buf[3 * x + 1];
                            r = row_buf[3 * x + 2];
                            break;
                        case RGB565: {
                            uint16_t c = ((uint16_t *)row_buf)[x];
                            uint8_t r5 = (c >> 11) & 0x1F;
                            uint8_t g6 = (c >> 5) & 0x3F;
                            uint8_t b5 = c & 0x1F;
                            r = (r5 << 3) | (r5 >> 2);
                            g = (g6 << 2) | (g6 >> 4);
                            b = (b5 << 3) | (b5 >> 2);
                            break;
                        }
                        case ARGB1555: {
                            uint16_t c = ((uint16_t *)row_buf)[x];
                            a = (c & 0x8000) ? 255 : 0;
                            uint8_t r5 = (c >> 10) & 0x1F;
                            uint8_t g5 = (c >> 5) & 0x1F;
                            uint8_t b5 = c & 0x1F;
                            r = (r5 << 3) | (r5 >> 2);
                            g = (g5 << 3) | (g5 >> 2);
                            b = (b5 << 3) | (b5 >> 2);
                            break;
                        }
                        case ARGB4: {
                            uint16_t c = ((uint16_t *)row_buf)[x];
                            uint8_t a4 = (c >> 12) & 0xF;
                            uint8_t r4 = (c >> 8) & 0xF;
                            uint8_t g4 = (c >> 4) & 0xF;
                            uint8_t b4 = c & 0xF;
                            a = (a4 << 4) | a4;
                            r = (r4 << 4) | r4;
                            g = (g4 << 4) | g4;
                            b = (b4 << 4) | b4;
                            break;
                        }
                        case L8:
                            r = g = b = row_buf[x];
                            break;
                        case ARGB2: {
                            uint8_t c = row_buf[x];
                            uint8_t a2 = (c >> 6) & 0x3;
                            uint8_t r2 = (c >> 4) & 0x3;
                            uint8_t g2 = (c >> 2) & 0x3;
                            uint8_t b2 = c & 0x3;
                            a = a2 * 85;
                            r = r2 * 85;
                            g = g2 * 85;
                            b = b2 * 85;
                            break;
                        }
                        default:
                            break;
                    }
                    argb_row[4 * x + 0] = b;
                    argb_row[4 * x + 1] = g;
                    argb_row[4 * x + 2] = r;
                    argb_row[4 * x + 3] = a;
                }

                /* Pack ARGB8 to target LVGL format */
                uint8_t *dst_row = buf->data + y * lv_stride;
                switch(lv_cf) {
                    case LV_COLOR_FORMAT_ARGB8888:
                    case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
                        lv_memcpy(dst_row, argb_row, w * 4);
                        break;
                    case LV_COLOR_FORMAT_XRGB8888:
                        for(int32_t x = 0; x < w; x++) {
                            dst_row[4 * x + 0] = argb_row[4 * x + 0];
                            dst_row[4 * x + 1] = argb_row[4 * x + 1];
                            dst_row[4 * x + 2] = argb_row[4 * x + 2];
                            dst_row[4 * x + 3] = 0xFF;
                        }
                        break;
                    case LV_COLOR_FORMAT_RGB888:
                        for(int32_t x = 0; x < w; x++) {
                            dst_row[3 * x + 0] = argb_row[4 * x + 0];
                            dst_row[3 * x + 1] = argb_row[4 * x + 1];
                            dst_row[3 * x + 2] = argb_row[4 * x + 2];
                        }
                        break;
                    case LV_COLOR_FORMAT_RGB565: {
                        uint16_t *d = (uint16_t *)dst_row;
                        for(int32_t x = 0; x < w; x++) {
                            d[x] = ((uint16_t)(argb_row[4 * x + 2] >> 3) << 11) |
                                   ((uint16_t)(argb_row[4 * x + 1] >> 2) << 5) |
                                   ((uint16_t)(argb_row[4 * x + 0] >> 3));
                        }
                        break;
                    }
                    case LV_COLOR_FORMAT_ARGB1555: {
                        uint16_t *d = (uint16_t *)dst_row;
                        for(int32_t x = 0; x < w; x++) {
                            d[x] = ((uint16_t)(argb_row[4 * x + 3] >> 7) << 15) |
                                   ((uint16_t)(argb_row[4 * x + 2] >> 3) << 10) |
                                   ((uint16_t)(argb_row[4 * x + 1] >> 3) << 5) |
                                   ((uint16_t)(argb_row[4 * x + 0] >> 3));
                        }
                        break;
                    }
                    case LV_COLOR_FORMAT_ARGB4444: {
                        uint16_t *d = (uint16_t *)dst_row;
                        for(int32_t x = 0; x < w; x++) {
                            d[x] = ((uint16_t)(argb_row[4 * x + 3] >> 4) << 12) |
                                   ((uint16_t)(argb_row[4 * x + 2] >> 4) << 8) |
                                   ((uint16_t)(argb_row[4 * x + 1] >> 4) << 4) |
                                   ((uint16_t)(argb_row[4 * x + 0] >> 4));
                        }
                        break;
                    }
                    case LV_COLOR_FORMAT_L8:
                    case LV_COLOR_FORMAT_A8:
                        for(int32_t x = 0; x < w; x++) {
                            dst_row[x] = argb_row[4 * x + 3];
                        }
                        break;
                    default:
                        LV_LOG_WARN("EVE5: Download unsupported target format %d", lv_cf);
                        lv_memcpy(dst_row, argb_row, LV_MIN(lv_stride, (uint32_t)(w * 4)));
                        break;
                }
            }

            lv_free(argb_row);
            break;
        }
    }

    lv_free(row_buf);
    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
