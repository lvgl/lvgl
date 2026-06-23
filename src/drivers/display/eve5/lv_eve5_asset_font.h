/**
 * @file lv_eve5_asset_font.h
 *
 * Custom EVE font wrapper for LVGL — loads a BT82X relocatable font asset
 * (`.reloc`) into RAM_G via CMD_LOADASSET and exposes it as an lv_font_t.
 *
 * The .reloc container holds a deflate-compressed font block (legacy /
 * extended format 1 / extended format 2) plus a relocation table. The
 * coprocessor's asset loader inflates the data, patches embedded pointers
 * to be relative to the load address, and binds bitmap-handle references
 * so the loaded block is immediately usable by CMD_TEXT.
 *
 * Until a `.esdm` metadata sidecar is available (see FONT_METADATA.md),
 * the caller passes per-font metadata (path, load options, optional metric
 * overrides) through lv_eve5_asset_font_info_t at create time. Most metric
 * fields can be left at 0 — they default to values read from the loaded
 * font binary.
 *
 * Detection: the EVE5 text renderer compares font->get_glyph_bitmap
 * against lv_eve5_asset_font_glyph_bitmap_sentinel to identify asset
 * fonts. Rendering requires LV_USE_DRAW_EVE5=1 on a BT820+ chip.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_ASSET_FONT_H
#define LV_EVE5_ASSET_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl_public.h"

#if LV_USE_EVE5

#include "EVE_Hal.h"
#include "EVE_GpuAlloc.h"

/**********************
 *      DEFINES
 **********************/

/** Sentinel get_glyph_bitmap callback used by lv_eve5_is_asset_font for
 *  pointer-equality detection. Returns NULL. */
LV_IMAGE_DSC_CONST void * lv_eve5_asset_font_glyph_bitmap_sentinel(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);

/** Font asset source. */
typedef enum {
    /** Stream a host file through lv_fs (LV_USE_FS_STDIO or any registered
     *  non-EVE LVGL FS driver). The file is pre-read into a host buffer
     *  then fed into the command FIFO. Use this for filesystem backends
     *  that don't have an EVE-side zero-copy alternative. */
    LV_EVE5_ASSET_FONT_SRC_FILE = 0,
    /** Load from the BT820 SD card. Bypasses LVGL I/O. Path may include
     *  an LVGL drive-letter prefix (e.g. "S:/foo.reloc"); it's stripped
     *  before the EVE FS commands. Sizing comes from EVE_queryResource_fs
     *  (CMD_QUERYASSET_fs on patched BT820+ firmware, SW probe over a
     *  CMD_FSREAD'd staging buffer otherwise); the actual load is then
     *  zero-copy CMD_LOADASSET(OPT_FS) when the CMD path was used, or
     *  MediaFIFO reuse of the staging buffer when the SW path was used. */
    LV_EVE5_ASSET_FONT_SRC_SDCARD,
    /** Zero-copy load from EVE QSPI flash via CMD_FLASHSOURCE +
     *  CMD_LOADASSET(OPT_FLASH). flash_address must be 64-byte aligned. */
    LV_EVE5_ASSET_FONT_SRC_FLASH,
} lv_eve5_asset_font_src_t;

/**
 * Metadata for loading a custom EVE font asset.
 *
 * Most metric fields are optional and only need to be supplied when they
 * differ from the values derived from the loaded font binary:
 *
 *   - line_height defaults to the binary's pixel_height
 *   - base_line / caps_height default to line_height when left at 0
 *   - underline_position / underline_thickness use LVGL's normal defaults
 *
 * The caller-supplied @ref first_character must match the value baked
 * into the binary (it's a CMD_SETFONT2 argument). For extended format 2
 * fonts it should be 0; for legacy fonts typically 32.
 */
typedef struct {
    /* --- Source --- */
    lv_eve5_asset_font_src_t source;
    const char * path;            /**< Required for SRC_FILE / SRC_SDCARD */
    uint32_t flash_address;       /**< Required for SRC_FLASH, must be 64-byte aligned */

    /* --- Metric overrides (0 = use default) --- */
    int32_t line_height;          /**< 0 = use pixel_height from binary */
    int32_t base_line;            /**< 0 = default to line_height */
    int8_t underline_position;    /**< Pixels below baseline (negative below); LVGL default -2 */
    int8_t underline_thickness;   /**< Underline thickness in pixels; LVGL default 1 */
    uint32_t first_character;     /**< CMD_SETFONT2 first-char argument (0 for ext2) */
} lv_eve5_asset_font_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Load a .reloc font asset and wrap it as an lv_font_t.
 *
 * The display's HAL context and GPU allocator are used for the load and
 * for the lifetime of the resulting font. The asset is loaded once into
 * RAM_G and stays there until lv_eve5_asset_font_destroy is called.
 *
 * @param disp  EVE5 display (booted; provides HAL + allocator)
 * @param info  Source and optional metric overrides
 * @return      lv_font_t backed by the loaded asset, or NULL on failure
 */
lv_font_t * lv_eve5_asset_font_create(lv_display_t * disp,
                                      const lv_eve5_asset_font_info_t * info);

/**
 * Free an asset font and its RAM_G allocation. Safe to call with NULL or
 * with a non-asset-font (no-op).
 */
void lv_eve5_asset_font_destroy(lv_font_t * font);

/**
 * Pointer-equality test: was @p font created by lv_eve5_asset_font_create?
 */
static inline bool lv_eve5_is_asset_font(const lv_font_t * font)
{
    return font != NULL && font->get_glyph_bitmap == lv_eve5_asset_font_glyph_bitmap_sentinel;
}

/**
 * Return the RAM_G base address of the loaded asset font block, or
 * GA_INVALID if the underlying allocation has been reclaimed. The draw
 * unit uses this to (re-)bind a bitmap handle via CMD_SETFONT.
 */
uint32_t lv_eve5_asset_font_get_address(const lv_font_t * font);

/**
 * Return the first-character value to pass to CMD_SETFONT2 for this font
 * (0 for extended format 2 / unicode fonts, the legacy `first_character`
 * field for legacy fonts).
 */
uint32_t lv_eve5_asset_font_get_first_char(const lv_font_t * font);

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_ASSET_FONT_H */
