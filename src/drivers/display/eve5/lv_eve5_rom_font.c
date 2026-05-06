/**
 * @file lv_eve5_rom_font.c
 *
 * EVE ROM font wrapper for LVGL.
 *
 * Parses the rom font's metrics block at create time into a run-based table
 * and exposes per-glyph widths via get_glyph_dsc, so LVGL's text wrapping,
 * alignment, and cursor positioning all see correct metrics. Three on-chip
 * formats are supported (per BT82X Programming Guide §5.5):
 *
 *   - Legacy (rom 16..25 on BT820, FT80X-style elsewhere): a 148-byte block
 *     starting with widths[128] at offset 0. One bulk read covers everything.
 *
 *   - Extended Format 1 (signature 0x0100AAFF): 40-byte header + per-page
 *     glyph and width pointers. Width data is one byte per character,
 *     contiguous, indexed via wptr[cp/128] + (cp%128). Used by FT81X-era
 *     ROM fonts and by user-loaded extended fonts.
 *
 *   - Extended Format 2 (signature 0x0200AAFF, rom 26..34 on BT820): 48-byte
 *     header + page table, each page is 128 character-descriptor (CD)
 *     pointers, each CD carries glyph address + width + optional kerning
 *     pair list. Width is the second uint32 of each CD.
 *
 * BT820 ROM fonts populate ASCII 0x20..0x7E (fonts 17 and 19 are legacy
 * box-drawing fonts at the same ASCII range); extended formats 1/2 carry
 * full unicode for user-loaded fonts and the loaders walk all populated
 * pages.
 *
 * Header/width parsing uses EVE_Hal_rdMem on RAM_G directly, so create
 * time is host-side only.
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
 *      DEFINES
 **********************/

#define ROM_FONT_SIG_EXT1 0x0100AAFFu
#define ROM_FONT_SIG_EXT2 0x0200AAFFu

/* Cap the format-2 CD bulk-read span. ROM-font CDs are contiguous so the
 * span is bounded by ~128*sizeof(CD); 64 KB is a generous safety net for
 * user-loaded fonts whose CDs may sit far apart. */
#define ROM_FONT_CD_BULK_MAX (64u * 1024u)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint16_t first;       /* first codepoint covered by this run */
    uint16_t count;       /* number of widths */
    uint8_t * widths;     /* widths[i] = adv_w for codepoint (first+i); 0 = no glyph */
} lv_eve5_rom_font_run_t;

typedef struct {
    lv_font_dsc_base_t base;
    uint8_t  rom_idx;
    uint8_t  format;            /* 0=legacy, 1=ext1, 2=ext2 */
    uint16_t pixel_height;
    uint8_t  pixel_width_max;
    uint8_t  num_runs;
    lv_eve5_rom_font_run_t * runs;
} lv_eve5_rom_font_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool rom_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out,
                                   uint32_t letter, uint32_t letter_next);

static bool load_legacy(EVE_HalContext * phost, uint32_t font_addr,
                        lv_eve5_rom_font_dsc_t * dsc, int32_t * out_base_line);
static bool load_ext1(EVE_HalContext * phost, uint32_t font_addr,
                      lv_eve5_rom_font_dsc_t * dsc, int32_t * out_base_line);
static bool load_ext2(EVE_HalContext * phost, uint32_t font_addr,
                      lv_eve5_rom_font_dsc_t * dsc, int32_t * out_base_line);

static void free_runs(lv_eve5_rom_font_dsc_t * dsc);
static bool runs_push(lv_eve5_rom_font_dsc_t * dsc, uint8_t * cap,
                      uint16_t first, uint16_t count, uint8_t * widths);

/**********************
 *      HELPERS
 **********************/

static inline uint16_t rd16le(const uint8_t * p, uint32_t o)
{
    return (uint16_t)(p[o] | ((uint16_t)p[o + 1] << 8));
}

static inline uint32_t rd32le(const uint8_t * p, uint32_t o)
{
    return (uint32_t)p[o]
           | ((uint32_t)p[o + 1] << 8)
           | ((uint32_t)p[o + 2] << 16)
           | ((uint32_t)p[o + 3] << 24);
}

/**
 * Bulk-read a uint32 array from EVE memory with explicit LE unpack. On LE
 * hosts the loop folds to a no-op store; on BE hosts it byte-swaps in place.
 */
static void rdMem_u32_le(EVE_HalContext * phost, uint32_t * dst, uint32_t addr, uint32_t count)
{
    EVE_Hal_rdMem(phost, (uint8_t *)dst, addr, count * 4u);
    /* Locals first to keep the byte reads ordered before the uint32 store. */
    for(uint32_t i = 0; i < count; i++) {
        const uint8_t * raw = (const uint8_t *)dst;
        uint8_t b0 = raw[i * 4u + 0];
        uint8_t b1 = raw[i * 4u + 1];
        uint8_t b2 = raw[i * 4u + 2];
        uint8_t b3 = raw[i * 4u + 3];
        dst[i] = (uint32_t)b0
                 | ((uint32_t)b1 << 8)
                 | ((uint32_t)b2 << 16)
                 | ((uint32_t)b3 << 24);
    }
}

static uint8_t glyph_width_lookup(const lv_eve5_rom_font_dsc_t * dsc, uint32_t cp)
{
    int lo = 0;
    int hi = (int)dsc->num_runs - 1;
    while(lo <= hi) {
        int m = (lo + hi) >> 1;
        const lv_eve5_rom_font_run_t * r = &dsc->runs[m];
        if(cp < r->first) hi = m - 1;
        else if(cp - (uint32_t)r->first >= (uint32_t)r->count) lo = m + 1;
        else return r->widths[cp - r->first];
    }
    return 0;
}

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
    uint32_t signature = EVE_Hal_rd32(phost, font_addr);

    lv_eve5_rom_font_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_eve5_rom_font_dsc_t));
    if(dsc == NULL) return NULL;
    dsc->rom_idx = rom_idx;

    int32_t base_line = 0;
    bool ok;
    if(signature == ROM_FONT_SIG_EXT2) {
        dsc->format = 2;
        ok = load_ext2(phost, font_addr, dsc, &base_line);
    }
    else if(signature == ROM_FONT_SIG_EXT1) {
        dsc->format = 1;
        ok = load_ext1(phost, font_addr, dsc, &base_line);
    }
    else {
        dsc->format = 0;
        ok = load_legacy(phost, font_addr, dsc, &base_line);
    }

    if(!ok) {
        free_runs(dsc);
        lv_free(dsc);
        return NULL;
    }

    lv_font_t * font = lv_malloc_zeroed(sizeof(lv_font_t));
    if(font == NULL) {
        free_runs(dsc);
        lv_free(dsc);
        return NULL;
    }

    font->get_glyph_dsc = rom_font_get_glyph_dsc;
    font->get_glyph_bitmap = lv_eve5_rom_font_glyph_bitmap_sentinel;
    font->release_glyph = NULL;
    font->line_height = (int32_t)dsc->pixel_height;
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
        free_runs(dsc);
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
 *   FORMAT LOADERS
 **********************/

/* Legacy 148-byte block: widths[128] at offset 0, pixel_width at 136,
 * pixel_height at 140. Width table is indexed by raw ASCII codepoint;
 * populated range is 0x20..0x7E. */
static bool load_legacy(EVE_HalContext * phost, uint32_t font_addr,
                        lv_eve5_rom_font_dsc_t * dsc, int32_t * out_base_line)
{
    uint8_t hdr[148];
    EVE_Hal_rdMem(phost, hdr, font_addr, sizeof(hdr));

    uint32_t pixel_width = rd32le(hdr, 136);
    uint32_t pixel_height = rd32le(hdr, 140);
    if(pixel_height == 0 || pixel_height > 1024 || pixel_width > 1024) return false;

    dsc->pixel_height = (uint16_t)pixel_height;
    dsc->pixel_width_max = (uint8_t)(pixel_width > 0xFF ? 0xFF : pixel_width);

    uint16_t first = 0x20;
    uint16_t last = 0x7E;
    uint16_t count = (uint16_t)(last - first + 1);
    uint8_t * widths = lv_malloc(count);
    if(widths == NULL) return false;
    lv_memcpy(widths, &hdr[first], count);

    uint8_t cap = 0;
    if(!runs_push(dsc, &cap, first, count, widths)) {
        lv_free(widths);
        return false;
    }

    *out_base_line = 0;
    return true;
}

/* Extended Format 1: header + gptr table + wptr table + width_data array.
 * wptr[p] is an offset (relative to the font block start) of page p's
 * 128-byte width array. */
static bool load_ext1(EVE_HalContext * phost, uint32_t font_addr,
                      lv_eve5_rom_font_dsc_t * dsc, int32_t * out_base_line)
{
    uint8_t hdr[40];
    EVE_Hal_rdMem(phost, hdr, font_addr, sizeof(hdr));

    uint32_t pixel_width = rd32le(hdr, 24);
    uint32_t pixel_height = rd32le(hdr, 28);
    uint32_t num_chars = rd32le(hdr, 36);
    if(pixel_height == 0 || pixel_height > 1024 || pixel_width > 1024) return false;
    if(num_chars == 0 || (num_chars & 0x7Fu) != 0) return false;
    uint32_t num_pages = num_chars / 128u;
    if(num_pages > 0x10000u) return false; /* sanity */

    dsc->pixel_height = (uint16_t)pixel_height;
    dsc->pixel_width_max = (uint8_t)(pixel_width > 0xFF ? 0xFF : pixel_width);

    /* wptr table follows gptr[N/128] which starts at +40. */
    uint32_t wptr_addr = font_addr + 40u + 4u * num_pages;
    uint32_t * wptr = lv_malloc(num_pages * sizeof(uint32_t));
    if(wptr == NULL) return false;
    rdMem_u32_le(phost, wptr, wptr_addr, num_pages);

    uint8_t cap = 0;
    bool ok = true;
    for(uint32_t p = 0; p < num_pages && ok; p++) {
        uint8_t * widths = lv_malloc(128);
        if(widths == NULL) { ok = false; break; }
        EVE_Hal_rdMem(phost, widths, font_addr + wptr[p], 128);

        bool any = false;
        for(int i = 0; i < 128; i++) if(widths[i]) { any = true; break; }
        if(!any) {
            lv_free(widths);
            continue;
        }
        if(!runs_push(dsc, &cap, (uint16_t)(p * 128u), 128, widths)) {
            lv_free(widths);
            ok = false;
        }
    }

    lv_free(wptr);
    if(!ok) return false;

    *out_base_line = 0;
    return true;
}

/* Extended Format 2: 48-byte header + optr page-pointer table.
 * Each page is 128 4-byte CD pointers; each CD has a 4-byte glyph address
 * followed by a 4-byte width (and an optional kerning pair list). */
static bool load_ext2(EVE_HalContext * phost, uint32_t font_addr,
                      lv_eve5_rom_font_dsc_t * dsc, int32_t * out_base_line)
{
    uint8_t hdr[48];
    EVE_Hal_rdMem(phost, hdr, font_addr, sizeof(hdr));

    uint32_t pixel_width = rd32le(hdr, 24);
    uint32_t pixel_height = rd32le(hdr, 28);
    uint32_t num_chars = rd32le(hdr, 36);
    uint16_t baseline_top = rd16le(hdr, 42);
    if(pixel_height == 0 || pixel_height > 1024 || pixel_width > 1024) return false;
    if(num_chars == 0) return false;
    uint32_t num_pages = (num_chars + 127u) / 128u;
    if(num_pages == 0 || num_pages > 0x10000u) return false;

    dsc->pixel_height = (uint16_t)pixel_height;
    dsc->pixel_width_max = (uint8_t)(pixel_width > 0xFF ? 0xFF : pixel_width);

    /* Page pointer table at +44 (BT82X programmer's guide §5.5.3). */
    uint32_t * optr = lv_malloc(num_pages * sizeof(uint32_t));
    if(optr == NULL) return false;
    rdMem_u32_le(phost, optr, font_addr + 44u, num_pages);

    uint8_t cap = 0;
    bool ok = true;
    uint32_t cd_ptrs[128];
    for(uint32_t p = 0; p < num_pages && ok; p++) {
        if(optr[p] == 0) continue;

        rdMem_u32_le(phost, cd_ptrs, optr[p], 128);

        /* Find non-null CD pointer span */
        uint32_t min_cd = 0xFFFFFFFFu;
        uint32_t max_cd = 0;
        for(int i = 0; i < 128; i++) {
            uint32_t cp = cd_ptrs[i];
            if(cp == 0) continue;
            if(cp < min_cd) min_cd = cp;
            if(cp > max_cd) max_cd = cp;
        }
        if(min_cd == 0xFFFFFFFFu) continue; /* page entirely null */

        uint8_t * widths = lv_malloc_zeroed(128);
        if(widths == NULL) { ok = false; break; }

        uint32_t span = max_cd + 8u - min_cd;
        if(span <= ROM_FONT_CD_BULK_MAX) {
            uint8_t * cd_buf = lv_malloc(span);
            if(cd_buf == NULL) {
                lv_free(widths);
                ok = false;
                break;
            }
            EVE_Hal_rdMem(phost, cd_buf, min_cd, span);
            for(int i = 0; i < 128; i++) {
                if(cd_ptrs[i] == 0) continue;
                uint32_t w = rd32le(cd_buf, cd_ptrs[i] - min_cd + 4u);
                widths[i] = (uint8_t)(w > 0xFF ? 0xFF : w);
            }
            lv_free(cd_buf);
        }
        else {
            /* Span too large for one bulk read; fall back to per-CD. */
            for(int i = 0; i < 128; i++) {
                if(cd_ptrs[i] == 0) continue;
                uint32_t w = EVE_Hal_rd32(phost, cd_ptrs[i] + 4u);
                widths[i] = (uint8_t)(w > 0xFF ? 0xFF : w);
            }
        }

        bool any = false;
        for(int i = 0; i < 128; i++) if(widths[i]) { any = true; break; }
        if(!any) {
            lv_free(widths);
            continue;
        }
        if(!runs_push(dsc, &cap, (uint16_t)(p * 128u), 128, widths)) {
            lv_free(widths);
            ok = false;
        }
    }

    lv_free(optr);
    if(!ok) return false;

    LV_UNUSED(baseline_top);
    *out_base_line = 0;
    return true;
}

/**********************
 *   RUN MANAGEMENT
 **********************/

static bool runs_push(lv_eve5_rom_font_dsc_t * dsc, uint8_t * cap,
                      uint16_t first, uint16_t count, uint8_t * widths)
{
    if(dsc->num_runs == *cap) {
        uint8_t new_cap = *cap ? (uint8_t)(*cap * 2) : (uint8_t)4;
        if(new_cap < dsc->num_runs + 1) return false; /* overflow */
        lv_eve5_rom_font_run_t * resized = lv_realloc(dsc->runs,
                                                      new_cap * sizeof(lv_eve5_rom_font_run_t));
        if(resized == NULL) return false;
        dsc->runs = resized;
        *cap = new_cap;
    }
    dsc->runs[dsc->num_runs].first = first;
    dsc->runs[dsc->num_runs].count = count;
    dsc->runs[dsc->num_runs].widths = widths;
    dsc->num_runs++;
    return true;
}

static void free_runs(lv_eve5_rom_font_dsc_t * dsc)
{
    if(dsc->runs == NULL) return;
    for(uint8_t i = 0; i < dsc->num_runs; i++) {
        lv_free(dsc->runs[i].widths);
    }
    lv_free(dsc->runs);
    dsc->runs = NULL;
    dsc->num_runs = 0;
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

    uint8_t w = glyph_width_lookup(dsc, letter);
    if(w == 0) return false;

    dsc_out->resolved_font = font;
    dsc_out->gid.index = letter;
    dsc_out->format = LV_FONT_GLYPH_FORMAT_A8;
    dsc_out->adv_w = w;
    dsc_out->box_w = w;
    dsc_out->box_h = dsc->pixel_height;
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;
    dsc_out->stride = 0;
    dsc_out->is_placeholder = 0;
    dsc_out->req_raw_bitmap = 0;
    return true;
}

#endif /* LV_USE_EVE5 */
