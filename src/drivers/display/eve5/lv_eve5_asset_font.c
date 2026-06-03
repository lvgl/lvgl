/**
 * @file lv_eve5_asset_font.c
 *
 * Loads BT82X .reloc font assets via CMD_LOADASSET and wraps them as
 * lv_font_t instances. See lv_eve5_asset_font.h for API.
 *
 * Load flow:
 *   1. Determine AssetSize. Source-specific:
 *      - SRC_FILE: lv_fs read of the 12-byte EVE_Gpu_AssetHeader.
 *      - SRC_FLASH: CMD_FLASHREAD of the header into a tiny RAM_G slot.
 *      - SRC_SDCARD (EVE_COCMD_PATCH_QUERY=1): CMD_FSSOURCE +
 *        CMD_QUERYASSET + CMD_GETPROPS — no RAM_G round-trip.
 *      - SRC_SDCARD (legacy): CMD_FSREAD the whole file into a temp
 *        RAM_G buffer, read back the header. Warns at load time.
 *   2. Allocate AssetSize bytes in RAM_G via EVE_GpuAlloc (4-byte aligned).
 *   3. Issue CMD_LOADASSET with the destination address; source data is
 *      either streamed into the command FIFO from a pre-read host buffer
 *      (lv_fs path), pointed at via CMD_FLASHSOURCE + OPT_FLASH, sourced
 *      via CMD_FSSOURCE + OPT_FS (SD fast path), or fed via MediaFIFO
 *      (SD legacy path).
 *   4. Wait for coprocessor completion. The asset loader inflates the
 *      data and patches embedded pointers to be relative to the load
 *      address, leaving a directly-usable font block in RAM_G.
 *   5. Parse the loaded metric block via lv_eve5_font_block_load to build
 *      the per-glyph width table used by LVGL's text layout.
 *
 * Locking: lv_fs file I/O runs OUTSIDE the HAL mutex because LVGL FS
 * drivers built on top of EVE (lv_fs_eve5_sdcard, lv_fs_eve5_flash) take
 * the mutex internally and lv_eve5_hal_lock is not recursive on every OS.
 * The HAL mutex is taken only around the CMD_LOADASSET issue, the read-
 * back of the loaded font block, and the asset-size probe for SRC_FLASH.
 *
 * The RAM_G allocation is held until lv_eve5_asset_font_destroy is
 * called — the font block must stay resident for as long as the lv_font_t
 * is referenced by any LVGL widget.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_eve5_asset_font.h"

#if LV_USE_EVE5 && (EVE_SUPPORT_CHIPID >= EVE_BT820)

#include "lv_eve5.h"
#include "lv_eve5_font_block.h"

#include "EVE_Hal.h"
#include "EVE_CoCmd.h"
#include "EVE_CoCmd_Ext.h"
#include "EVE_GpuTypes.h"
#include "EVE_MediaFifo.h"
#include "EVE_GpuAlloc.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_font_dsc_base_t base;
    EVE_HalContext * hal;
    EVE_GpuAlloc * allocator;
    EVE_GpuHandle gpu_handle;
    uint32_t first_character;
    /* Draw-time state: cached_handle persists the bitmap handle so the
     * same identity is used across frames, and last_bound_addr tracks
     * what address we last bound via CMD_SETFONT2 — both must match the
     * current allocator state before the bind can be reused. The address
     * is *always* re-queried via EVE_GpuAlloc_Get; it must never be
     * cached for use, only for change detection. */
    uint8_t cached_handle;       /**< 0xFF = none */
    uint32_t last_bound_addr;    /**< GA_INVALID = no prior bind */
    lv_eve5_font_block_t block;
} lv_eve5_asset_font_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint8_t * read_file_into_buffer(const char * path, uint32_t * out_size);

static bool issue_loadasset_from_buffer(EVE_HalContext * phost,
                                        const uint8_t * data, uint32_t size,
                                        uint32_t dst_addr);
static bool issue_loadasset_from_flash(EVE_HalContext * phost, uint32_t flash_addr,
                                       uint32_t dst_addr);
static bool probe_flash_asset_size(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   uint32_t flash_addr, uint32_t * out_size);

static bool load_asset_from_sdcard(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   const char * path, EVE_GpuHandle * out_handle,
                                   uint32_t * out_asset_size);
static const char * strip_lvgl_drive_letter(const char * path);

static bool font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                               uint32_t letter, uint32_t letter_next);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_IMAGE_DSC_CONST void * lv_eve5_asset_font_glyph_bitmap_sentinel(lv_font_glyph_dsc_t * g_dsc,
                                                                  lv_draw_buf_t * draw_buf)
{
    LV_UNUSED(g_dsc);
    LV_UNUSED(draw_buf);
    return NULL;
}

lv_font_t * lv_eve5_asset_font_create(lv_display_t * disp,
                                      const lv_eve5_asset_font_info_t * info)
{
    if(disp == NULL || info == NULL) return NULL;

    EVE_HalContext * phost = lv_eve5_get_hal(disp);
    EVE_GpuAlloc * allocator = lv_eve5_get_allocator(disp);
    if(phost == NULL || allocator == NULL) return NULL;

    if(EVE_CHIPID < EVE_BT820) {
        LV_LOG_WARN("EVE5 asset font: CMD_LOADASSET is BT820-only (chipid=%u)", (unsigned)EVE_CHIPID);
        return NULL;
    }

    uint32_t asset_size = 0;
    uint8_t * file_buffer = NULL;   /* SRC_FILE: pre-read outside lock */
    uint32_t file_size = 0;
    bool ok = false;
    EVE_GpuHandle handle = GA_HANDLE_INVALID;
    uint32_t dst_addr = GA_INVALID;
    lv_font_t * font = NULL;

    /* --- Phase 1: source-specific size probe --- */
    switch(info->source) {
        case LV_EVE5_ASSET_FONT_SRC_FILE:
            /* Host file via lv_fs. Pre-read into a buffer OUTSIDE the HAL
             * mutex so an lv_fs backend that takes the mutex internally
             * (lv_fs_eve5_sdcard / lv_fs_eve5_flash) doesn't recurse. The
             * caller is expected to pass a host path; for EVE storage,
             * use SRC_SDCARD or SRC_FLASH for the zero-copy bypass. */
            if(info->path == NULL) {
                LV_LOG_WARN("EVE5 asset font: path required");
                return NULL;
            }
            file_buffer = read_file_into_buffer(info->path, &file_size);
            if(file_buffer == NULL || file_size < sizeof(EVE_Gpu_AssetHeader)) {
                LV_LOG_WARN("EVE5 asset font: file read failed: %s", info->path);
                lv_free(file_buffer);
                return NULL;
            }
            {
                EVE_Gpu_AssetHeader hdr;
                lv_memcpy(&hdr, file_buffer, sizeof(hdr));
                if(hdr.Signature != EVE_GPU_ASSET_SIGNATURE) {
                    LV_LOG_WARN("EVE5 asset font: bad signature 0x%08X in %s",
                                (unsigned)hdr.Signature, info->path);
                    lv_free(file_buffer);
                    return NULL;
                }
                asset_size = hdr.AssetSize;
            }
            break;

        case LV_EVE5_ASSET_FONT_SRC_SDCARD:
            /* SD card: handled entirely in Phase 2 by load_asset_from_sdcard
             * (CMD_FSREAD into temp + MEDIAFIFO + CMD_LOADASSET). AssetSize
             * comes from the temp's EVE_Gpu_AssetHeader. No path-staging
             * here — uses raw CMD_FS* commands, independent of the LVGL
             * filesystem driver config. */
            if(info->path == NULL) {
                LV_LOG_WARN("EVE5 asset font: path required");
                return NULL;
            }
            break;

        case LV_EVE5_ASSET_FONT_SRC_FLASH:
            if((info->flash_address & 63u) != 0u) {
                LV_LOG_WARN("EVE5 asset font: flash address 0x%08X not 64-byte aligned",
                            (unsigned)info->flash_address);
                return NULL;
            }
#if LV_USE_OS
            lv_eve5_hal_lock(disp);
#endif
            if(!probe_flash_asset_size(phost, allocator, info->flash_address, &asset_size)) {
                LV_LOG_WARN("EVE5 asset font: flash header probe failed at 0x%08X",
                            (unsigned)info->flash_address);
#if LV_USE_OS
                lv_eve5_hal_unlock(disp);
#endif
                return NULL;
            }
#if LV_USE_OS
            lv_eve5_hal_unlock(disp);
#endif
            break;

        default:
            LV_LOG_WARN("EVE5 asset font: unknown source %d", info->source);
            return NULL;
    }

    /* AssetSize for SRC_SDCARD is determined inside load_asset_from_sdcard. */
    if(info->source != LV_EVE5_ASSET_FONT_SRC_SDCARD) {
        if(asset_size == 0 || asset_size > (64u * 1024u * 1024u)) {
            LV_LOG_WARN("EVE5 asset font: implausible AssetSize %u", (unsigned)asset_size);
            lv_free(file_buffer);
            return NULL;
        }
    }

    /* --- Phase 2: allocate, load, parse (HAL lock held) --- */
#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif

    bool stream_ok = false;
    switch(info->source) {
        case LV_EVE5_ASSET_FONT_SRC_SDCARD:
            /* load_asset_from_sdcard manages its own dst allocation: size
             * is unknown until after CMD_LOADASSET writes the font block. */
            stream_ok = load_asset_from_sdcard(phost, allocator, info->path,
                                               &handle, &asset_size);
            if(stream_ok) {
                dst_addr = EVE_GpuAlloc_Get(allocator, handle);
            }
            break;

        case LV_EVE5_ASSET_FONT_SRC_FLASH:
        case LV_EVE5_ASSET_FONT_SRC_FILE:
        default:
            /* GA_FIXED_FLAG pins the allocation — the asset's relocation
             * table patched absolute RAM_G addresses into the font block
             * (page table, CD glyph pointers), and CMD_SETFONT2 captured
             * the load address into coprocessor RAM. A defragger move
             * would invalidate all of those. 4-byte alignment is enough
             * since the relocator emits 4-byte-aligned BITMAP_SOURCE. */
            handle = EVE_GpuAlloc_Alloc(allocator, asset_size, GA_ALIGN_4 | GA_FIXED_FLAG);
            dst_addr = EVE_GpuAlloc_Get(allocator, handle);
            if(dst_addr == GA_INVALID) {
                LV_LOG_WARN("EVE5 asset font: RAM_G alloc failed (%u bytes)", (unsigned)asset_size);
                goto unlock_out;
            }
            if(info->source == LV_EVE5_ASSET_FONT_SRC_FLASH) {
                stream_ok = issue_loadasset_from_flash(phost, info->flash_address, dst_addr);
            }
            else {
                stream_ok = issue_loadasset_from_buffer(phost, file_buffer, file_size, dst_addr);
            }
            break;
    }

    if(!stream_ok || dst_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 asset font: CMD_LOADASSET failed");
        goto unlock_out;
    }

    /* Parse the loaded font block to build the width table. */
    lv_eve5_asset_font_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_eve5_asset_font_dsc_t));
    if(dsc == NULL) {
        LV_LOG_WARN("EVE5 asset font: dsc alloc failed");
        goto unlock_out;
    }

    int32_t binary_base_line = 0;
    if(!lv_eve5_font_block_load(phost, dst_addr, &dsc->block, &binary_base_line)) {
        LV_LOG_WARN("EVE5 asset font: font block parse failed at 0x%08X", (unsigned)dst_addr);
        lv_free(dsc);
        goto unlock_out;
    }

    dsc->hal = phost;
    dsc->allocator = allocator;
    dsc->gpu_handle = handle;
    dsc->first_character = info->first_character;
    dsc->cached_handle = 0xFFu;
    dsc->last_bound_addr = GA_INVALID;

    int32_t line_height = info->line_height > 0
                          ? info->line_height
                          : (int32_t)dsc->block.pixel_height;
    int32_t base_line = info->base_line > 0
                        ? info->base_line
                        : (binary_base_line > 0 ? binary_base_line : line_height);

    font = lv_malloc_zeroed(sizeof(lv_font_t));
    if(font == NULL) {
        lv_eve5_font_block_free(&dsc->block);
        lv_free(dsc);
        goto unlock_out;
    }

    font->get_glyph_dsc = font_get_glyph_dsc;
    font->get_glyph_bitmap = lv_eve5_asset_font_glyph_bitmap_sentinel;
    font->release_glyph = NULL;
    font->line_height = line_height;
    font->base_line = base_line;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->underline_position = info->underline_position != 0 ? info->underline_position : -2;
    font->underline_thickness = info->underline_thickness != 0 ? info->underline_thickness : 1;
    font->dsc = dsc;
    font->fallback = NULL;
    font->user_data = NULL;

    LV_LOG_INFO("EVE5 asset font loaded: addr=0x%08X size=%u fmt=%u height=%d",
                (unsigned)dst_addr, (unsigned)asset_size,
                (unsigned)dsc->block.format, (int)dsc->block.pixel_height);

    ok = true;

unlock_out:
    if(!ok && handle.Id != GA_HANDLE_INVALID.Id
       && EVE_GpuAlloc_Get(allocator, handle) != GA_INVALID) {
        EVE_GpuAlloc_Free(allocator, handle);
    }
#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif
    lv_free(file_buffer);
    return ok ? font : NULL;
}

void lv_eve5_asset_font_destroy(lv_font_t * font)
{
    if(!lv_eve5_is_asset_font(font)) return;
    lv_eve5_asset_font_dsc_t * dsc = (lv_eve5_asset_font_dsc_t *)font->dsc;
    if(dsc != NULL) {
        if(dsc->allocator != NULL && EVE_GpuAlloc_Get(dsc->allocator, dsc->gpu_handle) != GA_INVALID) {
            EVE_GpuAlloc_Free(dsc->allocator, dsc->gpu_handle);
        }
        lv_eve5_font_block_free(&dsc->block);
        lv_free(dsc);
    }
    lv_free(font);
}

uint32_t lv_eve5_asset_font_get_address(const lv_font_t * font)
{
    if(!lv_eve5_is_asset_font(font)) return GA_INVALID;
    const lv_eve5_asset_font_dsc_t * dsc = (const lv_eve5_asset_font_dsc_t *)font->dsc;
    if(dsc == NULL || dsc->allocator == NULL) return GA_INVALID;
    return EVE_GpuAlloc_Get(dsc->allocator, dsc->gpu_handle);
}

uint32_t lv_eve5_asset_font_get_first_char(const lv_font_t * font)
{
    if(!lv_eve5_is_asset_font(font)) return 0;
    const lv_eve5_asset_font_dsc_t * dsc = (const lv_eve5_asset_font_dsc_t *)font->dsc;
    return dsc != NULL ? dsc->first_character : 0;
}

/**********************
 * INTERNAL (DRAW UNIT)
 *
 * Exposed by extern declaration in lv_draw_eve5_rom_font.c — the draw-side
 * resolve binds a bitmap handle via CMD_SETFONT2 using these. The dsc
 * pointer itself serves as the stable owner identity for the handle pool.
 **********************/

void * lv_eve5_asset_font_get_owner_internal(const lv_font_t * font);
uint8_t lv_eve5_asset_font_get_cached_handle_internal(const lv_font_t * font);
void lv_eve5_asset_font_set_cached_handle_internal(const lv_font_t * font, uint8_t handle);
uint32_t lv_eve5_asset_font_get_last_bound_addr_internal(const lv_font_t * font);
void lv_eve5_asset_font_set_last_bound_addr_internal(const lv_font_t * font, uint32_t addr);

void * lv_eve5_asset_font_get_owner_internal(const lv_font_t * font)
{
    if(!lv_eve5_is_asset_font(font)) return NULL;
    return (void *)font->dsc;
}

uint8_t lv_eve5_asset_font_get_cached_handle_internal(const lv_font_t * font)
{
    if(!lv_eve5_is_asset_font(font)) return 0xFFu;
    const lv_eve5_asset_font_dsc_t * dsc = (const lv_eve5_asset_font_dsc_t *)font->dsc;
    return dsc != NULL ? dsc->cached_handle : 0xFFu;
}

void lv_eve5_asset_font_set_cached_handle_internal(const lv_font_t * font, uint8_t handle)
{
    if(!lv_eve5_is_asset_font(font)) return;
    lv_eve5_asset_font_dsc_t * dsc = (lv_eve5_asset_font_dsc_t *)font->dsc;
    if(dsc != NULL) dsc->cached_handle = handle;
}

uint32_t lv_eve5_asset_font_get_last_bound_addr_internal(const lv_font_t * font)
{
    if(!lv_eve5_is_asset_font(font)) return GA_INVALID;
    const lv_eve5_asset_font_dsc_t * dsc = (const lv_eve5_asset_font_dsc_t *)font->dsc;
    return dsc != NULL ? dsc->last_bound_addr : GA_INVALID;
}

void lv_eve5_asset_font_set_last_bound_addr_internal(const lv_font_t * font, uint32_t addr)
{
    if(!lv_eve5_is_asset_font(font)) return;
    lv_eve5_asset_font_dsc_t * dsc = (lv_eve5_asset_font_dsc_t *)font->dsc;
    if(dsc != NULL) dsc->last_bound_addr = addr;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Open @p path via lv_fs and read the entire contents into a heap buffer.
 * Caller frees the returned buffer with lv_free. Returns NULL on failure. */
static uint8_t * read_file_into_buffer(const char * path, uint32_t * out_size)
{
    *out_size = 0;
    lv_fs_file_t file;
    if(lv_fs_open(&file, path, LV_FS_MODE_RD) != LV_FS_RES_OK) {
        return NULL;
    }

    uint32_t size = 0;
    if(lv_fs_seek(&file, 0, LV_FS_SEEK_END) != LV_FS_RES_OK
       || lv_fs_tell(&file, &size) != LV_FS_RES_OK
       || lv_fs_seek(&file, 0, LV_FS_SEEK_SET) != LV_FS_RES_OK
       || size == 0) {
        lv_fs_close(&file);
        return NULL;
    }

    /* Pad to 4 bytes for EVE_Cmd_wrMem alignment. */
    uint32_t padded = (size + 3u) & ~3u;
    uint8_t * buf = lv_malloc(padded);
    if(buf == NULL) {
        lv_fs_close(&file);
        return NULL;
    }

    uint32_t got = 0;
    lv_fs_res_t r = lv_fs_read(&file, buf, size, &got);
    lv_fs_close(&file);
    if(r != LV_FS_RES_OK || got != size) {
        lv_free(buf);
        return NULL;
    }
    if(padded > size) {
        lv_memzero(buf + size, padded - size);
    }
    *out_size = size;
    return buf;
}

/* Issue CMD_LOADASSET + stream @p data into the command FIFO. The caller
 * has already zero-padded the buffer to 4-byte alignment. Caller holds
 * the HAL mutex. */
static bool issue_loadasset_from_buffer(EVE_HalContext * phost,
                                        const uint8_t * data, uint32_t size,
                                        uint32_t dst_addr)
{
    if(phost->CmdFault) {
        LV_LOG_ERROR("EVE5 asset font: coprocessor fault before CMD_LOADASSET");
        return false;
    }

    EVE_Cmd_wr32(phost, CMD_LOADASSET);
    EVE_Cmd_wr32(phost, dst_addr);
    EVE_Cmd_wr32(phost, 0u); /* options: 0 = command buffer */

    uint32_t padded = (size + 3u) & ~3u;
    if(!EVE_Cmd_wrMem(phost, data, padded)) {
        LV_LOG_ERROR("EVE5 asset font: command buffer write failed");
        return false;
    }
    if(!EVE_Cmd_waitFlush(phost)) {
        LV_LOG_ERROR("EVE5 asset font: CMD_LOADASSET execution failed");
        return false;
    }
    return true;
}

/* Zero-copy load from QSPI flash. Caller holds the HAL mutex. */
static bool issue_loadasset_from_flash(EVE_HalContext * phost, uint32_t flash_addr,
                                       uint32_t dst_addr)
{
    if(!EVE_CoCmd_loadAsset_flash(phost, dst_addr, flash_addr, 0)) {
        LV_LOG_ERROR("EVE5 asset font: flash CMD_LOADASSET failed");
        return false;
    }
    return true;
}

/* Read the 12-byte EVE_Gpu_AssetHeader from flash via CMD_FLASHREAD into a
 * temporary RAM_G slot, then read it back to host. Caller holds the HAL
 * mutex. CMD_FLASHREAD requires src 64-aligned, num multiple of 4. */
static bool probe_flash_asset_size(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   uint32_t flash_addr, uint32_t * out_size)
{
    const uint32_t hdr_size_aligned = 16u; /* 12 bytes padded */
    EVE_GpuHandle tmp = EVE_GpuAlloc_Alloc(alloc, hdr_size_aligned, GA_ALIGN_4);
    uint32_t tmp_addr = EVE_GpuAlloc_Get(alloc, tmp);
    if(tmp_addr == GA_INVALID) return false;

    EVE_CoCmd_flashRead(phost, tmp_addr, flash_addr, hdr_size_aligned);
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(alloc, tmp);
        return false;
    }
    EVE_Gpu_AssetHeader hdr;
    EVE_Hal_rdMem(phost, (uint8_t *)&hdr, tmp_addr, sizeof(hdr));
    EVE_GpuAlloc_Free(alloc, tmp);
    if(hdr.Signature != EVE_GPU_ASSET_SIGNATURE) return false;
    *out_size = hdr.AssetSize;
    return true;
}

/* Strip an LVGL drive-letter prefix ("S:/foo" → "/foo") so CMD_FSSOURCE
 * sees the SD-relative path. Bare paths (no `:`) are returned unchanged. */
static const char * strip_lvgl_drive_letter(const char * path)
{
    if(path == NULL) return NULL;
    if(path[0] != '\0' && path[1] == ':' && (path[2] == '/' || path[2] == '\\')) {
        return path + 2;
    }
    return path;
}

#if EVE_COCMD_PATCH_QUERY

/* SD card load (zero-copy fast path, enabled by EVE_COCMD_PATCH_QUERY):
 * CMD_QUERYASSET reports the inflated AssetSize via the coprocessor's
 * GetProps result without touching RAM_G. We then allocate the exact
 * destination and run CMD_LOADASSET(OPT_FS) which streams directly from
 * the SD card to the final RAM_G slot — no intermediate buffer, no
 * MediaFIFO. CMD_FSSOURCE is re-issued before each consuming command
 * because the firmware reopens the file per scan.
 *
 * Caller holds the HAL mutex. */
static bool load_asset_from_sdcard(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   const char * path, EVE_GpuHandle * out_handle,
                                   uint32_t * out_asset_size)
{
    if(!EVE_Hal_supportSdCard(phost)) {
        LV_LOG_WARN("EVE5 asset font: SD card not supported on this chip");
        return false;
    }
    if(EVE_CoCmd_sdAttach(phost, 0) != 0) {
        LV_LOG_WARN("EVE5 asset font: SD card attach failed");
        return false;
    }

    const char * sd_path = strip_lvgl_drive_letter(path);

    /* The queryassets patch's CMD_QUERYASSET writes the inflated size into
     * `image.source` (and nothing else); EVE_CoCmd_queryAsset_fs reads it
     * back via CMD_GETIMAGE. CMD_GETPROPS reads REG_EJPG_DST which the
     * query commands never touch, so it would always come back as 0. */
    uint32_t asset_size = 0;
    uint32_t q_res = EVE_CoCmd_queryAsset_fs(phost, sd_path, 0, &asset_size);
    if(q_res != 0) {
        LV_LOG_WARN("EVE5 asset font: CMD_QUERYASSET failed for %s (code %u)", path, (unsigned)q_res);
        return false;
    }
    if(asset_size == 0 || asset_size > (64u * 1024u * 1024u)) {
        LV_LOG_WARN("EVE5 asset font: implausible queried AssetSize %u for %s",
                    (unsigned)asset_size, path);
        return false;
    }

    /* GA_FIXED_FLAG pins the allocation (see SRC_FILE/SRC_FLASH branch). */
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, asset_size, GA_ALIGN_4 | GA_FIXED_FLAG);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 asset font: SD final RAM_G alloc failed (%u bytes)",
                    (unsigned)asset_size);
        return false;
    }

    uint32_t load_res = EVE_CoCmd_loadAsset_fs(phost, final_addr, sd_path, 0);
    if(load_res != 0) {
        LV_LOG_ERROR("EVE5 asset font: SD CMD_LOADASSET failed for %s (code %u)", path, (unsigned)load_res);
        EVE_GpuAlloc_Free(alloc, final_handle);
        return false;
    }
    EVE_Hal_requestFenceBeforeSwap(phost);

    *out_handle = final_handle;
    *out_asset_size = asset_size;
    return true;
}

#else /* EVE_COCMD_PATCH_QUERY */

/* SD card load (legacy intermediate-buffer path): CMD_FSREAD the .reloc
 * into a temp RAM_G buffer, peek the 12-byte AssetHeader for AssetSize,
 * allocate the destination, then run CMD_LOADASSET(OPT_MEDIAFIFO) sourcing
 * from the temp buffer. Mirrors lv_eve5_sdcard_load_image — one SD read,
 * exact destination size. The coprocessor consumes the FIFO synchronously
 * with EVE_Cmd_waitFlush, so the temp buffer is safe to free immediately
 * after.
 *
 * This path roundtrips the full compressed file through RAM_G. Define
 * EVE_COCMD_PATCH_QUERY=1 to enable the CMD_QUERYASSET fast path that
 * avoids the intermediate copy.
 *
 * Caller holds the HAL mutex. */
static bool load_asset_from_sdcard(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   const char * path, EVE_GpuHandle * out_handle,
                                   uint32_t * out_asset_size)
{
    LV_LOG_WARN("EVE5 asset font: SD load uses intermediate RAM_G buffer "
                "(define EVE_COCMD_PATCH_QUERY=1 for zero-copy CMD_QUERYASSET path)");

    /* Idempotent attach — lv_eve5_sdcard_ready() would re-take the HAL
     * mutex which our caller already holds. CMD_SDATTACH returns 0 on
     * success / already-attached. */
    if(!EVE_Hal_supportSdCard(phost)) {
        LV_LOG_WARN("EVE5 asset font: SD card not supported on this chip");
        return false;
    }
    if(EVE_CoCmd_sdAttach(phost, 0) != 0) {
        LV_LOG_WARN("EVE5 asset font: SD card attach failed");
        return false;
    }

    const char * sd_path = strip_lvgl_drive_letter(path);

    uint32_t file_size = EVE_CoCmd_fsSize(phost, sd_path);
    if(file_size == 0xFFFFFFFFu || file_size == 0) {
        LV_LOG_WARN("EVE5 asset font: CMD_FSSIZE failed for %s", path);
        return false;
    }

    /* CMD_FSREAD requires 32-byte alignment, MEDIAFIFO requires 4-byte size
     * alignment. Round up for both. */
    uint32_t fifo_size = (file_size + 3u) & ~3u;
    EVE_GpuHandle temp_handle = EVE_GpuAlloc_AlignedAlloc(alloc, fifo_size, 0, 32);
    uint32_t temp_addr = EVE_GpuAlloc_Get(alloc, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 asset font: SD temp RAM_G alloc failed (%u bytes)", fifo_size);
        return false;
    }

    if(EVE_CoCmd_fsRead(phost, temp_addr, sd_path) != 0) {
        LV_LOG_WARN("EVE5 asset font: CMD_FSREAD failed for %s", path);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }

    /* Peek the 12-byte header to learn the inflated AssetSize. */
    temp_addr = EVE_GpuAlloc_Get(alloc, temp_handle);
    EVE_Gpu_AssetHeader hdr;
    EVE_Hal_rdMem(phost, (uint8_t *)&hdr, temp_addr, sizeof(hdr));
    if(hdr.Signature != EVE_GPU_ASSET_SIGNATURE) {
        LV_LOG_WARN("EVE5 asset font: bad SD asset signature 0x%08X in %s",
                    (unsigned)hdr.Signature, path);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }
    if(hdr.AssetSize == 0 || hdr.AssetSize > (64u * 1024u * 1024u)) {
        LV_LOG_WARN("EVE5 asset font: implausible SD AssetSize %u", (unsigned)hdr.AssetSize);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }

    /* GA_FIXED_FLAG pins the allocation (see SRC_FILE/SRC_FLASH branch
     * above). 4-byte alignment is sufficient for the relocated
     * BITMAP_SOURCE pointers. */
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, hdr.AssetSize, GA_ALIGN_4 | GA_FIXED_FLAG);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 asset font: SD final RAM_G alloc failed (%u bytes)",
                    (unsigned)hdr.AssetSize);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }

    /* MEDIAFIFO points at the temp buffer; data is already there from
     * CMD_FSREAD, so we set REG_MEDIAFIFO_WRITE to file_size directly. */
    EVE_MediaFifo_close(phost);
    temp_addr = EVE_GpuAlloc_Get(alloc, temp_handle);
    if(!EVE_MediaFifo_set(phost, temp_addr, fifo_size)) {
        LV_LOG_WARN("EVE5 asset font: MediaFIFO set failed");
        EVE_GpuAlloc_Free(alloc, final_handle);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }
    EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, file_size);

    if(phost->CmdFault) {
        LV_LOG_ERROR("EVE5 asset font: coprocessor fault before SD CMD_LOADASSET");
        EVE_MediaFifo_close(phost);
        EVE_GpuAlloc_Free(alloc, final_handle);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }

    EVE_CoCmd_loadAsset(phost, final_addr, OPT_MEDIAFIFO);
    bool flushed = EVE_Cmd_waitFlush(phost);
    EVE_MediaFifo_close(phost);

    if(!flushed) {
        LV_LOG_ERROR("EVE5 asset font: SD CMD_LOADASSET failed for %s", path);
        EVE_GpuAlloc_Free(alloc, final_handle);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }
    EVE_Hal_requestFenceBeforeSwap(phost);

    /* Temp buffer was synchronously consumed by waitFlush — safe to free. */
    EVE_GpuAlloc_Free(alloc, temp_handle);

    *out_handle = final_handle;
    *out_asset_size = hdr.AssetSize;
    return true;
}

#endif /* EVE_COCMD_PATCH_QUERY */

/**********************
 *   GLYPH METRICS
 **********************/

static bool font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                               uint32_t letter, uint32_t letter_next)
{
    LV_UNUSED(letter_next);

    const lv_eve5_asset_font_dsc_t * dsc = (const lv_eve5_asset_font_dsc_t *)font->dsc;
    if(dsc == NULL) return false;

    if(letter > 0xFFFFu || letter < 0x20) return false;

    uint8_t w = lv_eve5_font_block_width(&dsc->block, letter);
    if(w == 0) return false;

    dsc_out->resolved_font = font;
    dsc_out->gid.index = letter;
    dsc_out->format = LV_FONT_GLYPH_FORMAT_A8;
    dsc_out->adv_w = w;
    dsc_out->box_w = w;
    dsc_out->box_h = dsc->block.pixel_height;
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;
    dsc_out->stride = 0;
    dsc_out->is_placeholder = 0;
    dsc_out->req_raw_bitmap = 0;
    return true;
}

#elif LV_USE_EVE5

/* Pre-BT820 stubs: CMD_LOADASSET is BT820-only. */

LV_IMAGE_DSC_CONST void * lv_eve5_asset_font_glyph_bitmap_sentinel(lv_font_glyph_dsc_t * g_dsc,
                                                                  lv_draw_buf_t * draw_buf)
{
    LV_UNUSED(g_dsc);
    LV_UNUSED(draw_buf);
    return NULL;
}

lv_font_t * lv_eve5_asset_font_create(lv_display_t * disp,
                                      const lv_eve5_asset_font_info_t * info)
{
    (void)disp; (void)info;
    LV_LOG_WARN("EVE5 asset font: requires BT820+ (CMD_LOADASSET)");
    return NULL;
}

void lv_eve5_asset_font_destroy(lv_font_t * font)            { (void)font; }
uint32_t lv_eve5_asset_font_get_address(const lv_font_t * f)   { (void)f; return 0xFFFFFFFFu; }
uint32_t lv_eve5_asset_font_get_first_char(const lv_font_t * f) { (void)f; return 0; }

void * lv_eve5_asset_font_get_owner_internal(const lv_font_t * f)            { (void)f; return NULL; }
uint8_t lv_eve5_asset_font_get_cached_handle_internal(const lv_font_t * f)   { (void)f; return 0xFFu; }
void lv_eve5_asset_font_set_cached_handle_internal(const lv_font_t * f, uint8_t h) { (void)f; (void)h; }
uint32_t lv_eve5_asset_font_get_last_bound_addr_internal(const lv_font_t * f) { (void)f; return 0xFFFFFFFFu; }
void lv_eve5_asset_font_set_last_bound_addr_internal(const lv_font_t * f, uint32_t a) { (void)f; (void)a; }

#endif /* LV_USE_EVE5 && (EVE_SUPPORT_CHIPID >= EVE_BT820) */
