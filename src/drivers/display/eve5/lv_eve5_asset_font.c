/**
 * @file lv_eve5_asset_font.c
 *
 * Loads BT82X .reloc font assets via CMD_LOADASSET and wraps them as
 * lv_font_t instances. See lv_eve5_asset_font.h for API.
 *
 * Load flow:
 *   1. Determine AssetSize via the eve_extras query/probe API. Source-specific:
 *      - SRC_FILE: lv_fs read into a host buffer, then EVE_probeResource over
 *        the bytes — verifies the RELOC signature and reports the 32-byte
 *        padded inflated size.
 *      - SRC_FLASH: EVE_queryResource_flash(hint=RELOC, preferCmd=true) —
 *        CMD_QUERYASSET_flash on patched BT820+, chunked CMD_FLASHREAD +
 *        EVE_probeResource otherwise.
 *      - SRC_SDCARD: EVE_queryResource_fs(preferCmd=true) — CMD_QUERYASSET_fs
 *        on patched BT820+; otherwise SW probe over a CMD_FSREAD'd staging
 *        buffer that gets handed back for reuse in step 3.
 *   2. Allocate AssetSize bytes in RAM_G via EVE_GpuAlloc (4-byte aligned).
 *   3. Issue CMD_LOADASSET with the destination address. Source data is:
 *      - SRC_FILE: streamed via MediaFIFO from the pre-read host buffer.
 *      - SRC_FLASH: zero-copy via CMD_FLASHSOURCE + OPT_FLASH.
 *      - SRC_SDCARD CMD path: zero-copy via CMD_FSSOURCE + OPT_FS.
 *      - SRC_SDCARD SW path: MediaFIFO over the staging buffer the query
 *        already populated (no second SD read).
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
#include "EVE_ResourceProbe.h"
#include "EVE_ResourceQuery.h"

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

static bool issue_loadasset_from_buffer(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                        const uint8_t * data, uint32_t size,
                                        uint32_t dst_addr);
static bool issue_loadasset_from_flash(EVE_HalContext * phost, uint32_t flash_addr,
                                       uint32_t dst_addr);

static bool load_asset_from_sdcard(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   lv_display_t * disp, const char * path,
                                   EVE_GpuHandle * out_handle, uint32_t * out_asset_size);
static const char * strip_lvgl_drive_letter(const char * path);

/* Adapter for EVE_queryResource_* fault recovery — wraps the driver's narrow
 * coprocessor reset (deferred-free retirement + draw-unit cache invalidation). */
static void asset_query_reset_cb(EVE_HalContext * phost, void * userdata)
{
    (void)phost;
    lv_eve5_reset_coprocessor((lv_display_t *)userdata);
}

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
        case LV_EVE5_ASSET_FONT_SRC_FILE: {
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
            /* Probe verifies the RELOC signature and reports the 32-byte-padded
             * inflated AssetSize that CMD_LOADASSET will allocate. */
            EVE_ResourceProbe probe;
            EVE_initProbe(&probe);
            EVE_ResourceInfo r_info;
            int s = EVE_probeResource(&probe, EVE_CHIPID, file_buffer, file_size, 0, &r_info);
            if(s != 1 || r_info.Type != EVE_RESOURCE_RELOC) {
                LV_LOG_WARN("EVE5 asset font: not a .reloc asset (probe=%d type=%u): %s",
                            s, (unsigned)r_info.Type, info->path);
                lv_free(file_buffer);
                return NULL;
            }
            asset_size = r_info.Size;
            break;
        }

        case LV_EVE5_ASSET_FONT_SRC_SDCARD:
            /* SD card: handled entirely in Phase 2 by load_asset_from_sdcard,
             * which calls EVE_queryResource_fs for sizing (CMD_QUERYASSET_fs
             * on patched BT820+, otherwise SW probe over a CMD_FSREAD'd staging
             * buffer that gets reused as the MediaFIFO source for the load). */
            if(info->path == NULL) {
                LV_LOG_WARN("EVE5 asset font: path required");
                return NULL;
            }
            break;

        case LV_EVE5_ASSET_FONT_SRC_FLASH: {
            if((info->flash_address & 63u) != 0u) {
                LV_LOG_WARN("EVE5 asset font: flash address 0x%08X not 64-byte aligned",
                            (unsigned)info->flash_address);
                return NULL;
            }
#if LV_USE_OS
            lv_eve5_hal_lock(disp);
#endif
            /* Query: CMD_QUERYASSET_flash on patched BT820+, otherwise chunked
             * CMD_FLASHREAD + EVE_probeResource. The Size we get back is the
             * 32-byte-padded inflated payload, ready for allocation. */
            EVE_ResourceInfo r_info;
            bool q_ok = EVE_queryResource_flash(phost, allocator,
                                                (int32_t)info->flash_address,
                                                /*size hint*/ 64u * 1024u,
                                                EVE_RESOURCE_RELOC, 0, /*preferCmd*/ true,
                                                asset_query_reset_cb, disp, &r_info);
#if LV_USE_OS
            lv_eve5_hal_unlock(disp);
#endif
            if(!q_ok || r_info.Type != EVE_RESOURCE_RELOC) {
                LV_LOG_WARN("EVE5 asset font: flash query failed at 0x%08X",
                            (unsigned)info->flash_address);
                return NULL;
            }
            asset_size = r_info.Size;
            break;
        }

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
             * comes from EVE_queryResource_fs (CMD or SW path). */
            stream_ok = load_asset_from_sdcard(phost, allocator, disp, info->path,
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
                stream_ok = issue_loadasset_from_buffer(phost, allocator, file_buffer, file_size, dst_addr);
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
    if(!ok && handle.Id != GA_HANDLE_INVALID.Id) {
        /* Plain Free: the load was flushed synchronously and the block was
         * never bound to a bitmap handle, so nothing in flight samples it.
         * Free validates the handle internally. */
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
        if(dsc->allocator != NULL) {
            /* ScopedFree: an in-flight DL may still sample the font block
             * through its CMD_SETFONT2 bind; the resolve-time Gets stamped
             * the last referencing scope, which gates the release. */
            EVE_GpuAlloc_ScopedFree(dsc->allocator, dsc->gpu_handle);
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

/* Stage @p data in a RAM_G temp and feed CMD_LOADASSET via MediaFifo
 * (OPT_MEDIAFIFO). Trailing bytes past the relocator's end-of-stream get
 * dropped instead of running as commands. Pre-flush + SuppressErrorOverlay
 * isolate any decode fault to this attempt so lv_eve5_reset_coprocessor
 * can recover narrowly. Caller holds the HAL mutex. */
static bool issue_loadasset_from_buffer(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                        const uint8_t * data, uint32_t size,
                                        uint32_t dst_addr)
{
    uint32_t fifo_size = (size + 3u) & ~3u;
    EVE_GpuHandle temp_handle = EVE_GpuAlloc_AlignedAlloc(alloc, fifo_size, 0, 32);
    uint32_t temp_addr = EVE_GpuAlloc_Get(alloc, temp_handle);
    if(temp_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 asset font: failed to allocate %u-byte MediaFifo buffer", fifo_size);
        return false;
    }
    EVE_Hal_wrMem(phost, temp_addr, data, size);

    EVE_MediaFifo_close(phost);
    if(!EVE_MediaFifo_set(phost, temp_addr, fifo_size)) {
        LV_LOG_ERROR("EVE5 asset font: MediaFifo setup failed");
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }
    EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, size);

    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_MediaFifo_close(phost);
        EVE_GpuAlloc_Free(alloc, temp_handle);
        return false;
    }
    phost->SuppressErrorOverlay = true;

    EVE_CoCmd_loadAsset(phost, dst_addr, OPT_MEDIAFIFO);
    bool ok = EVE_Cmd_waitFlush(phost);

    EVE_MediaFifo_close(phost);
    EVE_GpuAlloc_Free(alloc, temp_handle);

    if(!ok) {
        LV_LOG_ERROR("EVE5 asset font: CMD_LOADASSET execution failed");
        if(phost->CmdFault) lv_eve5_reset_coprocessor(lv_eve5_disp_from_hal(phost));
        phost->SuppressErrorOverlay = false;
        return false;
    }
    phost->SuppressErrorOverlay = false;
    return true;
}

/* Zero-copy load from QSPI flash. Pre-flush + SuppressErrorOverlay isolate
 * any decode fault so lv_eve5_reset_coprocessor can recover narrowly.
 * Caller holds the HAL mutex. */
static bool issue_loadasset_from_flash(EVE_HalContext * phost, uint32_t flash_addr,
                                       uint32_t dst_addr)
{
    if(!EVE_Cmd_waitFlush(phost)) return false;
    phost->SuppressErrorOverlay = true;

    bool ok = EVE_CoCmd_loadAsset_flash(phost, dst_addr, flash_addr, 0);

    if(!ok) {
        LV_LOG_ERROR("EVE5 asset font: flash CMD_LOADASSET failed");
        if(phost->CmdFault) lv_eve5_reset_coprocessor(lv_eve5_disp_from_hal(phost));
    }
    phost->SuppressErrorOverlay = false;
    return ok;
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

/* SD card load: one path serving both BT820 firmware variants.
 *
 * EVE_queryResource_fs(preferCmd=true) picks CMD_QUERYASSET_fs on patched
 * BT820+ (no RAM_G round-trip — direct AssetSize) and falls back to a SW
 * probe over a CMD_FSREAD'd staging buffer otherwise. The staging handle is
 * handed back so the actual load can reuse the staged bytes through
 * OPT_MEDIAFIFO — no second SD read.
 *
 *   CMD path success → CMD_LOADASSET_fs(OPT_FS) — zero-copy from SD.
 *   SW path success  → MediaFIFO over the staging buffer + CMD_LOADASSET.
 *
 * Caller holds the HAL mutex. */
static bool load_asset_from_sdcard(EVE_HalContext * phost, EVE_GpuAlloc * alloc,
                                   lv_display_t * disp, const char * path,
                                   EVE_GpuHandle * out_handle, uint32_t * out_asset_size)
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

    EVE_ResourceInfo r_info;
    EVE_GpuHandle staging = GA_HANDLE_INVALID;
    uint32_t staging_size = 0;
    if(!EVE_queryResource_fs(phost, alloc, sd_path, 0, /*preferCmd*/ true,
                             asset_query_reset_cb, disp,
                             &staging, &staging_size, &r_info)) {
        LV_LOG_WARN("EVE5 asset font: queryResource failed for %s", path);
        return false;
    }
    if(r_info.Type != EVE_RESOURCE_RELOC) {
        LV_LOG_WARN("EVE5 asset font: %s isn't a .reloc asset (type=%u)",
                    path, (unsigned)r_info.Type);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
        return false;
    }
    uint32_t asset_size = r_info.Size;

    /* GA_FIXED_FLAG pins the allocation (see SRC_FILE/SRC_FLASH branch).
     * The relocator patched absolute RAM_G pointers into the font block. */
    EVE_GpuHandle final_handle = EVE_GpuAlloc_Alloc(alloc, asset_size, GA_ALIGN_4 | GA_FIXED_FLAG);
    uint32_t final_addr = EVE_GpuAlloc_Get(alloc, final_handle);
    if(final_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5 asset font: SD final RAM_G alloc failed (%u bytes)",
                    (unsigned)asset_size);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
        return false;
    }

    /* Pre-flush + SuppressErrorOverlay isolate any CMD_LOADASSET fault. */
    if(!EVE_Cmd_waitFlush(phost)) {
        EVE_GpuAlloc_Free(alloc, final_handle);
        if(staging.Id != GA_HANDLE_INVALID.Id) EVE_GpuAlloc_Free(alloc, staging);
        return false;
    }
    phost->SuppressErrorOverlay = true;

    bool load_ok = false;

    if(staging.Id == GA_HANDLE_INVALID.Id) {
        /* CMD path was taken by the query — stream directly from SD. */
#if EVE_COCMD_PATCH_QUERY
        uint32_t load_res = EVE_CoCmd_loadAsset_fs(phost, final_addr, sd_path, 0);
        load_ok = (load_res == 0);
#endif
    }
    else {
        /* SW path: the staging buffer holds the compressed file. Re-publish
         * it as a MediaFIFO so CMD_LOADASSET inflates directly into final_addr. */
        uint32_t staging_addr = EVE_GpuAlloc_Get(alloc, staging);
        if(staging_addr != GA_INVALID) {
            uint32_t fifo_size = (staging_size + 3u) & ~3u;
            EVE_MediaFifo_close(phost);
            if(EVE_MediaFifo_set(phost, staging_addr, fifo_size)) {
                EVE_Hal_wr32(phost, REG_MEDIAFIFO_WRITE, staging_size);
                EVE_CoCmd_loadAsset(phost, final_addr, OPT_MEDIAFIFO);
                load_ok = EVE_Cmd_waitFlush(phost);
                EVE_MediaFifo_close(phost);
            }
        }
        EVE_GpuAlloc_Free(alloc, staging);
    }

    if(!load_ok) {
        LV_LOG_ERROR("EVE5 asset font: SD CMD_LOADASSET failed for %s", path);
        if(phost->CmdFault) lv_eve5_reset_coprocessor(disp);
        EVE_GpuAlloc_Free(alloc, final_handle);
        phost->SuppressErrorOverlay = false;
        return false;
    }
    phost->SuppressErrorOverlay = false;
    EVE_Hal_requestFenceBeforeSwap(phost);

    *out_handle = final_handle;
    *out_asset_size = asset_size;
    return true;
}

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
