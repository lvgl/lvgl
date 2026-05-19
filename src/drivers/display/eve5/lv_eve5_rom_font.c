/**
 * @file lv_eve5_rom_font.c
 *
 * EVE ROM font wrapper for LVGL.
 *
 * Parses the rom font's metrics block (legacy / extended format 1 / extended
 * format 2 — see lv_eve5_font_block.c) at create time into a run-based
 * per-glyph width table. LVGL sees correct adv_w / box_w from get_glyph_dsc,
 * so text wrapping, alignment, and cursor positioning match bitmap fonts.
 *
 * The actual glyph rendering happens in the EVE5 draw unit's text renderer
 * via CMD_TEXT — the rom font block is already resident at a fixed RAM
 * address baked into the chip, so no host-side bitmap load is needed.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_eve5_rom_font.h"

#if LV_USE_EVE5

#include "lv_eve5_font_block.h"
#include "../../../stdlib/lv_mem.h"
#include "EVE_HalDefs.h"
#include "EVE_GpuDefs.h"
#include "EVE_CoCmd.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_font_dsc_base_t base;
    uint8_t  rom_idx;
    lv_eve5_font_block_t block;
} lv_eve5_rom_font_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool rom_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                   uint32_t letter, uint32_t letter_next);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_IMAGE_DSC_CONST void * lv_eve5_rom_font_glyph_bitmap_sentinel(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    LV_UNUSED(g_dsc);
    LV_UNUSED(draw_buf);
    return NULL;
}

lv_font_t * lv_eve5_rom_font_create(EVE_HalContext * phost, uint8_t rom_idx)
{
    if(phost == NULL) return NULL;

    /* Indices 16..31 are universal (FT800+); 32..34 require LARGEFONT
     * (FT810+ excluding BT88X). Compile-time gate first, runtime gate
     * second for MULTI builds. */
    if(rom_idx < 16) return NULL;
#ifdef EVE_SUPPORT_LARGEFONT
    if(rom_idx >= 35) return NULL;
    if(rom_idx >= 32 && !EVE_Hal_supportLargeFont(phost)) return NULL;
#else
    if(rom_idx >= 32) return NULL;
#endif

    uint32_t font_addr = EVE_Hal_romFontAddress(phost, rom_idx);

    lv_eve5_rom_font_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_eve5_rom_font_dsc_t));
    if(dsc == NULL) return NULL;
    dsc->rom_idx = rom_idx;

    int32_t base_line = 0;
    if(!lv_eve5_font_block_load(phost, font_addr, &dsc->block, &base_line)) {
        lv_eve5_font_block_free(&dsc->block);
        lv_free(dsc);
        return NULL;
    }

    lv_font_t * font = lv_malloc_zeroed(sizeof(lv_font_t));
    if(font == NULL) {
        lv_eve5_font_block_free(&dsc->block);
        lv_free(dsc);
        return NULL;
    }

    font->get_glyph_dsc = rom_font_get_glyph_dsc;
    font->get_glyph_bitmap = lv_eve5_rom_font_glyph_bitmap_sentinel;
    font->release_glyph = NULL;
    font->line_height = (int32_t)dsc->block.pixel_height;
    font->base_line = base_line;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->underline_position = -2;
    font->underline_thickness = 1;
    font->dsc = dsc;
    font->fallback = NULL;
    font->user_data = NULL;

    return font;
}

void lv_eve5_rom_font_destroy(lv_font_t * font)
{
    if(font == NULL) return;
    if(!lv_eve5_is_rom_font(font)) return;
    if(font->dsc) {
        lv_eve5_rom_font_dsc_t * dsc = (lv_eve5_rom_font_dsc_t *)font->dsc;
        lv_eve5_font_block_free(&dsc->block);
        lv_free(dsc);
    }
    lv_free(font);
}

uint8_t lv_eve5_rom_font_get_index(const lv_font_t * font)
{
    if(!lv_eve5_is_rom_font(font)) return 0xFF;
    const lv_eve5_rom_font_dsc_t * dsc = (const lv_eve5_rom_font_dsc_t *)font->dsc;
    return dsc->rom_idx;
}

/**********************
 *   GLYPH METRICS
 **********************/

static bool rom_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                   uint32_t letter, uint32_t letter_next)
{
    LV_UNUSED(letter_next);

    const lv_eve5_rom_font_dsc_t * dsc = (const lv_eve5_rom_font_dsc_t *)font->dsc;

    /* Runs use uint16_t codepoints; control chars below 0x20 are stripped
     * by LVGL's lv_text_is_marker before reaching the renderer. */
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

#endif /* LV_USE_EVE5 */
