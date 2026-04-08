/**
 * @file lv_draw_eve5_image_load.c
 *
 * EVE5 (BT820) Image Loading and Decode
 *
 * Contains:
 * - Image source resolution via LVGL decoder
 * - Hardware JPEG/PNG decode via CMD_LOADIMAGE (LVGL FS path)
 * - SD card direct image loading
 * - Flash direct image loading
 * - lv_draw_eve5_load_image public API
 * - Lightweight image decoder registration for JPEG/PNG header parsing
 *
 * Format conversion and upload to RAM_G are in lv_draw_eve5_image_upload.c.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"
#include "../../drivers/display/eve5/lv_eve5_image_private.h"

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
#if LV_USE_FS_EVE5_FLASH
#include "../../drivers/display/eve5/lv_eve5_flash.h"
#endif

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
bool lv_draw_eve5_resolve_image_source(const void *src, eve5_resolved_image_t *resolved, lv_draw_unit_t *draw_unit)
{
    lv_memzero(resolved, sizeof(*resolved));

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        resolved->img_dsc = (const lv_image_dsc_t *)src;
        resolved->decoder_open = false;
        if(draw_unit != NULL) {
            lv_draw_buf_ensure_resident((lv_draw_buf_t *)resolved->img_dsc, draw_unit);
        }
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
        if(draw_unit != NULL) {
            lv_draw_buf_ensure_resident((lv_draw_buf_t *)resolved->img_dsc, draw_unit);
        }
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
 * HARDWARE IMAGE LOADING (LVGL FS)
 *
 * Streams JPEG/PNG files through CMD_LOADIMAGE for hardware decompression.
 * Uses LVGL filesystem API for broad platform compatibility.
 **********************/

/**
 * Try to load a JPEG/PNG image via EVE hardware decoder.
 *
 * Uses CMD_LOADIMAGE to stream compressed file data through the command buffer
 * for hardware decompression. This is faster than CPU decoding and avoids
 * intermediate host memory allocation for the decompressed image.
 *
 * This path is used for:
 * - LVGL filesystem images (A:/, native paths via any lv_fs_drv_t)
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

#if LV_USE_FS_EVE5_FLASH
    /* Skip flash paths — they have a dedicated zero-copy path */
    if(lv_eve5_flash_is_path(path)) {
        return false;
    }
#endif

    /* Check for supported extensions */
    bool is_jpeg = eve5_has_extension(path, ".jpg") || eve5_has_extension(path, ".jpeg");
    bool is_png = eve5_has_extension(path, ".png");
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
        parsed = eve5_parse_jpeg_dimensions(header_buf, header_read, &img_w, &img_h);
    }
    else {
        parsed = eve5_parse_png_dimensions(header_buf, header_read, &img_w, &img_h);
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
        (void)bit_depth; (void)ct_str;
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
    int32_t bpp = eve5_format_bpp(out_fmt);
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
                                   Esd_GpuHandle *out_handle, uint32_t *out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char *path = (const char *)src;
    if(!lv_eve5_sdcard_is_path(path)) {
        return false;
    }

    /* Check for supported extensions */
    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        return false;
    }

    /* Load and decode via SD card direct path.
     * Note: lv_eve5_sdcard_load_image handles HAL locking internally,
     * so we must release our lock before calling it. */
    Esd_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt, img_offset, pal_offset;

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif
    bool ok = lv_eve5_sdcard_load_image(path, &handle, &img_w, &img_h, &img_fmt, &img_offset, &pal_offset);
#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    if(!ok) {
        return false;
    }

    uint32_t alloc_base = Esd_GpuAlloc_Get(u->allocator, handle);
    if(alloc_base == GA_INVALID) {
        LV_LOG_WARN("EVE5: SD card load succeeded but RAM_G address invalid");
        return false;
    }

    *ram_g_addr = alloc_base + img_offset;
    *eve_format = (uint16_t)img_fmt;
    *src_w = (int32_t)img_w;
    *src_h = (int32_t)img_h;

    /* Calculate stride from actual format returned by HW decoder */
    int32_t bpp = eve5_format_bpp(img_fmt);
    *eve_stride = ALIGN_UP((int32_t)img_w * bpp, 4);
    if(out_handle) *out_handle = handle;
    if(out_palette_addr) *out_palette_addr = (pal_offset != GA_INVALID) ? (alloc_base + pal_offset) : GA_INVALID;

    LV_LOG_INFO("EVE5: Loaded %s via SD card direct path (%dx%d fmt=%u)", path, img_w, img_h, img_fmt);
    return true;
}
#endif /* LV_USE_FS_EVE5_SDCARD */

#if LV_USE_FS_EVE5_FLASH
/**
 * Try to load a JPEG/PNG image directly from EVE flash.
 *
 * Uses CMD_FLASHSOURCE + CMD_LOADIMAGE with OPT_FLASH for zero-copy decode.
 * Flash address is parsed from the path (e.g., "F:/4096").
 */
bool lv_draw_eve5_try_load_flash_image(lv_draw_eve5_unit_t *u, const void *src,
                                   uint32_t *ram_g_addr, uint16_t *eve_format,
                                   int32_t *eve_stride, int32_t *src_w, int32_t *src_h,
                                   Esd_GpuHandle *out_handle, uint32_t *out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char *path = (const char *)src;
    if(!lv_eve5_flash_is_path(path)) {
        return false;
    }

    /* Check for supported extensions */
    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        return false;
    }

    Esd_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt, img_offset, pal_offset;

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_display_get_default());
#endif
    bool ok = lv_eve5_flash_load_image(path, &handle, &img_w, &img_h, &img_fmt, &img_offset, &pal_offset);
#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

    if(!ok) {
        return false;
    }

    uint32_t alloc_base = Esd_GpuAlloc_Get(u->allocator, handle);
    if(alloc_base == GA_INVALID) {
        LV_LOG_WARN("EVE5: Flash load succeeded but RAM_G address invalid");
        return false;
    }

    *ram_g_addr = alloc_base + img_offset;
    *eve_format = (uint16_t)img_fmt;
    *src_w = (int32_t)img_w;
    *src_h = (int32_t)img_h;

    int32_t bpp = eve5_format_bpp(img_fmt);
    *eve_stride = ALIGN_UP((int32_t)img_w * bpp, 4);
    if(out_handle) *out_handle = handle;
    if(out_palette_addr) *out_palette_addr = (pal_offset != GA_INVALID) ? (alloc_base + pal_offset) : GA_INVALID;

    LV_LOG_INFO("EVE5: Loaded %s via flash direct path (%dx%d fmt=%u)", path, img_w, img_h, img_fmt);
    return true;
}
#endif /* LV_USE_FS_EVE5_FLASH */

/**********************
 * UNIFIED IMAGE RESOLUTION
 **********************/

/**
 * Helper: populate eve5_gpu_image_t from a try_load result (handle + raw addresses).
 * Computes defrag-safe offsets from the handle base.
 */
static void fill_gpu_image_from_load(lv_draw_eve5_unit_t *u, eve5_gpu_image_t *out,
                                      Esd_GpuHandle handle,
                                      uint32_t ram_g_addr, uint32_t palette_addr,
                                      uint16_t eve_format, int32_t eve_stride,
                                      int32_t w, int32_t h)
{
    uint32_t base = Esd_GpuAlloc_Get(u->allocator, handle);
    out->gpu_handle = handle;
    out->eve_format = eve_format;
    out->eve_stride = eve_stride;
    out->width = w;
    out->height = h;
    out->image_offset = (base != GA_INVALID && ram_g_addr >= base) ? (ram_g_addr - base) : 0;
    out->palette_offset = (palette_addr != GA_INVALID && base != GA_INVALID && palette_addr >= base)
                          ? (palette_addr - base) : GA_INVALID;
}

/**
 * Unified image resolution — loads any image source to GPU via the best available path.
 * Chains: file cache → SD card → flash → HW decode → SW decode + upload.
 * Assumes HAL mutex is LOCKED on entry (unlocks internally for file I/O).
 * The returned handle is owned by a cache — caller must NOT free it.
 */
bool lv_draw_eve5_resolve_to_gpu(lv_draw_eve5_unit_t *u, const void *src,
                                   eve5_gpu_image_t *out)
{
    lv_image_src_t src_type = lv_image_src_get_type(src);

    /* ===== FILE sources ===== */
    if(src_type == LV_IMAGE_SRC_FILE) {
        uint32_t hash = lv_draw_eve5_hash_path((const char *)src);

        /* 1. Raw file cache */
        if(lv_draw_eve5_image_cache_find_by_hash(u, hash, out)) {
            return true;
        }

        /* Temporary variables for try_load results */
        uint32_t ram_g_addr = GA_INVALID, palette_addr = GA_INVALID;
        uint16_t eve_format = ARGB8;
        int32_t eve_stride = 0, src_w = 0, src_h = 0;
        Esd_GpuHandle handle = GA_HANDLE_INVALID;
        bool loaded = false;

        /* 2. SD card direct decode (handles its own HAL lock/unlock) */
#if LV_USE_FS_EVE5_SDCARD
        if(!loaded) {
            loaded = lv_draw_eve5_try_load_sdcard_image(u, src, &ram_g_addr, &eve_format,
                                                &eve_stride, &src_w, &src_h, &handle, &palette_addr);
        }
#endif

        /* 3. Flash direct decode (handles its own HAL lock/unlock) */
#if LV_USE_FS_EVE5_FLASH
        if(!loaded) {
            loaded = lv_draw_eve5_try_load_flash_image(u, src, &ram_g_addr, &eve_format,
                                                &eve_stride, &src_w, &src_h, &handle, &palette_addr);
        }
#endif

        /* 4. HW decode via LVGL FS (expects HAL unlocked) */
#if EVE5_HW_IMAGE_DECODE
        if(!loaded) {
#if LV_USE_OS
            lv_eve5_hal_unlock(lv_display_get_default());
#endif
            loaded = lv_draw_eve5_try_load_file_image(u, src, &ram_g_addr, &eve_format,
                                                &eve_stride, &src_w, &src_h, &handle, &palette_addr);
#if LV_USE_OS
            lv_eve5_hal_lock(lv_display_get_default());
#endif
        }
#endif

        /* If HW paths succeeded, compute offsets and cache */
        if(loaded) {
            fill_gpu_image_from_load(u, out, handle, ram_g_addr, palette_addr,
                                      eve_format, eve_stride, src_w, src_h);
            /* Insert into raw file cache for next frame */
            lv_draw_eve5_image_cache_insert_raw(u,
                (uintptr_t)src, hash, handle, eve_format, eve_stride,
                (int16_t)src_w, (int16_t)src_h, out->image_offset, out->palette_offset);
            return true;
        }

        /* 5. SW decode via LVGL decoder + upload (needs HAL unlocked for decoder) */
        eve5_resolved_image_t resolved = {0};
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        bool ok = lv_draw_eve5_resolve_image_source(src, &resolved, &u->base_unit);
#if LV_USE_OS
        lv_eve5_hal_lock(lv_display_get_default());
#endif
        if(!ok) return false;

        bool uploaded = lv_draw_eve5_upload_image_to_gpu(u, resolved.img_dsc, out);
        lv_draw_eve5_release_image_source(&resolved);
        return uploaded;
    }

    /* ===== VARIABLE sources ===== */
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t *img_dsc = (const lv_image_dsc_t *)src;

        /* Check vram_res first — set by ensure_resident → vram_upload_cb for
         * decoded images from LVGL's cache. Skips the image cache entirely. */
        lv_draw_eve5_vram_res_t *vr = eve5_get_image_vram_res(img_dsc);
        if(vr != NULL) {
            uint32_t base = Esd_GpuAlloc_Get(u->allocator, vr->gpu_handle);
            if(base != GA_INVALID) {
                out->gpu_handle = vr->gpu_handle;
                out->eve_format = vr->eve_format;
                out->eve_stride = (int32_t)vr->stride;
                out->width = img_dsc->header.w;
                out->height = img_dsc->header.h;
                out->image_offset = vr->source_offset;
                out->palette_offset = vr->palette_offset;
                return true;
            }
            /* GPU allocation evicted — fall through to re-upload */
        }

        /* Const ROM images (no vram_res) and eviction recovery:
         * upload_image_to_gpu checks its own image cache, then uploads. */
        return lv_draw_eve5_upload_image_to_gpu(u, img_dsc, out);
    }

    LV_LOG_WARN("EVE5: Unsupported image source type %d", src_type);
    return false;
}

/**********************
 * IMAGE LOADING API (LEGACY WRAPPER)
 **********************/

uint32_t lv_draw_eve5_load_image(lv_draw_eve5_unit_t *u, const void *src,
                                   uint16_t *out_format, int32_t *out_stride,
                                   int32_t *out_w, int32_t *out_h,
                                   Esd_GpuHandle *out_handle,
                                   uint32_t *out_palette_addr)
{
    eve5_gpu_image_t img;
    if(!lv_draw_eve5_resolve_to_gpu(u, src, &img)) {
        if(out_handle) *out_handle = GA_HANDLE_INVALID;
        if(out_palette_addr) *out_palette_addr = GA_INVALID;
        return GA_INVALID;
    }

    uint32_t addr, pal_addr;
    eve5_gpu_image_resolve(u->allocator, &img, &addr, &pal_addr);

    *out_format = img.eve_format;
    *out_stride = img.eve_stride;
    *out_w = img.width;
    *out_h = img.height;
    if(out_handle) *out_handle = img.gpu_handle;
    if(out_palette_addr) *out_palette_addr = pal_addr;

    return addr;
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
        ok = eve5_parse_jpeg_dimensions(buf, bytes_read, &w, &h);
    }
    else {
        ok = eve5_parse_png_dimensions(buf, bytes_read, &w, &h);
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
