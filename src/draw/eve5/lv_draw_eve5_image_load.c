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
#include "EVE_ResourceProbe.h"
#include "EVE_ResourceQuery.h"
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

/* Defined in the decoder section below. Forward-declared here because the
 * SD loader (lv_draw_eve5_try_load_sdcard_image, this block) consumes the
 * info→open staging handoff installed by eve5_decoder_info. */
static bool eve5_pending_staging_take(const char * path, EVE_GpuHandle * out_handle,
                                      uint32_t * out_size, EVE_ResourceInfo * out_info);

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

    bool is_jpeg, is_png;
    if(!eve5_is_jpeg_or_png(path, &is_jpeg, &is_png)) {
        return false;
    }
    LV_UNUSED(is_jpeg);
    LV_UNUSED(is_png);

    EVE_HalContext * phost = u->hal;

    lv_fs_file_t file;
    if(lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) {
        LV_LOG_WARN("EVE5: Failed to open file: %s", path);
        return false;
    }

    /* Discover file size up front: we need it for the RAM_G staging buffer
     * and to bound the probe loop. */
    uint32_t file_size = 0;
    if(lv_fs_seek(&file, 0, LV_FS_SEEK_END) != LV_FS_RES_OK
       || lv_fs_tell(&file, &file_size) != LV_FS_RES_OK
       || file_size == 0
       || lv_fs_seek(&file, 0, LV_FS_SEEK_SET) != LV_FS_RES_OK) {
        LV_LOG_WARN("EVE5: file size discovery failed for %s", path);
        lv_fs_close(&file);
        return false;
    }

    /* Probe the header: exact decoded size, EVE bitmap format, palette size,
     * and HARDWARE_LOADABLE gate. The probe is incremental — feed chunks
     * until it returns done (1) or invalid (-1). Most JPEG/PNG headers
     * complete on the first chunk; indexed PNGs may scan to PLTE. The
     * HARDWARE_LOADABLE flag replaces the manual color_type / bit_depth
     * screening that used to gate PNGs the chip can't decode. */
    EVE_ResourceProbe probe;
    EVE_initProbe(&probe);
    EVE_ResourceInfo info;
    int probe_status = 0;
    {
        uint8_t probe_buf[1024];
        uint32_t bytes_left = file_size;
        while(bytes_left > 0) {
            uint32_t want = bytes_left > sizeof(probe_buf) ? sizeof(probe_buf) : bytes_left;
            uint32_t got = 0;
            if(lv_fs_read(&file, probe_buf, want, &got) != LV_FS_RES_OK || got == 0) break;
            probe_status = EVE_probeResource(&probe, EVE_CHIPID, probe_buf, got,
                                             OPT_TRUECOLOR, &info);
            bytes_left -= got;
            if(probe_status != 0) break;
        }
    }
    if(probe_status != 1
       || (info.Type != EVE_RESOURCE_JPEG && info.Type != EVE_RESOURCE_PNG)
       || !(info.Flags & EVE_RESOURCE_FLAG_HARDWARE_LOADABLE)) {
        LV_LOG_INFO("EVE5: %s declined for HW decode (probe=%d type=%u flags=0x%02x)",
                    path, probe_status, (unsigned)info.Type, (unsigned)info.Flags);
        lv_fs_close(&file);
        return false;
    }
    LV_LOG_INFO("EVE5 HW_DECODE: %s %ux%u fmt=%u size=%u palette=%u",
                path, (unsigned)info.Width, (unsigned)info.Height,
                (unsigned)info.Format, (unsigned)info.Size, (unsigned)info.PaletteSize);

#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    /* PALETTEDARGB8 callers must accept a separate palette address. */
    if(info.Format == PALETTEDARGB8 && out_palette_addr == NULL) {
        LV_LOG_INFO("EVE5 HW_DECODE: %s is PALETTEDARGB8 but caller didn't supply palette out", path);
        lv_fs_close(&file);
        return false;
    }
#endif

    /* Rewind for the full file read into the MediaFIFO staging buffer. */
    if(lv_fs_seek(&file, 0, LV_FS_SEEK_SET) != LV_FS_RES_OK) {
        lv_fs_close(&file);
        return false;
    }

#if LV_USE_OS
    lv_eve5_hal_lock(lv_eve5_disp_from_hal(u->hal));
#endif

    /* GC-flagged: decoded images self-heal through the decoder cache when
     * the handle goes invalid (sweep on pre-BT820, pressure eviction on
     * BT820+). CMD_LOADIMAGE writes in 32-byte units and the chip's sampling
     * unit assumes 32-byte source alignment, so both base and size must be
     * 32-aligned. A misaligned source produces a uniform N-pixel right shift
     * on byte-per-pixel formats like PALETTEDARGB8 where the chip rounds the
     * source address down. */
    uint32_t alloc_size = (info.Size + 31u) & ~31u;
    EVE_GpuHandle handle = EVE_GpuAlloc_Alloc(u->allocator, alloc_size, GA_ALIGN_32 | GA_GC_FLAG);
    uint32_t addr = EVE_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate %u bytes for decoded image", (unsigned)info.Size);
#if LV_USE_OS
        lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif
        lv_fs_close(&file);
        return false;
    }

    lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);

    /* Stage the compressed file in RAM_G and feed CMD_LOADIMAGE through
     * MediaFifo (OPT_MEDIAFIFO) so the decoder reads from an isolated
     * buffer rather than the cocmd FIFO. Trailing bytes past the decoder's
     * end-of-stream get dropped instead of being interpreted as commands —
     * defends against malformed or attacker-crafted PNG/JPEG. */
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

    /* Stream the file into temp_addr. SD card and flash paths are excluded
     * above, so lv_fs_read won't hit an EVE FS driver that would deadlock
     * on the non-recursive HAL mutex. */
    uint8_t chunk_buf[8192];
    uint32_t write_offset = 0;
    uint32_t remaining = file_size;
    bool success = true;
    while(remaining > 0) {
        uint32_t chunk_size = remaining > sizeof(chunk_buf) ? sizeof(chunk_buf) : remaining;
        uint32_t bytes_read = 0;
        if(lv_fs_read(&file, chunk_buf, chunk_size, &bytes_read) != LV_FS_RES_OK
           || bytes_read == 0) {
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

    uint32_t loadimage_opts = OPT_NODL | OPT_MEDIAFIFO;
    if(EVE_Hal_supportRenderTarget(phost)) loadimage_opts |= OPT_TRUECOLOR;

    EVE_CoCmd_loadImage(phost, addr, loadimage_opts);
    bool decoded = EVE_Cmd_waitFlush(phost);

    EVE_MediaFifo_close(phost);
    EVE_GpuAlloc_Free(u->allocator, temp_handle);

    if(!decoded) {
        LV_LOG_ERROR("EVE5: CMD_LOADIMAGE failed for %s", path);
        EVE_GpuAlloc_Free(u->allocator, handle);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(disp);
        phost->SuppressErrorOverlay = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return false;
    }
    phost->SuppressErrorOverlay = false;
    EVE_Hal_requestFenceBeforeSwap(phost);

#if LV_USE_OS
    lv_eve5_hal_unlock(lv_eve5_disp_from_hal(u->hal));
#endif

    /* Paletted layout is generation-dependent — derive from the probe's
     * EVE_RESOURCE_FLAG_PALETTE_AFTER_DATA bit rather than assuming
     * palette-front. EVE3/BT820 = palette-front (palette at base, image at
     * base+PaletteSize); EVE4 = palette-after (image at base, palette at
     * base+stride*h). Non-paletted formats (RGB8/ARGB8/RGB565/L8/...) have
     * PaletteSize == 0 and resolve to image-at-base / palette = GA_INVALID. */
    *eve_format = (uint16_t)info.Format;
    *eve_stride = (int32_t)info.Stride;
    *src_w = (int32_t)info.Width;
    *src_h = (int32_t)info.Height;
    if(out_handle) *out_handle = handle;
    if(info.PaletteSize == 0) {
        *ram_g_addr = addr;
        if(out_palette_addr) *out_palette_addr = GA_INVALID;
    }
    else if(info.Flags & EVE_RESOURCE_FLAG_PALETTE_AFTER_DATA) {
        *ram_g_addr = addr;
        if(out_palette_addr) *out_palette_addr = addr + (uint32_t)info.Stride * info.Height;
    }
    else {
        *ram_g_addr = addr + info.PaletteSize;
        if(out_palette_addr) *out_palette_addr = addr;
    }

    LV_LOG_INFO("EVE5: HW decoded %s (%ux%u fmt=%u stride=%d) via CMD_LOADIMAGE",
                path, (unsigned)info.Width, (unsigned)info.Height,
                (unsigned)info.Format, (int)info.Stride);
    return true;
}

/**********************
 * LVGL .bin DIRECT LOAD
 *
 * LVGL's `.bin` is just a 12-byte header (lv_image_header_t — see LVGL_BIN_FORMAT.md)
 * plus a body whose byte layout matches the in-memory layout LVGL would use for the
 * same lv_color_format_t. For every CF whose pixel encoding is the same as an EVE
 * bitmap format we can stream the body straight into RAM_G — no CPU decode, no
 * CMD_LOADIMAGE, no LVGL bin decoder.
 *
 * Bypassing LVGL's bin decoder also sidesteps the LV_BIN_DECODER_RAM_LOAD=0
 * holes (L8 / AL88 / ARGB1555 / ARGB4444 / ARGB2222 / ARGB8888_PREMULTIPLIED
 * have no get_area_cb path). The EVE5 driver doesn't need that path at all
 * because the bytes are already in the format the chip samples.
 **********************/

/* All LVGL CFs that lv_draw_eve5_upload_image_to_gpu can convert and upload.
 * The bin loader uses this both as the gate for claiming a file and to decide
 * between the SD zero-copy path (subset that's a direct memcpy from LVGL body
 * bytes to the EVE format) and the upload-machinery fallback (the rest). */
bool lv_draw_eve5_lvgl_bin_cf_supported(uint8_t lv_cf)
{
    switch((lv_color_format_t)lv_cf) {
        /* Direct-copy CFs (covered by SD zero-copy on stride match): */
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A1:
        case LV_COLOR_FORMAT_A2:
        case LV_COLOR_FORMAT_A4:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
        case LV_COLOR_FORMAT_AL88:           /* EVE LA8 (BT820+); declined at runtime if chipid < BT820 */
#endif
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
        case LV_COLOR_FORMAT_ARGB2222:
#endif
        /* Conversion-required CFs that lv_draw_eve5_get_eve_format_info /
         * lv_draw_eve5_upload_image_to_gpu already handle correctly: */
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_I8:
            return true;
        default:
            return false;
    }
}

/* True if the LVGL bin body bytes are already in the EVE bitmap format that
 * lv_draw_eve5_get_eve_format_info would map to, with no conversion required.
 * Stride match is checked separately (see the caller). @p phost is needed for
 * the multi-target EVE_CHIPID expansion.
 *
 * Generation thresholds per the chip "Bitmap formats" matrix:
 *   codes 0-11 (ARGB1555 / L1 / L4 / L8 / RGB332 / ARGB2 / ARGB4 / RGB565,
 *               plus PALETTED / TEXT8X8 / TEXTVGA / BARGRAPH): every chip
 *   codes 14-17 (PALETTED565 / 4444 / 8 / L2): FT810+
 *   code  31    (GLFORMAT): BT815+
 *   codes 19-28 (RGB8 / ARGB8 / PALETTEDARGB8 / RGB6 / ARGB6 / LA1-LA8 /
 *               YCBCR): BT820+
 */
static bool eve5_lvgl_bin_cf_direct_copy(EVE_HalContext * phost, lv_color_format_t lv_cf)
{
    LV_UNUSED(phost);
    switch(lv_cf) {
        /* Codes 0-11: available on every chip. */
        case LV_COLOR_FORMAT_L8:        /* L8 = 3 */
        case LV_COLOR_FORMAT_A1:        /* L1 = 1 */
        case LV_COLOR_FORMAT_A4:        /* L4 = 2 */
        case LV_COLOR_FORMAT_A8:        /* L8 = 3 */
        case LV_COLOR_FORMAT_RGB565:    /* RGB565 = 7 */
        case LV_COLOR_FORMAT_ARGB1555:  /* ARGB1555 = 0 */
        case LV_COLOR_FORMAT_ARGB4444:  /* ARGB4 = 6 */
        case LV_COLOR_FORMAT_ARGB2222:  /* ARGB2 = 5 */
            return true;
        /* A2 → L2 (code 17). FT810+; pre-FT810 the upload falls back to L4
         * with bit-replication, so direct-copy isn't valid there. */
        case LV_COLOR_FORMAT_A2:
            return EVE_CHIPID >= EVE_FT810;
#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
        /* RGB8 (19), ARGB8 (20), LA8 (27). BT820+ — pre-BT820 the upload
         * either converts to 16bpp (RGB8/ARGB8) or declines (LA8). */
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
        case LV_COLOR_FORMAT_AL88:
            return EVE_CHIPID >= EVE_BT820;
#endif
        default:
            return false;
    }
}

/* Map an LVGL direct-copy CF to its EVE bitmap format. Called only for CFs
 * eve5_lvgl_bin_cf_direct_copy returned true on. */
static uint16_t eve5_lvgl_bin_cf_to_eve_format(lv_color_format_t lv_cf, bool * is_premul)
{
    switch(lv_cf) {
        case LV_COLOR_FORMAT_L8:        return L8;
        case LV_COLOR_FORMAT_A1:        return L1;
#if (EVE_SUPPORT_CHIPID >= EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)
        case LV_COLOR_FORMAT_A2:        return L2;
#endif
        case LV_COLOR_FORMAT_A4:        return L4;
        case LV_COLOR_FORMAT_A8:        return L8;
        case LV_COLOR_FORMAT_RGB565:    return RGB565;
        case LV_COLOR_FORMAT_ARGB1555:  return ARGB1555;
        case LV_COLOR_FORMAT_ARGB4444:  return ARGB4;
#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
        case LV_COLOR_FORMAT_AL88:      return LA8;
#endif
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        case LV_COLOR_FORMAT_ARGB2222:  return ARGB2;
        case LV_COLOR_FORMAT_RGB888:    return RGB8;
        case LV_COLOR_FORMAT_ARGB8888:  return ARGB8;
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            *is_premul = true;
            return ARGB8;
#endif
        default:                        return 0;  /* unreachable */
    }
}

/* EVE bitmap stride for a CF and width — same formula as the upload uses
 * (lv_draw_eve5_get_eve_format_info), 4-byte-aligned. The SD zero-copy path
 * needs LVGL's on-disk stride to match this exactly. */
static uint32_t eve5_lvgl_bin_eve_stride(lv_color_format_t lv_cf, uint32_t w)
{
    uint32_t bpp = lv_color_format_get_bpp(lv_cf);
    return ((w * bpp + 7u) / 8u + 3u) & ~3u;
}

bool lv_draw_eve5_try_load_lvgl_bin_image(lv_draw_eve5_unit_t * u, const void * src,
                                          uint32_t * ram_g_addr, uint16_t * eve_format,
                                          int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                          EVE_GpuHandle * out_handle, uint32_t * out_palette_addr,
                                          lv_color_format_t * out_lv_cf,
                                          bool * out_is_premultiplied)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_FILE) return false;
    const char * path = (const char *)src;
    if(!eve5_has_extension(path, ".bin")) return false;

    /* Header read before any chip ops. For an SD path lv_fs_read implicitly
     * triggers ensure_file_loaded → the whole file lands in RAM_G; for a host
     * path it just pulls bytes from disk. Either way we get the 12-byte
     * lv_image_header_t into a host buffer. */
    lv_fs_file_t file;
    if(lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) {
        LV_LOG_WARN("EVE5 bin: open failed: %s", path);
        return false;
    }

    lv_image_header_t header;
    uint32_t rn = 0;
    if(lv_fs_read(&file, &header, sizeof(header), &rn) != LV_FS_RES_OK
       || rn != sizeof(header)
       || header.magic != LV_IMAGE_HEADER_MAGIC
       || (header.flags & LV_IMAGE_FLAGS_COMPRESSED)
       || !lv_draw_eve5_lvgl_bin_cf_supported((uint8_t)header.cf)) {
        lv_fs_close(&file);
        return false;
    }

    lv_color_format_t lv_cf = (lv_color_format_t)header.cf;
    uint32_t w = header.w;
    uint32_t h = header.h;
    uint32_t lvgl_stride = header.stride;
    bool is_premul = (header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED) != 0
                     || lv_cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;

    if(w == 0 || h == 0 || lvgl_stride == 0) {
        lv_fs_close(&file);
        return false;
    }

    EVE_HalContext * phost = u->hal;
    lv_display_t * disp = lv_eve5_disp_from_hal(phost);

    /* SD ZERO-COPY PATH
     * The SD FS driver's ensure_file_loaded has CMD_FSREAD'd the whole bin into
     * RAM_G to satisfy our header read above. If the bin's CF is one whose
     * body bytes are already in the target EVE format (direct copy) and the
     * LVGL stride matches what get_eve_format_info would compute, we can
     * steal that RAM_G allocation and point the bitmap source at
     * `base + 12 + palette_bytes` — no host bounce, no second SD read, no
     * format conversion. The 12-byte header sits unused at the start of the
     * allocation (negligible waste). */
#if LV_USE_FS_EVE5_SDCARD
    if(lv_eve5_sdcard_is_path(path)
       && eve5_lvgl_bin_cf_direct_copy(phost, lv_cf)
       && lvgl_stride == eve5_lvgl_bin_eve_stride(lv_cf, w)) {

        EVE_GpuAlloc * steal_alloc = NULL;
        EVE_GpuHandle steal_handle = GA_HANDLE_INVALID;
        uint32_t steal_size = 0;
        if(lv_eve5_sdcard_steal_ramg(file.file_d, &steal_alloc, &steal_handle, &steal_size)) {
            lv_fs_close(&file);

            bool premul_out = is_premul;
            uint16_t fmt = eve5_lvgl_bin_cf_to_eve_format(lv_cf, &premul_out);

#if LV_USE_OS
            lv_eve5_hal_lock(disp);
#endif
            uint32_t base = EVE_GpuAlloc_Get(steal_alloc, steal_handle);
#if LV_USE_OS
            lv_eve5_hal_unlock(disp);
#endif
            if(base == GA_INVALID) {
                EVE_GpuAlloc_Free(steal_alloc, steal_handle);
                return false;
            }

            uint32_t palette_bytes = 0;  /* direct-copy list excludes indexed */
            *ram_g_addr = base + sizeof(lv_image_header_t) + palette_bytes;
            *eve_format = fmt;
            *eve_stride = (int32_t)lvgl_stride;
            *src_w = (int32_t)w;
            *src_h = (int32_t)h;
            if(out_handle) *out_handle = steal_handle;
            if(out_palette_addr) {
                *out_palette_addr = palette_bytes > 0 ? base + sizeof(lv_image_header_t) : GA_INVALID;
            }
            if(out_lv_cf) *out_lv_cf = lv_cf;
            if(out_is_premultiplied) *out_is_premultiplied = premul_out;

            LV_LOG_INFO("EVE5 bin: SD zero-copy %s (%ux%u lvcf=0x%02x → eve_fmt=%u stride=%u%s)",
                        path, (unsigned)w, (unsigned)h, (unsigned)lv_cf,
                        (unsigned)fmt, (unsigned)lvgl_stride, premul_out ? " premul" : "");
            return true;
        }
        /* Steal failed (file already stolen or freed) — drop through to the
         * host-buffer + upload path below, which will re-read the file. */
    }
#endif

    /* GENERAL PATH — host buffer + upload machinery
     * Route through lv_draw_eve5_upload_image_to_gpu so every CF the upload
     * mapping handles (RGB565_SWAPPED byte swap, XRGB8888 X→A, RGB565A8 plane
     * split, I1/I2/I4/I8 palette expansion, etc.) works without duplicating
     * that conversion code here. */
    uint32_t file_size = 0;
    if(lv_fs_seek(&file, 0, LV_FS_SEEK_END) != LV_FS_RES_OK
       || lv_fs_tell(&file, &file_size) != LV_FS_RES_OK
       || file_size <= sizeof(header)
       || lv_fs_seek(&file, sizeof(header), LV_FS_SEEK_SET) != LV_FS_RES_OK) {
        lv_fs_close(&file);
        return false;
    }
    uint32_t body_size = file_size - sizeof(header);

    uint8_t * body = lv_malloc(body_size);
    if(body == NULL) {
        LV_LOG_WARN("EVE5 bin: host buffer alloc failed (%u bytes) for %s",
                    (unsigned)body_size, path);
        lv_fs_close(&file);
        return false;
    }

    if(lv_fs_read(&file, body, body_size, &rn) != LV_FS_RES_OK || rn != body_size) {
        LV_LOG_WARN("EVE5 bin: body read failed for %s", path);
        lv_free(body);
        lv_fs_close(&file);
        return false;
    }
    lv_fs_close(&file);

    /* Synthetic image dsc: upload only reads from data + header, never
     * touches reserved / vram_res fields outside the attach step. */
    lv_image_dsc_t synth;
    lv_memzero(&synth, sizeof(synth));
    synth.header = header;
    synth.data = body;
    synth.data_size = body_size;

#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif
    lv_eve5_vram_res_t * up_vr = lv_draw_eve5_upload_image_to_gpu(u, &synth);
#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif

    lv_free(body);

    if(up_vr == NULL) {
        LV_LOG_WARN("EVE5 bin: upload failed for %s (lvcf=0x%02x)",
                    path, (unsigned)lv_cf);
        return false;
    }

    /* Transfer the gpu_handle to the caller and free the upload's vram_res
     * shell — decoder_open builds its own vram_res with the same handle. */
    EVE_GpuHandle handle = up_vr->gpu_handle;
    uint16_t eve_fmt = up_vr->eve_format;
    uint32_t up_stride = up_vr->stride;
    int32_t up_w = up_vr->width;
    int32_t up_h = up_vr->height;
    uint32_t up_src_off = up_vr->source_offset;
    uint32_t up_pal_off = up_vr->palette_offset;
    lv_free(up_vr);

    uint32_t base = EVE_GpuAlloc_Get(u->allocator, handle);
    if(base == GA_INVALID) {
        EVE_GpuAlloc_Free(u->allocator, handle);
        return false;
    }

    *ram_g_addr = base + up_src_off;
    *eve_format = eve_fmt;
    *eve_stride = (int32_t)up_stride;
    *src_w = up_w;
    *src_h = up_h;
    if(out_handle) *out_handle = handle;
    if(out_palette_addr) *out_palette_addr = (up_pal_off != GA_INVALID) ? base + up_pal_off : GA_INVALID;
    if(out_lv_cf) *out_lv_cf = lv_cf;
    if(out_is_premultiplied) *out_is_premultiplied = is_premul;

    LV_LOG_INFO("EVE5 bin: upload %s (%ux%u lvcf=0x%02x → eve_fmt=%u stride=%u%s)",
                path, (unsigned)up_w, (unsigned)up_h, (unsigned)lv_cf,
                (unsigned)eve_fmt, (unsigned)up_stride, is_premul ? " premul" : "");
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
    bool ok;

    /* Fast path: info_cb already CMD_FSREAD-staged the compressed file and
     * resolved EVE_ResourceInfo via EVE_queryResource_fs (SW fallback when
     * the patch isn't loaded). Take ownership of the staged buffer and run
     * CMD_LOADIMAGE OPT_MEDIAFIFO against it — no second SD read. The
     * staged variant always frees the staging handle, win or lose. */
    EVE_GpuHandle staged = GA_HANDLE_INVALID;
    uint32_t staged_size = 0;
    EVE_ResourceInfo staged_info;
    if(eve5_pending_staging_take(path, &staged, &staged_size, &staged_info)) {
        ok = lv_eve5_sdcard_load_image_staged(path, staged, staged_size, &staged_info,
                                              &handle, &img_w, &img_h, &img_fmt,
                                              &img_offset, &pal_offset);
    }
    else {
        /* No staged input — either info_cb wasn't called (e.g., loaders
         * invoked outside the LVGL decoder framework) or the patched CMD
         * path was used (no staging produced). Re-query + load. */
        ok = lv_eve5_sdcard_load_image(path, &handle, &img_w, &img_h, &img_fmt,
                                       &img_offset, &pal_offset);
    }

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

/**********************
 * INFO→OPEN STAGING HANDOFF
 **********************/

/* Single-slot handoff for the EVE-SD info→open transition. When the
 * patch_queryassets firmware patch isn't loaded (or isn't compiled in),
 * info_cb's EVE_queryResource_fs SW fallback CMD_FSREAD-stages the whole
 * compressed file into RAM_G to drive the host probe. Without this slot,
 * open_cb's lv_eve5_sdcard_load_image would query (and stage) AGAIN — a
 * second SD round-trip for the same bytes. The slot lets open_cb take
 * ownership of the staged buffer and run CMD_LOADIMAGE OPT_MEDIAFIFO
 * against it directly via lv_eve5_sdcard_load_image_staged.
 *
 * LVGL calls open_cb immediately after the info_cb that produced
 * LV_RESULT_OK on the same dsc, with no interleaved info_cbs from other
 * decoders, so a single slot suffices. eve5_pending_staging_set is the
 * only write path; it frees the prior staging before installing a new
 * one. eve5_pending_staging_take wipes the slot on hit so a later open
 * for a different path can't accidentally consume a stale handle. */
typedef struct {
    char path[EVE5_DECODER_BAD_PATH_MAX];
    EVE_GpuHandle handle;
    uint32_t size;
    EVE_ResourceInfo info;
} eve5_pending_staging_t;

static eve5_pending_staging_t s_pending_staging = { {0}, GA_HANDLE_INIT, 0, {0} };

static void eve5_pending_staging_clear(void)
{
    if(s_pending_staging.handle.Id != GA_HANDLE_INVALID.Id && s_decoder_unit != NULL) {
        EVE_GpuAlloc_Free(s_decoder_unit->allocator, s_pending_staging.handle);
    }
    s_pending_staging.path[0] = '\0';
    s_pending_staging.handle = GA_HANDLE_INVALID;
    s_pending_staging.size = 0;
}

static void eve5_pending_staging_set(const char * path, EVE_GpuHandle handle,
                                     uint32_t size, const EVE_ResourceInfo * info)
{
    /* Always free a prior staging before installing a new one — never leak. */
    eve5_pending_staging_clear();
    if(path == NULL || handle.Id == GA_HANDLE_INVALID.Id) return;
    lv_strncpy(s_pending_staging.path, path, EVE5_DECODER_BAD_PATH_MAX);
    s_pending_staging.path[EVE5_DECODER_BAD_PATH_MAX - 1] = '\0';
    s_pending_staging.handle = handle;
    s_pending_staging.size = size;
    s_pending_staging.info = *info;
}

/* On hit, wipes the slot WITHOUT freeing the handle — ownership transfers
 * to the caller, which is responsible for freeing (either consuming via
 * lv_eve5_sdcard_load_image_staged, which frees on both success and
 * failure paths, or freeing directly). The slot must be wiped because the
 * caller now owns the only reference; leaving it indexed would cause a
 * double-free on the next eve5_pending_staging_clear. */
static bool eve5_pending_staging_take(const char * path, EVE_GpuHandle * out_handle,
                                      uint32_t * out_size, EVE_ResourceInfo * out_info)
{
    if(path == NULL || s_pending_staging.handle.Id == GA_HANDLE_INVALID.Id) return false;
    if(lv_strcmp(s_pending_staging.path, path) != 0) return false;
    *out_handle = s_pending_staging.handle;
    *out_size = s_pending_staging.size;
    *out_info = s_pending_staging.info;
    s_pending_staging.path[0] = '\0';
    s_pending_staging.handle = GA_HANDLE_INVALID;
    s_pending_staging.size = 0;
    return true;
}

/* Hook EVE_queryResource_*'s fault recovery into lv_eve5_reset_coprocessor so
 * the SD/flash query paths get the same narrow reset (with bitmap-handle
 * pool / rom font cache invalidation) that the regular loader paths use. */
static void decoder_query_reset_cb(EVE_HalContext * phost, void * userdata)
{
    (void)phost;
    lv_eve5_reset_coprocessor((lv_display_t *)userdata);
}

/* Map an EVE bitmap format back to its LVGL color format equivalent for the
 * decoded buffer's header.cf.
 *
 * @p prefer_luminance_for_l8 disambiguates the L8 ↔ {LVGL L8, LVGL A8} pair:
 *   - true  → L8 ⇒ LV_COLOR_FORMAT_L8 (luminance-as-RGB intent). Used for the
 *             HW JPEG/PNG path because PNG ct=0 / JPEG OPT_MONO / grayscale-
 *             palette promotion all carry a grayscale-image intent. The
 *             draw-time BITMAP_SWIZZLE(ALPHA,ALPHA,ALPHA,ONE) restores
 *             luminance-RGB sampling on top of EVE's alpha-with-white L8.
 *   - false → L8 ⇒ LV_COLOR_FORMAT_A8 (alpha-only intent). Used for the
 *             .esdm raw-asset path: native EVE bitmaps in L# format are
 *             alpha-only by EVE's sampling rules (R=255, G=255, B=255,
 *             A=value), and the converter that produced them already treated
 *             them as alpha masks.
 *
 * Sub-byte L1/L2/L4 always map to A1/A2/A4 — LVGL has no sub-byte luminance
 * format, and the typical sub-byte source on EVE is an alpha mask anyway. */
static lv_color_format_t eve_format_to_lv_cf(uint16_t eve_fmt, bool prefer_luminance_for_l8)
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
            return prefer_luminance_for_l8 ? LV_COLOR_FORMAT_L8 : LV_COLOR_FORMAT_A8;
        case L4:
            return LV_COLOR_FORMAT_A4;
#if (EVE_SUPPORT_CHIPID >= EVE_FT810) || defined(EVE_MULTI_GRAPHICS_TARGET)
        case L2:
            return LV_COLOR_FORMAT_A2;
#endif
        case L1:
            return LV_COLOR_FORMAT_A1;
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
    if(s_decoder_unit == NULL) return LV_RESULT_INVALID;

    const char * fn = dsc->src;

    /* Known-bad: a prior open_cb attempt at this path failed HW LOAD.
     * Decline so LVGL falls through to the SW decoder. Cheap O(32) compare
     * vs. the cost of re-running QUERY+LOAD and re-faulting the chip. */
    if(eve5_decoder_is_path_bad(fn)) return LV_RESULT_INVALID;

    const char * ext = lv_fs_get_ext(fn);
    bool is_jpeg = (lv_strcmp(ext, "jpg") == 0) || (lv_strcmp(ext, "jpeg") == 0);
    bool is_png = (lv_strcmp(ext, "png") == 0);
    bool is_bin = (lv_strcmp(ext, "bin") == 0);
    if(is_bin) {
        /* LVGL .bin: read the 12-byte header and claim it when we have a path
         * (SD zero-copy / upload-machinery fallback) that can serve it. SD
         * pre-loads the whole file as a side effect of the header read here —
         * decoder_open does the same to set up steal_ramg or refill a host
         * buffer (two SD reads for the same file in the worst case; the second
         * one is the price for a stateless per-decoder dispatch). */
        lv_fs_file_t bin_file;
        if(lv_fs_open(&bin_file, fn, LV_FS_MODE_RD) != LV_FS_RES_OK) {
            return LV_RESULT_INVALID;
        }
        lv_image_header_t bin_hdr;
        uint32_t rn = 0;
        bool ok = lv_fs_read(&bin_file, &bin_hdr, sizeof(bin_hdr), &rn) == LV_FS_RES_OK
                  && rn == sizeof(bin_hdr)
                  && bin_hdr.magic == LV_IMAGE_HEADER_MAGIC;
        lv_fs_close(&bin_file);
        if(!ok) return LV_RESULT_INVALID;
        if(bin_hdr.flags & LV_IMAGE_FLAGS_COMPRESSED) return LV_RESULT_INVALID;
        if(!lv_draw_eve5_lvgl_bin_cf_supported((uint8_t)bin_hdr.cf)) return LV_RESULT_INVALID;
        header->cf = bin_hdr.cf;
        header->w = bin_hdr.w;
        header->h = bin_hdr.h;
        header->stride = bin_hdr.stride;
        header->flags = bin_hdr.flags;
        return LV_RESULT_OK;
    }
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
    LV_UNUSED(is_jpeg);
    LV_UNUSED(is_png);

    lv_draw_eve5_unit_t * u = s_decoder_unit;
    EVE_HalContext * phost = u->hal;
    EVE_ResourceInfo info;

#if LV_USE_FS_EVE5_SDCARD
    /* SD path: drive EVE_queryResource_fs(preferCmd=true, ga=u->allocator).
     * When the patch_queryassets firmware patch is present, the CMD path
     * (CMD_QUERYIMAGE_fs) runs and no staging is produced — outStaging stays
     * GA_HANDLE_INVALID and info_cb completes with no RAM_G touched. When
     * the patch is absent, the SW fallback CMD_FSREAD-stages the whole
     * compressed file into RAM_G to drive the host probe; the staging
     * handle is handed back via outStagingHandle/outStagingSize and stashed
     * for open_cb to consume via lv_eve5_sdcard_load_image_staged — no
     * second SD round-trip. */
    if(lv_eve5_sdcard_is_path(fn)) {
        if(!lv_eve5_sdcard_ready()) return LV_RESULT_INVALID;

        const char * sd_path = fn;
        if(fn[1] == ':' && (fn[2] == '/' || fn[2] == '\\')) sd_path = fn + 2;

        lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);
        EVE_GpuHandle staging = GA_HANDLE_INVALID;
        uint32_t staging_size = 0;
#if LV_USE_OS
        lv_eve5_hal_lock(disp);
#endif
        bool ok = EVE_queryResource_fs(u->hal, u->allocator, sd_path, OPT_TRUECOLOR,
                                       /*preferCmd*/ true,
                                       decoder_query_reset_cb, disp,
                                       &staging, &staging_size, &info);
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        if(!ok) {
            /* Defensive: SW fallback returns false without populating
             * staging out-params, but if a future change ever paths through
             * here with a live staging, free it rather than leak. */
            if(staging.Id != GA_HANDLE_INVALID.Id) {
                EVE_GpuAlloc_Free(u->allocator, staging);
            }
            return LV_RESULT_INVALID;
        }
        /* Hand staging (if any) to open_cb. Set replaces any prior slot
         * contents, freeing the previous handle — no leak on repeat info_cb. */
        eve5_pending_staging_set(fn, staging, staging_size, &info);
    }
    else
#endif
    {
        /* Non-SD path: drive EVE_probeResource over chunked lv_fs reads.
         * The HARDWARE_LOADABLE flag from the probe replaces the manual
         * PNG color_type / bit_depth screening — same gate, same chipId-
         * derived decision, derived in one place. */
        EVE_ResourceProbe probe;
        EVE_initProbe(&probe);
        int probe_status = 0;
        uint8_t buf[1024];
        for(;;) {
            uint32_t got = 0;
            if(lv_fs_read(&dsc->file, buf, sizeof(buf), &got) != LV_FS_RES_OK || got == 0) break;
            probe_status = EVE_probeResource(&probe, EVE_CHIPID, buf, got,
                                             OPT_TRUECOLOR, &info);
            if(probe_status != 0) break;
        }
        if(probe_status != 1) return LV_RESULT_INVALID;
    }

    if(info.Type != EVE_RESOURCE_JPEG && info.Type != EVE_RESOURCE_PNG) return LV_RESULT_INVALID;
    if(!(info.Flags & EVE_RESOURCE_FLAG_HARDWARE_LOADABLE)) {
        LV_LOG_INFO("EVE5 decoder: declining %s (probe says not HW-decodable on this chip)", fn);
        return LV_RESULT_INVALID;
    }

    header->cf = LV_COLOR_FORMAT_RAW;
    header->w = (int32_t)info.Width;
    header->h = (int32_t)info.Height;
    header->stride = (int32_t)info.Stride;
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
    bool is_bin_ext = eve5_has_extension(path, ".bin");

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
    /* Overrides populated only by the .bin loader. UNKNOWN means: fall back to
     * eve_format_to_lv_cf inference (the EVE-native label) for the cache CF
     * and leave is_premultiplied / sample_as_luminance at the chip-default
     * (false). */
    lv_color_format_t bin_lv_cf = LV_COLOR_FORMAT_UNKNOWN;
    bool bin_is_premultiplied = false;

    if(is_bin_ext) {
        /* LVGL .bin → direct RAM_G stream in the declared EVE-native format,
         * bypassing LVGL's bin decoder entirely. Lets us validate EVE5 against
         * the SW reference renderer on the same files (toggle LV_USE_DRAW_EVE5
         * in the demo's main.c to unregister the EVE5 decoder + draw unit). */
        loaded = lv_draw_eve5_try_load_lvgl_bin_image(u, dsc->src, &ram_g_addr, &eve_format,
                                                     &eve_stride, &src_w, &src_h, &handle, &palette_addr,
                                                     &bin_lv_cf, &bin_is_premultiplied);
    }
    else if(!is_image_ext) {
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
    vr->is_premultiplied = bin_is_premultiplied;
    vr->has_content = true;
    /* is_swapchain stays zero — only the driver-owned full_buf vr sets it. */

    /* Resolve the LVGL color format the decoded buffer carries:
     *   - .bin source: respect the bin header's CF verbatim (LVGL L8 / A8 /
     *     ARGB8888_PREMULTIPLIED etc. — the user's authored intent).
     *   - HW JPEG / PNG: L8 means a grayscale image (PNG ct=0, JPEG OPT_MONO,
     *     grayscale-palette promotion). Map L8 → LVGL L8 so the draw-time
     *     swizzle renders it as luminance-RGB.
     *   - ESDM raw asset: L# is a native EVE bitmap authored as an alpha
     *     mask. Map L8 → LVGL A8 so it renders as alpha by EVE's default
     *     sampling, with no swizzle.
     *
     * is_image_ext (set up at the top of decoder_open) discriminates the
     * JPEG/PNG path from the ESDM path. Drive sample_as_luminance off the
     * resolved LVGL CF so the bin and HW-luminance paths share the swizzle. */
    lv_color_format_t lv_cf = (bin_lv_cf != LV_COLOR_FORMAT_UNKNOWN)
                              ? bin_lv_cf
                              : eve_format_to_lv_cf(eve_format, /*prefer_luminance_for_l8*/ is_image_ext);
    vr->sample_as_luminance = (lv_cf == LV_COLOR_FORMAT_L8);
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
