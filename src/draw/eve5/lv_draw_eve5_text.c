/**
 * @file lv_draw_eve5_text.c
 *
 * EVE5 (BT820) Text Drawing
 *
 * Handles LABEL and LETTER tasks:
 * - fmt_txt fonts: direct bitmap upload to GPU, L1/L2/L4/L8 formats
 * - Generic fonts: on-demand A8 glyph rendering via get_glyph_bitmap callback
 * - Image glyphs: emoji and other full-color inline images
 * - ROM font fallback: EVE built-in fonts for unsupported formats
 *
 * Supports rotation, scale, and skew transforms on individual glyphs.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_label.h"

/**********************
 * STATIC VARIABLES
 **********************/

/* Glyph callback context (LVGL is single-threaded) */
static lv_draw_eve5_unit_t * s_current_unit = NULL;
static lv_layer_t * s_current_layer = NULL;
static const lv_draw_letter_dsc_t * s_current_letter_dsc = NULL;

static lv_draw_eve5_unit_t * s_alpha_unit = NULL;
static lv_layer_t * s_alpha_layer = NULL;
static const lv_draw_letter_dsc_t * s_alpha_letter_dsc = NULL;

/**********************
 * STATIC PROTOTYPES
 **********************/

static bool glyph_bitmap_to_ramg_aligned(lv_draw_eve5_unit_t * u, uint32_t addr,
                                         const uint8_t * src, uint32_t width,
                                         uint32_t height, uint32_t eve_stride,
                                         uint8_t src_stride_align, uint8_t bpp);
static uint32_t compute_glyph_count(const lv_font_fmt_txt_dsc_t * font_dsc);
static bool upload_whole_font(lv_draw_eve5_unit_t * u, const lv_font_fmt_txt_dsc_t * font_dsc,
                              lv_draw_eve5_font_vram_t * fv);
static bool upload_single_glyph(lv_draw_eve5_unit_t * u, const lv_font_fmt_txt_dsc_t * font_dsc,
                                lv_draw_eve5_font_vram_t * fv, uint32_t gid);
static void emit_glyph_vertex(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                              lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc,
                              const lv_draw_letter_dsc_t * letter_dsc,
                              uint16_t g_w, uint16_t g_h,
                              int32_t x, int32_t y);
static void draw_glyph_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc,
                          lv_draw_fill_dsc_t * fill_dsc, const lv_area_t * fill_area);
static void alpha_glyph_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc,
                           lv_draw_fill_dsc_t * fill_dsc, const lv_area_t * fill_area);

/**********************
 * HELPERS
 **********************/

static uint32_t bpp_to_eve_format(uint8_t bpp)
{
    switch(bpp) {
        case 1:
            return L1;
        case 2:
            return L2;
        case 4:
            return L4;
        case 8:
            return L8;
        default:
            return L4;
    }
}

static bool is_bpp_supported(uint8_t bpp)
{
    return bpp == 1 || bpp == 2 || bpp == 4 || bpp == 8;
}

/**
 * Upload glyph bitmap to RAM_G with EVE-aligned stride.
 * Handles stride differences between LVGL font formats and EVE requirements.
 */
static bool glyph_bitmap_to_ramg_aligned(lv_draw_eve5_unit_t * u, uint32_t addr,
                                         const uint8_t * src, uint32_t width,
                                         uint32_t height, uint32_t eve_stride,
                                         uint8_t src_stride_align, uint8_t bpp)
{
    uint32_t natural_stride = (width * bpp + 7) / 8;

    uint8_t * row_buf = lv_malloc(eve_stride);
    if(!row_buf) {
        LV_LOG_ERROR("EVE5: Failed to allocate glyph row buffer");
        return false;
    }

    bool simple_copy = (bpp == 8)
                       || (src_stride_align == 1)
                       || (src_stride_align == 0 && (width * bpp) % 8 == 0);

    if(simple_copy) {
        for(uint32_t y = 0; y < height; y++) {
            lv_memzero(row_buf, eve_stride);
            lv_memcpy(row_buf, src + y * natural_stride, natural_stride);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
        return true;
    }

    if(src_stride_align > 0) {
        uint32_t src_stride = ALIGN_UP(natural_stride, src_stride_align);
        for(uint32_t y = 0; y < height; y++) {
            lv_memzero(row_buf, eve_stride);
            lv_memcpy(row_buf, src + y * src_stride, natural_stride);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
        return true;
    }

    /* 4bpp with odd width: nibbles pack continuously across rows */
    if(bpp == 4) {
        uint32_t src_i = 0;
        uint8_t key = 0;

        for(uint32_t y = 0; y < height; y++) {
            lv_memzero(row_buf, eve_stride);

            uint32_t row_i;
            for(row_i = 0; row_i < (width / 2); ++row_i) {
                uint8_t n1, n2;
                if(key == 0) {
                    n1 = GET_NIBBLE_HI(src[src_i]);
                    n2 = GET_NIBBLE_LO(src[src_i]);
                }
                else {
                    n1 = GET_NIBBLE_LO(src[src_i - 1]);
                    n2 = GET_NIBBLE_HI(src[src_i]);
                }
                row_buf[row_i] = (n1 << 4) | n2;
                src_i++;
            }

            if(width % 2 != 0) {
                row_buf[row_i] = (key == 0) ?
                                 (GET_NIBBLE_HI(src[src_i]) << 4) :
                                 (GET_NIBBLE_LO(src[src_i - 1]) << 4);
            }

            key = (key == 0) ? 1 : 0;
            src_i += (key == 1) ? 1 : 0;

            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }

        lv_free(row_buf);
        return true;
    }

    /* 1bpp/2bpp fallback */
    for(uint32_t y = 0; y < height; y++) {
        lv_memzero(row_buf, eve_stride);
        lv_memcpy(row_buf, src + y * natural_stride, natural_stride);
        EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
    }

    lv_free(row_buf);
    return true;
}

/**********************
 * FONT VRAM MANAGEMENT
 **********************/

/**
 * Compute glyph count from cmap tables (max_gid + 1).
 * Sparse cmaps use list_length to avoid over-allocation for CJK ranges.
 */
static uint32_t compute_glyph_count(const lv_font_fmt_txt_dsc_t * font_dsc)
{
    uint32_t max_gid = 0;
    for(uint16_t i = 0; i < font_dsc->cmap_num; i++) {
        const lv_font_fmt_txt_cmap_t * cmap = &font_dsc->cmaps[i];
        uint32_t end_gid;
        if(cmap->type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY ||
           cmap->type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
            end_gid = cmap->glyph_id_start + cmap->list_length;
        }
        else {
            end_gid = cmap->glyph_id_start + cmap->range_length;
        }
        if(end_gid > max_gid) max_gid = end_gid;
    }
    return max_gid + 1;
}

/**
 * Upload entire font as a single GPU allocation with offset table.
 */
static bool upload_whole_font(lv_draw_eve5_unit_t * u, const lv_font_fmt_txt_dsc_t * font_dsc,
                              lv_draw_eve5_font_vram_t * fv)
{
    uint8_t bpp = (uint8_t)font_dsc->bpp;
    uint32_t glyph_count = fv->glyph_count;

    uint32_t total_size = 0;
    for(uint32_t gid = 0; gid < glyph_count; gid++) {
        const lv_font_fmt_txt_glyph_dsc_t * g = &font_dsc->glyph_dsc[gid];
        if(g->box_w == 0 || g->box_h == 0) continue;
        uint32_t stride = ALIGN_UP((g->box_w * bpp + 7) / 8, 4);
        total_size += stride * g->box_h;
    }

    if(total_size == 0) return false;

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, total_size, GA_ALIGN_4);
    uint32_t base_addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(base_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Failed to allocate whole font (%"PRIu32" bytes, %"PRIu32" glyphs)",
                    total_size, glyph_count);
        return false;
    }

    uint32_t * offsets = lv_malloc(glyph_count * sizeof(uint32_t));
    if(!offsets) {
        Esd_GpuAlloc_Free(u->allocator, handle);
        return false;
    }

    uint32_t offset = 0;
    for(uint32_t gid = 0; gid < glyph_count; gid++) {
        const lv_font_fmt_txt_glyph_dsc_t * g = &font_dsc->glyph_dsc[gid];
        if(g->box_w == 0 || g->box_h == 0) {
            offsets[gid] = GA_INVALID;
            continue;
        }

        uint32_t stride = ALIGN_UP((g->box_w * bpp + 7) / 8, 4);
        offsets[gid] = offset;

        const uint8_t * glyph_bitmap = &font_dsc->glyph_bitmap[g->bitmap_index];
        glyph_bitmap_to_ramg_aligned(u, base_addr + offset,
                                     glyph_bitmap, g->box_w, g->box_h,
                                     stride, font_dsc->stride, bpp);
        offset += stride * g->box_h;
    }

    fv->gpu_handle = handle;
    fv->glyph_offsets = offsets;
    fv->whole_font = true;

    LV_LOG_INFO("EVE5: Uploaded whole font (%"PRIu32" glyphs, %"PRIu32" bytes, %"PRIu8" bpp)",
                glyph_count, total_size, bpp);
    return true;
}

/**
 * Upload a single glyph on demand (per-glyph mode).
 */
static bool upload_single_glyph(lv_draw_eve5_unit_t * u, const lv_font_fmt_txt_dsc_t * font_dsc,
                                lv_draw_eve5_font_vram_t * fv, uint32_t gid)
{
    if(gid >= fv->glyph_count) return false;

    const lv_font_fmt_txt_glyph_dsc_t * g = &font_dsc->glyph_dsc[gid];
    if(g->box_w == 0 || g->box_h == 0) return false;

    uint8_t bpp = fv->bpp;
    uint32_t stride = ALIGN_UP((g->box_w * bpp + 7) / 8, 4);
    uint32_t glyph_size = stride * g->box_h;

    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, glyph_size, GA_ALIGN_4);
    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) return false;

    const uint8_t * glyph_bitmap = &font_dsc->glyph_bitmap[g->bitmap_index];
    if(!glyph_bitmap_to_ramg_aligned(u, addr, glyph_bitmap, g->box_w, g->box_h,
                                     stride, font_dsc->stride, bpp)) {
        Esd_GpuAlloc_Free(u->allocator, handle);
        return false;
    }

    fv->glyph_handles[gid] = handle;
    return true;
}

static void font_list_remove(lv_draw_eve5_unit_t * u, lv_draw_eve5_font_vram_t * fv)
{
    if(fv->prev) fv->prev->next = fv->next;
    else u->font_list = fv->next;
    if(fv->next) fv->next->prev = fv->prev;
    fv->prev = fv->next = NULL;
}

static void font_list_insert(lv_draw_eve5_unit_t * u, lv_draw_eve5_font_vram_t * fv)
{
    fv->prev = NULL;
    fv->next = u->font_list;
    if(u->font_list) u->font_list->prev = fv;
    u->font_list = fv;
}

void lv_draw_eve5_vram_font_free(lv_draw_unit_t * draw_unit, void * font_ptr)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_font_t * font = (lv_font_t *)font_ptr;

    lv_draw_eve5_font_vram_t * fv = eve5_get_font_vram(font);
    if(fv == NULL) return;

    font_list_remove(u, fv);

    /* Early exit: gpu_handle is GA_HANDLE_INVALID when never assigned (sentinel) */
    if(fv->whole_font && fv->gpu_handle.Id != GA_HANDLE_INVALID.Id) {
        Esd_GpuAlloc_Free(u->allocator, fv->gpu_handle);
    }
    if(fv->glyph_handles) {
        for(uint32_t i = 0; i < fv->glyph_count; i++) {
            /* Early exit: glyph_handles[i] is GA_HANDLE_INVALID when never loaded (sentinel) */
            if(fv->glyph_handles[i].Id != GA_HANDLE_INVALID.Id) {
                Esd_GpuAlloc_Free(u->allocator, fv->glyph_handles[i]);
            }
        }
    }

    if(fv->glyph_offsets) lv_free(fv->glyph_offsets);
    if(fv->glyph_handles) lv_free(fv->glyph_handles);
    lv_free(fv);
    ((lv_font_dsc_base_t *)font->dsc)->vram_res = NULL;
}

/**
 * Ensure font VRAM residency. Creates or validates GPU state.
 *
 * Small plain fonts use whole-font mode (single allocation, offset table).
 * Large or compressed fonts use per-glyph mode (demand-loaded handles).
 */
lv_draw_eve5_font_vram_t * lv_draw_eve5_font_ensure(lv_draw_eve5_unit_t * u,
                                                    const lv_font_t * font)
{
    const lv_font_fmt_txt_dsc_t * font_dsc = (const lv_font_fmt_txt_dsc_t *)font->dsc;

    lv_draw_eve5_font_vram_t * fv = eve5_get_font_vram(font);
    if(fv != NULL) {
        if(fv->base.unit != (lv_draw_unit_t *)u) {
            lv_draw_unit_t * old_unit = fv->base.unit;
            if(old_unit && old_unit->vram_font_free_cb) {
                old_unit->vram_font_free_cb(old_unit, font);
            }
            else {
                if(fv->glyph_offsets) lv_free(fv->glyph_offsets);
                if(fv->glyph_handles) lv_free(fv->glyph_handles);
                lv_free(fv);
                ((lv_font_dsc_base_t *)font->dsc)->vram_res = NULL;
            }
            fv = NULL;
        }
        else if(fv->whole_font) {
            if(Esd_GpuAlloc_Get(u->allocator, fv->gpu_handle) != GA_INVALID) {
                return fv;
            }
            font_list_remove(u, fv);
            if(fv->glyph_offsets) lv_free(fv->glyph_offsets);
            fv->glyph_offsets = NULL;
            fv->gpu_handle = GA_HANDLE_INVALID;
        }
        else {
            return fv;
        }
    }

    if(fv == NULL) {
        fv = lv_malloc_zeroed(sizeof(lv_draw_eve5_font_vram_t));
        if(!fv) return NULL;

        fv->base.unit = (lv_draw_unit_t *)u;
        fv->bpp = (uint8_t)font_dsc->bpp;
        fv->glyph_count = compute_glyph_count(font_dsc);
        fv->gpu_handle = GA_HANDLE_INVALID;

        ((lv_font_dsc_base_t *)font->dsc)->vram_res = (struct _lv_draw_buf_vram_res_t *)fv;
    }

    /* Whole-font for small plain fonts, per-glyph otherwise */
    bool use_whole = (fv->glyph_count <= EVE5_FONT_WHOLE_THRESHOLD)
                     && (font_dsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN);

    if(use_whole) {
        if(font_dsc->glyph_dsc[fv->glyph_count - 1].bitmap_index > EVE5_FONT_WHOLE_MAX_BYTES) {
            use_whole = false;
        }
    }

    if(use_whole) {
        uint32_t total_size = 0;
        uint8_t bpp = fv->bpp;
        for(uint32_t gid = 0; gid < fv->glyph_count; gid++) {
            const lv_font_fmt_txt_glyph_dsc_t * g = &font_dsc->glyph_dsc[gid];
            if(g->box_w == 0 || g->box_h == 0) continue;
            total_size += ALIGN_UP((g->box_w * bpp + 7) / 8, 4) * g->box_h;
        }
        if(total_size > EVE5_FONT_WHOLE_MAX_BYTES) {
            LV_LOG_INFO("EVE5: Font too large for whole-font (%"PRIu32" bytes > %d), using per-glyph",
                        total_size, EVE5_FONT_WHOLE_MAX_BYTES);
            use_whole = false;
        }
    }

    if(use_whole) {
        if(upload_whole_font(u, font_dsc, fv)) {
            font_list_insert(u, fv);
            return fv;
        }
        LV_LOG_INFO("EVE5: Whole-font upload failed, falling back to per-glyph");
    }

    fv->whole_font = false;
    fv->glyph_handles = lv_malloc(fv->glyph_count * sizeof(Esd_GpuHandle));
    if(!fv->glyph_handles) {
        lv_free(fv);
        ((lv_font_dsc_base_t *)font->dsc)->vram_res = NULL;
        return NULL;
    }
    for(uint32_t i = 0; i < fv->glyph_count; i++) {
        fv->glyph_handles[i] = GA_HANDLE_INVALID;
    }

    font_list_insert(u, fv);
    return fv;
}

/**
 * Get GPU address for a glyph. Uploads on demand for per-glyph mode.
 */
uint32_t lv_draw_eve5_font_get_glyph(lv_draw_eve5_unit_t * u,
                                     lv_draw_eve5_font_vram_t * fv,
                                     const lv_font_t * font,
                                     uint32_t gid, uint16_t * out_stride)
{
    const lv_font_fmt_txt_dsc_t * font_dsc = (const lv_font_fmt_txt_dsc_t *)font->dsc;

    if(gid >= fv->glyph_count) return GA_INVALID;

    const lv_font_fmt_txt_glyph_dsc_t * g = &font_dsc->glyph_dsc[gid];
    if(g->box_w == 0 || g->box_h == 0) return GA_INVALID;

    *out_stride = ALIGN_UP((g->box_w * fv->bpp + 7) / 8, 4);

    if(fv->whole_font) {
        if(fv->glyph_offsets[gid] == GA_INVALID) return GA_INVALID;
        uint32_t base = Esd_GpuAlloc_Get(u->allocator, fv->gpu_handle);
        if(base == GA_INVALID) return GA_INVALID;
        return base + fv->glyph_offsets[gid];
    }

    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, fv->glyph_handles[gid]);
    if(addr != GA_INVALID) return addr;

    if(!upload_single_glyph(u, font_dsc, fv, gid)) return GA_INVALID;

    return Esd_GpuAlloc_Get(u->allocator, fv->glyph_handles[gid]);
}

/**********************
 * GENERIC FONT SUPPORT
 **********************/

/**
 * Ensure generic (non-fmt_txt) font VRAM residency.
 * Generic fonts render glyphs to A8 on demand via get_glyph_bitmap.
 */
static lv_draw_eve5_font_vram_t * font_ensure_generic(lv_draw_eve5_unit_t * u, const lv_font_t * font)
{
    lv_draw_eve5_font_vram_t * fv = eve5_get_font_vram(font);
    if(fv != NULL) {
        if(fv->base.unit != (lv_draw_unit_t *)u) {
            lv_draw_unit_t * old_unit = fv->base.unit;
            if(old_unit && old_unit->vram_font_free_cb) {
                old_unit->vram_font_free_cb(old_unit, font);
            }
            else {
                if(fv->glyph_handles) lv_free(fv->glyph_handles);
                lv_free(fv);
                ((lv_font_dsc_base_t *)font->dsc)->vram_res = NULL;
            }
            fv = NULL;
        }
        else {
            return fv;
        }
    }

    fv = lv_malloc_zeroed(sizeof(lv_draw_eve5_font_vram_t));
    if(!fv) return NULL;

    fv->base.unit = (lv_draw_unit_t *)u;
    fv->bpp = 8;
    fv->glyph_count = 0;
    fv->gpu_handle = GA_HANDLE_INVALID;
    fv->whole_font = false;
    fv->glyph_handles = NULL;

    ((lv_font_dsc_base_t *)font->dsc)->vram_res = (struct _lv_draw_buf_vram_res_t *)fv;
    font_list_insert(u, fv);
    return fv;
}

/**
 * Grow glyph handle array to accommodate gid.
 */
static bool font_generic_grow(lv_draw_eve5_font_vram_t * fv, uint32_t gid)
{
    uint32_t needed = gid + 1;
    if(needed <= fv->glyph_count) return true;

    uint32_t new_count = needed < 64 ? 64 : ALIGN_UP(needed, 64);
    Esd_GpuHandle *new_handles = lv_realloc(fv->glyph_handles, new_count * sizeof(Esd_GpuHandle));
    if(!new_handles) return false;

    for(uint32_t i = fv->glyph_count; i < new_count; i++) {
        new_handles[i] = GA_HANDLE_INVALID;
    }
    fv->glyph_handles = new_handles;
    fv->glyph_count = new_count;
    return true;
}

/**
 * Get or upload a generic font glyph from A8 draw_buf.
 */
static uint32_t font_get_generic_glyph(lv_draw_eve5_unit_t * u,
                                       lv_draw_eve5_font_vram_t * fv,
                                       lv_draw_glyph_dsc_t * glyph_dsc,
                                       uint16_t * out_stride)
{
    uint32_t gid = glyph_dsc->g->gid.index;
    uint16_t g_w = glyph_dsc->g->box_w;
    uint16_t g_h = glyph_dsc->g->box_h;

    if(g_w == 0 || g_h == 0) return GA_INVALID;

    uint32_t eve_stride = ALIGN_UP(g_w, 4);
    *out_stride = (uint16_t)eve_stride;

    if(!font_generic_grow(fv, gid)) return GA_INVALID;

    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, fv->glyph_handles[gid]);
    if(addr != GA_INVALID) return addr;

    const lv_draw_buf_t * glyph_data = lv_font_get_glyph_bitmap(glyph_dsc->g, glyph_dsc->_draw_buf);
    if(glyph_data == NULL || glyph_data->data == NULL) return GA_INVALID;

    uint32_t glyph_size = eve_stride * g_h;
    Esd_GpuHandle handle = Esd_GpuAlloc_Alloc(u->allocator, glyph_size, GA_ALIGN_4);
    addr = Esd_GpuAlloc_Get(u->allocator, handle);
    if(addr == GA_INVALID) return GA_INVALID;

    uint32_t src_stride = glyph_data->header.stride;
    if(src_stride == 0) src_stride = g_w;

    if(eve_stride == g_w) {
        EVE_Hal_wrMem(u->hal, addr, glyph_data->data, eve_stride * g_h);
    }
    else {
        /* Zero-pad each row to EVE stride alignment */
        uint8_t * row_buf = lv_malloc(eve_stride);
        if(row_buf == NULL) {
            Esd_GpuAlloc_Free(u->allocator, handle);
            return GA_INVALID;
        }
        for(int32_t y = 0; y < g_h; y++) {
            lv_memzero(row_buf, eve_stride);
            lv_memcpy(row_buf, glyph_data->data + y * src_stride, g_w);
            EVE_Hal_wrMem(u->hal, addr + y * eve_stride, row_buf, eve_stride);
        }
        lv_free(row_buf);
    }

    fv->glyph_handles[gid] = handle;
    return addr;
}

/**********************
 * GLYPH TRANSFORM + DRAW
 **********************/

/**
 * Emit a glyph vertex with optional affine transform.
 */
static void emit_glyph_vertex(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                              lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc,
                              const lv_draw_letter_dsc_t * letter_dsc,
                              uint16_t g_w, uint16_t g_h,
                              int32_t x, int32_t y)
{
    bool has_letter_transform = (letter_dsc != NULL)
                                && (letter_dsc->rotation != 0
                                    || letter_dsc->scale_x != LV_SCALE_NONE
                                    || letter_dsc->scale_y != LV_SCALE_NONE
                                    || letter_dsc->skew_x != 0
                                    || letter_dsc->skew_y != 0);
    bool has_label_rotation = (letter_dsc == NULL)
                              && (glyph_dsc->rotation % 3600 != 0);

    if(has_letter_transform) {
        int32_t draw_vx = t->clip_area.x1 - layer->buf_area.x1;
        int32_t draw_vy = t->clip_area.y1 - layer->buf_area.y1;

        image_skew_t xform;
        if(!compute_image_skew(&xform,
                               letter_dsc->rotation, letter_dsc->scale_x, letter_dsc->scale_y,
                               letter_dsc->skew_x, letter_dsc->skew_y,
                               letter_dsc->pivot.x, letter_dsc->pivot.y,
                               g_w, g_h, x, y, draw_vx, draw_vy))
            return;

        EVE_CoDl_saveContext(u->hal);
        apply_image_skew(u->hal, &xform, NEAREST, 0, 0);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_restoreContext(u->hal);
    }
    else if(has_label_rotation) {
        int32_t draw_vx = t->clip_area.x1 - layer->buf_area.x1;
        int32_t draw_vy = t->clip_area.y1 - layer->buf_area.y1;
        int32_t pivot_x = glyph_dsc->pivot.x;
        int32_t pivot_y = glyph_dsc->g->box_h + glyph_dsc->g->ofs_y;

        image_skew_t xform;
        if(!compute_image_skew(&xform,
                               glyph_dsc->rotation, LV_SCALE_NONE, LV_SCALE_NONE,
                               0, 0,
                               pivot_x, pivot_y,
                               g_w, g_h, x, y, draw_vx, draw_vy))
            return;

        EVE_CoDl_saveContext(u->hal);
        apply_image_skew(u->hal, &xform, NEAREST, 0, 0);
        EVE_CoDl_vertex2f_0(u->hal, draw_vx, draw_vy);
        EVE_CoDl_restoreContext(u->hal);
    }
    else {
        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, g_w, g_h);
        EVE_CoDl_vertex2f_0(u->hal, x, y);
    }
}

/**********************
 * GLYPH CALLBACKS
 **********************/

static void draw_glyph_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc,
                          lv_draw_fill_dsc_t * fill_dsc, const lv_area_t * fill_area)
{
    lv_draw_eve5_unit_t * u = s_current_unit;
    lv_layer_t * layer = s_current_layer;

    if(u == NULL || layer == NULL) return;

    /* Underline/strikethrough */
    if(fill_dsc && fill_area) {
        int32_t x1 = fill_area->x1 - layer->buf_area.x1;
        int32_t y1 = fill_area->y1 - layer->buf_area.y1;
        int32_t x2 = fill_area->x2 - layer->buf_area.x1;
        int32_t y2 = fill_area->y2 - layer->buf_area.y1;

        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1, y1);
        EVE_CoDl_vertex2f_0(u->hal, x2, y2);
        EVE_CoDl_end(u->hal);
    }

    if(glyph_dsc == NULL) return;

    const lv_font_t * font = glyph_dsc->g->resolved_font;

    if(!font) {
        LV_LOG_WARN("EVE5: Font not resolved");
        return;
    }

    if(glyph_dsc->format == LV_FONT_GLYPH_FORMAT_NONE) return;

    /* Image glyph (e.g., emoji): draw as full-color image */
    if(glyph_dsc->format == LV_FONT_GLYPH_FORMAT_IMAGE) {
        const void * img_src = lv_font_get_glyph_bitmap(glyph_dsc->g, glyph_dsc->_draw_buf);
        if(img_src == NULL) return;

        int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
        int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;
        uint16_t g_w = glyph_dsc->g->box_w;
        uint16_t g_h = glyph_dsc->g->box_h;

        lv_eve5_vram_res_t * vr = lv_draw_eve5_resolve_to_gpu(u, img_src);
        if(vr == NULL) return;

        uint32_t addr, palette_addr;
        eve5_vram_res_resolve(u->allocator, vr, &addr, &palette_addr);
        if(addr == GA_INVALID) return;

        EVE_CoDl_end(u->hal);

        EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
        EVE_CoDl_colorA(u->hal, glyph_dsc->opa);
        EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
        EVE_CoDl_bitmapSource(u->hal, addr);
        set_palette_if_needed(u->hal, vr->eve_format, palette_addr);
        eve5_set_bitmap_layout(u->hal, vr->eve_format, (int32_t)vr->stride, g_h);
        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, g_w, g_h);
        EVE_CoDl_bitmapTransform_identity(u->hal);

        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, x, y);
        EVE_CoDl_end(u->hal);

        EVE_CoDl_begin(u->hal, BITMAPS);
        return;
    }

    if(glyph_dsc->format > LV_FONT_GLYPH_FORMAT_IMAGE) return;

    uint16_t g_w, g_h, g_stride;
    uint32_t ram_g_addr;
    uint32_t eve_format;

    if(font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
        lv_font_fmt_txt_dsc_t * font_dsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
        if(!is_bpp_supported((uint8_t)font_dsc->bpp)) {
            LV_LOG_WARN("EVE5: Unsupported font bpp: %d", font_dsc->bpp);
            return;
        }

        lv_draw_eve5_font_vram_t * fv = lv_draw_eve5_font_ensure(u, font);
        if(!fv) return;

        uint32_t gid = glyph_dsc->g->gid.index;
        const lv_font_fmt_txt_glyph_dsc_t * g_dsc = &font_dsc->glyph_dsc[gid];

        g_w = g_dsc->box_w;
        g_h = g_dsc->box_h;
        ram_g_addr = lv_draw_eve5_font_get_glyph(u, fv, font, gid, &g_stride);
        eve_format = bpp_to_eve_format((uint8_t)font_dsc->bpp);
    }
    else {
        lv_draw_eve5_font_vram_t * fv = font_ensure_generic(u, font);
        if(!fv) return;

        g_w = glyph_dsc->g->box_w;
        g_h = glyph_dsc->g->box_h;
        ram_g_addr = font_get_generic_glyph(u, fv, glyph_dsc, &g_stride);
        eve_format = L8;
    }

    if(ram_g_addr == GA_INVALID) return;

    int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
    int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;

    EVE_CoDl_colorRgb(u->hal, glyph_dsc->color.red, glyph_dsc->color.green, glyph_dsc->color.blue);
    EVE_CoDl_colorA(u->hal, glyph_dsc->opa);

    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, (uint8_t)eve_format, g_stride, g_h);

    emit_glyph_vertex(u, layer, t, glyph_dsc, s_current_letter_dsc, g_w, g_h, x, y);
}

static void alpha_glyph_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_dsc,
                           lv_draw_fill_dsc_t * fill_dsc, const lv_area_t * fill_area)
{
    lv_draw_eve5_unit_t * u = s_alpha_unit;
    lv_layer_t * layer = s_alpha_layer;

    if(u == NULL || layer == NULL) return;

    if(fill_dsc && fill_area) {
        int32_t x1 = fill_area->x1 - layer->buf_area.x1;
        int32_t y1 = fill_area->y1 - layer->buf_area.y1;
        int32_t x2 = fill_area->x2 - layer->buf_area.x1;
        int32_t y2 = fill_area->y2 - layer->buf_area.y1;

        EVE_CoDl_begin(u->hal, RECTS);
        EVE_CoDl_vertex2f_0(u->hal, x1, y1);
        EVE_CoDl_vertex2f_0(u->hal, x2, y2);
        EVE_CoDl_end(u->hal);
    }

    if(glyph_dsc == NULL) return;

    if(glyph_dsc->format == LV_FONT_GLYPH_FORMAT_NONE) return;

    if(glyph_dsc->format == LV_FONT_GLYPH_FORMAT_IMAGE) {
        const void * img_src = lv_font_get_glyph_bitmap(glyph_dsc->g, glyph_dsc->_draw_buf);
        if(img_src == NULL) return;

        int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
        int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;
        uint16_t g_w = glyph_dsc->g->box_w;
        uint16_t g_h = glyph_dsc->g->box_h;

        lv_eve5_vram_res_t * vr = lv_draw_eve5_resolve_to_gpu(u, img_src);
        if(vr == NULL) return;

        uint32_t addr, palette_addr;
        eve5_vram_res_resolve(u->allocator, vr, &addr, &palette_addr);
        if(addr == GA_INVALID) return;

        EVE_CoDl_end(u->hal);

        EVE_CoDl_colorA(u->hal, glyph_dsc->opa);
        EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
        EVE_CoDl_bitmapSource(u->hal, addr);
        set_palette_if_needed(u->hal, vr->eve_format, palette_addr);
        eve5_set_bitmap_layout(u->hal, vr->eve_format, (int32_t)vr->stride, g_h);
        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, g_w, g_h);
        EVE_CoDl_bitmapTransform_identity(u->hal);

        EVE_CoDl_begin(u->hal, BITMAPS);
        EVE_CoDl_vertex2f_0(u->hal, x, y);
        EVE_CoDl_end(u->hal);

        EVE_CoDl_begin(u->hal, BITMAPS);
        return;
    }

    if(glyph_dsc->format > LV_FONT_GLYPH_FORMAT_IMAGE) return;

    const lv_font_t * font = glyph_dsc->g->resolved_font;

    uint16_t g_w, g_h, g_stride;
    uint32_t ram_g_addr;
    uint32_t eve_format;

    if(font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
        lv_font_fmt_txt_dsc_t * font_dsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
        if(!is_bpp_supported((uint8_t)font_dsc->bpp)) return;

        lv_draw_eve5_font_vram_t * fv = lv_draw_eve5_font_ensure(u, font);
        if(!fv) return;

        uint32_t gid = glyph_dsc->g->gid.index;
        const lv_font_fmt_txt_glyph_dsc_t * g_dsc = &font_dsc->glyph_dsc[gid];

        g_w = g_dsc->box_w;
        g_h = g_dsc->box_h;
        ram_g_addr = lv_draw_eve5_font_get_glyph(u, fv, font, gid, &g_stride);
        eve_format = bpp_to_eve_format((uint8_t)font_dsc->bpp);
    }
    else {
        lv_draw_eve5_font_vram_t * fv = font_ensure_generic(u, font);
        if(!fv) return;

        g_w = glyph_dsc->g->box_w;
        g_h = glyph_dsc->g->box_h;
        ram_g_addr = font_get_generic_glyph(u, fv, glyph_dsc, &g_stride);
        eve_format = L8;
    }

    if(ram_g_addr == GA_INVALID) return;

    int32_t x = glyph_dsc->letter_coords->x1 - layer->buf_area.x1;
    int32_t y = glyph_dsc->letter_coords->y1 - layer->buf_area.y1;

    EVE_CoDl_colorA(u->hal, glyph_dsc->opa);

    EVE_CoDl_bitmapSource(u->hal, ram_g_addr);
    EVE_CoDl_bitmapLayout(u->hal, (uint8_t)eve_format, g_stride, g_h);

    emit_glyph_vertex(u, layer, t, glyph_dsc, s_alpha_letter_dsc, g_w, g_h, x, y);
}

/**********************
 * LABEL DRAWING
 **********************/

void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t * u, lv_draw_task_t * t)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    lv_draw_label_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;

    bool use_bitmap_font = false;
    if(dsc->font && dsc->font->get_glyph_bitmap != NULL) {
        if(dsc->font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
            const lv_font_fmt_txt_dsc_t * font_dsc = dsc->font->dsc;
            use_bitmap_font = is_bpp_supported((uint8_t)font_dsc->bpp);
        }
        else {
            use_bitmap_font = true;
        }
    }

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);

    if(use_bitmap_font) {
        /* BT820 L-format decodes as (255,255,255,L) on hardware */
        EVE_CoDl_bitmapTransform_identity(u->hal);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_begin(u->hal, BITMAPS);

        s_current_unit = u;
        s_current_layer = layer;

        lv_draw_label_iterate_characters(t, dsc, &t->area, draw_glyph_cb);

        s_current_unit = NULL;
        s_current_layer = NULL;

        EVE_CoDl_end(u->hal);
    }
    else {
        /* ROM font fallback (ASCII only) */
        int32_t x = t->area.x1 - layer->buf_area.x1;
        int32_t y = t->area.y1 - layer->buf_area.y1;

        uint8_t eve_font = 31;
        if(dsc->font) {
            int32_t font_h = lv_font_get_line_height(dsc->font);
            if(font_h <= 8) eve_font = 16;
            else if(font_h <= 13) eve_font = 26;
            else if(font_h <= 16) eve_font = 27;
            else if(font_h <= 20) eve_font = 29;
            else if(font_h <= 25) eve_font = 30;
            else eve_font = 31;
        }

        EVE_CoCmd_text(u->hal, x, y, eve_font, 0, dsc->text);
    }
}

void lv_draw_eve5_hal_draw_letter(lv_draw_eve5_unit_t * u, lv_draw_task_t * t)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_letter_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(u->hal, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(u->hal, dsc->opa);
    EVE_CoDl_bitmapTransform_identity(u->hal);
    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);

    if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
        EVE_CoDl_blendFunc(u->hal, SRC_ALPHA, ONE);
    }

    EVE_CoDl_begin(u->hal, BITMAPS);

    s_current_unit = u;
    s_current_layer = layer;
    s_current_letter_dsc = dsc;

    lv_draw_glyph_dsc_t glyph_dsc;
    lv_draw_glyph_dsc_init(&glyph_dsc);
    glyph_dsc.opa = dsc->opa;
    glyph_dsc.bg_coords = NULL;
    glyph_dsc.color = dsc->color;
    glyph_dsc.rotation = dsc->rotation;
    glyph_dsc.pivot = dsc->pivot;

    lv_draw_unit_draw_letter(t, &glyph_dsc, &(lv_point_t) {
        .x = t->area.x1, .y = t->area.y1
    }, dsc->font, dsc->unicode, draw_glyph_cb);

    s_current_unit = NULL;
    s_current_layer = NULL;
    s_current_letter_dsc = NULL;

    EVE_CoDl_end(u->hal);

    if(dsc->blend_mode == LV_BLEND_MODE_ADDITIVE) {
        EVE_CoDl_blendFunc_default(u->hal);
    }

    if(glyph_dsc._draw_buf) {
        lv_draw_buf_destroy(glyph_dsc._draw_buf);
        glyph_dsc._draw_buf = NULL;
    }
}

/**********************
 * ALPHA PASS
 **********************/

/**
 * @param alpha_to_rgb false: direct-to-alpha pass, true: L8 render-target pass
 */
void lv_draw_eve5_alpha_draw_label(lv_draw_eve5_unit_t * u, lv_draw_task_t * t, bool alpha_to_rgb)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_label_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->text == NULL || dsc->text[0] == '\0') return;

    bool use_bitmap_font = false;
    if(dsc->font && dsc->font->get_glyph_bitmap != NULL) {
        if(dsc->font->get_glyph_bitmap == lv_font_get_bitmap_fmt_txt) {
            const lv_font_fmt_txt_dsc_t * font_dsc = dsc->font->dsc;
            use_bitmap_font = is_bpp_supported((uint8_t)font_dsc->bpp);
        }
        else {
            use_bitmap_font = true;
        }
    }

    if(!use_bitmap_font) {
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

        int32_t x = t->area.x1 - layer->buf_area.x1;
        int32_t y = t->area.y1 - layer->buf_area.y1;

        uint8_t eve_font = 31;
        if(dsc->font) {
            int32_t font_h = lv_font_get_line_height(dsc->font);
            if(font_h <= 8) eve_font = 16;
            else if(font_h <= 13) eve_font = 26;
            else if(font_h <= 16) eve_font = 27;
            else if(font_h <= 20) eve_font = 29;
            else if(font_h <= 25) eve_font = 30;
            else eve_font = 31;
        }

        if(alpha_to_rgb) EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
        EVE_CoDl_colorA(u->hal, dsc->opa);
        EVE_CoCmd_text(u->hal, x, y, eve_font, 0, dsc->text);
        return;
    }

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    if(alpha_to_rgb) EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    EVE_CoDl_colorA(u->hal, dsc->opa);
    EVE_CoDl_bitmapTransform_identity(u->hal);
    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_begin(u->hal, BITMAPS);

    s_alpha_unit = u;
    s_alpha_layer = layer;

    lv_draw_label_iterate_characters(t, dsc, &t->area, alpha_glyph_cb);

    s_alpha_unit = NULL;
    s_alpha_layer = NULL;

    EVE_CoDl_end(u->hal);
}

/**
 * @param alpha_to_rgb false: direct-to-alpha pass, true: L8 render-target pass
 */
void lv_draw_eve5_alpha_draw_letter(lv_draw_eve5_unit_t * u, lv_draw_task_t * t, bool alpha_to_rgb)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_letter_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    if(alpha_to_rgb) EVE_CoDl_colorRgb(u->hal, 255, 255, 255);
    EVE_CoDl_colorA(u->hal, dsc->opa);
    EVE_CoDl_bitmapTransform_identity(u->hal);
    EVE_CoDl_bitmapHandle(u->hal, u->hal->CoScratchHandle);
    EVE_CoDl_begin(u->hal, BITMAPS);

    s_alpha_unit = u;
    s_alpha_layer = layer;
    s_alpha_letter_dsc = dsc;

    lv_draw_glyph_dsc_t glyph_dsc;
    lv_draw_glyph_dsc_init(&glyph_dsc);
    glyph_dsc.opa = dsc->opa;
    glyph_dsc.bg_coords = NULL;
    glyph_dsc.color = dsc->color;
    glyph_dsc.rotation = dsc->rotation;
    glyph_dsc.pivot = dsc->pivot;

    lv_draw_unit_draw_letter(t, &glyph_dsc, &(lv_point_t) {
        .x = t->area.x1, .y = t->area.y1
    }, dsc->font, dsc->unicode, alpha_glyph_cb);

    s_alpha_unit = NULL;
    s_alpha_layer = NULL;
    s_alpha_letter_dsc = NULL;

    EVE_CoDl_end(u->hal);

    if(glyph_dsc._draw_buf) {
        lv_draw_buf_destroy(glyph_dsc._draw_buf);
        glyph_dsc._draw_buf = NULL;
    }
}

#endif /* LV_USE_DRAW_EVE5 */
