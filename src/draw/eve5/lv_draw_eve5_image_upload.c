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

/**********************
 * FORMAT MAPPING
 **********************/

bool lv_draw_eve5_get_eve_format_info(lv_color_format_t src_cf,
                                      uint16_t * eve_format,
                                      uint8_t * bits_per_pixel,
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
            *bits_per_pixel = 8;  /* Expanded to 8-bit indices for EVE */
            *needs_conversion = true;
            break;

        case LV_COLOR_FORMAT_I8:
            *eve_format = PALETTEDARGB8;
            *bits_per_pixel = 8;
            *needs_conversion = true;  /* Needs palette prefix upload */
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

    if(!lv_draw_eve5_get_eve_format_info(src_cf, &eve_format, &bpp, &needs_conversion)) {
        return NULL;
    }

    if(src_stride == 0) {
        uint32_t src_bpp = lv_color_format_get_bpp(src_cf);
        src_stride = (src_w * src_bpp + 7) / 8;
    }

    int32_t eve_stride = ALIGN_UP((src_w * bpp + 7) / 8, 4);
    int32_t eve_size = eve_stride * src_h;

    /* EVE PALETTEDARGB8 always uses 256-entry ARGB8888 palette (1024 bytes) */
    uint16_t palette_size = 0;
    if(LV_COLOR_FORMAT_IS_INDEXED(src_cf)) {
        palette_size = 256 * sizeof(lv_color32_t);
    }

    /* Allocate RAM_G space */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, palette_size + eve_size, GA_ALIGN_4);
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
            case LV_COLOR_FORMAT_I4: {
                    /* LVGL I1/I2/I4: [palette N×ARGB8888][packed indices]
                     * EVE PALETTEDARGB8: [256×ARGB8888 palette][8-bit indices]
                     * Expand palette and unpack sub-byte indices. */
                    uint32_t src_palette_entries = LV_COLOR_INDEXED_PALETTE_SIZE(src_cf);
                    uint32_t src_palette_bytes = src_palette_entries * sizeof(lv_color32_t);
                    const uint8_t * palette_data = src_buf;
                    const uint8_t * index_data = src_buf + src_palette_bytes;

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
                    lv_free(tmp_buf);
                    break;
                }

            case LV_COLOR_FORMAT_I8: {
                    /* LVGL I8: [256×ARGB8888 palette][8-bit indices]
                     * EVE PALETTEDARGB8: same layout, upload contiguously. */
                    const uint8_t * palette_data = src_buf;
                    const uint8_t * index_data = src_buf + palette_size;

                    EVE_Hal_wrMem(u->hal, base_addr, palette_data, palette_size);

                    int32_t row_bytes = (src_w * bpp + 7) / 8;
                    if(eve_stride == src_stride) {
                        EVE_Hal_wrMem(u->hal, ram_g_addr, index_data, eve_size);
                    }
                    else {
                        uint8_t * row_buf = lv_malloc(eve_stride);
                        if(row_buf != NULL) {
                            for(int32_t y = 0; y < src_h; y++) {
                                lv_memzero(row_buf, eve_stride);
                                lv_memcpy(row_buf, index_data + y * src_stride, row_bytes);
                                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, row_buf, eve_stride);
                            }
                            lv_free(row_buf);
                        }
                        else {
                            for(int32_t y = 0; y < src_h; y++) {
                                EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride,
                                              index_data + y * src_stride, row_bytes);
                            }
                        }
                    }
                    break;
                }

            case LV_COLOR_FORMAT_RGB565_SWAPPED: {
                    uint8_t * tmp_buf = lv_malloc(eve_stride);
                    if(!tmp_buf) {
                        LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                        Esd_GpuAlloc_Free(u->allocator, handle);
                        return NULL;
                    }

                    for(int32_t y = 0; y < src_h; y++) {
                        lv_memzero(tmp_buf, eve_stride);
                        convert_rgb565_byteswap(src_buf + y * src_stride, tmp_buf, src_w);
                        EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                    }
                    lv_free(tmp_buf);
                    break;
                }

            case LV_COLOR_FORMAT_XRGB8888: {
                    uint8_t * tmp_buf = lv_malloc(eve_stride);
                    if(!tmp_buf) {
                        LV_LOG_ERROR("EVE5: Failed to allocate conversion buffer");
                        Esd_GpuAlloc_Free(u->allocator, handle);
                        return NULL;
                    }

                    for(int32_t y = 0; y < src_h; y++) {
                        lv_memzero(tmp_buf, eve_stride);
                        convert_xrgb8888_to_rgb8(src_buf + y * src_stride, tmp_buf, src_w);
                        EVE_Hal_wrMem(u->hal, ram_g_addr + y * eve_stride, tmp_buf, eve_stride);
                    }
                    lv_free(tmp_buf);
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
                        convert_rgb565a8_to_argb8(src_buf + y * src_stride,
                                                  alpha_buf + y * alpha_stride,
                                                  tmp_buf, src_w);
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
