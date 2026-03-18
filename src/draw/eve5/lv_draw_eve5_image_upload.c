/**
 * @file lv_draw_eve5_image_upload.c
 *
 * EVE5 (BT820) Image Format Conversion and Upload to RAM_G
 *
 * Separated from lv_draw_eve5_image_load.c. Contains:
 * - Pixel format conversion (RGB565A8→ARGB8, XRGB→RGB8, byteswap, etc.)
 * - LVGL→EVE format mapping
 * - Image upload to RAM_G (cached and uncached variants)
 *
 * The image loading/decode paths (SD card, flash, HW decode, SW decode,
 * image decoder registration) remain in lv_draw_eve5_image_load.c.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"

/**********************
 * STATIC PROTOTYPES
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w);
static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w);
static void convert_rgb565_byteswap(const uint8_t *src, uint8_t *dst, uint32_t w);

/**********************
 * IMAGE FORMAT CONVERSION
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        uint16_t rgb565 = ((const uint16_t *)rgb)[x];

        /* Expand 5/6/5 bits to 8 bits with proper bit replication for accuracy */
        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;

        uint8_t r = (r5 << 3) | (r5 >> 2);  /* 5-bit to 8-bit */
        uint8_t g = (g6 << 2) | (g6 >> 4);  /* 6-bit to 8-bit */
        uint8_t b = (b5 << 3) | (b5 >> 2);  /* 5-bit to 8-bit */

        /* EVE ARGB8 is BGRA in memory (little-endian) */
        dst[4 * x + 0] = b;
        dst[4 * x + 1] = g;
        dst[4 * x + 2] = r;
        dst[4 * x + 3] = alpha[x];
    }
}

static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        /* LVGL XRGB8888 is BGRX in memory */
        dst[3 * x + 0] = src[4 * x + 0];  /* B */
        dst[3 * x + 1] = src[4 * x + 1];  /* G */
        dst[3 * x + 2] = src[4 * x + 2];  /* R */
        /* Skip X (alpha) byte */
    }
}

static void convert_rgb565_byteswap(const uint8_t *src, uint8_t *dst, uint32_t w)
{
    const uint16_t *s = (const uint16_t *)src;
    uint16_t *d = (uint16_t *)dst;
    for(uint32_t x = 0; x < w; x++) {
        uint16_t v = s[x];
        d[x] = (v >> 8) | (v << 8);
    }
}

/**********************
 * FORMAT MAPPING
 **********************/

bool lv_draw_eve5_get_eve_format_info(lv_color_format_t src_cf,
                                 uint16_t *eve_format,
                                 uint8_t *bits_per_pixel,
                                 bool *needs_conversion)
{
    *needs_conversion = false;

    switch(src_cf) {
        case LV_COLOR_FORMAT_A1:
            *eve_format = L1;
            *bits_per_pixel = 1;
            break;

        case LV_COLOR_FORMAT_A2:
            *eve_format = L2;
            *bits_per_pixel = 2;
            break;

        case LV_COLOR_FORMAT_A4:
            *eve_format = L4;
            *bits_per_pixel = 4;
            break;

        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A8:
            *eve_format = L8;
            *bits_per_pixel = 8;
            break;

        case LV_COLOR_FORMAT_ARGB2222:
            *eve_format = ARGB2;
            *bits_per_pixel = 8;
            break;

        case LV_COLOR_FORMAT_RGB565:
            *eve_format = RGB565;
            *bits_per_pixel = 16;
            break;

        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            *eve_format = RGB565;
            *bits_per_pixel = 16;
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_ARGB1555:
            *eve_format = ARGB1555;
            *bits_per_pixel = 16;
            break;

        case LV_COLOR_FORMAT_ARGB4444:
            *eve_format = ARGB4;
            *bits_per_pixel = 16;
            break;

        case LV_COLOR_FORMAT_RGB888:
            *eve_format = RGB8;
            *bits_per_pixel = 24;
            break;

        case LV_COLOR_FORMAT_XRGB8888:
            *eve_format = RGB8;
            *bits_per_pixel = 24;
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            *eve_format = ARGB8;
            *bits_per_pixel = 32;
            break;

        case LV_COLOR_FORMAT_RGB565A8:
            *eve_format = ARGB8;
            *bits_per_pixel = 32;
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
            *eve_format = PALETTEDARGB8;
            *bits_per_pixel = 8; /* Expanded to 8-bit indices for EVE */
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_I8:
            *eve_format = PALETTEDARGB8;
            *bits_per_pixel = 8;
            *needs_conversion = true; /* Needs palette prefix upload */
            break;

        default:
            LV_LOG_WARN("EVE5: Unsupported image format %d", src_cf);
            return false;
    }

    return true;
}

/**********************
 * IMAGE UPLOAD (CACHED) — HANDLE-BASED
 **********************/

bool lv_draw_eve5_upload_image_to_gpu(lv_draw_eve5_unit_t *u,
                                       const lv_image_dsc_t *img_dsc,
                                       eve5_gpu_image_t *out)
{
    const uint8_t *src_buf = img_dsc->data;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    uint16_t eve_format;
    uint8_t bpp; /* bits per pixel (EVE output format) */
    bool needs_conversion;

    if(!lv_draw_eve5_get_eve_format_info(src_cf, &eve_format, &bpp, &needs_conversion)) {
        return false;
    }

    /* Default source stride: use source format's BPP (may differ from EVE bpp for conversions) */
    if(src_stride == 0) {
        uint32_t src_bpp = lv_color_format_get_bpp(src_cf);
        src_stride = (src_w * src_bpp + 7) / 8;
    }

    int32_t eve_stride = ALIGN_UP((src_w * bpp + 7) / 8, 4);
    int32_t eve_size = eve_stride * src_h;

    /* Palette size for indexed formats.
     * EVE PALETTEDARGB8 always uses 256-entry ARGB8888 palette (1024 bytes).
     * For I1/I2/I4 the source palette is smaller but we pad to 256 entries. */
    uint16_t palette_size = 0;
    if(LV_COLOR_FORMAT_IS_INDEXED(src_cf)) {
        palette_size = 256 * sizeof(lv_color32_t); /* 1024 bytes */
    }

    /* Check canvas cache first — if this draw_buf was rendered to by EVE5,
     * use the GPU texture directly without re-uploading stale CPU data.
     * The canvas may be in native format (e.g., RGB565 from direct image load)
     * or ARGB8 (from rendered content). */
    {
        lv_draw_eve5_canvas_cache_t *ccache = &u->canvas_cache;
        for(uint32_t i = 0; i < EVE5_CANVAS_CACHE_CAPACITY; i++) {
            lv_draw_eve5_canvas_cache_entry_t *e = &ccache->entries[i];
            if(!e->valid || e->data_ptr != src_buf) continue;
            uint32_t addr = Esd_GpuAlloc_Get(u->allocator, e->gpu_handle);
            if(addr == GA_INVALID) { e->valid = false; continue; }
            out->gpu_handle = e->gpu_handle;
            out->eve_format = e->eve_format;
            out->eve_stride = (int32_t)e->stride;
            out->width = (int32_t)e->width;
            out->height = (int32_t)e->height;
            out->image_offset = e->source_offset;
            /* palette_addr in canvas cache is a raw address — compute offset
             * from allocation base. Safe as long as no defrag occurs between
             * insertion and lookup (which doesn't happen in practice). */
            out->palette_offset = (e->palette_addr != 0 && e->palette_addr != GA_INVALID)
                                  ? (e->palette_addr - addr) : GA_INVALID;
            LV_LOG_TRACE("EVE5: Canvas GPU texture for image at 0x%08X (fmt=%d)",
                         addr + e->source_offset, e->eve_format);
            return true;
        }
    }

    /* Check image cache */
    if(lv_draw_eve5_image_cache_find(u, img_dsc, out)) {
        return true;
    }

    /* Allocate RAM_G space (palette + index data for indexed formats) */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, palette_size + eve_size, GA_ALIGN_4);
    uint32_t base_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(base_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate image in RAM_G (%u bytes)", palette_size + eve_size);
        return false;
    }

    /* For paletted formats, index data starts after palette */
    uint32_t ram_g_addr = base_addr + palette_size;

    /* Upload based on format */
    if(!needs_conversion) {
        /* Direct copy — native EVE format, no per-pixel conversion needed.
         * Covers: A1, A2, A4, L8, A8, ARGB2222, RGB565, ARGB1555,
         * ARGB4444, RGB888, ARGB8888. */
        int32_t row_bytes = (src_w * bpp + 7) / 8;
        if(eve_stride == src_stride) {
            EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
        }
        else {
            for(int32_t y = 0; y < src_h; y++) {
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                              src_buf + y * src_stride, row_bytes);
            }
        }
    }
    else switch(src_cf) {
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4: {
            /* LVGL I1/I2/I4 layout: [palette N×ARGB8888][packed index data]
             * where N = 2/4/16 entries for I1/I2/I4.
             * EVE PALETTEDARGB8 needs 256-entry palette + 8-bit indices.
             * Expand palette to 256 entries (pad with transparent black)
             * and unpack sub-byte indices to 8-bit. */
            uint32_t src_palette_entries = LV_COLOR_INDEXED_PALETTE_SIZE(src_cf);
            uint32_t src_palette_bytes = src_palette_entries * sizeof(lv_color32_t);
            const uint8_t *palette_data = src_buf;
            const uint8_t *index_data = src_buf + src_palette_bytes;

            /* Upload source palette entries, then zero-fill remainder to 256 */
            EVE_Hal_wrMem(u->hal, base_addr, palette_data, src_palette_bytes);
            if(src_palette_entries < 256) {
                uint32_t pad_bytes = (256 - src_palette_entries) * sizeof(lv_color32_t);
                /* Zero-fill padding (transparent black) */
                uint8_t *zeros = lv_calloc(1, pad_bytes);
                if(zeros) {
                    EVE_Hal_wrMem(u->hal, base_addr + src_palette_bytes, zeros, pad_bytes);
                    lv_free(zeros);
                }
            }

            /* Expand packed indices to 8-bit, one row at a time */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate index expansion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return false;
            }

            uint32_t src_bpp_val = lv_color_format_get_bpp(src_cf);
            uint32_t pixels_per_byte = 8 / src_bpp_val;
            uint32_t index_mask = (1u << src_bpp_val) - 1u;

            for(int32_t y = 0; y < src_h; y++) {
                const uint8_t *src_row = index_data + y * src_stride;
                lv_memzero(tmp_buf, eve_stride);
                for(int32_t x = 0; x < src_w; x++) {
                    uint32_t byte_idx = x / pixels_per_byte;
                    /* MSB-first packing: high bits are leftmost pixel */
                    uint32_t bit_shift = (pixels_per_byte - 1 - (x % pixels_per_byte)) * src_bpp_val;
                    tmp_buf[x] = (src_row[byte_idx] >> bit_shift) & index_mask;
                }
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        case LV_COLOR_FORMAT_I8: {
            /* LVGL I8 layout: [palette 256×ARGB8888][index data]
             * EVE PALETTEDARGB8: same palette format, 8-bit indices.
             * Upload as one contiguous blob: [palette][indices] */
            const uint8_t *palette_data = src_buf;
            const uint8_t *index_data = src_buf + palette_size;

            /* Upload palette to base_addr */
            EVE_Hal_wrMem(u->hal, base_addr, palette_data, palette_size);

            /* Upload index data to ram_g_addr (= base_addr + palette_size) */
            int32_t row_bytes = (src_w * bpp + 7) / 8;
            if(eve_stride == src_stride) {
                EVE_Hal_wrMem(u->hal, ram_g_addr, index_data, eve_size);
            }
            else {
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  index_data + y * src_stride, row_bytes);
                }
            }
            break;
        }

        case LV_COLOR_FORMAT_RGB565_SWAPPED: {
            /* Byte-swap each 16-bit pixel to native RGB565 */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return false;
            }

            for(int32_t y = 0; y < src_h; y++) {
                convert_rgb565_byteswap(src_buf + y * src_stride, tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        case LV_COLOR_FORMAT_XRGB8888: {
            /* Convert XRGB8888 to RGB8 (strip alpha) */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return false;
            }

            for(int32_t y = 0; y < src_h; y++) {
                convert_xrgb8888_to_rgb8(src_buf + y * src_stride, tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        case LV_COLOR_FORMAT_RGB565A8: {
            /* Convert RGB565+A8 to ARGB8 */
            uint8_t *tmp_buf = lv_malloc(eve_stride);
            if(!tmp_buf) {
                LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                Esd_GpuAlloc_Free(u->allocator, handle);
                return false;
            }

            /* Alpha plane follows RGB data */
            const uint8_t *alpha_buf = src_buf + src_h * src_stride;
            int32_t alpha_stride = src_stride / 2;  /* Alpha is 1 byte per pixel vs 2 for RGB565 */

            for(int32_t y = 0; y < src_h; y++) {
                convert_rgb565a8_to_argb8(src_buf + y * src_stride,
                                           alpha_buf + y * alpha_stride,
                                           tmp_buf, src_w);
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
            }
            lv_free(tmp_buf);
            break;
        }

        default:
            /* Should not reach here due to earlier check */
            Esd_GpuAlloc_Free(u->allocator, handle);
            return false;
    }

    /* Populate result (handle-relative offsets, defrag-safe).
     * LVGL layout: [palette @ offset 0][index data @ offset palette_size]. */
    out->gpu_handle = handle;
    out->eve_format = eve_format;
    out->eve_stride = eve_stride;
    out->width = src_w;
    out->height = src_h;
    out->image_offset = palette_size;
    out->palette_offset = (palette_size > 0) ? 0 : GA_INVALID;

    /* Insert into cache (allocator owns the memory, we just index it) */
    lv_draw_eve5_image_cache_insert(u, img_dsc, handle, eve_format, eve_stride,
                                     out->image_offset, out->palette_offset);

    LV_LOG_TRACE("EVE5: Uploaded image %dx%d cf=%d as EVE format %d at 0x%08X (palette %u)",
                 src_w, src_h, src_cf, eve_format, ram_g_addr, palette_size);

    return true;
}

/**********************
 * IMAGE UPLOAD (CACHED) — LEGACY WRAPPER
 **********************/

uint32_t lv_draw_eve5_upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint16_t *out_eve_format, int32_t *out_eve_stride,
                              uint32_t *out_palette_addr)
{
    eve5_gpu_image_t img;
    if(!lv_draw_eve5_upload_image_to_gpu(u, img_dsc, &img)) return GA_INVALID;

    uint32_t addr, pal_addr;
    eve5_gpu_image_resolve(u->allocator, &img, &addr, &pal_addr);

    *out_eve_format = img.eve_format;
    *out_eve_stride = img.eve_stride;
    if(out_palette_addr) *out_palette_addr = pal_addr;

    return addr;
}

/**********************
 * IMAGE UPLOAD (UNCACHED)
 **********************/

uint32_t lv_draw_eve5_upload_image_uncached(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                                        uint16_t *out_eve_format, int32_t *out_eve_stride,
                                        Esd_GpuHandle *out_handle)
{
    const uint8_t *src_buf = img_dsc->data;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    uint16_t eve_format;
    uint8_t bpp;
    bool needs_conversion;

    if(!lv_draw_eve5_get_eve_format_info(src_cf, &eve_format, &bpp, &needs_conversion)) {
        *out_handle = GA_HANDLE_INVALID;
        return GA_INVALID;
    }

    if(src_stride == 0) {
        uint32_t src_bpp = lv_color_format_get_bpp(src_cf);
        src_stride = (src_w * src_bpp + 7) / 8;
    }

    int32_t eve_stride = ALIGN_UP((src_w * bpp + 7) / 8, 4);
    int32_t eve_size = eve_stride * src_h;

    /* For simplicity, don't support paletted formats in uncached path */
    if(LV_COLOR_FORMAT_IS_INDEXED(src_cf)) {
        *out_handle = GA_HANDLE_INVALID;
        return GA_INVALID;
    }

    *out_eve_format = eve_format;
    *out_eve_stride = eve_stride;

    /* Allocate RAM_G space */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, eve_size, GA_ALIGN_128);
    uint32_t ram_g_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(ram_g_addr == GA_INVALID) {
        *out_handle = GA_HANDLE_INVALID;
        return GA_INVALID;
    }

    *out_handle = handle;

    /* Upload based on format */
    if(!needs_conversion) {
        int32_t row_bytes = (src_w * bpp + 7) / 8;
        if(eve_stride == src_stride) {
            EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
        }
        else {
            for(int32_t y = 0; y < src_h; y++) {
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                              src_buf + y * src_stride, row_bytes);
            }
        }
    }
    else {
        /* Handle conversions - simplified for common cases */
        uint8_t *row_buf = lv_malloc(eve_stride);
        if(!row_buf) {
            Esd_GpuAlloc_Free(u->allocator, handle);
            *out_handle = GA_HANDLE_INVALID;
            return GA_INVALID;
        }

        for(int32_t y = 0; y < src_h; y++) {
            const uint8_t *src_row = src_buf + y * src_stride;
            switch(src_cf) {
                case LV_COLOR_FORMAT_RGB565_SWAPPED:
                    convert_rgb565_byteswap(src_row, row_buf, src_w);
                    break;
                case LV_COLOR_FORMAT_XRGB8888:
                    convert_xrgb8888_to_rgb8(src_row, row_buf, src_w);
                    break;
                default:
                    lv_memcpy(row_buf, src_row, (src_w * bpp + 7) / 8);
                    break;
            }
            EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
    }

    return ram_g_addr;
}

#endif /* LV_USE_DRAW_EVE5 */
