/**
 * @file lv_draw_eve5_image_load.c
 *
 * EVE5 (BT820) Image Loading, Format Conversion, and Upload
 *
 * Separated from lv_draw_eve5_image.c. Contains:
 * - Pixel format conversion (RGB565A8→ARGB8, XRGB→RGB8, byteswap, etc.)
 * - LVGL→EVE format mapping
 * - Image upload to RAM_G (cached and uncached variants)
 * - Image source resolution via LVGL decoder
 * - Hardware JPEG/PNG decode via CMD_LOADIMAGE (STDIO path)
 * - SD card direct image loading
 * - lv_draw_eve5_load_image public API
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"
#include "../lv_image_decoder_private.h"
#if LV_USE_OS
#include "../../drivers/display/eve5/lv_eve5.h"
#endif
#if LV_USE_FS_EVE5_SDCARD
#include "../../drivers/display/eve5/lv_eve5_sdcard.h"
#endif

/**********************
 * STATIC PROTOTYPES
 **********************/

static void convert_rgb565a8_to_argb8(const uint8_t *rgb, const uint8_t *alpha,
                                       uint8_t *dst, uint32_t w);
static void convert_xrgb8888_to_rgb8(const uint8_t *src, uint8_t *dst, uint32_t w);
static void convert_rgb565_byteswap(const uint8_t *src, uint8_t *dst, uint32_t w);
#if EVE5_HW_IMAGE_DECODE
static bool file_has_extension(const char *path, const char *ext);
static bool parse_jpeg_dimensions(const uint8_t *data, uint32_t size,
                                   uint32_t *width, uint32_t *height);
static bool parse_png_dimensions(const uint8_t *data, uint32_t size,
                                  uint32_t *width, uint32_t *height);
#endif /* EVE5_HW_IMAGE_DECODE */
static uint32_t upload_image_uncached(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                                        uint16_t *out_eve_format, int32_t *out_eve_stride,
                                        Esd_GpuHandle *out_handle);

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
 * IMAGE UPLOAD
 **********************/

uint32_t lv_draw_eve5_upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint16_t *out_eve_format, int32_t *out_eve_stride,
                              uint32_t *out_palette_addr)
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
        return GA_INVALID;
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

    *out_eve_format = eve_format;
    *out_eve_stride = eve_stride;

    /* Check canvas cache first — if this draw_buf was rendered to by EVE5,
     * use the GPU texture directly without re-uploading stale CPU data.
     * The canvas may be in native format (e.g., RGB565 from direct image load)
     * or ARGB8 (from rendered content). */
    uint32_t canvas_w, canvas_h, canvas_aw;
    uint16_t canvas_format;
    uint32_t canvas_stride;
    uint32_t canvas_palette_addr;
    uint32_t canvas_addr = lv_draw_eve5_canvas_cache_lookup(u, (const void *)src_buf,
                                                              &canvas_w, &canvas_h, &canvas_aw,
                                                              &canvas_format, &canvas_stride,
                                                              &canvas_palette_addr);
    if(canvas_addr != GA_INVALID) {
        *out_eve_format = canvas_format;
        *out_eve_stride = canvas_stride;
        if(out_palette_addr) *out_palette_addr = canvas_palette_addr;
        LV_LOG_TRACE("EVE5: Using canvas GPU texture for image at 0x%08X (fmt=%d)",
                     canvas_addr, canvas_format);
        return canvas_addr;
    }

    /* Check image cache */
    uint16_t cached_palette_size = 0;
    uint32_t cached_addr = lv_draw_eve5_image_cache_lookup(u, img_dsc, out_eve_format,
                                                            out_eve_stride, &cached_palette_size);
    if(cached_addr != GA_INVALID) {
        if(out_palette_addr) *out_palette_addr = (cached_palette_size > 0) ? (cached_addr - cached_palette_size) : GA_INVALID;
        return cached_addr;
    }

    /* Allocate RAM_G space (palette + index data for indexed formats) */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, palette_size + eve_size, GA_ALIGN_4);
    uint32_t base_addr = Esd_GpuAlloc_Get(u->allocator, handle);

    if(base_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate image in RAM_G (%u bytes)", palette_size + eve_size);
        return GA_INVALID;
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
                return GA_INVALID;
            }

            uint32_t src_bpp = lv_color_format_get_bpp(src_cf);
            uint32_t pixels_per_byte = 8 / src_bpp;
            uint32_t index_mask = (1u << src_bpp) - 1u;

            for(int32_t y = 0; y < src_h; y++) {
                const uint8_t *src_row = index_data + y * src_stride;
                lv_memzero(tmp_buf, eve_stride);
                for(int32_t x = 0; x < src_w; x++) {
                    uint32_t byte_idx = x / pixels_per_byte;
                    /* MSB-first packing: high bits are leftmost pixel */
                    uint32_t bit_shift = (pixels_per_byte - 1 - (x % pixels_per_byte)) * src_bpp;
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
                return GA_INVALID;
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
                return GA_INVALID;
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
                return GA_INVALID;
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
            return GA_INVALID;
    }

    /* Insert into cache (allocator owns the memory, we just index it) */
    lv_draw_eve5_image_cache_insert(u, img_dsc, handle, eve_format, eve_stride, palette_size);

    if(out_palette_addr) *out_palette_addr = (palette_size > 0) ? base_addr : GA_INVALID;

    LV_LOG_TRACE("EVE5: Uploaded image %dx%d cf=%d as EVE format %d at 0x%08X (palette %u)",
                 src_w, src_h, src_cf, eve_format, ram_g_addr, palette_size);

    return ram_g_addr;
}

/**********************
 * IMAGE SOURCE RESOLUTION
 **********************/

/**
 * Resolve an image source to an lv_image_dsc_t.
 * For LV_IMAGE_SRC_VARIABLE, returns the pointer directly.
 * For LV_IMAGE_SRC_FILE, opens the decoder with use_indexed=true
 * so indexed formats are preserved (not converted to ARGB8888).
 * Returns true on success, false if the source cannot be resolved.
 */
bool lv_draw_eve5_resolve_image_source(const void *src, eve5_resolved_image_t *resolved)
{
    lv_memzero(resolved, sizeof(*resolved));

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        resolved->img_dsc = (const lv_image_dsc_t *)src;
        resolved->decoder_open = false;
        return true;
    }

    if(src_type == LV_IMAGE_SRC_FILE) {
        lv_image_decoder_args_t args;
        lv_memzero(&args, sizeof(args));
        args.use_indexed = true;

        lv_result_t res = lv_image_decoder_open(&resolved->decoder_dsc, src, &args);
        if(res != LV_RESULT_OK || resolved->decoder_dsc.decoded == NULL) {
            if(res == LV_RESULT_OK) lv_image_decoder_close(&resolved->decoder_dsc);
            LV_LOG_WARN("EVE5: Failed to decode file image");
            return false;
        }

        resolved->img_dsc = (const lv_image_dsc_t *)resolved->decoder_dsc.decoded;
        resolved->decoder_open = true;
        return true;
    }

    LV_LOG_WARN("EVE5: Unsupported image source type %d", src_type);
    return false;
}

/**
 * Release a resolved image source.
 * Closes the decoder session if one was opened.
 */
void lv_draw_eve5_release_image_source(eve5_resolved_image_t *resolved)
{
    if(resolved->decoder_open) {
        lv_image_decoder_close(&resolved->decoder_dsc);
        resolved->decoder_open = false;
    }
    resolved->img_dsc = NULL;
}

#if EVE5_HW_IMAGE_DECODE
/**********************
 * HARDWARE IMAGE LOADING (STDIO)
 *
 * Streams JPEG/PNG files through CMD_LOADIMAGE for hardware decompression.
 * Uses LVGL filesystem API for broad platform compatibility.
 **********************/

/**
 * Check if a file extension matches (case-insensitive).
 */
static bool file_has_extension(const char *path, const char *ext)
{
    size_t path_len = lv_strlen(path);
    size_t ext_len = lv_strlen(ext);
    if(path_len < ext_len) return false;

    const char *path_ext = path + path_len - ext_len;
    for(size_t i = 0; i < ext_len; i++) {
        char c1 = path_ext[i];
        char c2 = ext[i];
        if(c1 >= 'A' && c1 <= 'Z') c1 += ('a' - 'A');
        if(c2 >= 'A' && c2 <= 'Z') c2 += ('a' - 'A');
        if(c1 != c2) return false;
    }
    return true;
}

/**
 * Parse JPEG header to extract image dimensions.
 * Looks for SOF0 (0xFFC0) or SOF2 (0xFFC2) markers.
 */
static bool parse_jpeg_dimensions(const uint8_t *data, uint32_t size,
                                   uint32_t *width, uint32_t *height)
{
    if(size < 11) return false;
    if(data[0] != 0xFF || data[1] != 0xD8) return false;

    uint32_t pos = 2;
    while(pos + 4 < size) {
        if(data[pos] != 0xFF) {
            pos++;
            continue;
        }

        uint8_t marker = data[pos + 1];
        if(marker == 0xC0 || marker == 0xC2) {
            if(pos + 9 > size) return false;
            *height = ((uint32_t)data[pos + 5] << 8) | data[pos + 6];
            *width = ((uint32_t)data[pos + 7] << 8) | data[pos + 8];
            return true;
        }

        if(marker == 0xD8 || marker == 0xD9 || (marker >= 0xD0 && marker <= 0xD7)) {
            pos += 2;
        }
        else if(pos + 4 <= size) {
            uint32_t len = ((uint32_t)data[pos + 2] << 8) | data[pos + 3];
            pos += 2 + len;
        }
        else {
            break;
        }
    }
    return false;
}

/**
 * Parse PNG header to extract image dimensions.
 * Dimensions are in the IHDR chunk immediately after the signature.
 */
static bool parse_png_dimensions(const uint8_t *data, uint32_t size,
                                  uint32_t *width, uint32_t *height)
{
    if(size < 24) return false;

    static const uint8_t png_sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    for(int i = 0; i < 8; i++) {
        if(data[i] != png_sig[i]) return false;
    }

    if(data[12] != 'I' || data[13] != 'H' || data[14] != 'D' || data[15] != 'R') return false;

    *width = ((uint32_t)data[16] << 24) | ((uint32_t)data[17] << 16) |
             ((uint32_t)data[18] << 8) | data[19];
    *height = ((uint32_t)data[20] << 24) | ((uint32_t)data[21] << 16) |
              ((uint32_t)data[22] << 8) | data[23];
    return true;
}

/**
 * Try to load a JPEG/PNG image via EVE hardware decoder.
 *
 * Uses CMD_LOADIMAGE to stream compressed file data through the command buffer
 * for hardware decompression. This is faster than CPU decoding and avoids
 * intermediate host memory allocation for the decompressed image.
 *
 * This path is used for:
 * - STDIO filesystem images (A:/, native paths)
 * - Any LVGL filesystem except EVE5 SD card (which has its own zero-copy path)
 *
 * Note: This function expects the HAL mutex to be UNLOCKED on entry (since it
 * does file I/O which may trigger other filesystem drivers). It locks the HAL
 * internally for EVE commands and returns with HAL still UNLOCKED.
 */
bool lv_draw_eve5_try_load_file_image(lv_draw_eve5_unit_t *u, const void *src,
                                 uint32_t *ram_g_addr, uint16_t *eve_format,
                                 int32_t *eve_stride, int32_t *src_w, int32_t *src_h,
                                 Esd_GpuHandle *out_handle, uint32_t *out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char *path = (const char *)src;

#if LV_USE_FS_EVE5_SDCARD
    /* Skip SD card paths — they have a dedicated zero-copy path */
    if(lv_eve5_sdcard_is_path(path)) {
        return false;
    }
#endif

    /* Check for supported extensions */
    bool is_jpeg = file_has_extension(path, ".jpg") || file_has_extension(path, ".jpeg");
    bool is_png = file_has_extension(path, ".png");
    if(!is_jpeg && !is_png) {
        return false;
    }

    /* Open file via LVGL filesystem (HAL unlocked — file I/O may need it) */
    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("EVE5: Failed to open file: %s", path);
        return false;
    }

    /* Read header for dimension parsing (1KB is enough for JPEG/PNG headers) */
    uint8_t header_buf[1024];
    uint32_t header_read = 0;
    res = lv_fs_read(&file, header_buf, sizeof(header_buf), &header_read);
    if(res != LV_FS_RES_OK || header_read < 24) {
        LV_LOG_WARN("EVE5: Failed to read file header: %s", path);
        lv_fs_close(&file);
        return false;
    }

    /* Parse dimensions */
    uint32_t img_w = 0, img_h = 0;
    bool parsed;
    if(is_jpeg) {
        parsed = parse_jpeg_dimensions(header_buf, header_read, &img_w, &img_h);
    }
    else {
        parsed = parse_png_dimensions(header_buf, header_read, &img_w, &img_h);
    }

    /* Log header details for debugging */
    if(is_png && header_read >= 26) {
        uint8_t bit_depth = header_buf[24];
        uint8_t color_type = header_buf[25];
        const char *ct_str = "unknown";
        if(color_type == 0) ct_str = "grayscale";
        else if(color_type == 2) ct_str = "RGB";
        else if(color_type == 3) ct_str = "indexed";
        else if(color_type == 4) ct_str = "gray+alpha";
        else if(color_type == 6) ct_str = "RGBA";
        LV_LOG_INFO("EVE5 HW_DECODE: PNG %s: %ux%u depth=%u color_type=%u(%s)",
                     path, img_w, img_h, bit_depth, color_type, ct_str);
    }
    else if(is_jpeg) {
        LV_LOG_INFO("EVE5 HW_DECODE: JPEG %s: %ux%u", path, img_w, img_h);
    }

    if(!parsed || img_w == 0 || img_h == 0) {
        LV_LOG_WARN("EVE5: Failed to parse image dimensions: %s", path);
        lv_fs_close(&file);
        return false;
    }

    /* Get file size */
    lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    uint32_t file_pos = 0;
    lv_fs_tell(&file, &file_pos);
    uint32_t file_size = file_pos;
    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);

    if(file_size == 0) {
        LV_LOG_WARN("EVE5: Empty file: %s", path);
        lv_fs_close(&file);
        return false;
    }

    /* Calculate decoded image size - allocate for worst case (ARGB8 = 4 bpp).
     * Actual format is determined after decode via EVE_CoCmd_getImage. */
    int32_t decoded_stride = ALIGN_UP((int32_t)(img_w * 4), 4);
    uint32_t decoded_size = (uint32_t)(decoded_stride * (int32_t)img_h);

    /* Lock HAL for EVE commands */
#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    /* Allocate RAM_G for decoded image */
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, decoded_size, GA_ALIGN_4);
    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate %u bytes for decoded image", decoded_size);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        lv_fs_close(&file);
        return false;
    }

    EVE_HalContext *phost = u->hal;

    /* Check for coprocessor fault before starting */
    if(phost->CmdFault) {
        LV_LOG_ERROR("EVE5: Coprocessor fault before CMD_LOADIMAGE");
        Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        lv_fs_close(&file);
        return false;
    }

    /* Issue CMD_LOADIMAGE and stream file data through command buffer.
     * This is the same approach as EVE_Util_loadImageFile_ex.
     * OPT_TRUECOLOR: decode to full 8-bit formats (RGB8/ARGB8) instead of
     * default RGB565/ARGB4 which cause banding on gradients. */
    EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
    EVE_Cmd_wr32(phost, addr);
    EVE_Cmd_wr32(phost, OPT_NODL | OPT_TRUECOLOR);

    /* Stream file data in chunks.
     * File reads are done while holding HAL lock — this is OK for STDIO but
     * may need adjustment if other filesystem drivers require the HAL. */
    uint8_t chunk_buf[8192];
    uint32_t remaining = file_size;
    bool success = true;

    while(remaining > 0 && success) {
        uint32_t chunk_size = remaining > sizeof(chunk_buf) ? sizeof(chunk_buf) : remaining;
        uint32_t bytes_read = 0;

        res = lv_fs_read(&file, chunk_buf, chunk_size, &bytes_read);
        if(res != LV_FS_RES_OK || bytes_read == 0) {
            LV_LOG_ERROR("EVE5: File read error at offset %u", file_size - remaining);
            success = false;
            break;
        }

        remaining -= bytes_read;

        /* Pad to 4-byte alignment for command buffer */
        uint32_t padded_size = (bytes_read + 3) & ~3U;
        if(padded_size > bytes_read) {
            lv_memzero(chunk_buf + bytes_read, padded_size - bytes_read);
        }

        if(!EVE_Cmd_wrMem(phost, chunk_buf, padded_size)) {
            LV_LOG_ERROR("EVE5: Command buffer write failed");
            success = false;
            break;
        }
    }

    lv_fs_close(&file);

    if(!success) {
        Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        return false;
    }

    /* Wait for coprocessor to finish decoding */
    if(!EVE_Cmd_waitFlush(phost)) {
        LV_LOG_ERROR("EVE5: CMD_LOADIMAGE failed for %s", path);
        Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        return false;
    }

    /* Get actual format from coprocessor (usually RGB565, but PNG with alpha → ARGB8).
     * CMD_GETIMAGE reads coprocessor's internal bitmap state, which CMD_LOADIMAGE
     * sets even with OPT_NODL (confirmed by HAL's own loadImageFile pattern). */
    uint32_t out_source = 0, out_fmt = 0, out_w = 0, out_h = 0, out_palette = 0;
    bool got_image = EVE_CoCmd_getImage(phost, &out_source, &out_fmt, &out_w, &out_h, &out_palette);

    LV_LOG_INFO("EVE5 HW_DECODE: getImage: source=0x%08x fmt=%u w=%u h=%u palette=0x%08x (alloc=0x%08x)",
                out_source, out_fmt, out_w, out_h, out_palette, addr);

    if(got_image) {
        /* For paletted formats, CMD_LOADIMAGE stores palette at alloc start
         * and index data at a separate offset. Use addresses from getImage. */
        if(out_fmt == PALETTEDARGB8) {
            if(!out_palette_addr) {
                /* Caller can't handle paletted format — bail out */
                LV_LOG_INFO("EVE5 HW_DECODE: PALETTEDARGB8 not supported by caller, falling back to SW for %s", path);
                Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
                lv_eve5_hal_unlock(lv_display_get_default());
#endif
                return false;
            }
            *out_palette_addr = out_palette;
            addr = out_source;
        }
        else {
            /* Non-paletted: source should match allocation start */
            if(out_source != addr) {
                LV_LOG_WARN("EVE5 HW_DECODE: source mismatch: getImage=0x%08x alloc=0x%08x for %s",
                            out_source, addr, path);
                addr = out_source;
            }
            if(out_palette_addr) *out_palette_addr = GA_INVALID;
        }
        if(out_w != img_w || out_h != img_h) {
            LV_LOG_WARN("EVE5 HW_DECODE: size changed: header=%ux%u getImage=%ux%u for %s",
                        img_w, img_h, out_w, out_h, path);
        }
        img_w = out_w;
        img_h = out_h;
    }
    else {
        LV_LOG_WARN("EVE5 HW_DECODE: getImage failed for %s, assuming RGB565 %ux%u", path, img_w, img_h);
        out_fmt = RGB565;
        if(out_palette_addr) *out_palette_addr = GA_INVALID;
    }

    /* Recalculate stride based on actual format.
     * HW decoder output stride = width * bpp (packed, no alignment padding). */
    int32_t bpp = 2;  /* Default RGB565 */
    if(out_fmt == ARGB8) bpp = 4;
    else if(out_fmt == RGB8) bpp = 3;
    else if(out_fmt == L8 || out_fmt == PALETTEDARGB8) bpp = 1;
    decoded_stride = (int32_t)(img_w * bpp);

    LV_LOG_INFO("EVE5 HW_DECODE: final: fmt=%u bpp=%d stride=%d size=%u (worst-case was %u) for %s",
                out_fmt, bpp, (int)decoded_stride,
                (unsigned)(decoded_stride * (int32_t)img_h), decoded_size, path);

    /* Trim allocation to actual size if smaller than worst-case.
     * For paletted formats, include palette data before index data. */
    uint32_t index_size = (uint32_t)(decoded_stride * (int32_t)img_h);
    uint32_t palette_offset = (out_fmt == PALETTEDARGB8) ? (uint32_t)(addr - Esd_GpuAlloc_Get(u->allocator, handle)) : 0;
    uint32_t actual_size = palette_offset + index_size;
    if(actual_size < decoded_size) {
        Esd_GpuAlloc_Truncate(u->allocator, handle, actual_size);
    }

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif

    *ram_g_addr = addr;
    *eve_format = (uint16_t)out_fmt;
    *eve_stride = decoded_stride;
    *src_w = (int32_t)img_w;
    *src_h = (int32_t)img_h;
    if(out_handle) *out_handle = handle;

    LV_LOG_INFO("EVE5: HW decoded %s (%ux%u fmt=%u stride=%d bpp=%d) via CMD_LOADIMAGE",
                path, (unsigned)img_w, (unsigned)img_h, (unsigned)out_fmt, (int)decoded_stride, (int)bpp);
    return true;
}
#endif /* EVE5_HW_IMAGE_DECODE */

#if LV_USE_FS_EVE5_SDCARD
/**
 * Try to load a JPEG/PNG image directly from the EVE5 SD card.
 *
 * This is a zero-copy decode path using EVE's hardware JPEG/PNG decoder:
 * SD card → RAM_G (compressed) → RAM_G (decoded via MEDIAFIFO).
 * Only the header bytes touch host memory for dimension parsing.
 */
bool lv_draw_eve5_try_load_sdcard_image(lv_draw_eve5_unit_t *u, const void *src,
                                   uint32_t *ram_g_addr, uint16_t *eve_format,
                                   int32_t *eve_stride, int32_t *src_w, int32_t *src_h,
                                   Esd_GpuHandle *out_handle)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char *path = (const char *)src;
    if(!lv_eve5_sdcard_is_path(path)) {
        return false;
    }

    /* Check for supported extensions (case-insensitive) */
    size_t path_len = lv_strlen(path);
    bool is_jpeg = false, is_png = false;

    if(path_len > 4) {
        const char *ext = path + path_len - 4;
        if(ext[0] == '.') {
            char e1 = ext[1], e2 = ext[2], e3 = ext[3];
            if(e1 >= 'A' && e1 <= 'Z') e1 += 32;
            if(e2 >= 'A' && e2 <= 'Z') e2 += 32;
            if(e3 >= 'A' && e3 <= 'Z') e3 += 32;
            if(e1 == 'j' && e2 == 'p' && e3 == 'g') is_jpeg = true;
            else if(e1 == 'p' && e2 == 'n' && e3 == 'g') is_png = true;
        }
    }
    if(!is_jpeg && path_len > 5) {
        const char *ext = path + path_len - 5;
        if(ext[0] == '.') {
            char e1 = ext[1], e2 = ext[2], e3 = ext[3], e4 = ext[4];
            if(e1 >= 'A' && e1 <= 'Z') e1 += 32;
            if(e2 >= 'A' && e2 <= 'Z') e2 += 32;
            if(e3 >= 'A' && e3 <= 'Z') e3 += 32;
            if(e4 >= 'A' && e4 <= 'Z') e4 += 32;
            if(e1 == 'j' && e2 == 'p' && e3 == 'e' && e4 == 'g') is_jpeg = true;
        }
    }

    if(!is_jpeg && !is_png) {
        return false;
    }

    /* Load and decode via SD card direct path.
     * Note: lv_eve5_sdcard_load_image handles HAL locking internally,
     * so we must release our lock before calling it. */
    Esd_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt;

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif
    bool ok = lv_eve5_sdcard_load_image(path, &handle, &img_w, &img_h, &img_fmt);
#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    if(!ok) {
        return false;
    }

    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: SD card load succeeded but RAM_G address invalid");
        return false;
    }

    *ram_g_addr = addr;
    *eve_format = (uint16_t)img_fmt;
    *src_w = (int32_t)img_w;
    *src_h = (int32_t)img_h;
    *eve_stride = ALIGN_UP((int32_t)img_w * 2, 4);  /* RGB565 = 2 bytes/pixel */
    if(out_handle) *out_handle = handle;

    LV_LOG_INFO("EVE5: Loaded %s via SD card direct path (%dx%d)", path, img_w, img_h);
    return true;
}
#endif /* LV_USE_FS_EVE5_SDCARD */

/**********************
 * IMAGE LOADING API
 **********************/

static uint32_t upload_image_uncached(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
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

uint32_t lv_draw_eve5_load_image(lv_draw_eve5_unit_t *u, const void *src,
                                   uint16_t *out_format, int32_t *out_stride,
                                   int32_t *out_w, int32_t *out_h,
                                   Esd_GpuHandle *out_handle,
                                   uint32_t *out_palette_addr)
{
    uint32_t ram_g_addr = GA_INVALID;
    uint16_t eve_format = ARGB8;
    int32_t eve_stride = 0;
    int32_t src_w = 0, src_h = 0;

    *out_handle = GA_HANDLE_INVALID;
    if(out_palette_addr) *out_palette_addr = GA_INVALID;

#if EVE5_HW_IMAGE_DECODE
    /* Try hardware decode path first (JPEG/PNG via CMD_LOADIMAGE) */
    if(lv_draw_eve5_try_load_file_image(u, src, &ram_g_addr, &eve_format, &eve_stride,
                           &src_w, &src_h, out_handle, out_palette_addr)) {
        *out_format = eve_format;
        *out_stride = eve_stride;
        *out_w = src_w;
        *out_h = src_h;
        return ram_g_addr;
    }
#endif

    /* Fallback: decode via LVGL decoder, upload to RAM_G (uncached). */
    LV_LOG_WARN("EVE5: lv_draw_eve5_load_image using SW decode");

    eve5_resolved_image_t resolved_src = {0};
#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif
    bool resolved = lv_draw_eve5_resolve_image_source(src, &resolved_src);
#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif
    if(!resolved) {
        return GA_INVALID;
    }
    const lv_image_dsc_t *img_dsc = resolved_src.img_dsc;
    src_w = img_dsc->header.w;
    src_h = img_dsc->header.h;
    ram_g_addr = upload_image_uncached(u, img_dsc, &eve_format, &eve_stride, out_handle);
    lv_draw_eve5_release_image_source(&resolved_src);

    if(ram_g_addr == GA_INVALID) {
        return GA_INVALID;
    }

    *out_format = eve_format;
    *out_stride = eve_stride;
    *out_w = src_w;
    *out_h = src_h;
    return ram_g_addr;
}

/**********************
 * EVE5 IMAGE DECODER
 *
 * Lightweight LVGL image decoder that provides JPEG/PNG header info
 * without requiring a full software decoder library (tjpgd, lodepng).
 * The actual decoding is handled by the EVE5 draw unit's HW decode
 * path (CMD_LOADIMAGE) at render time.
 *
 * This decoder is registered during lv_draw_eve5_init() and enables
 * lv_image_set_src("path/to/image.jpg") to succeed by providing
 * dimensions via info_cb. The open_cb returns INVALID to signal that
 * this decoder cannot produce pixel data — LVGL will still accept
 * the image source because get_info succeeded.
 **********************/

#if EVE5_HW_IMAGE_DECODE

static lv_result_t eve5_decoder_info(lv_image_decoder_t *decoder,
                                      lv_image_decoder_dsc_t *dsc,
                                      lv_image_header_t *header)
{
    LV_UNUSED(decoder);

    if(dsc->src_type != LV_IMAGE_SRC_FILE) return LV_RESULT_INVALID;

    const char *fn = dsc->src;
    const char *ext = lv_fs_get_ext(fn);

    bool is_jpeg = (lv_strcmp(ext, "jpg") == 0) || (lv_strcmp(ext, "jpeg") == 0);
    bool is_png = (lv_strcmp(ext, "png") == 0);
    if(!is_jpeg && !is_png) return LV_RESULT_INVALID;

    /* Read header from the already-opened file handle */
    uint8_t buf[1024];
    uint32_t bytes_read = 0;
    lv_fs_read(&dsc->file, buf, sizeof(buf), &bytes_read);
    if(bytes_read < 24) return LV_RESULT_INVALID;

    uint32_t w = 0, h = 0;
    bool ok;
    if(is_jpeg) {
        ok = parse_jpeg_dimensions(buf, bytes_read, &w, &h);
    }
    else {
        ok = parse_png_dimensions(buf, bytes_read, &w, &h);
    }

    if(!ok || w == 0 || h == 0) return LV_RESULT_INVALID;

    /* Report as RAW — the EVE5 draw unit handles actual format detection
     * and HW decoding at render time. LVGL only needs w/h to size the widget. */
    header->cf = LV_COLOR_FORMAT_RAW;
    header->w = (int32_t)w;
    header->h = (int32_t)h;
    header->stride = (int32_t)(w * 3);  /* Approximate for sizing */

    return LV_RESULT_OK;
}

static lv_result_t eve5_decoder_open(lv_image_decoder_t *decoder,
                                      lv_image_decoder_dsc_t *dsc)
{
    LV_UNUSED(decoder);
    LV_UNUSED(dsc);
    /* EVE5 HW decode happens at draw time, not at decoder open time.
     * Return INVALID so LVGL knows this decoder can't produce pixel data.
     * The EVE5 draw unit intercepts the IMAGE task and uses CMD_LOADIMAGE. */
    return LV_RESULT_INVALID;
}

void lv_draw_eve5_register_image_decoder(void)
{
    lv_image_decoder_t *dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, eve5_decoder_info);
    lv_image_decoder_set_open_cb(dec, eve5_decoder_open);
    dec->name = "EVE5_HW";
}

#endif /* EVE5_HW_IMAGE_DECODE */

#endif /* LV_USE_DRAW_EVE5 */
