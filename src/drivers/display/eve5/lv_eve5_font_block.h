/**
 * @file lv_eve5_font_block.h
 *
 * EVE font metric-block parser, shared between ROM-font and asset-font
 * wrappers. Reads a legacy / extended format 1 / extended format 2 font
 * block from RAM_G and produces a run-based per-glyph width table.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_FONT_BLOCK_H
#define LV_EVE5_FONT_BLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl_public.h"

#if LV_USE_EVE5

#include "EVE_Hal.h"

#include <stdint.h>
#include <stdbool.h>

/**********************
 *      TYPEDEFS
 **********************/

/** Contiguous codepoint range with one advance-width byte per codepoint. */
typedef struct {
    uint16_t first;   /**< first codepoint covered by this run */
    uint16_t count;   /**< number of widths */
    uint8_t * widths; /**< widths[i] = adv_w for codepoint (first+i); 0 = no glyph */
} lv_eve5_font_block_run_t;

/** Parsed metric block for a font residing at a known RAM_G address. */
typedef struct {
    uint8_t format;             /**< 0 = legacy, 1 = extended format 1, 2 = extended format 2 */
    uint16_t pixel_height;      /**< Cell height in pixels (`pixel_height` field from the binary) */
    uint8_t pixel_width_max;
    uint16_t baseline;          /**< Baseline from top of cell, in pixels. 0 if the binary
                                 *   format doesn't carry the value (legacy / ext1). For ext2
                                 *   it's the value at offset +42 of the font block. */
    uint16_t midline;           /**< Midline (x-height) from top of cell, in pixels. ext2 only;
                                 *   0 for legacy / ext1. */
    uint8_t num_runs;
    lv_eve5_font_block_run_t * runs;
} lv_eve5_font_block_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Parse a font metric block at @p font_addr.
 *
 * Auto-detects legacy / ext1 / ext2 from the first 32-bit word read from
 * RAM_G. On success populates @p out — caller owns the allocated `runs`
 * array and must release it with lv_eve5_font_block_free.
 *
 * @param phost           HAL context (booted)
 * @param font_addr       RAM_G start of the font block
 * @param out             Receives the parsed block
 * @param out_base_line   Receives the font baseline in pixels. Legacy / ext1
 *                        formats don't store it; the loader writes 0 there.
 * @return                true on success
 */
bool lv_eve5_font_block_load(EVE_HalContext * phost, uint32_t font_addr,
                             lv_eve5_font_block_t * out, int32_t * out_base_line);

/** Release the runs array; sets the block to empty. Safe on a zeroed block. */
void lv_eve5_font_block_free(lv_eve5_font_block_t * block);

/** Advance width for @p codepoint, or 0 if the codepoint isn't in the font. */
uint8_t lv_eve5_font_block_width(const lv_eve5_font_block_t * block, uint32_t codepoint);

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_FONT_BLOCK_H */
