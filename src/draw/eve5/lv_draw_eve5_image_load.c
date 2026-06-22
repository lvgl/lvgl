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

#include "../lv_image_decoder_private.h"
#include "../../misc/cache/instance/lv_image_header_cache.h"
#if LV_USE_FS_EVE5_SDCARD
    #include "../../drivers/display/eve5/lv_eve5_sdcard.h"
#endif
#if LV_USE_FS_EVE5_FLASH
    #include "../../drivers/display/eve5/lv_eve5_flash.h"
#endif

/**********************
 * HW-DECODE BAD-PATH CACHE
 **********************/

/* LVGL commits to whichever decoder's info_cb returns OK first and caches
 * that commitment in its header cache. If our open_cb later fails (e.g.
 * BT820 firmware rejects an L1 PNG with "unsupported PNG in
 * cmd_loadimage"), LVGL doesn't fall back — every subsequent open of the
 * same path hits the header cache and re-enters our failing open_cb. We
 * also can't peek file contents in info_cb to predict failure (on SD that
 * would force the lazy whole-file load CMD_QUERYIMAGE exists to avoid).
 *
 * The bad-path cache breaks the loop: open_cb's failure path marks the
 * path here and drops LVGL's header-cache entry. The next info_cb sees
 * the mark and returns INVALID, so LVGL iterates onward and the SW
 * decoder (LodePNG, etc.) gets the commit. The mark also turns "QUERY
 * runs once per image" into a real invariant — without it CMD_QUERYIMAGE
 * would re-run from info_cb every frame on the failure path. Round-robin
 * eviction handles aging if many distinct bad paths show up. */
#define EVE5_DECODER_BAD_CACHE_SIZE 32
#define EVE5_DECODER_BAD_PATH_MAX   128

static struct {
    char paths[EVE5_DECODER_BAD_CACHE_SIZE][EVE5_DECODER_BAD_PATH_MAX];
    uint32_t next_slot;
} s_eve5_bad_paths;

static bool eve5_decoder_is_path_bad(const char * path)
{
    if(path == NULL) return false;
    for(uint32_t i = 0; i < EVE5_DECODER_BAD_CACHE_SIZE; i++) {
        if(s_eve5_bad_paths.paths[i][0] != 0
           && lv_strcmp(s_eve5_bad_paths.paths[i], path) == 0) {
            return true;
        }
    }
    return false;
}

static void eve5_decoder_mark_path_bad(const char * path)
{
    if(path == NULL) return;
    if(eve5_decoder_is_path_bad(path)) return;

    /* Prefer an empty slot first so the first 32 distinct failures all
     * stick before round-robin eviction starts. */
    for(uint32_t i = 0; i < EVE5_DECODER_BAD_CACHE_SIZE; i++) {
        if(s_eve5_bad_paths.paths[i][0] == 0) {
            lv_strncpy(s_eve5_bad_paths.paths[i], path, EVE5_DECODER_BAD_PATH_MAX);
            s_eve5_bad_paths.paths[i][EVE5_DECODER_BAD_PATH_MAX - 1] = '\0';
            return;
        }
    }

    lv_strncpy(s_eve5_bad_paths.paths[s_eve5_bad_paths.next_slot], path, EVE5_DECODER_BAD_PATH_MAX);
    s_eve5_bad_paths.paths[s_eve5_bad_paths.next_slot][EVE5_DECODER_BAD_PATH_MAX - 1] = '\0';
    s_eve5_bad_paths.next_slot = (s_eve5_bad_paths.next_slot + 1) % EVE5_DECODER_BAD_CACHE_SIZE;
}

/**********************
 * IMAGE SOURCE RESOLUTION
 **********************/

/**
 * Resolve an image source to an lv_image_dsc_t.
 * For files, opens with use_indexed=true to preserve indexed formats.
 */
bool lv_draw_eve5_resolve_image_source(const void * src, eve5_resolved_image_t * resolved,
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
                                      EVE_GpuHandle *out_handle, uint32_t * out_palette_addr)
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

    /* Allocate for worst case; actual format determined after decode.
     * Paletted output is a palette (1024 bytes for PALETTEDARGB8) plus w*h
     * indices, which exceeds the ARGB8 size for images under ~342 pixels. */
    int32_t decoded_stride = (int32_t)(img_w * 4);
    uint32_t decoded_size = (uint32_t)(decoded_stride * (int32_t)img_h);
    uint32_t paletted_size = 256 * 4 + img_w * img_h;
    if(paletted_size > decoded_size) decoded_size = paletted_size;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_eve5_disp_from_hal(u->hal));
#endif

    /* GC-flagged: decoded images self-heal through the decoder cache when
     * the handle goes invalid (sweep on pre-BT820, pressure eviction on
     * BT820+) */
    uint32_t alloc_flags = GA_ALIGN_4 | GA_GC_FLAG;
    EVE_GpuHandle handle = EVE_GpuAlloc_Alloc(u->allocator, decoded_size, alloc_flags);
    uint32_t addr = EVE_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate %u bytes for decoded image", decoded_size);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif
        lv_fs_close(&file);
        return false;
    }

    EVE_HalContext *phost = u->hal;
    lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);

    /* Stage the compressed file in RAM_G and feed CMD_LOADIMAGE through
     * MediaFifo (OPT_MEDIAFIFO) so the decoder reads from an isolated
     * buffer rather than the cocmd FIFO. Trailing bytes past the
     * decoder's end-of-stream get dropped instead of being interpreted
     * as commands, defending against malformed or attacker-crafted PNG/JPEG. */
    uint32_t fifo_size = (file_size + 3u) & ~3u;
    EVE_GpuHandle temp_handle = EVE_GpuAlloc_AlignedAlloc(u->allocator, fifo_size, 0, 32);
    uint32_t temp_addr = EVE_GpuAlloc_Get(u->allocator, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate %u-byte MediaFifo buffer for %s",
                    fifo_size, path);
        EVE_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        lv_fs_close(&file);
        return false;
    }

    /* Stream the file into temp_addr (file → host chunk → RAM_G).
     * SD card and flash paths are excluded above, so lv_fs_read won't hit
     * an EVE FS driver that would deadlock on the non-recursive HAL mutex. */
    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    uint8_t chunk_buf[8192];
    uint32_t write_offset = 0;
    uint32_t remaining = file_size;
    bool success = true;
    while(remaining > 0) {
        uint32_t chunk_size = remaining > sizeof(chunk_buf) ? sizeof(chunk_buf) : remaining;
        uint32_t bytes_read = 0;
        res = lv_fs_read(&file, chunk_buf, chunk_size, &bytes_read);
        if(res != LV_FS_RES_OK || bytes_read == 0) {
            LV_LOG_ERROR("EVE5: File read error at offset %u", file_size - remaining);
            success = false;
            break;
        }
        EVE_Hal_wrMem(phost, temp_addr + write_offset, chunk_buf, bytes_read);
        write_offset += bytes_read;
        remaining -= bytes_read;
    }
    lv_fs_close(&file);

    if(!success) {
        EVE_GpuAlloc_Free(u->allocator, temp_handle);
        EVE_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return false;
    }

    /* Wrap MediaFifo over the populated buffer and stamp REG_MEDIAFIFO_WRITE
     * with the actual byte count so the coprocessor sees a fully-populated
     * fifo (same trick the legacy SD load uses after CMD_FSREAD). */
    EVE_MediaFifo_close(phost);
    if(!EVE_MediaFifo_set(phost, temp_addr, fifo_size)) {
        LV_LOG_ERROR("EVE5: MediaFifo setup failed for %s", path);
        EVE_GpuAlloc_Free(u->allocator, temp_handle);
        EVE_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return false;
    }
    EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, file_size);

    /* OPT_TRUECOLOR is BT820-only — it requests RGB8/ARGB8 output instead of
     * the default RGB565/ARGB4 (which avoids gradient banding). On earlier
     * gens those output formats don't exist, so we drop the flag and accept
     * the default 16bpp output. */
    uint32_t loadimage_opts = OPT_NODL | OPT_MEDIAFIFO;
    if(EVE_Hal_supportRenderTarget(phost)) loadimage_opts |= OPT_TRUECOLOR;

    /* Drain the FIFO so any subsequent fault is attributable to CMD_LOADIMAGE
     * itself, not stale queued work; bracket with SuppressErrorOverlay so the
     * HAL's debug overlay doesn't flash for an isolated fault we'll reset
     * immediately. Mirrors Esd_LoadResourceEx / Esd_LoadBitmapEx. */
    if(!EVE_Cmd_waitFlush(phost)) {
        LV_LOG_ERROR("EVE5: Pre-load flush failed for %s", path);
        EVE_MediaFifo_close(phost);
        EVE_GpuAlloc_Free(u->allocator, temp_handle);
        EVE_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = true;

    EVE_CoCmd_loadImage(phost, addr, loadimage_opts);
    bool decoded = EVE_Cmd_waitFlush(phost);

    EVE_MediaFifo_close(phost);
    EVE_GpuAlloc_Free(u->allocator, temp_handle);

    if(!decoded) {
        LV_LOG_ERROR("EVE5: CMD_LOADIMAGE failed for %s", path);
        EVE_GpuAlloc_Free(u->allocator, handle);
        /* Narrow reset: the pre-flush isolated this fault, so we can recover
         * the coprocessor without disturbing any earlier successful work. */
        if(phost->CmdFault) lv_eve5_reset_coprocessor(disp);
        phost->SuppressErrorOverlay = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = false;

    EVE_Hal_requestFenceBeforeSwap(phost);

    /* Query format from the coprocessor. CMD_GETIMAGE (returning source/fmt/w/h/palette
     * in one call) is BT817+. On earlier chips we use CMD_GETIMAGE_FORMAT, which falls
     * back to reading the format word from a hardcoded coprocessor RAM address
     * (0x3097e8) for backward compat — see EVE_CoCmd_IO.c. Earlier-gen output formats
     * are not paletted-with-separate-address, so we use the addr we passed to
     * CMD_LOADIMAGE for the source and the parsed JPEG/PNG dimensions for w/h. */
    uint32_t out_source = 0, out_fmt = 0, out_w = 0, out_h = 0, out_palette = 0;
    bool got_image = false;

#if (EVE_SUPPORT_CHIPID >= EVE_BT817)
    if(EVE_CHIPID >= EVE_BT817) {
        got_image = EVE_CoCmd_getImage(phost, &out_source, &out_fmt, &out_w, &out_h, &out_palette);
        LV_LOG_INFO("EVE5 HW_DECODE: getImage: source=0x%08x fmt=%u w=%u h=%u palette=0x%08x (alloc=0x%08x)",
                    out_source, out_fmt, out_w, out_h, out_palette, addr);
    }
    else
#endif
    {
        got_image = EVE_CoCmd_getImage_format(phost, &out_fmt);
        out_source = addr;
        out_w = img_w;
        out_h = img_h;
        out_palette = 0;
        LV_LOG_INFO("EVE5 HW_DECODE: getImage_format: fmt=%u (header w=%u h=%u alloc=0x%08x)",
                    out_fmt, img_w, img_h, addr);
    }

    if(got_image) {
        /* For PALETTEDARGB8, CMD_LOADIMAGE stores palette at alloc start.
         * PALETTEDARGB8 is BT820-only — pre-BT820 builds skip this branch. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        if(out_fmt == PALETTEDARGB8) {
            if(!out_palette_addr) {
                LV_LOG_INFO("EVE5 HW_DECODE: PALETTEDARGB8 not supported by caller, falling back to SW for %s", path);
                EVE_GpuAlloc_Free(u->allocator, handle);
#if LV_USE_OS
                lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif
                return false;
            }
            *out_palette_addr = out_palette;
            addr = out_source;
        }
        else
#endif
        {
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
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    uint32_t palette_offset = (out_fmt == PALETTEDARGB8) ? (uint32_t)(addr - EVE_GpuAlloc_Get(u->allocator, handle)) : 0;
#else
    uint32_t palette_offset = 0;
#endif
    uint32_t actual_size = palette_offset + index_size;
    if(actual_size < decoded_size) {
        EVE_GpuAlloc_Truncate(u->allocator, handle, actual_size);
    }

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
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

/**********************
 * ESDM SIDECAR LOADING
 *
 * A ".esdm" metadata sidecar lets a raw / deflate / relocatable-asset bitmap
 * load directly to RAM_G in its native EVE format, skipping CMD_LOADIMAGE
 * decode. The sidecar supplies the EVE bitmap format, dimensions, stride, and
 * load method (see IMAGE_FORMATS.md and esd_core/Esd_BitmapInfo.c).
 *
 * EVE SD card images use raw coprocessor FS commands (in lv_eve5_sdcard.c) for
 * both the sidecar and the data, deliberately never touching the LVGL SD
 * filesystem driver — opening the sidecar through lv_fs would invalidate the
 * coprocessor open-file state for the image the decoder framework already holds.
 * Host images read the sidecar and data through lv_fs.
 **********************/

/* Read exactly @p max_bytes of a host file into RAM_G via direct memory writes.
 * Caller holds the HAL lock; the file is on the host filesystem (SD/flash paths
 * are dispatched elsewhere), so lv_fs reads don't re-enter the HAL mutex. */
static bool eve5_esdm_host_read_raw(lv_draw_eve5_unit_t * u, const char * path,
                                    uint32_t dst, uint32_t max_bytes)
{
    lv_fs_file_t f;
    if(lv_fs_open(&f, path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;

    uint8_t chunk[4096];
    uint32_t done = 0;
    bool ok = true;
    while(done < max_bytes) {
        uint32_t want = max_bytes - done;
        if(want > sizeof(chunk)) want = sizeof(chunk);
        uint32_t got = 0;
        if(lv_fs_read(&f, chunk, want, &got) != LV_FS_RES_OK || got == 0) {
            ok = false;
            break;
        }
        EVE_Hal_wrMem(u->hal, dst + done, chunk, got);
        done += got;
    }
    lv_fs_close(&f);
    return ok && (done == max_bytes);
}

/* Stage a host file in RAM_G and feed CMD_INFLATE2 (deflate) or CMD_LOADASSET
 * (relocatable asset) through MediaFifo (OPT_MEDIAFIFO). Same safety model
 * as the .jpg/.png CMD_LOADIMAGE path: the compressed body never co-mingles
 * with the cocmd FIFO, and a load fault is narrowly recovered by
 * lv_eve5_reset_coprocessor. Caller holds the HAL lock. */
static bool eve5_esdm_host_stream(lv_draw_eve5_unit_t * u, const char * path,
                                  bool is_asset, uint32_t dst)
{
#if !((EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET))
    if(is_asset) return false;  /* CMD_LOADASSET is BT820+ */
#endif

    EVE_HalContext * phost = u->hal;
    lv_display_t * disp = lv_eve5_disp_from_hal(phost);

    /* Discover file size, then read the body into a RAM_G temp buffer.
     * Caller already holds the HAL lock, so lv_fs must be host-side only
     * (the EVE5 SD/flash FS drivers would deadlock on re-entry). */
    lv_fs_file_t f;
    if(lv_fs_open(&f, path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;
    uint32_t file_size = 0;
    if(lv_fs_seek(&f, 0, LV_FS_SEEK_END) != LV_FS_RES_OK
       || lv_fs_tell(&f, &file_size) != LV_FS_RES_OK
       || lv_fs_seek(&f, 0, LV_FS_SEEK_SET) != LV_FS_RES_OK
       || file_size == 0) {
        lv_fs_close(&f);
        return false;
    }

    uint32_t fifo_size = (file_size + 3u) & ~3u;
    EVE_GpuHandle temp_handle = EVE_GpuAlloc_AlignedAlloc(u->allocator, fifo_size, 0, 32);
    uint32_t temp_addr = EVE_GpuAlloc_Get(u->allocator, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 esdm: failed to allocate %u-byte MediaFifo buffer for %s",
                    fifo_size, path);
        lv_fs_close(&f);
        return false;
    }

    uint8_t chunk[8192];
    uint32_t write_offset = 0;
    bool ok = true;
    for(;;) {
        uint32_t got = 0;
        if(lv_fs_read(&f, chunk, sizeof(chunk), &got) != LV_FS_RES_OK) { ok = false; break; }
        if(got == 0) break;
        EVE_Hal_wrMem(phost, temp_addr + write_offset, chunk, got);
        write_offset += got;
    }
    lv_fs_close(&f);

    if(!ok || write_offset != file_size) {
        EVE_GpuAlloc_Free(u->allocator, temp_handle);
        return false;
    }

    EVE_MediaFifo_close(phost);
    if(!EVE_MediaFifo_set(phost, temp_addr, fifo_size)) {
        LV_LOG_ERROR("EVE5 esdm: MediaFifo setup failed for %s", path);
        EVE_GpuAlloc_Free(u->allocator, temp_handle);
        return false;
    }
    EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, file_size);

    /* Pre-flush + SuppressErrorOverlay so any fault is isolated to the
     * just-issued load and the HAL's debug overlay doesn't flash for it. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_MediaFifo_close(phost);
        EVE_GpuAlloc_Free(u->allocator, temp_handle);
        return false;
    }
    phost->SuppressErrorOverlay = true;

    if(is_asset) {
#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
        EVE_CoCmd_loadAsset(phost, dst, OPT_MEDIAFIFO);
#endif
    }
    else {
        /* On BT81X CMD_INFLATE2 carries an options word (the original
         * CMD_INFLATE doesn't); on BT820 the natural CMD_INFLATE opcode
         * was renamed and now takes options like BT81X CMD_INFLATE2 did.
         * EVE_CoCmd_inflate2 emits the right opcode via
         * CMD_INFLATE2_COMPATIBILITY for either chip family — required so
         * BT820 single-target builds (where the raw CMD_INFLATE2 macro is
         * removed) still compile, and so we don't accidentally hit BT81X's
         * data-follows CMD_INFLATE form. Works on BT815+. */
        EVE_CoCmd_inflate2(phost, dst, OPT_MEDIAFIFO);
    }
    bool loaded = EVE_Cmd_waitFlush(phost);

    EVE_MediaFifo_close(phost);
    EVE_GpuAlloc_Free(u->allocator, temp_handle);

    if(!loaded) {
        if(phost->CmdFault) lv_eve5_reset_coprocessor(disp);
        phost->SuppressErrorOverlay = false;
        return false;
    }
    phost->SuppressErrorOverlay = false;
    return true;
}

/* Read + parse the ".esdm" sidecar for @p path. SD card sidecars use raw
 * coprocessor FS commands (bypassing the LVGL SD driver); host sidecars use
 * lv_fs. Returns false if no valid BMP sidecar exists. */
static bool eve5_probe_esdm(const char * path, eve5_esdm_bmp_t * out)
{
    uint8_t meta[EVE5_ESDM_MAX];
    uint32_t got = 0;

#if LV_USE_FS_EVE5_FLASH
    if(lv_eve5_flash_is_path(path)) return false;
#endif
#if LV_USE_FS_EVE5_SDCARD
    if(lv_eve5_sdcard_is_path(path)) {
        if(!lv_eve5_sdcard_read_esdm(path, meta, sizeof(meta), &got)) return false;
        return eve5_parse_esdm_bmp(meta, got, out);
    }
#endif

    char meta_path[256];
    if(!eve5_esdm_meta_path(path, meta_path, sizeof(meta_path))) return false;
    lv_fs_file_t f;
    if(lv_fs_open(&f, meta_path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;
    lv_fs_read(&f, meta, sizeof(meta), &got);
    lv_fs_close(&f);
    return eve5_parse_esdm_bmp(meta, got, out);
}

/* Host filesystem branch of the esdm loader.
 * HAL mutex: expects UNLOCKED on entry, locks internally, returns UNLOCKED. */
static bool eve5_load_esdm_host(lv_draw_eve5_unit_t * u, const char * path,
                                uint32_t * ram_g_addr, uint16_t * eve_format,
                                int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                EVE_GpuHandle *out_handle, uint32_t * out_palette_addr)
{
    char meta_path[256];
    if(!eve5_esdm_meta_path(path, meta_path, sizeof(meta_path))) return false;

    lv_fs_file_t mf;
    if(lv_fs_open(&mf, meta_path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;
    uint8_t meta[EVE5_ESDM_MAX];
    uint32_t got = 0;
    lv_fs_read(&mf, meta, sizeof(meta), &got);
    lv_fs_close(&mf);

    eve5_esdm_bmp_t bmp;
    if(!eve5_parse_esdm_bmp(meta, got, &bmp)) return false;
    if(bmp.compression == EVE5_ESDM_IMAGE) return false;  /* use the .jpg/.png path */
    if(bmp.has_swizzle) {
        LV_LOG_WARN("EVE5 esdm: bitmap swizzle not applied for %s", path);
    }

    bool paletted = (bmp.palette_size > 0);
    char pal_path[256];
    if(paletted && !eve5_esdm_palette_path(path, bmp.ext_len, bmp.palette_ext, pal_path, sizeof(pal_path))) {
        LV_LOG_WARN("EVE5 esdm: cannot derive palette path for %s", path);
        return false;
    }

    /* Palette (if any) precedes the bitmap in one contiguous allocation,
     * matching the driver's PALETTEDARGB8 layout. */
    uint32_t pal_region = paletted ? (((uint32_t)bmp.palette_size + 31u) & ~31u) : 0;
    uint32_t data_bytes = (bmp.compression == EVE5_ESDM_ASSET && bmp.raw_size)
                          ? bmp.raw_size : (uint32_t)(bmp.stride * bmp.height);
    uint32_t alloc_size = pal_region + data_bytes;

#if LV_USE_OS
    lv_eve5_hal_lock(lv_eve5_disp_from_hal(u->hal));
#endif

    bool ok = false;
    uint32_t pal_ofs = GA_INVALID;
    EVE_GpuHandle handle = EVE_GpuAlloc_Alloc(u->allocator, alloc_size, GA_ALIGN_4 | GA_GC_FLAG);
    uint32_t base = EVE_GpuAlloc_Get(u->allocator, handle);
    if(base != GA_INVALID) {
        bool loaded = true;

        if(paletted) {
            loaded = eve5_esdm_host_read_raw(u, pal_path, base, bmp.palette_size);
            if(loaded) pal_ofs = 0;
            else LV_LOG_WARN("EVE5 esdm: palette read failed: %s", pal_path);
        }

        if(loaded) {
            uint32_t dst = base + pal_region;
            switch(bmp.compression) {
                case EVE5_ESDM_RAW:
                    loaded = eve5_esdm_host_read_raw(u, path, dst, data_bytes);
                    break;
                case EVE5_ESDM_DEFLATE:
                    loaded = eve5_esdm_host_stream(u, path, false, dst);
                    break;
                case EVE5_ESDM_ASSET:
                    loaded = eve5_esdm_host_stream(u, path, true, dst);
                    break;
                default:
                    loaded = false;
                    break;
            }
        }

        if(loaded) {
            EVE_Hal_requestFenceBeforeSwap(u->hal);
            ok = true;
        }
        else {
            EVE_GpuAlloc_Free(u->allocator, handle);
        }
    }

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif

    if(!ok) return false;

    *ram_g_addr = base + pal_region;
    *eve_format = (uint16_t)bmp.format;
    *eve_stride = bmp.stride;
    *src_w = bmp.width;
    *src_h = bmp.height;
    if(out_handle) *out_handle = handle;
    if(out_palette_addr) *out_palette_addr = (pal_ofs != GA_INVALID) ? (base + pal_ofs) : GA_INVALID;

    LV_LOG_INFO("EVE5 esdm: loaded %s (%dx%d fmt=%u stride=%d comp=%u%s)",
                path, (int)bmp.width, (int)bmp.height, (unsigned)bmp.format,
                (int)bmp.stride, (unsigned)bmp.compression, paletted ? " paletted" : "");
    return true;
}

bool lv_draw_eve5_try_load_esdm_image(lv_draw_eve5_unit_t * u, const void * src,
                                      uint32_t * ram_g_addr, uint16_t * eve_format,
                                      int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                      EVE_GpuHandle *out_handle, uint32_t * out_palette_addr)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) return false;
    const char * path = (const char *)src;

#if LV_USE_FS_EVE5_FLASH
    /* Flash resources are addressed, not named — no sidecar applies. */
    if(lv_eve5_flash_is_path(path)) return false;
#endif

#if LV_USE_FS_EVE5_SDCARD
    if(lv_eve5_sdcard_is_path(path)) {
        EVE_GpuHandle handle;
        uint32_t w = 0, h = 0, fmt = 0, img_ofs = 0, pal_ofs = GA_INVALID;
        int32_t stride = 0;
        if(!lv_eve5_sdcard_load_esdm(path, &handle, &w, &h, &fmt, &stride, &img_ofs, &pal_ofs))
            return false;
        uint32_t base = EVE_GpuAlloc_Get(u->allocator, handle);
        if(base == GA_INVALID) return false;
        *ram_g_addr = base + img_ofs;
        *eve_format = (uint16_t)fmt;
        *eve_stride = stride;
        *src_w = (int32_t)w;
        *src_h = (int32_t)h;
        if(out_handle) *out_handle = handle;
        if(out_palette_addr) *out_palette_addr = (pal_ofs != GA_INVALID) ? (base + pal_ofs) : GA_INVALID;
        return true;
    }
#endif

    return eve5_load_esdm_host(u, path, ram_g_addr, eve_format, eve_stride, src_w, src_h,
                               out_handle, out_palette_addr);
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
                                        EVE_GpuHandle *out_handle, uint32_t * out_palette_addr)
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

    EVE_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt, img_offset, pal_offset;

    bool ok = lv_eve5_sdcard_load_image(path, &handle, &img_w, &img_h, &img_fmt, &img_offset, &pal_offset);

    if(!ok) {
        return false;
    }

    uint32_t alloc_base = EVE_GpuAlloc_Get(u->allocator, handle);
    if(alloc_base == GA_INVALID) {
        LV_LOG_WARN("EVE5: SD card load succeeded but RAM_G address invalid");
        return false;
    }

    *ram_g_addr = alloc_base + img_offset;
    *eve_format = (uint16_t)img_fmt;
    *src_w = (int32_t)img_w;
    *src_h = (int32_t)img_h;

    int32_t bpp = eve5_format_bpp(img_fmt);
    /* HW decoder output stride = width * bpp (packed, no padding) */
    *eve_stride = (int32_t)img_w * bpp;
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
                                       EVE_GpuHandle *out_handle, uint32_t * out_palette_addr)
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

    EVE_GpuHandle handle;
    uint32_t img_w, img_h, img_fmt, img_offset, pal_offset;

    bool ok = lv_eve5_flash_load_image(path, &handle, &img_w, &img_h, &img_fmt, &img_offset, &pal_offset);

    if(!ok) {
        return false;
    }

    uint32_t alloc_base = EVE_GpuAlloc_Get(u->allocator, handle);
    if(alloc_base == GA_INVALID) {
        LV_LOG_WARN("EVE5: Flash load succeeded but RAM_G address invalid");
        return false;
    }

    *ram_g_addr = alloc_base + img_offset;
    *eve_format = (uint16_t)img_fmt;
    *src_w = (int32_t)img_w;
    *src_h = (int32_t)img_h;

    int32_t bpp = eve5_format_bpp(img_fmt);
    /* HW decoder output stride = width * bpp (packed, no padding) */
    *eve_stride = (int32_t)img_w * bpp;
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
lv_eve5_vram_res_t * lv_draw_eve5_resolve_to_gpu(lv_draw_eve5_unit_t * u, const void * src)
{
    lv_image_src_t src_type = lv_image_src_get_type(src);

    if(src_type == LV_IMAGE_SRC_FILE) {
        eve5_resolved_image_t resolved = {0};
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif
        bool ok = lv_draw_eve5_resolve_image_source(src, &resolved, &u->base_unit);
#if LV_USE_OS
        lv_eve5_hal_lock(lv_eve5_disp_from_hal(u->hal));
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
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        case ARGB8:
            return LV_COLOR_FORMAT_ARGB8888;
        case RGB8:
            return LV_COLOR_FORMAT_RGB888;
        case PALETTEDARGB8:
            return LV_COLOR_FORMAT_I8;
#endif
        case RGB565:
            return LV_COLOR_FORMAT_RGB565;
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

    /* Known-bad: a prior open_cb attempt at this path failed HW LOAD.
     * Decline so LVGL falls through to the SW decoder. Cheap O(32) compare
     * vs. the cost of re-running QUERY+LOAD and re-faulting the chip. */
    if(eve5_decoder_is_path_bad(fn)) return LV_RESULT_INVALID;

    const char * ext = lv_fs_get_ext(fn);

    bool is_jpeg = (lv_strcmp(ext, "jpg") == 0) || (lv_strcmp(ext, "jpeg") == 0);
    bool is_png = (lv_strcmp(ext, "png") == 0);
    if(!is_jpeg && !is_png) {
        /* Non-image extension: accept it only if a usable ".esdm" sidecar
         * describes a raw/deflate/asset bitmap. Image-compressed sidecars
         * (JPEG/PNG payloads) need a .jpg/.png extension to reach the
         * CMD_LOADIMAGE path, so they are declined here. */
        eve5_esdm_bmp_t bmp;
        if(!eve5_probe_esdm(fn, &bmp)) return LV_RESULT_INVALID;
        if(bmp.compression == EVE5_ESDM_IMAGE) return LV_RESULT_INVALID;
        header->cf = LV_COLOR_FORMAT_RAW;
        header->w = bmp.width;
        header->h = bmp.height;
        header->stride = bmp.stride;
        return LV_RESULT_OK;
    }

    uint32_t w = 0, h = 0;

#if LV_USE_FS_EVE5_SDCARD
    /* SD path: CMD_QUERYIMAGE is the only probe we want. lv_fs_read on an
     * SD path would force the lazy whole-file load (ensure_file_loaded
     * via CMD_FSREAD), which QUERY exists to avoid. Decline cleanly on
     * QUERY failure (missing file, malformed sig, firmware reject) so
     * LVGL falls through to the SW decoder — which has to lv_fs_read the
     * file anyway, but only when SW decode is actually required. */
    if(lv_eve5_sdcard_is_path(fn)) {
        if(lv_eve5_sdcard_query_image_dims(fn, &w, &h)) {
            header->cf = LV_COLOR_FORMAT_RAW;
            header->w = (int32_t)w;
            header->h = (int32_t)h;
            header->stride = (int32_t)(w * 3);
            return LV_RESULT_OK;
        }
        return LV_RESULT_INVALID;
    }
#endif

    /* Non-SD path: header peek is cheap. Parse dimensions, and for PNG
     * additionally screen out IHDR color_type / bit_depth combinations
     * BT820's CMD_LOADIMAGE rejects ("unsupported PNG in cmd_loadimage"),
     * so LVGL falls through to the SW decoder without ever entering a
     * fault/reset cycle. SD paths can't do this — CMD_QUERYIMAGE returns
     * only w/h, not color type — so they rely on the reactive bad-path
     * cache populated from open_cb failures. */
    uint8_t buf[1024];
    uint32_t bytes_read = 0;
    lv_fs_read(&dsc->file, buf, sizeof(buf), &bytes_read);
    if(bytes_read < 24) return LV_RESULT_INVALID;

    bool ok;
    if(is_jpeg) {
        ok = eve5_parse_jpeg_dimensions(buf, bytes_read, &w, &h);
    }
    else {
        ok = eve5_parse_png_dimensions(buf, bytes_read, &w, &h);
        /* IHDR at bytes 16-28: w(16-19), h(20-23), bit_depth(24), color_type(25).
         * BT820 firmware accepts bit_depth 8 only (1/2/4-bit PNGs — grayscale
         * AND indexed — and 16-bit-per-channel PNGs are rejected by
         * cmd_loadimage with "unsupported PNG"). At bit_depth 8 the supported
         * color types are 0 (L8 grayscale), 2 (RGB24), 3 (PALETTEDARGB8 from
         * 256-color palette), 6 (RGBA32). color_type 4 (gray+alpha) is
         * conservatively declined here. Anything declined falls through to
         * the SW decoder without entering a fault/reset cycle. */
        if(ok && bytes_read >= 26) {
            uint8_t bit_depth = buf[24];
            uint8_t color_type = buf[25];
            bool hw_supported = (bit_depth == 8) &&
                (color_type == 0 || color_type == 2
                 || color_type == 3 || color_type == 6);
            if(!hw_supported) {
                LV_LOG_INFO("EVE5 decoder: declining PNG color_type=%u bit_depth=%u for %s"
                            " (HW unsupported), SW decoder will handle",
                            (unsigned)color_type, (unsigned)bit_depth, fn);
                return LV_RESULT_INVALID;
            }
        }
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

    /* Snapshot fault state on entry: if the chip was already faulted
     * (something outside our load guards faulted between the frame-boundary
     * catch and now), every helper's pre-flush will fail in cascade and
     * loaded ends up false — but that's a chip-state failure, not a
     * file-content failure, and marking the path bad would be a false
     * negative. Only mark on failures observed against a clean chip. */
    bool pre_existing_fault = (u->hal != NULL && u->hal->CmdFault);

    bool is_jpeg, is_png;
    bool is_image_ext = eve5_is_jpeg_or_png(path, &is_jpeg, &is_png);

    uint32_t ram_g_addr = GA_INVALID, palette_addr = GA_INVALID;
    /* Initial format placeholder; the chosen loader fills in the actual EVE
     * format. Pre-BT820 has no ARGB8 — use RGB565 so the symbol exists. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    uint16_t eve_format = ARGB8;
#else
    uint16_t eve_format = RGB565;
#endif
    int32_t eve_stride = 0, src_w = 0, src_h = 0;
    EVE_GpuHandle handle = GA_HANDLE_INVALID;
    bool loaded = false;

    if(!is_image_ext) {
        /* Non-image extension: a ".esdm" sidecar drives a direct raw / deflate /
         * asset load in the bitmap's native EVE format (SD card and host both
         * handled, SD bypassing the LVGL filesystem driver). */
        loaded = lv_draw_eve5_try_load_esdm_image(u, dsc->src, &ram_g_addr, &eve_format,
                                                  &eve_stride, &src_w, &src_h, &handle, &palette_addr);
    }
    else {
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
    }

    if(!loaded) {
        /* Only persist the verdict if the chip was clean on entry —
         * otherwise the failure is attributable to the chip state, not
         * the file, and the next attempt should re-evaluate this path
         * against a recovered chip rather than route it to SW forever.
         * (Frame-boundary fault catch will have reset by then.) */
        if(!pre_existing_fault) {
            /* Mark the path so info_cb declines on next attempt (header
             * probe / QUERY won't run again for it), and drop LVGL's
             * stale header-cache entry that ties this path to our
             * decoder. Together this routes the next
             * lv_image_decoder_open to the SW decoder, breaking the
             * perpetual retry of our failed HW LOAD. The earlier narrow
             * reset (in the loader's fault guard) already recovered the
             * chip — this is just decoder-chain bookkeeping. */
            eve5_decoder_mark_path_bad(path);
            lv_image_header_cache_drop(path);
        }
        return LV_RESULT_INVALID;
    }

    /* Detect grayscale PALETTEDARGB8 and promote to L8.
     * BT820 produces PALETTEDARGB8 for grayscale PNGs. If the palette is a
     * simple grayscale ramp (R=G=B=i, A=255), reinterpret as L8 because the index
     * data is already the luminance value. Enables SW renderer's apply_mask.
     * PALETTEDARGB8 is BT820-only — pre-BT820 builds skip this entire block. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    if(eve_format == PALETTEDARGB8 && palette_addr != GA_INVALID)
#else
    if(0)
#endif
    {
#if LV_USE_OS
        lv_eve5_hal_lock(lv_eve5_disp_from_hal(u->hal));
#endif
        uint8_t pal_buf[256 * 4];
        EVE_Hal_rdMem(u->hal, pal_buf, palette_addr, sizeof(pal_buf));
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
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
    uint32_t alloc_base = EVE_GpuAlloc_Get(u->allocator, handle);
    uint32_t source_offset = (alloc_base != GA_INVALID && ram_g_addr >= alloc_base)
                             ? (ram_g_addr - alloc_base) : 0;
    uint32_t pal_offset = (palette_addr != GA_INVALID && alloc_base != GA_INVALID && palette_addr >= alloc_base)
                          ? (palette_addr - alloc_base) : GA_INVALID;
    uint32_t alloc_size = (uint32_t)(eve_stride * src_h);
    if(pal_offset != GA_INVALID) alloc_size += 256 * 4;

    lv_eve5_vram_res_t * vr = lv_malloc_zeroed(sizeof(lv_eve5_vram_res_t));
    if(vr == NULL) {
        EVE_GpuAlloc_Free(u->allocator, handle);
        return LV_RESULT_INVALID;
    }
    vr->base.unit = (lv_draw_unit_t *)u;
    vr->base.size = alloc_size;
    vr->gpu_handle = handle;
    vr->eve_format = eve_format;
    vr->stride = (uint32_t)eve_stride;
    vr->width = src_w;
    vr->height = src_h;
    vr->source_offset = source_offset;
    vr->palette_offset = pal_offset;
    vr->is_premultiplied = false;
    vr->has_content = true;
    /* is_swapchain stays zero — only the driver-owned full_buf vr sets it. */

    lv_color_format_t lv_cf = eve_format_to_lv_cf(eve_format);
    lv_draw_buf_t * decoded = lv_malloc_zeroed(sizeof(lv_draw_buf_t));
    if(decoded == NULL) {
        lv_free(vr);
        EVE_GpuAlloc_Free(u->allocator, handle);
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
