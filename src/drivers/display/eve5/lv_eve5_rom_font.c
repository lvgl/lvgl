/**
 * @file lv_eve5_rom_font.c
 *
 * EVE ROM font wrapper for LVGL.
 *
 * Reads PixelHeight and PixelWidth from the rom font header at create time:
 *   - Legacy (FT80X, no signature): EVE_Gpu_Fonts at offsets 136/140
 *   - Format 1 (BT81X+): EVE_Gpu_FontsExt at offsets 24/28, signature 0x0100AAFF
 *   - Format 2 (BT820 unicode): same offsets, signature 0x0200AAFF
 *
 * Layout uses uniform PixelWidth as adv_w for every glyph. Per-glyph widths
 * could be queried via CMD_TEXTDIM at create time but adds firmware round-trips
 * proportional to the supported codepoint range; left as a future refinement.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_eve5_rom_font.h"

#if LV_USE_EVE5

#include "../../../stdlib/lv_mem.h"
#include "../../../stdlib/lv_string.h"
#include "EVE_HalDefs.h"
#include "EVE_GpuDefs.h"
#include "EVE_CoCmd.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_font_dsc_base_t base;
    uint8_t rom_idx;
    uint16_t pixel_width;
    uint16_t pixel_height;
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
    /* Rom fonts have no host-side bitmap; the EVE5 text renderer issues
     * CMD_TEXT directly using the rom index stored in the font dsc. */
    return NULL;
}

lv_font_t * lv_eve5_rom_font_create(EVE_HalContext * phost, uint8_t rom_idx)
{
    if(phost == NULL) return NULL;

    /* Bounds: indices 16..31 are universal (FT800+), 32..34 require LARGEFONT
     * (FT810+ excluding BT88X). Compile-time CAP filters single-target builds
     * that don't support LARGEFONT at all; runtime supportLargeFont covers
     * MULTI builds where the actual chip may be smaller than the multi-set
     * upper bound. */
    if(rom_idx < 16) return NULL;
#ifdef EVE_SUPPORT_LARGEFONT
    if(rom_idx >= 35) return NULL;
    if(rom_idx >= 32 && !EVE_Hal_supportLargeFont(phost)) return NULL;
#else
    if(rom_idx >= 32) return NULL;
#endif

    /* Read height/width from the rom font header. The signature word at offset 0
     * picks the layout: 0x0100AAFF / 0x0200AAFF use EVE_Gpu_FontsExt (PixelWidth
     * at 24, PixelHeight at 28); legacy fonts have FontWidthInPixels at 136 and
     * FontHeightInPixels at 140. Mirrors the offset selection in
     * Esd_InitRomFontHeight. */
    uint32_t font_addr = EVE_Hal_romFontAddress(phost, rom_idx);
    uint32_t signature = EVE_Hal_rd32(phost, font_addr);
    uint32_t width_addr;
    uint32_t height_addr;
    if(signature == 0x0100AAFF || signature == 0x0200AAFF) {
        width_addr = font_addr + 24;
        height_addr = font_addr + 28;
    }
    else {
        width_addr = font_addr + 136;
        height_addr = font_addr + 140;
    }
    uint32_t pixel_width = EVE_Hal_rd32(phost, width_addr);
    uint32_t pixel_height = EVE_Hal_rd32(phost, height_addr);

    if(pixel_height == 0 || pixel_height > 1024 || pixel_width > 1024) {
        /* Sanity check — unreadable headers (or chips that don't have this rom
         * font baked in) yield garbage. Refuse to build a font around it. */
        return NULL;
    }

    lv_eve5_rom_font_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_eve5_rom_font_dsc_t));
    if(dsc == NULL) return NULL;

    dsc->rom_idx = rom_idx;
    dsc->pixel_width = (uint16_t)pixel_width;
    dsc->pixel_height = (uint16_t)pixel_height;

    lv_font_t * font = lv_malloc_zeroed(sizeof(lv_font_t));
    if(font == NULL) {
        lv_free(dsc);
        return NULL;
    }

    font->get_glyph_dsc = rom_font_get_glyph_dsc;
    font->get_glyph_bitmap = lv_eve5_rom_font_glyph_bitmap_sentinel;
    font->release_glyph = NULL;
    font->line_height = (int32_t)pixel_height;
    font->base_line = 0; /* Firmware places baseline at the bottom of the cell */
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
    if(font->dsc) lv_free((void *)font->dsc);
    lv_free(font);
}

uint8_t lv_eve5_rom_font_get_index(const lv_font_t * font)
{
    if(!lv_eve5_is_rom_font(font)) return 0xFF;
    const lv_eve5_rom_font_dsc_t * dsc = (const lv_eve5_rom_font_dsc_t *)font->dsc;
    return dsc->rom_idx;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool rom_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                   uint32_t letter, uint32_t letter_next)
{
    LV_UNUSED(letter_next);

    const lv_eve5_rom_font_dsc_t * dsc = (const lv_eve5_rom_font_dsc_t *)font->dsc;

    /* Rom fonts 16..31 cover ASCII (0x20..0x7E commonly). Rom fonts 32..34 on
     * BT820 carry full unicode tables — we accept any non-control codepoint
     * and let the firmware decide rendering. Codepoints with no glyph render
     * as nothing or a placeholder; LVGL doesn't get to fall back to another
     * font in that case, but advertising support keeps text wrapping sane. */
    if(letter == 0) return false;
    if(dsc->rom_idx < 32) {
        /* Pre-BT820 rom fonts and small BT820 fonts: ASCII range only */
        if(letter > 0x7E) return false;
    }

    dsc_out->resolved_font = font;
    dsc_out->gid.index = letter;
    dsc_out->format = LV_FONT_GLYPH_FORMAT_A8; /* nominal — renderer ignores it for rom fonts */
    dsc_out->adv_w = dsc->pixel_width;
    dsc_out->box_w = dsc->pixel_width;
    dsc_out->box_h = dsc->pixel_height;
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;
    dsc_out->stride = 0;
    dsc_out->is_placeholder = 0;
    dsc_out->req_raw_bitmap = 0;
    return true;
}

#endif /* LV_USE_EVE5 */
