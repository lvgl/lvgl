/**
 * @file lv_eve5_font_block.c
 *
 * Parser for the three on-chip font metric block formats (BT82X
 * Programming Guide §5.5):
 *
 *   - Legacy (148 B fixed): widths[128] at offset 0; pixel_width at +136;
 *     pixel_height at +140. Populated range is 0x20..0x7E.
 *
 *   - Extended Format 1 (signature 0x0100AAFF): 40-byte header followed by
 *     gptr[N/128] and wptr[N/128] tables. Width data is one byte per
 *     codepoint, contiguous, indexed via wptr[cp/128] + (cp%128). Used by
 *     FT81x-era ROM fonts and by user-loaded extended fonts.
 *
 *   - Extended Format 2 (signature 0x0200AAFF): 48-byte header followed by
 *     a page-pointer table optr[N/128]. Each page is 128 character-
 *     descriptor pointers; each CD carries glyph address + width (+ an
 *     optional kerning pair list). Width is the second uint32 of each CD.
 *
 * Header/width parsing reads RAM_G via EVE_Hal_rdMem; parse time is host-
 * side only.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_eve5_font_block.h"

#if LV_USE_EVE5

#include "../../../stdlib/lv_mem.h"
#include "EVE_HalDefs.h"
#include "EVE_GpuDefs.h"

/**********************
 *      DEFINES
 **********************/

#define FONT_BLOCK_SIG_EXT1 0x0100AAFFu
#define FONT_BLOCK_SIG_EXT2 0x0200AAFFu

/* Cap the format-2 CD bulk-read span. ROM-font CDs are contiguous so the
 * span is bounded by ~128 * sizeof(CD); 64 KB is a generous safety net for
 * user-loaded fonts whose CDs may sit far apart. */
#define FONT_BLOCK_CD_BULK_MAX (64u * 1024u)

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool load_legacy(EVE_HalContext * phost, uint32_t font_addr,
                        lv_eve5_font_block_t * out, int32_t * out_base_line);
static bool load_ext1(EVE_HalContext * phost, uint32_t font_addr,
                      lv_eve5_font_block_t * out, int32_t * out_base_line);
static bool load_ext2(EVE_HalContext * phost, uint32_t font_addr,
                      lv_eve5_font_block_t * out, int32_t * out_base_line);

static bool runs_push(lv_eve5_font_block_t * out, uint8_t * cap,
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

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_eve5_font_block_load(EVE_HalContext * phost, uint32_t font_addr,
                             lv_eve5_font_block_t * out, int32_t * out_base_line)
{
    if(phost == NULL || out == NULL || out_base_line == NULL) return false;

    lv_memzero(out, sizeof(*out));
    *out_base_line = 0;

    uint32_t signature = EVE_Hal_rd32(phost, font_addr);

    if(signature == FONT_BLOCK_SIG_EXT2) {
        out->format = 2;
        return load_ext2(phost, font_addr, out, out_base_line);
    }
    if(signature == FONT_BLOCK_SIG_EXT1) {
        out->format = 1;
        return load_ext1(phost, font_addr, out, out_base_line);
    }
    out->format = 0;
    return load_legacy(phost, font_addr, out, out_base_line);
}

void lv_eve5_font_block_free(lv_eve5_font_block_t * block)
{
    if(block == NULL || block->runs == NULL) return;
    for(uint8_t i = 0; i < block->num_runs; i++) {
        lv_free(block->runs[i].widths);
    }
    lv_free(block->runs);
    block->runs = NULL;
    block->num_runs = 0;
}

uint8_t lv_eve5_font_block_width(const lv_eve5_font_block_t * block, uint32_t codepoint)
{
    if(block == NULL || block->runs == NULL) return 0;
    int lo = 0;
    int hi = (int)block->num_runs - 1;
    while(lo <= hi) {
        int m = (lo + hi) >> 1;
        const lv_eve5_font_block_run_t * r = &block->runs[m];
        if(codepoint < r->first) hi = m - 1;
        else if(codepoint - (uint32_t)r->first >= (uint32_t)r->count) lo = m + 1;
        else return r->widths[codepoint - r->first];
    }
    return 0;
}

/**********************
 *   FORMAT LOADERS
 **********************/

/* Legacy 148-byte block: widths[128] at offset 0, pixel_width at +136,
 * pixel_height at +140. Width table is indexed by raw ASCII codepoint;
 * populated range is 0x20..0x7E. */
static bool load_legacy(EVE_HalContext * phost, uint32_t font_addr,
                        lv_eve5_font_block_t * out, int32_t * out_base_line)
{
    uint8_t hdr[148];
    EVE_Hal_rdMem(phost, hdr, font_addr, sizeof(hdr));

    uint32_t pixel_width = rd32le(hdr, 136);
    uint32_t pixel_height = rd32le(hdr, 140);
    if(pixel_height == 0 || pixel_height > 1024 || pixel_width > 1024) return false;

    out->pixel_height = (uint16_t)pixel_height;
    out->pixel_width_max = (uint8_t)(pixel_width > 0xFF ? 0xFF : pixel_width);

    uint16_t first = 0x20;
    uint16_t last = 0x7E;
    uint16_t count = (uint16_t)(last - first + 1);
    uint8_t * widths = lv_malloc(count);
    if(widths == NULL) return false;
    lv_memcpy(widths, &hdr[first], count);

    uint8_t cap = 0;
    if(!runs_push(out, &cap, first, count, widths)) {
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
                      lv_eve5_font_block_t * out, int32_t * out_base_line)
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

    out->pixel_height = (uint16_t)pixel_height;
    out->pixel_width_max = (uint8_t)(pixel_width > 0xFF ? 0xFF : pixel_width);

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
        if(!runs_push(out, &cap, (uint16_t)(p * 128u), 128, widths)) {
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
                      lv_eve5_font_block_t * out, int32_t * out_base_line)
{
    uint8_t hdr[48];
    EVE_Hal_rdMem(phost, hdr, font_addr, sizeof(hdr));

    uint32_t pixel_width = rd32le(hdr, 24);
    uint32_t pixel_height = rd32le(hdr, 28);
    uint32_t num_chars = rd32le(hdr, 36);
    uint16_t midline = rd16le(hdr, 40);
    uint16_t baseline = rd16le(hdr, 42);
    if(pixel_height == 0 || pixel_height > 1024 || pixel_width > 1024) return false;
    if(num_chars == 0) return false;
    uint32_t num_pages = (num_chars + 127u) / 128u;
    if(num_pages == 0 || num_pages > 0x10000u) return false;

    out->pixel_height = (uint16_t)pixel_height;
    out->pixel_width_max = (uint8_t)(pixel_width > 0xFF ? 0xFF : pixel_width);
    out->baseline = baseline;
    out->midline = midline;

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
        if(span <= FONT_BLOCK_CD_BULK_MAX) {
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
        if(!runs_push(out, &cap, (uint16_t)(p * 128u), 128, widths)) {
            lv_free(widths);
            ok = false;
        }
    }

    lv_free(optr);
    if(!ok) return false;

    /* Convert from "baseline from top of cell" (binary's convention) to
     * "pixels below baseline available for descenders" (LVGL's base_line
     * convention: cell_height - baseline_from_top). */
    *out_base_line = baseline > 0 ? (int32_t)pixel_height - (int32_t)baseline : 0;
    if(*out_base_line < 0) *out_base_line = 0;
    return true;
}

/**********************
 *   RUN MANAGEMENT
 **********************/

static bool runs_push(lv_eve5_font_block_t * out, uint8_t * cap,
                      uint16_t first, uint16_t count, uint8_t * widths)
{
    if(out->num_runs == *cap) {
        uint8_t new_cap = *cap ? (uint8_t)(*cap * 2) : (uint8_t)4;
        if(new_cap < out->num_runs + 1) return false; /* overflow */
        lv_eve5_font_block_run_t * resized = lv_realloc(out->runs,
                                                       new_cap * sizeof(lv_eve5_font_block_run_t));
        if(resized == NULL) return false;
        out->runs = resized;
        *cap = new_cap;
    }
    out->runs[out->num_runs].first = first;
    out->runs[out->num_runs].count = count;
    out->runs[out->num_runs].widths = widths;
    out->num_runs++;
    return true;
}

#endif /* LV_USE_EVE5 */
