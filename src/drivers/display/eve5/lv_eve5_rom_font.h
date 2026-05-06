/**
 * @file lv_eve5_rom_font.h
 *
 * EVE ROM Font wrapper for LVGL.
 *
 * Builds an lv_font_t that delegates rendering to the EVE coprocessor's
 * built-in fonts (rom indices 16..34). Glyphs render via CMD_TEXT — the
 * firmware handles unicode cell-page lookups, so multibyte UTF-8 is
 * supported transparently for fonts that carry the data (BT820 fonts
 * 32..34). For layout, the wrapper currently exposes the rom font's
 * uniform PixelWidth as advance width for every glyph; this is correct
 * for monospace rom fonts and a slight overestimate for proportional
 * ones (text wraps a touch more aggressively than necessary).
 *
 * Detection: the EVE5 text renderer compares font->get_glyph_bitmap
 * against lv_eve5_rom_font_glyph_bitmap_sentinel to identify rom fonts.
 *
 * No SW fallback — when the EVE5 draw unit is disabled (LV_USE_DRAW_EVE5=0)
 * a rom font renders nothing. Pair with a fallback font if that matters.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_ROM_FONT_H
#define LV_EVE5_ROM_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5

#include "../../../font/lv_font.h"
#include "EVE_Hal.h"

/**********************
 *      DEFINES
 **********************/

/** Sentinel get_glyph_bitmap callback. The EVE5 text renderer treats any font
 *  whose get_glyph_bitmap matches this pointer as a rom font and dispatches
 *  CMD_TEXT instead of the bitmap upload path. The function itself returns
 *  NULL — rom font glyphs have no host-side bitmap. */
LV_IMAGE_DSC_CONST void * lv_eve5_rom_font_glyph_bitmap_sentinel(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an lv_font_t backed by an EVE ROM font.
 *
 * Reads height/width metrics from the rom font header at create time. The
 * returned font is heap-allocated; release with lv_eve5_rom_font_destroy.
 *
 * @param phost     EVE HAL context (must already be booted)
 * @param rom_idx   ROM font index (16..31 universal, 32..34 BT820 large fonts)
 * @return          newly allocated lv_font_t, or NULL if the index is out of range
 *                  or the chip doesn't support that rom font
 */
lv_font_t * lv_eve5_rom_font_create(EVE_HalContext * phost, uint8_t rom_idx);

/**
 * Free a rom font previously returned by lv_eve5_rom_font_create.
 * Safe to call with NULL.
 */
void lv_eve5_rom_font_destroy(lv_font_t * font);

/**
 * Test whether a font was created by lv_eve5_rom_font_create.
 * Cheap pointer comparison against the sentinel callback.
 */
static inline bool lv_eve5_is_rom_font(const lv_font_t * font)
{
    return font != NULL && font->get_glyph_bitmap == lv_eve5_rom_font_glyph_bitmap_sentinel;
}

/**
 * Return the rom index of a rom font (16..34), or 0xFF if `font` isn't one.
 */
uint8_t lv_eve5_rom_font_get_index(const lv_font_t * font);

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_ROM_FONT_H */
