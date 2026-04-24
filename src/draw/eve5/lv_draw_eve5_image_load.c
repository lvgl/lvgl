/**
 * @file lv_draw_eve5_image_load.c
 *
 * EVE5 (BT820) Image Loading and Decode
 *
 * Image loading priority chain:
 * 1. LVGL image cache (prior decode results)
 * 2. SD card direct path (zero-copy HW decode)
 * 3. Flash direct path (zero-copy HW decode)
 * 4. CMD_LOADIMAGE via LVGL FS (streaming HW decode)
 * 5. SW decode + upload (fallback for unsupported formats)
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
 * For files, opens with use_indexed=true to preserve indexed formats.
 */
bool lv_draw_eve5_resolve_image_source(LV_IMAGE_DSC_CONST void * src, eve5_resolved_image_t * resolved,
                                       lv_draw_unit_t * draw_unit)
{
    lv_memzero(resolved, sizeof(*resolved));

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        resolved->img_dsc = (LV_IMAGE_DSC_CONST lv_image_dsc_t *)src;
        resolved->decoder_open = false;
        if(draw_unit != NULL) {
            if(!lv_draw_buf_ensure_resident((lv_draw_buf_t *)resolved->img_dsc, draw_unit)) {
                LV_LOG_WARN("EVE5: Failed to ensure variable image residency");
                return false;
            }
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

        resolved->img_dsc = (LV_IMAGE_DSC_CONST lv_image_dsc_t *)resolved->decoder_dsc.decoded;
        resolved->decoder_open = true;
        if(draw_unit != NULL) {
            if(!lv_draw_buf_ensure_resident((lv_draw_buf_t *)resolved->img_dsc, draw_unit)) {
                LV_LOG_WARN("EVE5: Failed to ensure file image residency");
                lv_image_decoder_close(&resolved->decoder_dsc);
                resolved->decoder_open = false;
                resolved->img_dsc = NULL;
                return false;
            }
        }
        return true;
    }

    LV_LOG_WARN("EVE5: Unsupported image source type %d", src_type);
    return false;
}

void lv_draw_eve5_release_image_source(eve5_resolved_image_t * resolved)
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
 **********************/

/**
 * Stream a JPEG/PNG through CMD_LOADIMAGE for hardware decompression.
 *
 * HAL mutex: expects UNLOCKED on entry (file I/O may need it), locks internally
 * for EVE commands, returns with HAL UNLOCKED.
 */
bool lv_draw_eve5_try_load_file_image(lv_draw_eve5_unit_t * u, const void * src,
                                      uint32_t * ram_g_addr, uint16_t * eve_format,
                                      int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                      Esd_GpuHandle *out_handle, uint32_t * out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char * path = (const char *)src;

#if LV_USE_FS_EVE5_SDCARD
    if(lv_eve5_sdcard_is_path(path)) {
        return false;
    }
#endif

#if LV_USE_FS_EVE5_FLASH
    if(lv_eve5_flash_is_path(path)) {
        return false;
    }
#endif

    bool is_jpeg = eve5_has_extension(path, ".jpg") || eve5_has_extension(path, ".jpeg");
    bool is_png = eve5_has_extension(path, ".png");
    if(!is_jpeg && !is_png) {
        return false;
    }

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("EVE5: Failed to open file: %s", path);
        return false;
    }

    /* Read header for dimension parsing */
    uint8_t header_buf[1024];
    uint32_t header_read = 0;
    res = lv_fs_read(&file, header_buf, sizeof(header_buf), &header_read);
    if(res != LV_FS_RES_OK || header_read < 24) {
        LV_LOG_WARN("EVE5: Failed to read file header: %s", path);
        lv_fs_close(&file);
        return false;
    }

    uint32_t img_w = 0, img_h = 0;
    bool parsed;
    if(is_jpeg) {
        parsed = eve5_parse_jpeg_dimensions(header_buf, header_read, &img_w, &img_h);
    }
    else {
        parsed = eve5_parse_png_dimensions(header_buf, header_read, &img_w, &img_h);
    }

    if(is_png && header_read >= 26) {
        uint8_t bit_depth = header_buf[24];
        uint8_t color_type = header_buf[25];
        const char * ct_str = "unknown";
        if(color_type == 0) ct_str = "grayscale";
        else if(color_type == 2) ct_str = "RGB";
        else if(color_type == 3) ct_str = "indexed";
        else if(color_type == 4) ct_str = "gray+alpha";
        else if(color_type == 6) ct_str = "RGBA";
        (void)bit_depth;
        (void)ct_str;
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

    /* Allocate for worst case (ARGB8); actual format determined after decode */
    int32_t decoded_stride = ALIGN_UP((int32_t)(img_w * 4), 4);
    uint32_t decoded_size = (uint32_t)(decoded_stride * (int32_t)img_h);

#if LV_USE_OS
    lv_eve5_hal_lock(lv_display_get_default());
#endif

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

    if(phost->CmdFault) {
        LV_LOG_ERROR("EVE5: Coprocessor fault before CMD_LOADIMAGE");
        Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        lv_fs_close(&file);
        return false;
    }

    /* OPT_TRUECOLOR: decode to RGB8/ARGB8 instead of RGB565/ARGB4 (avoids banding) */
    EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
    EVE_Cmd_wr32(phost, addr);
    EVE_Cmd_wr32(phost, OPT_NODL | OPT_TRUECOLOR);

    /* Stream file data while holding HAL lock.
     * SD card and flash paths are excluded above, so lv_fs_read won't hit
     * an EVE FS driver that would deadlock on the non-recursive HAL mutex. */
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

    if(!EVE_Cmd_waitFlush(phost)) {
        LV_LOG_ERROR("EVE5: CMD_LOADIMAGE failed for %s", path);
        Esd_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        return false;
    }

    EVE_Hal_requestFenceBeforeSwap(phost);

    /* Get actual format from coprocessor */
    uint32_t out_source = 0, out_fmt = 0, out_w = 0, out_h = 0, out_palette = 0;
    bool got_image = EVE_CoCmd_getImage(phost, &out_source, &out_fmt, &out_w, &out_h, &out_palette);

    LV_LOG_INFO("EVE5 HW_DECODE: getImage: source=0x%08x fmt=%u w=%u h=%u palette=0x%08x (alloc=0x%08x)",
                out_source, out_fmt, out_w, out_h, out_palette, addr);

    if(got_image) {
        /* For PALETTEDARGB8, CMD_LOADIMAGE stores palette at alloc start */
        if(out_fmt == PALETTEDARGB8) {
            if(!out_palette_addr) {
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

    /* HW decoder output stride = width * bpp (packed, no padding) */
    int32_t bpp = eve5_format_bpp(out_fmt);
    decoded_stride = (int32_t)(img_w * bpp);

    LV_LOG_INFO("EVE5 HW_DECODE: final: fmt=%u bpp=%d stride=%d size=%u (worst-case was %u) for %s",
                out_fmt, bpp, (int)decoded_stride,
                (unsigned)(decoded_stride * (int32_t)img_h), decoded_size, path);

    /* Trim allocation to actual size */
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
 * Load JPEG/PNG directly from EVE5 SD card (zero-copy HW decode).
 * HAL mutex: expects UNLOCKED (lv_eve5_sdcard_load_image locks internally).
 */
bool lv_draw_eve5_try_load_sdcard_image(lv_draw_eve5_unit_t * u, const void * src,
                                        uint32_t * ram_g_addr, uint16_t * eve_format,
                                        int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                        Esd_GpuHandle *out_handle, uint32_t * out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char * path = (const char *)src;
    if(!lv_eve5_sdcard_is_path(path)) {
        return false;
    }

    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        return false;
    }

    Esd_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt, img_offset, pal_offset;

    bool ok = lv_eve5_sdcard_load_image(path, &handle, &img_w, &img_h, &img_fmt, &img_offset, &pal_offset);

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
 * Load JPEG/PNG directly from EVE flash (zero-copy HW decode).
 * HAL mutex: expects UNLOCKED (lv_eve5_flash_load_image locks internally).
 */
bool lv_draw_eve5_try_load_flash_image(lv_draw_eve5_unit_t * u, const void * src,
                                       uint32_t * ram_g_addr, uint16_t * eve_format,
                                       int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                       Esd_GpuHandle *out_handle, uint32_t * out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) {
        return false;
    }

    const char * path = (const char *)src;
    if(!lv_eve5_flash_is_path(path)) {
        return false;
    }

    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        return false;
    }

    Esd_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt, img_offset, pal_offset;

    bool ok = lv_eve5_flash_load_image(path, &handle, &img_w, &img_h, &img_fmt, &img_offset, &pal_offset);

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
 * Load any image source to GPU via the best available path.
 * HAL mutex: expects LOCKED on entry, unlocks internally for file I/O.
 *
 * Returns a pointer to the vram_res that owns the GPU allocation.
 * The pointer is stable for the duration of the current render call
 * (backed by a vram_res on a draw_buf, or by the image cache).
 * Caller must NOT free the handle; it is cache/vram_res-owned.
 *
 * Returns NULL on failure.
 */
lv_eve5_vram_res_t * lv_draw_eve5_resolve_to_gpu(lv_draw_eve5_unit_t * u, LV_IMAGE_DSC_CONST void * src)
{
    lv_image_src_t src_type = lv_image_src_get_type(src);

    if(src_type == LV_IMAGE_SRC_FILE) {
        eve5_resolved_image_t resolved = {0};
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        bool ok = lv_draw_eve5_resolve_image_source(src, &resolved, &u->base_unit);
#if LV_USE_OS
        lv_eve5_hal_lock(lv_display_get_default());
#endif
        if(!ok) return NULL;

        /* upload_image_to_gpu checks existing vram_res, uploads if needed,
         * and attaches vram_res to the image descriptor. */
        lv_eve5_vram_res_t * vr = lv_draw_eve5_upload_image_to_gpu(u, resolved.img_dsc);
        lv_draw_eve5_release_image_source(&resolved);
        return vr;
    }

    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        return lv_draw_eve5_upload_image_to_gpu(u, (LV_IMAGE_DSC_CONST lv_image_dsc_t *)src);
    }

    LV_LOG_WARN("EVE5: Unsupported image source type %d", src_type);
    return NULL;
}

/**********************
 * EVE5 IMAGE DECODER
 *
 * LVGL decoder for JPEG/PNG hardware decoding. Creates VRAM-only draw_bufs
 * that LVGL caches. On cache eviction, vram_free_cb releases GPU memory.
 **********************/

#if EVE5_HW_IMAGE_DECODE

static lv_draw_eve5_unit_t * s_decoder_unit;

static lv_color_format_t eve_format_to_lv_cf(uint16_t eve_fmt)
{
    switch(eve_fmt) {
        case ARGB8:
            return LV_COLOR_FORMAT_ARGB8888;
        case RGB8:
            return LV_COLOR_FORMAT_RGB888;
        case RGB565:
            return LV_COLOR_FORMAT_RGB565;
        case PALETTEDARGB8:
            return LV_COLOR_FORMAT_I8;
        case L8:
            return LV_COLOR_FORMAT_L8;
        default:
            return LV_COLOR_FORMAT_ARGB8888;
    }
}

static lv_result_t eve5_decoder_info(lv_image_decoder_t * decoder,
                                     lv_image_decoder_dsc_t * dsc,
                                     lv_image_header_t * header)
{
    LV_UNUSED(decoder);

    if(dsc->src_type != LV_IMAGE_SRC_FILE) return LV_RESULT_INVALID;

    const char * fn = dsc->src;
    const char * ext = lv_fs_get_ext(fn);

    bool is_jpeg = (lv_strcmp(ext, "jpg") == 0) || (lv_strcmp(ext, "jpeg") == 0);
    bool is_png = (lv_strcmp(ext, "png") == 0);
    if(!is_jpeg && !is_png) return LV_RESULT_INVALID;

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

    header->cf = LV_COLOR_FORMAT_RAW;
    header->w = (int32_t)w;
    header->h = (int32_t)h;
    header->stride = (int32_t)(w * 3);

    return LV_RESULT_OK;
}

static lv_result_t eve5_decoder_open(lv_image_decoder_t * decoder,
                                     lv_image_decoder_dsc_t * dsc)
{
    if(dsc->src_type != LV_IMAGE_SRC_FILE) return LV_RESULT_INVALID;
    if(s_decoder_unit == NULL) return LV_RESULT_INVALID;

    lv_draw_eve5_unit_t * u = s_decoder_unit;
    const char * path = (const char *)dsc->src;

    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        return LV_RESULT_INVALID;
    }

    uint32_t ram_g_addr = GA_INVALID, palette_addr = GA_INVALID;
    uint16_t eve_format = ARGB8;
    int32_t eve_stride = 0, src_w = 0, src_h = 0;
    Esd_GpuHandle handle = GA_HANDLE_INVALID;
    bool loaded = false;

#if LV_USE_FS_EVE5_SDCARD
    if(!loaded) {
        loaded = lv_draw_eve5_try_load_sdcard_image(u, dsc->src, &ram_g_addr, &eve_format,
                                                    &eve_stride, &src_w, &src_h, &handle, &palette_addr);
    }
#endif

#if LV_USE_FS_EVE5_FLASH
    if(!loaded) {
        loaded = lv_draw_eve5_try_load_flash_image(u, dsc->src, &ram_g_addr, &eve_format,
                                                   &eve_stride, &src_w, &src_h, &handle, &palette_addr);
    }
#endif

    if(!loaded) {
        loaded = lv_draw_eve5_try_load_file_image(u, dsc->src, &ram_g_addr, &eve_format,
                                                  &eve_stride, &src_w, &src_h, &handle, &palette_addr);
    }

    if(!loaded) {
        return LV_RESULT_INVALID;
    }

    /* Detect grayscale PALETTEDARGB8 and promote to L8.
     * BT820 produces PALETTEDARGB8 for grayscale PNGs. If the palette is a
     * simple grayscale ramp (R=G=B=i, A=255), reinterpret as L8 because the index
     * data is already the luminance value. Enables SW renderer's apply_mask. */
    if(eve_format == PALETTEDARGB8 && palette_addr != GA_INVALID) {
#if LV_USE_OS
        lv_eve5_hal_lock(lv_display_get_default());
#endif
        uint8_t pal_buf[256 * 4];
        EVE_Hal_rdMem(u->hal, pal_buf, palette_addr, sizeof(pal_buf));
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_display_get_default());
#endif
        bool is_grayscale = true;
        for(uint32_t i = 0; i < 256; i++) {
            uint8_t b = pal_buf[i * 4 + 0];
            uint8_t g = pal_buf[i * 4 + 1];
            uint8_t r = pal_buf[i * 4 + 2];
            uint8_t a = pal_buf[i * 4 + 3];
            if(r != (uint8_t)i || g != (uint8_t)i || b != (uint8_t)i || a != 255) {
                is_grayscale = false;
                break;
            }
        }
        if(is_grayscale) {
            LV_LOG_INFO("EVE5 HW decoder: PALETTEDARGB8 has grayscale ramp palette, promoting to L8");
            eve_format = L8;
            palette_addr = GA_INVALID;
        }
    }

    /* Compute handle-relative offsets (defrag-safe) */
    uint32_t alloc_base = Esd_GpuAlloc_Get(u->allocator, handle);
    uint32_t source_offset = (alloc_base != GA_INVALID && ram_g_addr >= alloc_base)
                             ? (ram_g_addr - alloc_base) : 0;
    uint32_t pal_offset = (palette_addr != GA_INVALID && alloc_base != GA_INVALID && palette_addr >= alloc_base)
                          ? (palette_addr - alloc_base) : GA_INVALID;
    uint32_t alloc_size = (uint32_t)(eve_stride * src_h);
    if(pal_offset != GA_INVALID) alloc_size += 256 * 4;

    lv_eve5_vram_res_t * vr = lv_malloc(sizeof(lv_eve5_vram_res_t));
    if(vr == NULL) {
        Esd_GpuAlloc_Free(u->allocator, handle);
        return LV_RESULT_INVALID;
    }
    vr->base.unit = (lv_draw_unit_t *)u;
    vr->base.size = alloc_size;
    vr->gpu_handle = handle;
    vr->eve_format = eve_format;
    vr->stride = (uint32_t)eve_stride;
    vr->source_offset = source_offset;
    vr->palette_offset = pal_offset;
    vr->is_premultiplied = false;
    vr->has_content = true;

    lv_color_format_t lv_cf = eve_format_to_lv_cf(eve_format);
    lv_draw_buf_t * decoded = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    if(decoded == NULL) {
        lv_free(vr);
        Esd_GpuAlloc_Free(u->allocator, handle);
        return LV_RESULT_INVALID;
    }
    decoded->header.magic = LV_IMAGE_HEADER_MAGIC;
    decoded->header.w = src_w;
    decoded->header.h = src_h;
    decoded->header.cf = lv_cf;
    decoded->header.stride = eve_stride;
    decoded->header.flags = LV_IMAGE_FLAGS_ALLOCATED;
    decoded->data_size = alloc_size;
    decoded->data = NULL;
    decoded->unaligned_data = NULL;
    decoded->handlers = lv_draw_buf_get_image_handlers();
    decoded->vram_res = (lv_draw_buf_vram_res_t *)vr;

    dsc->decoded = decoded;

    if(lv_image_cache_is_enabled()) {
        lv_image_cache_data_t search_key;
        lv_memzero(&search_key, sizeof(search_key));
        search_key.src = dsc->src;
        search_key.src_type = dsc->src_type;
        search_key.slot.size = alloc_size;

        dsc->cache_entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);
    }

    dsc->header.w = src_w;
    dsc->header.h = src_h;
    dsc->header.cf = lv_cf;
    dsc->header.stride = eve_stride;

    LV_LOG_INFO("EVE5 HW decoder: opened %s (%dx%d fmt=%d) → LVGL cache", path, src_w, src_h, eve_format);

    return LV_RESULT_OK;
}

void lv_draw_eve5_register_image_decoder(lv_draw_eve5_unit_t * unit)
{
    s_decoder_unit = unit;

    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, eve5_decoder_info);
    lv_image_decoder_set_open_cb(dec, eve5_decoder_open);
    dec->name = "EVE5_HW";
}

#endif /* EVE5_HW_IMAGE_DECODE */

#endif /* LV_USE_DRAW_EVE5 */
