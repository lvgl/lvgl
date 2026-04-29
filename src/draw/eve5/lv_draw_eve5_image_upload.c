/**
 * @file lv_draw_eve5_image_upload.c
 *
 * EVE5 (BT820) Image Format Conversion and Upload to RAM_G
 *
 * Contains:
 * - Pixel format conversion (RGB565A8→ARGB8, XRGB→RGB8, byteswap, etc.)
 * - LVGL→EVE format mapping
 * - Image upload to RAM_G (cached and uncached variants)
 *
 * Image loading/decode paths (SD card, flash, HW decode, SW decode) are in
 * lv_draw_eve5_image_load.c.
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

static void convert_rgb565a8_to_argb8(const uint8_t * rgb, const uint8_t * alpha,
                                      uint8_t * dst, uint32_t w);
static void convert_xrgb8888_to_rgb8(const uint8_t * src, uint8_t * dst, uint32_t w);
static void convert_rgb565_byteswap(const uint8_t * src, uint8_t * dst, uint32_t w);
static void convert_argb8888_to_argb4(const uint8_t * src, uint8_t * dst, uint32_t w);
static void convert_rgb888_to_rgb565(const uint8_t * src, uint8_t * dst, uint32_t w);
static void convert_xrgb8888_to_rgb565(const uint8_t * src, uint8_t * dst, uint32_t w);
static void convert_rgb565a8_to_argb4(const uint8_t * rgb, const uint8_t * alpha,
                                      uint8_t * dst, uint32_t w);
static void convert_a2_to_l4(const uint8_t * src, uint8_t * dst, uint32_t w);

/**********************
 * IMAGE FORMAT CONVERSION
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t * rgb, const uint8_t * alpha,
                                      uint8_t * dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        uint16_t rgb565 = ((const uint16_t *)rgb)[x];

        /* Expand 5/6/5 bits to 8 bits with bit replication for accuracy */
        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;

        uint8_t r = (r5 << 3) | (r5 >> 2);
        uint8_t g = (g6 << 2) | (g6 >> 4);
        uint8_t b = (b5 << 3) | (b5 >> 2);

        /* EVE ARGB8 is BGRA in memory (little-endian) */
        dst[4 * x + 0] = b;
        dst[4 * x + 1] = g;
        dst[4 * x + 2] = r;
        dst[4 * x + 3] = alpha[x];
    }
}

static void convert_xrgb8888_to_rgb8(const uint8_t * src, uint8_t * dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        /* LVGL XRGB8888 is BGRX in memory */
        dst[3 * x + 0] = src[4 * x + 0];  /* B */
        dst[3 * x + 1] = src[4 * x + 1];  /* G */
        dst[3 * x + 2] = src[4 * x + 2];  /* R */
    }
}

static void convert_rgb565_byteswap(const uint8_t * src, uint8_t * dst, uint32_t w)
{
    const uint16_t * s = (const uint16_t *)src;
    uint16_t * d = (uint16_t *)dst;
    for(uint32_t x = 0; x < w; x++) {
        uint16_t v = s[x];
        d[x] = (v >> 8) | (v << 8);
    }
}

/* Pre-BT820 fallbacks: 8-bits-per-channel formats lose precision when
 * mapped onto the 16bpp formats available on EVE1-EVE4 (ARGB4, RGB565). */

static void convert_argb8888_to_argb4(const uint8_t * src, uint8_t * dst, uint32_t w)
{
    /* LVGL ARGB8888: BGRA bytes. EVE ARGB4 (16-bit value, A in MSB nibble):
     * stored little-endian as low byte = (G<<4)|B, high byte = (A<<4)|R. */
    uint16_t * d = (uint16_t *)dst;
    for(uint32_t x = 0; x < w; x++) {
        uint8_t b = src[4 * x + 0];
        uint8_t g = src[4 * x + 1];
        uint8_t r = src[4 * x + 2];
        uint8_t a = src[4 * x + 3];
        d[x] = (uint16_t)(((a & 0xF0) << 8) | ((r & 0xF0) << 4) | (g & 0xF0) | (b >> 4));
    }
}

static void convert_rgb888_to_rgb565(const uint8_t * src, uint8_t * dst, uint32_t w)
{
    /* LVGL RGB888: BGR bytes (3bpp). */
    uint16_t * d = (uint16_t *)dst;
    for(uint32_t x = 0; x < w; x++) {
        uint8_t b = src[3 * x + 0];
        uint8_t g = src[3 * x + 1];
        uint8_t r = src[3 * x + 2];
        d[x] = (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    }
}

static void convert_xrgb8888_to_rgb565(const uint8_t * src, uint8_t * dst, uint32_t w)
{
    /* LVGL XRGB8888: BGRX bytes. Drop X. */
    uint16_t * d = (uint16_t *)dst;
    for(uint32_t x = 0; x < w; x++) {
        uint8_t b = src[4 * x + 0];
        uint8_t g = src[4 * x + 1];
        uint8_t r = src[4 * x + 2];
        d[x] = (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    }
}

static void convert_rgb565a8_to_argb4(const uint8_t * rgb, const uint8_t * alpha,
                                      uint8_t * dst, uint32_t w)
{
    /* RGB plane is RGB565; alpha plane is one A8 byte per pixel.
     * Combine into ARGB4 with bit-replicated 4-bit channels. */
    uint16_t * d = (uint16_t *)dst;
    const uint16_t * s = (const uint16_t *)rgb;
    for(uint32_t x = 0; x < w; x++) {
        uint16_t c = s[x];
        uint8_t r5 = (uint8_t)((c >> 11) & 0x1F);
        uint8_t g6 = (uint8_t)((c >> 5) & 0x3F);
        uint8_t b5 = (uint8_t)(c & 0x1F);
        uint8_t r4 = r5 >> 1;             /* R5 → R4 (drop LSB) */
        uint8_t g4 = g6 >> 2;             /* G6 → G4 */
        uint8_t b4 = b5 >> 1;             /* B5 → B4 */
        uint8_t a4 = (uint8_t)(alpha[x] >> 4);
        d[x] = (uint16_t)((a4 << 12) | (r4 << 8) | (g4 << 4) | b4);
    }
}

/* A2 (LVGL): 4 pixels per byte, MSB-first, 2 bits each.
 * L4 (EVE): 2 pixels per byte, high nibble = left pixel.
 * Bit-replicate the 2-bit value to 4 bits so 0/1/2/3 → 0/5/A/F (preserves
 * proportional alpha levels). Caller must zero dst_row beforehand. */
static void convert_a2_to_l4(const uint8_t * src, uint8_t * dst, uint32_t w)
{
    for(uint32_t x = 0; x < w; x++) {
        uint32_t src_byte = x >> 2;
        uint32_t src_shift = (3 - (x & 3)) << 1;
        uint8_t v = (uint8_t)((src[src_byte] >> src_shift) & 0x3);
        uint8_t l4 = (uint8_t)((v << 2) | v);

        uint32_t dst_byte = x >> 1;
        if((x & 1) == 0) {
            dst[dst_byte] = (uint8_t)(l4 << 4);
        }
        else {
            dst[dst_byte] |= l4;
        }
    }
}

/**
 * Public per-row converter: dispatches to the right per-pixel conversion
 * based on the (LVGL format, EVE format) pair.
 *
 * Limitations: handles only formats that have a 1:1 pixel layout. RGB565A8
 * (separate alpha plane) and indexed formats (palette lookup) need their
 * own paths that don't fit this signature.
 */
bool lv_draw_eve5_convert_row(lv_color_format_t lv_cf, uint16_t eve_fmt,
                              const uint8_t * src_row, uint8_t * dst_row, int32_t w)
{
    switch(lv_cf) {
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            if(eve_fmt == RGB565) {
                convert_rgb565_byteswap(src_row, dst_row, (uint32_t)w);
                return true;
            }
            return false;

        case LV_COLOR_FORMAT_RGB888:
            if(eve_fmt == RGB565) {
                convert_rgb888_to_rgb565(src_row, dst_row, (uint32_t)w);
                return true;
            }
            return false;

        case LV_COLOR_FORMAT_XRGB8888:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(eve_fmt == RGB8) {
                convert_xrgb8888_to_rgb8(src_row, dst_row, (uint32_t)w);
                return true;
            }
#endif
            if(eve_fmt == RGB565) {
                convert_xrgb8888_to_rgb565(src_row, dst_row, (uint32_t)w);
                return true;
            }
            return false;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            if(eve_fmt == ARGB4) {
                convert_argb8888_to_argb4(src_row, dst_row, (uint32_t)w);
                return true;
            }
            return false;

        case LV_COLOR_FORMAT_A2:
            if(eve_fmt == L4) {
                convert_a2_to_l4(src_row, dst_row, (uint32_t)w);
                return true;
            }
            return false;

        default:
            return false;
    }
}

/**********************
 * FORMAT MAPPING
 **********************/

bool lv_draw_eve5_get_eve_format_info(EVE_HalContext *hal,
                                      lv_color_format_t src_cf,
                                      uint16_t * eve_format,
                                      uint8_t * bits_per_pixel,
                                      bool *needs_conversion)
{
    EVE_HalContext *phost = hal;
    LV_UNUSED(phost);
    *needs_conversion = false;

    /* RGB8 / ARGB8 / PALETTEDARGB8 are BT820-only; earlier gens fall back to
     * 16bpp formats with lossy bit-per-channel conversion at upload. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    bool has_argb8 = EVE_Hal_supportRenderTarget(hal);
#else
    bool has_argb8 = false;
#endif

    switch(src_cf) {
        case LV_COLOR_FORMAT_A1:
            *eve_format = L1;
            *bits_per_pixel = 1;
            break;

        case LV_COLOR_FORMAT_A2:
            /* L2 was added in EVE2 (FT810+). FT800/FT801 only have L1 and L4 — fall
             * back to L4 with 2bpp→4bpp bit-replication so 0/1/2/3 → 0/5/A/F. */
            if(EVE_CHIPID >= EVE_FT810) {
                *eve_format = L2;
                *bits_per_pixel = 2;
            }
            else {
                *eve_format = L4;
                *bits_per_pixel = 4;
                *needs_conversion = true;
            }
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
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(has_argb8) {
                *eve_format = RGB8;
                *bits_per_pixel = 24;
            }
            else
#endif
            {
                *eve_format = RGB565;
                *bits_per_pixel = 16;
                *needs_conversion = true;
            }
            break;

        case LV_COLOR_FORMAT_XRGB8888:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(has_argb8) {
                *eve_format = RGB8;
                *bits_per_pixel = 24;
            }
            else
#endif
            {
                *eve_format = RGB565;
                *bits_per_pixel = 16;
            }
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(has_argb8) {
                *eve_format = ARGB8;
                *bits_per_pixel = 32;
            }
            else
#endif
            {
                *eve_format = ARGB4;
                *bits_per_pixel = 16;
                *needs_conversion = true;
            }
            break;

        case LV_COLOR_FORMAT_RGB565A8:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(has_argb8) {
                *eve_format = ARGB8;
                *bits_per_pixel = 32;
            }
            else
#endif
            {
                *eve_format = ARGB4;
                *bits_per_pixel = 16;
            }
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_I8:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
            if(has_argb8) {
                *eve_format = PALETTEDARGB8;
                *bits_per_pixel = 8;  /* Expanded to 8-bit indices for EVE */
                *needs_conversion = true;
            }
            else
#endif
            {
                /* No PALETTEDARGB8 on earlier chips. Expand to ARGB4 inline
                 * — palette is applied per pixel during upload. */
                *eve_format = ARGB4;
                *bits_per_pixel = 16;
                *needs_conversion = true;
            }
            break;

        default:
            LV_LOG_WARN("EVE5: Unsupported image format %d", src_cf);
            return false;
    }

    return true;
}

/**********************
 * IMAGE UPLOAD
 **********************/

/**
 * Upload image to GPU. Allocates and attaches vram_res directly on img_dsc.
 * Returns pointer to the attached vram_res, or NULL on failure.
 * If vram_res already exists and is valid, returns it without re-uploading.
 */
lv_eve5_vram_res_t * lv_draw_eve5_upload_image_to_gpu(lv_draw_eve5_unit_t * u,
                                                      LV_IMAGE_DSC_CONST lv_image_dsc_t * img_dsc)
{
    /* Check vram_res for image already uploaded to GPU */
    lv_eve5_vram_res_t * existing = eve5_get_image_vram_res(img_dsc);
    if(existing != NULL) {
        uint32_t addr = Esd_GpuAlloc_Get(u->allocator, existing->gpu_handle);
        if(addr != GA_INVALID) {
            existing->width = img_dsc->header.w;
            existing->height = img_dsc->header.h;
            return existing;
        }
        /* Handle expired: free stale vram_res */
        lv_free(existing);
        img_dsc->vram_res = NULL;
    }

    const uint8_t * src_buf = img_dsc->data;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    uint16_t eve_format;
    uint8_t bpp;
    bool needs_conversion;

    if(!lv_draw_eve5_get_eve_format_info(u->hal, src_cf, &eve_format, &bpp, &needs_conversion)) {
        return NULL;
    }

    if(src_stride == 0) {
        uint32_t src_bpp = lv_color_format_get_bpp(src_cf);
        src_stride = (src_w * src_bpp + 7) / 8;
    }

    int32_t eve_stride = ALIGN_UP((src_w * bpp + 7) / 8, 4);
    int32_t eve_size = eve_stride * src_h;

    /* EVE PALETTEDARGB8 always uses 256-entry ARGB8888 palette (1024 bytes).
     * On non-RT chips we expand indexed images to ARGB4 inline — the palette
     * is consumed during conversion, so no GPU palette is needed. */
    uint16_t palette_size = 0;
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    if(LV_COLOR_FORMAT_IS_INDEXED(src_cf) && eve_format == PALETTEDARGB8) {
        palette_size = 256 * sizeof(lv_color32_t);
    }
#endif

    /* Allocate RAM_G space. On pre-BT820 the allocator caps live handles at 64;
     * flag image source uploads as GC so they can be reclaimed under pressure. */
    uint32_t alloc_flags = GA_ALIGN_4 | (EVE_Hal_supportRenderTarget(u->hal) ? 0 : GA_GC_FLAG);
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, palette_size + eve_size, alloc_flags);
    uint32_t base_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(base_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate image in RAM_G (%u bytes)", palette_size + eve_size);
        return NULL;
    }

    uint32_t ram_g_addr = base_addr + palette_size;

    if(!needs_conversion) {
        /* Direct copy, native EVE format */
        int32_t row_bytes = (src_w * bpp + 7) / 8;
        if(eve_stride == src_stride) {
            EVE_Hal_wrMem(u->hal, ram_g_addr, src_buf, eve_size);
        }
        else if(eve_stride == row_bytes) {
            /* Stride matches pixel data width — no padding, upload row by row */
            for(int32_t y = 0; y < src_h; y++) {
                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                              src_buf + y * src_stride, row_bytes);
            }
        }
        else {
            /* EVE stride wider than pixel data — zero-pad each row */
            uint8_t * row_buf = lv_malloc(eve_stride);
            if(row_buf != NULL) {
                for(int32_t y = 0; y < src_h; y++) {
                    lv_memzero(row_buf, eve_stride);
                    lv_memcpy(row_buf, src_buf + y * src_stride, row_bytes);
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, row_buf, eve_stride);
                }
                lv_free(row_buf);
            }
            else {
                /* Fallback: upload pixel data without padding (padding bytes undefined) */
                for(int32_t y = 0; y < src_h; y++) {
                    EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                  src_buf + y * src_stride, row_bytes);
                }
            }
        }
    }
    else switch(src_cf) {
            case LV_COLOR_FORMAT_I1:
            case LV_COLOR_FORMAT_I2:
            case LV_COLOR_FORMAT_I4:
            case LV_COLOR_FORMAT_I8: {
                    uint32_t src_palette_entries = LV_COLOR_INDEXED_PALETTE_SIZE(src_cf);
                    uint32_t src_palette_bytes = src_palette_entries * sizeof(lv_color32_t);
                    const uint8_t * palette_data = src_buf;
                    const uint8_t * index_data = src_buf + src_palette_bytes;

#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
                    if(eve_format == PALETTEDARGB8) {
                        /* BT820 path: write ARGB8888 palette to GPU + 8-bit indices. */
                        EVE_Hal_wrMem(u->hal, base_addr, palette_data, src_palette_bytes);
                        if(src_palette_entries < 256) {
                            uint32_t pad_bytes = (256 - src_palette_entries) * sizeof(lv_color32_t);
                            uint8_t * zeros = lv_calloc(1, pad_bytes);
                            if(zeros) {
                                EVE_Hal_wrMem(u->hal, base_addr + src_palette_bytes, zeros, pad_bytes);
                                lv_free(zeros);
                            }
                        }

                        uint8_t * tmp_buf = lv_malloc(eve_stride);
                        if(!tmp_buf) {
                            LV_LOG_ERROR("EVE5: Failed to allocate index expansion buffer");
                            Esd_GpuAlloc_Free(u->allocator, handle);
                            return NULL;
                        }

                        if(src_cf == LV_COLOR_FORMAT_I8) {
                            int32_t row_bytes = src_w;
                            for(int32_t y = 0; y < src_h; y++) {
                                lv_memzero(tmp_buf, eve_stride);
                                lv_memcpy(tmp_buf, index_data + y * src_stride, row_bytes);
                                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                            }
                        }
                        else {
                            uint32_t src_bpp_val = lv_color_format_get_bpp(src_cf);
                            uint32_t pixels_per_byte = 8 / src_bpp_val;
                            uint32_t index_mask = (1u << src_bpp_val) - 1u;

                            for(int32_t y = 0; y < src_h; y++) {
                                const uint8_t * src_row = index_data + y * src_stride;
                                lv_memzero(tmp_buf, eve_stride);
                                for(int32_t x = 0; x < src_w; x++) {
                                    uint32_t byte_idx = x / pixels_per_byte;
                                    /* MSB-first packing */
                                    uint32_t bit_shift = (pixels_per_byte - 1 - (x % pixels_per_byte)) * src_bpp_val;
                                    tmp_buf[x] = (src_row[byte_idx] >> bit_shift) & index_mask;
                                }
                                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                            }
                        }
                        lv_free(tmp_buf);
                    }
                    else
#endif
                    {
                        /* Pre-BT820 path: no GPU palette. Convert palette to ARGB4
                         * once on host, then expand each pixel through the LUT. */
                        uint16_t palette_argb4[256];
                        lv_memzero(palette_argb4, sizeof(palette_argb4));
                        uint32_t pal_n = src_palette_entries < 256 ? src_palette_entries : 256;
                        for(uint32_t i = 0; i < pal_n; i++) {
                            uint8_t b = palette_data[i * 4 + 0];
                            uint8_t g = palette_data[i * 4 + 1];
                            uint8_t r = palette_data[i * 4 + 2];
                            uint8_t a = palette_data[i * 4 + 3];
                            palette_argb4[i] = (uint16_t)(((a & 0xF0) << 8) | ((r & 0xF0) << 4)
                                                          | (g & 0xF0) | (b >> 4));
                        }

                        uint8_t * tmp_buf = lv_malloc(eve_stride);
                        if(!tmp_buf) {
                            LV_LOG_ERROR("EVE5: Failed to allocate ARGB4 expansion buffer");
                            Esd_GpuAlloc_Free(u->allocator, handle);
                            return NULL;
                        }

                        if(src_cf == LV_COLOR_FORMAT_I8) {
                            for(int32_t y = 0; y < src_h; y++) {
                                lv_memzero(tmp_buf, eve_stride);
                                uint16_t * dst_row = (uint16_t *)tmp_buf;
                                const uint8_t * src_row = index_data + y * src_stride;
                                for(int32_t x = 0; x < src_w; x++) {
                                    dst_row[x] = palette_argb4[src_row[x]];
                                }
                                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                            }
                        }
                        else {
                            uint32_t src_bpp_val = lv_color_format_get_bpp(src_cf);
                            uint32_t pixels_per_byte = 8 / src_bpp_val;
                            uint32_t index_mask = (1u << src_bpp_val) - 1u;

                            for(int32_t y = 0; y < src_h; y++) {
                                lv_memzero(tmp_buf, eve_stride);
                                uint16_t * dst_row = (uint16_t *)tmp_buf;
                                const uint8_t * src_row = index_data + y * src_stride;
                                for(int32_t x = 0; x < src_w; x++) {
                                    uint32_t byte_idx = x / pixels_per_byte;
                                    uint32_t bit_shift = (pixels_per_byte - 1 - (x % pixels_per_byte)) * src_bpp_val;
                                    uint8_t idx = (src_row[byte_idx] >> bit_shift) & index_mask;
                                    dst_row[x] = palette_argb4[idx];
                                }
                                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                            }
                        }
                        lv_free(tmp_buf);
                    }
                    break;
                }

            case LV_COLOR_FORMAT_RGB565A8: {
                    uint8_t * tmp_buf = lv_malloc(eve_stride);
                    if(!tmp_buf) {
                        LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                        Esd_GpuAlloc_Free(u->allocator, handle);
                        return NULL;
                    }

                    /* Alpha plane follows RGB data */
                    const uint8_t * alpha_buf = src_buf + src_h * src_stride;
                    int32_t alpha_stride = src_stride / 2;

                    for(int32_t y = 0; y < src_h; y++) {
                        lv_memzero(tmp_buf, eve_stride);
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
                        if(eve_format == ARGB8) {
                            convert_rgb565a8_to_argb8(src_buf + y * src_stride,
                                                      alpha_buf + y * alpha_stride,
                                                      tmp_buf, src_w);
                        }
                        else
#endif
                        { /* ARGB4 fallback for non-RT */
                            convert_rgb565a8_to_argb4(src_buf + y * src_stride,
                                                      alpha_buf + y * alpha_stride,
                                                      tmp_buf, src_w);
                        }
                        EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                    }
                    lv_free(tmp_buf);
                    break;
                }

            /* All other conversion cases route through the per-row helper. */
            case LV_COLOR_FORMAT_A2:
            case LV_COLOR_FORMAT_RGB565_SWAPPED:
            case LV_COLOR_FORMAT_RGB888:
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
            case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED: {
                    uint8_t * tmp_buf = lv_malloc(eve_stride);
                    if(!tmp_buf) {
                        LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                        Esd_GpuAlloc_Free(u->allocator, handle);
                        return NULL;
                    }

                    for(int32_t y = 0; y < src_h; y++) {
                        lv_memzero(tmp_buf, eve_stride);
                        if(!lv_draw_eve5_convert_row(src_cf, eve_format,
                                                     src_buf + y * src_stride, tmp_buf, src_w)) {
                            LV_LOG_ERROR("EVE5: No row conversion for cf=%d eve_fmt=%d", src_cf, eve_format);
                            lv_free(tmp_buf);
                            Esd_GpuAlloc_Free(u->allocator, handle);
                            return NULL;
                        }
                        EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                    }
                    lv_free(tmp_buf);
                    break;
                }

            default:
                Esd_GpuAlloc_Free(u->allocator, handle);
                return NULL;
        }

    /* All RAM_G writes above (palette + pixel data, direct or converted) must
     * be visible to the graphics engine before it samples this bitmap. */
    EVE_Hal_requestFenceBeforeSwap(u->hal);

    /* Allocate and attach vram_res to the image descriptor */
    lv_eve5_vram_res_t * vr = lv_malloc(sizeof(lv_eve5_vram_res_t));
    if(vr == NULL) {
        Esd_GpuAlloc_PendingFree(u->allocator, handle);
        return NULL;
    }

    lv_memzero(vr, sizeof(*vr));
    vr->base.unit = (lv_draw_unit_t *)u;
    vr->base.size = palette_size + eve_size;
    vr->gpu_handle = handle;
    vr->eve_format = eve_format;
    vr->stride = eve_stride;
    vr->width = src_w;
    vr->height = src_h;
    vr->source_offset = palette_size;
    vr->palette_offset = (palette_size > 0) ? 0 : GA_INVALID;
    vr->has_content = true;

    /* If the application crashes here, it's likely that img_dsc is declared const */
    img_dsc->vram_res = (struct _lv_draw_buf_vram_res_t *)vr;

    LV_LOG_TRACE("EVE5: Uploaded image %dx%d cf=%d as EVE format %d at 0x%08X (palette %u)",
                 src_w, src_h, src_cf, eve_format, ram_g_addr, palette_size);

    return vr;
}

#endif /* LV_USE_DRAW_EVE5 */
