/**
 * @file lv_draw_eve5_private.h
 *
 * EVE5 (BT820) Draw Unit Private Header
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_DRAW_EVE5_PRIVATE_H
#define LV_DRAW_EVE5_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 * INCLUDES
 *********************/
#include "lv_draw_eve5.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw_private.h"
#include "../lv_draw_image.h"
#include "../lv_draw_label_private.h"
#include "../lv_image_decoder_private.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../../font/fmt_txt/lv_font_fmt_txt.h"
#include "../../drivers/display/eve5/lv_eve5.h"

/*********************
 * CONFIGURATION
 *********************/

/*
 * QA Configuration: Force SW rendering for specific task types.
 * Set to 1 to render via software fallback, 0 for hardware rendering.
 */
#ifndef LV_DRAW_EVE5_SW_FILL
#define LV_DRAW_EVE5_SW_FILL 0
#endif
#ifndef LV_DRAW_EVE5_SW_BORDER
#define LV_DRAW_EVE5_SW_BORDER 0
#endif
#ifndef LV_DRAW_EVE5_SW_LINE
#define LV_DRAW_EVE5_SW_LINE 0
#endif
#ifndef LV_DRAW_EVE5_SW_TRIANGLE
#define LV_DRAW_EVE5_SW_TRIANGLE 0
#endif
#ifndef LV_DRAW_EVE5_SW_LABEL
#define LV_DRAW_EVE5_SW_LABEL 0
#endif
#ifndef LV_DRAW_EVE5_SW_ARC
#define LV_DRAW_EVE5_SW_ARC 0
#endif
#ifndef LV_DRAW_EVE5_SW_BOX_SHADOW
#define LV_DRAW_EVE5_SW_BOX_SHADOW 0
#endif
#ifndef LV_DRAW_EVE5_SW_CANVAS
#define LV_DRAW_EVE5_SW_CANVAS 0
#endif

/*
 * Master switch for the entire SW fallback feature (cache, render-to-buffer, upload).
 * Defaults to enabled when any per-task LV_DRAW_EVE5_SW_* flag is set.
 * Set to 0 to strip all SW fallback code and save memory/executable size.
 */
#ifndef LV_DRAW_EVE5_SW_FALLBACK
#define LV_DRAW_EVE5_SW_FALLBACK (LV_DRAW_EVE5_SW_FILL || LV_DRAW_EVE5_SW_BORDER || \
                                  LV_DRAW_EVE5_SW_LINE || LV_DRAW_EVE5_SW_TRIANGLE || LV_DRAW_EVE5_SW_LABEL || \
                                  LV_DRAW_EVE5_SW_ARC || LV_DRAW_EVE5_SW_BOX_SHADOW || LV_DRAW_EVE5_SW_CANVAS)
#endif

/* Whole-font upload thresholds. Fonts exceeding either limit use per-glyph mode. */
#ifndef EVE5_FONT_WHOLE_THRESHOLD
#define EVE5_FONT_WHOLE_THRESHOLD       1024        /* Max glyph count */
#endif
#ifndef EVE5_FONT_WHOLE_MAX_BYTES
#define EVE5_FONT_WHOLE_MAX_BYTES       (256 * 1024) /* Max GPU allocation size */
#endif

#if LV_DRAW_EVE5_SW_FALLBACK
#ifndef EVE5_SW_CACHE_CAPACITY
#define EVE5_SW_CACHE_CAPACITY      32
#endif
#endif
#define EVE5_SHADOW_TEX_SIZE        64

/* Hardware image decode via CMD_LOADIMAGE (set to 0 to force CPU decoding) */
#ifndef EVE5_HW_IMAGE_DECODE
#define EVE5_HW_IMAGE_DECODE 1
#endif

/* Alpha pass stencil approximation for border masking (experimental) */
#define EVE5_ALPHA_STENCIL_APPROX 1

/* Multi-step stencil AA for rounded gradient fills (not useful with rendertarget alpha) */
#define EVE5_ALPHA_STENCIL_MULTISTEP 0

/* L8 render-target alpha recovery for tasks that trash alpha. See lv_draw_eve5.c header. */
#define EVE5_USE_RENDERTARGET_ALPHA 1

/* Test mode: split each layer's task queue into two slices at a varying point */
#ifndef EVE5_TEST_SLICE_SPLIT
#define EVE5_TEST_SLICE_SPLIT 0
#endif

/* Replace all float usage with integer-only alternatives (for FPU-less targets) */
#ifndef LV_DRAW_EVE5_NO_FLOAT
#define LV_DRAW_EVE5_NO_FLOAT 1
#endif

/* Force RGB8 for opaque layer render targets instead of RGB565 */
#ifndef LV_DRAW_EVE5_OPAQUE_LAYER_RGB8
#define LV_DRAW_EVE5_OPAQUE_LAYER_RGB8 0
#endif

/*********************
 * UTILITY MACROS
 *********************/
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

/* Fixed point helpers */
#define F16(x) ((int32_t)((x) * 65536L))
#define F16_SCALE_DIV_256(x) ((int32_t)((int64_t)(x) * 256))
#define DEGREES(a) ((uint16_t)(((a) % 3600) * 65536L / 3600))  /* Input: 0.1 degree units */

/* Nibble extraction for font bitmaps */
#define GET_NIBBLE_HI(w) ((uint8_t)((w) >> 4))
#define GET_NIBBLE_LO(w) ((uint8_t)((w) & 0x0F))

/**********************
 * TYPEDEFS
 **********************/

/* Font VRAM residency: one entry per font, stores either a single whole-font
 * GPU allocation with per-glyph offset table, or per-glyph GPU handles. */
typedef struct lv_draw_eve5_font_vram_t {
    lv_draw_buf_vram_res_t base;    /**< Must be first member (.unit = owning draw unit) */
    struct lv_draw_eve5_font_vram_t * prev; /**< Intrusive list: previous resident font */
    struct lv_draw_eve5_font_vram_t * next; /**< Intrusive list: next resident font */
    Esd_GpuHandle gpu_handle;       /**< Whole-font: single GPU allocation. Per-glyph: GA_HANDLE_INVALID */
    uint32_t * glyph_offsets;       /**< Whole-font: per-gid byte offset into gpu_handle. Per-glyph: NULL */
    Esd_GpuHandle * glyph_handles;  /**< Per-glyph: per-gid handle array. Whole-font: NULL */
    uint32_t glyph_count;           /**< Array size for glyph_offsets or glyph_handles */
    uint8_t bpp;                    /**< Bits per pixel (1/2/4/8) */
    bool whole_font;                /**< true = single allocation, false = per-glyph handles */
} lv_draw_eve5_font_vram_t;


#if LV_DRAW_EVE5_SW_FALLBACK
/* SW cache entry: caches SW-rendered textures (box shadows, etc.) */
typedef struct {
    void * dsc_data;
    uint32_t dsc_size;
    lv_draw_task_type_t type;
    int32_t w;
    int32_t h;
    Esd_GpuHandle handle;
    uint32_t eve_stride;
    uint32_t last_used_frame;
    bool valid;
} lv_draw_eve5_sw_cache_entry_t;

typedef struct {
    lv_draw_eve5_sw_cache_entry_t entries[EVE5_SW_CACHE_CAPACITY];
    uint32_t frame;
    bool initialized;
} lv_draw_eve5_sw_cache_t;
#endif

/* Shadow texture slot, one per ratio index */
typedef struct {
    Esd_GpuHandle corner_handle;
    Esd_GpuHandle edge_handle;
} lv_draw_eve5_shadow_slot_t;

/* Resolved image source: direct pointer or decoder session */
typedef struct {
    LV_IMAGE_DSC_CONST lv_image_dsc_t * img_dsc;
    lv_image_decoder_dsc_t decoder_dsc;
    bool decoder_open;
} eve5_resolved_image_t;

/* Image transform parameters */
typedef struct {
#if LV_DRAW_EVE5_NO_FLOAT
    int32_t ia, ib, ic, i_d, ie, i_f;  /* 16.16 fixed-point */
#else
    float ia, ib, ic, i_d, ie, i_f;
#endif
    int32_t bmp_w, bmp_h;
#if LV_DRAW_EVE5_NO_FLOAT
    int32_t p;  /* 1 = signed 1.15, 0 = unsigned 8.8 */
#else
    bool p;
    float unity;
#endif
} image_skew_t;

/* Render slice: defines a task range and optional previous slice output */
typedef struct {
    lv_draw_task_t * start;     /**< First task in slice (NULL = layer->draw_task_head) */
    lv_draw_task_t * end;       /**< Exclusive end (NULL = all remaining) */
    Esd_GpuHandle prev_handle;  /**< Previous slice render output (GA_HANDLE_INVALID = none) */
    bool isolated;              /**< Force clear to (0,0,0,0): ignore canvas content and prev_handle */
} lv_draw_eve5_slice_t;

/* EVE5 draw unit */
typedef struct {
    lv_draw_unit_t base_unit;
    EVE_HalContext * hal;
    Esd_GpuAlloc * allocator;

    /* Asset caches */
    lv_draw_eve5_font_vram_t * font_list; /**< Head of intrusive list of resident fonts */
#if LV_DRAW_EVE5_SW_FALLBACK
    lv_draw_eve5_sw_cache_t sw_cache;
#endif

    /* Re-entrancy guard for SW fallback */
    bool rendering_in_progress;

    /* Box shadow texture cache */
    lv_draw_eve5_shadow_slot_t shadow_slots[EVE5_SHADOW_TEX_SIZE];

    /* Per-layer alpha repair tracking (layer-relative coordinates) */
    lv_area_t alpha_opaque_area;
    int32_t alpha_opaque_radius;
    lv_area_t alpha_trashed_area;
    bool has_alpha_opaque;
    bool has_alpha_trashed;
    bool alpha_needs_rendertarget;

    /* Canvas incremental render: original content for alpha compositing */
    uint32_t canvas_orig_addr;
    uint16_t canvas_orig_format;
    uint32_t canvas_orig_stride;
    uint32_t canvas_orig_palette;
    int32_t canvas_orig_w;
    int32_t canvas_orig_h;
} lv_draw_eve5_unit_t;

/**********************
 * INLINE HELPERS
 **********************/

static inline lv_eve5_vram_res_t * eve5_get_vram_res(lv_layer_t * layer)
{
    if(layer == NULL || layer->draw_buf == NULL || layer->draw_buf->vram_res == NULL) return NULL;
    return (lv_eve5_vram_res_t *)layer->draw_buf->vram_res;
}

static inline lv_draw_eve5_font_vram_t * eve5_get_font_vram_from_dsc(lv_font_dsc_base_t * font_dsc)
{
    if(font_dsc == NULL) return NULL;
    if(font_dsc->vram_res == NULL) return NULL;
    return (lv_draw_eve5_font_vram_t *)font_dsc->vram_res;
}

static inline lv_draw_eve5_font_vram_t * eve5_get_font_vram(const lv_font_t * font)
{
    if(font == NULL || font->dsc == NULL) return NULL;
    return eve5_get_font_vram_from_dsc((lv_font_dsc_base_t *)font->dsc);
}

static inline lv_eve5_vram_res_t * eve5_get_image_vram_res(const lv_image_dsc_t * img)
{
    if(img == NULL || img->vram_res == NULL) return NULL;
    return (lv_eve5_vram_res_t *)img->vram_res;
}

static inline void eve5_vram_res_resolve(Esd_GpuAlloc *alloc, const lv_eve5_vram_res_t * vr,
                                         uint32_t * out_addr, uint32_t * out_palette_addr)
{
    uint32_t base = Esd_GpuAlloc_Get(alloc, vr->gpu_handle);
    if(base != GA_INVALID) {
        *out_addr = base + vr->source_offset;
        *out_palette_addr = (vr->palette_offset != GA_INVALID) ? (base + vr->palette_offset) : GA_INVALID;
    }
    else {
        *out_addr = GA_INVALID;
        *out_palette_addr = GA_INVALID;
    }
}

/* Fill+border area matching: LVGL shrinks fill by up to 2px per edge when border is opaque with radius */
static inline bool eve5_fill_border_area_match(const lv_area_t * fill_area, const lv_area_t * border_area)
{
    int32_t dx1 = fill_area->x1 - border_area->x1;
    int32_t dy1 = fill_area->y1 - border_area->y1;
    int32_t dx2 = fill_area->x2 - border_area->x2;
    int32_t dy2 = fill_area->y2 - border_area->y2;
    return (dx1 >= 0 && dx1 <= 2) && (dy1 >= 0 && dy1 <= 2) &&
           (dx2 <= 0 && dx2 >= -2) && (dy2 <= 0 && dy2 >= -2);
}

static inline void set_palette_if_needed(EVE_HalContext *phost, uint16_t eve_format, uint32_t palette_addr)
{
    if(eve_format == PALETTEDARGB8 && palette_addr != GA_INVALID) {
        EVE_CoDl_paletteSource(phost, palette_addr);
    }
}

/** Check if an EVE bitmap format requires GLFORMAT + BITMAP_EXT_FORMAT mode.
 *  Legacy formats (ARGB8, RGB565, L8, etc.) fit in the 5-bit layout field (0-30).
 *  Extended formats (ASTC, etc.) exceed this range and need GLFORMAT (31). */
static inline bool eve5_is_extended_format(uint16_t eve_format)
{
    return eve_format > GLFORMAT;
}

/**
 * Set bitmap layout with correct GLFORMAT handling.
 * Extended formats (ASTC, etc.) use GLFORMAT + BITMAP_EXT_FORMAT + identity swizzle.
 * Legacy formats use the format code directly in the layout byte.
 */
static inline void eve5_set_bitmap_layout(EVE_HalContext *phost, uint16_t eve_format,
                                          int32_t stride, int32_t height)
{
    if(eve5_is_extended_format(eve_format)) {
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, stride, height);
        EVE_CoDl_bitmapExtFormat(phost, eve_format);
        EVE_CoDl_bitmapSwizzle(phost, RED, GREEN, BLUE, ALPHA);
    }
    else {
        EVE_CoDl_bitmapLayout(phost, (uint8_t)eve_format, stride, height);
    }
}

/* Alpha repair tracking: call for fully opaque fills (records largest) */
static inline void lv_draw_eve5_track_alpha_opaque(lv_draw_eve5_unit_t * u,
                                                   int32_t x1, int32_t y1,
                                                   int32_t x2, int32_t y2,
                                                   int32_t radius)
{
    int64_t new_size = (int64_t)(x2 - x1 + 1) * (y2 - y1 + 1);
    if(!u->has_alpha_opaque) {
        u->alpha_opaque_area = (lv_area_t) {
            x1, y1, x2, y2
        };
        u->alpha_opaque_radius = radius;
        u->has_alpha_opaque = true;
    }
    else {
        int64_t old_size = (int64_t)lv_area_get_width(&u->alpha_opaque_area)
                           * lv_area_get_height(&u->alpha_opaque_area);
        if(new_size > old_size) {
            u->alpha_opaque_area = (lv_area_t) {
                x1, y1, x2, y2
            };
            u->alpha_opaque_radius = radius;
        }
    }
}

/* Alpha repair tracking: call for operations that trash alpha as scratch space */
static inline void lv_draw_eve5_track_alpha_trashed(lv_draw_eve5_unit_t * u,
                                                    int32_t x1, int32_t y1,
                                                    int32_t x2, int32_t y2)
{
    if(!u->has_alpha_trashed) {
        u->alpha_trashed_area = (lv_area_t) {
            x1, y1, x2, y2
        };
        u->has_alpha_trashed = true;
    }
    else {
        if(x1 < u->alpha_trashed_area.x1) u->alpha_trashed_area.x1 = x1;
        if(y1 < u->alpha_trashed_area.y1) u->alpha_trashed_area.y1 = y1;
        if(x2 > u->alpha_trashed_area.x2) u->alpha_trashed_area.x2 = x2;
        if(y2 > u->alpha_trashed_area.y2) u->alpha_trashed_area.y2 = y2;
    }
}

/* Slice iteration helpers */
static inline lv_draw_task_t * eve5_slice_first(const lv_draw_eve5_slice_t * slice, lv_layer_t * layer)
{
    return slice->start ? slice->start : layer->draw_task_head;
}

/* Loop condition: while(t && t != slice->end) */

/**********************
 * CACHE API
 **********************/

/* Font VRAM residency — stored on font->vram_res.
 * vram_font_free_cb implementation registered on the draw unit. */
void lv_draw_eve5_vram_font_free(lv_draw_unit_t * draw_unit, lv_font_dsc_base_t * font_dsc);
lv_draw_eve5_font_vram_t * lv_draw_eve5_font_ensure(lv_draw_eve5_unit_t * u,
                                                    const lv_font_t * font);
uint32_t lv_draw_eve5_font_get_glyph(lv_draw_eve5_unit_t * u,
                                     lv_draw_eve5_font_vram_t * fv,
                                     const lv_font_t * font,
                                     uint32_t gid, uint16_t * out_stride);

#if LV_DRAW_EVE5_SW_FALLBACK
/* SW cache */
void lv_draw_eve5_sw_cache_init(lv_draw_eve5_unit_t * u);
void lv_draw_eve5_sw_cache_deinit(lv_draw_eve5_unit_t * u);
void lv_draw_eve5_sw_cache_new_frame(lv_draw_eve5_unit_t * u);
bool lv_draw_eve5_sw_cache_lookup(lv_draw_eve5_unit_t * u, lv_draw_task_type_t type,
                                  int32_t w, int32_t h, const void * dsc_data, uint32_t dsc_size,
                                  Esd_GpuHandle *out_handle, uint32_t * out_stride);
void lv_draw_eve5_sw_cache_insert(lv_draw_eve5_unit_t * u, lv_draw_task_type_t type,
                                  int32_t w, int32_t h, const void * dsc_data, uint32_t dsc_size,
                                  Esd_GpuHandle handle, uint32_t eve_stride);
void lv_draw_eve5_sw_cache_drop(lv_draw_eve5_unit_t * u, lv_draw_task_type_t type,
                                int32_t w, int32_t h, const void * dsc_data, uint32_t dsc_size);
#endif

/**********************
 * VRAM & FORMAT API
 **********************/

void lv_draw_eve5_register_vram_callbacks(lv_draw_eve5_unit_t * u);
bool lv_draw_eve5_get_render_target_format(lv_color_format_t lv_cf, uint16_t * eve_fmt, uint8_t * bpp);
bool lv_draw_eve5_get_eve_format_info(lv_color_format_t src_cf, uint16_t * eve_format,
                                      uint8_t * bits_per_pixel, bool *needs_conversion);
bool lv_draw_eve5_download_image(lv_draw_eve5_unit_t * u, lv_draw_buf_t * buf, const lv_eve5_vram_res_t * vr);

#if LV_DRAW_EVE5_SW_FALLBACK
/**********************
 * TEXTURE API
 **********************/

Esd_GpuHandle lv_draw_eve5_hal_upload_texture(lv_draw_eve5_unit_t * u, const uint8_t * buf_data,
                                              int32_t buf_w, int32_t buf_h, uint32_t * out_stride);
void lv_draw_eve5_hal_draw_texture(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                   uint32_t ram_g_addr, int32_t tex_w, int32_t tex_h,
                                   uint32_t eve_stride, const lv_area_t * draw_area);
bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t * u, Esd_GpuHandle handle);
#endif

/**********************
 * IMAGE LOADING API
 **********************/

bool lv_draw_eve5_resolve_image_source(LV_IMAGE_DSC_CONST void * src, eve5_resolved_image_t * resolved,
                                       lv_draw_unit_t * draw_unit);
void lv_draw_eve5_release_image_source(eve5_resolved_image_t * resolved);
lv_eve5_vram_res_t * lv_draw_eve5_upload_image_to_gpu(lv_draw_eve5_unit_t * u,
                                                      LV_IMAGE_DSC_CONST lv_image_dsc_t * img_dsc);
lv_eve5_vram_res_t * lv_draw_eve5_resolve_to_gpu(lv_draw_eve5_unit_t * u, LV_IMAGE_DSC_CONST void * src);

#if EVE5_HW_IMAGE_DECODE
void lv_draw_eve5_register_image_decoder(lv_draw_eve5_unit_t * unit);
bool lv_draw_eve5_try_load_file_image(lv_draw_eve5_unit_t * u, const void * src,
                                      uint32_t * ram_g_addr, uint16_t * eve_format,
                                      int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                      Esd_GpuHandle *out_handle, uint32_t * out_palette_addr);
#endif
#if LV_USE_FS_EVE5_SDCARD
bool lv_draw_eve5_try_load_sdcard_image(lv_draw_eve5_unit_t * u, const void * src,
                                        uint32_t * ram_g_addr, uint16_t * eve_format,
                                        int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                        Esd_GpuHandle *out_handle, uint32_t * out_palette_addr);
#endif
#if LV_USE_FS_EVE5_FLASH
bool lv_draw_eve5_try_load_flash_image(lv_draw_eve5_unit_t * u, const void * src,
                                       uint32_t * ram_g_addr, uint16_t * eve_format,
                                       int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                       Esd_GpuHandle *out_handle, uint32_t * out_palette_addr);
#endif

/**********************
 * LAYER MANAGEMENT
 **********************/

void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer, bool is_screen,
                                 const lv_draw_eve5_slice_t * slice);
void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer, bool is_screen,
                                   int rendered_count);

/* L8 render-target alpha pass */
Esd_GpuHandle lv_draw_eve5_hal_init_l8_rendertarget(lv_draw_eve5_unit_t * u,
                                                    int32_t aligned_w, int32_t aligned_h,
                                                    int32_t w, int32_t h);
void lv_draw_eve5_hal_finish_l8_rendertarget(lv_draw_eve5_unit_t * u);
void lv_draw_eve5_hal_blit_l8_to_alpha(lv_draw_eve5_unit_t * u, uint32_t l8_addr,
                                       int32_t aligned_w, int32_t aligned_h, int32_t w, int32_t h);

/**********************
 * PRIMITIVE DRAWING
 **********************/

/* Utilities */
void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t * u, const lv_area_t * clip, const lv_area_t * layer_area);
void lv_draw_eve5_clear_stencil(lv_draw_eve5_unit_t * u, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                const lv_area_t * clip, const lv_area_t * layer_area);
void lv_draw_eve5_draw_rect(lv_draw_eve5_unit_t * u, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                            int32_t radius, const lv_area_t * clip_area, const lv_area_t * layer_area);
void draw_circle_subpx(lv_draw_eve5_unit_t * u, int32_t cx2, int32_t cy2, int32_t r16);
void build_triangle_stencil(EVE_HalContext *phost, const lv_point_t p[3]);
bool setup_gradient_bitmap(lv_draw_eve5_unit_t * u, const lv_grad_dsc_t * grad,
                           lv_opa_t opa, int32_t w, int32_t h, bool alpha_to_rgb);
void build_colorkey_stencil(EVE_HalContext *phost, const lv_image_colorkey_t * colorkey,
                            uint16_t eve_format, int32_t eve_stride, int32_t layout_h,
                            int32_t vx, int32_t vy);

/* RGB pass drawing */
void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_fill_with_border(lv_draw_eve5_unit_t * u,
                                            const lv_draw_task_t * fill_task,
                                            const lv_draw_task_t * border_task);
void lv_draw_eve5_hal_draw_line(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb, bool use_hv_opt);
void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t * u, lv_draw_task_t * t);
void lv_draw_eve5_hal_draw_letter(lv_draw_eve5_unit_t * u, lv_draw_task_t * t);
void lv_draw_eve5_hal_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_hal_draw_mask_rect(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_box_shadow_init(lv_draw_eve5_unit_t * u);

/* Image transform */
bool compute_image_skew(image_skew_t * out, int32_t rotation, int32_t scale_x, int32_t scale_y,
                        int32_t skew_x, int32_t skew_y, int32_t pivot_x, int32_t pivot_y,
                        int32_t src_w, int32_t src_h, int32_t img_x, int32_t img_y,
                        int32_t draw_vx, int32_t draw_vy);
void apply_image_skew(EVE_HalContext *phost, const image_skew_t * skew,
                      uint8_t bmp_filter, int32_t tile_w, int32_t tile_h);

/**********************
 * RENDER LOOPS
 **********************/

int lv_draw_eve5_render_tasks(lv_draw_eve5_unit_t * u, lv_layer_t * layer, bool is_screen, bool finish_tasks,
                              const lv_draw_eve5_slice_t * slice);
void lv_draw_eve5_opaque_prepass(lv_draw_eve5_unit_t * u, lv_layer_t * layer, const lv_draw_eve5_slice_t * slice);
void lv_draw_eve5_alpha_pass(lv_draw_eve5_unit_t * u, lv_layer_t * layer, const lv_draw_eve5_slice_t * slice);
bool lv_draw_eve5_is_fully_inside_opaque(lv_draw_eve5_unit_t * u, const lv_area_t * task_area,
                                         const lv_area_t * clip_area, const lv_area_t * layer_area);
bool lv_draw_eve5_line_should_use_hv_opt(const lv_draw_task_t * t, const lv_draw_task_t * prev);

#if EVE5_USE_RENDERTARGET_ALPHA
void lv_draw_eve5_check_alpha_recovery(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                       const lv_draw_eve5_slice_t * slice);
Esd_GpuHandle lv_draw_eve5_render_alpha_to_l8(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                              int32_t aligned_w, int32_t aligned_h,
                                              int32_t w, int32_t h,
                                              const lv_draw_eve5_slice_t * slice);
#endif

/**********************
 * ALPHA PASS DRAWING
 **********************/

void lv_draw_eve5_alpha_draw_fill(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_alpha_draw_fill_with_border(lv_draw_eve5_unit_t * u,
                                              const lv_draw_task_t * fill_task,
                                              const lv_draw_task_t * border_task);
void lv_draw_eve5_alpha_draw_border(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_alpha_draw_line(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool use_hv_opt);
void lv_draw_eve5_alpha_draw_triangle(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_hal_alpha_draw_image(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_alpha_draw_label(lv_draw_eve5_unit_t * u, lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_alpha_draw_letter(lv_draw_eve5_unit_t * u, lv_draw_task_t * t, bool alpha_to_rgb);
void lv_draw_eve5_alpha_draw_arc(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb);

/*
 * Alpha accuracy predicates: returns true when direct-to-alpha is insufficient.
 * Used by check_alpha_recovery() to trigger L8 render-target path.
 * Must stay in sync with corresponding RGB and alpha pass draw functions.
 */
bool lv_draw_eve5_fill_needs_alpha_rendertarget(const lv_draw_task_t * t);
bool lv_draw_eve5_border_needs_alpha_rendertarget(const lv_draw_task_t * t);
bool lv_draw_eve5_line_needs_alpha_rendertarget(const lv_draw_task_t * t);
bool lv_draw_eve5_arc_needs_alpha_rendertarget(const lv_draw_task_t * t);
bool lv_draw_eve5_image_needs_alpha_rendertarget(const lv_draw_task_t * t);

/**********************
 * SPECIAL CASES
 **********************/

/* Canvas optimization */
bool lv_draw_eve5_try_canvas_direct_image(lv_draw_eve5_unit_t * u, lv_layer_t * layer);

/* Gaussian blur (separable 5/7/9-tap pyramid, see lv_draw_eve5_gaussian.c) */
bool lv_draw_eve5_gaussian_blur(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                Esd_GpuHandle dst_handle, const lv_draw_task_t * blur_task);

/* Blend mode support (MULTIPLY, SUBTRACTIVE, DIFFERENCE) */
bool lv_draw_eve5_blend_multiply(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                 Esd_GpuHandle dst_handle, Esd_GpuHandle src_handle,
                                 Esd_GpuHandle *out_result);
bool lv_draw_eve5_blend_subtractive(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                    Esd_GpuHandle dst_handle, Esd_GpuHandle src_handle,
                                    Esd_GpuHandle *out_result);
bool lv_draw_eve5_blend_difference(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                   Esd_GpuHandle dst_handle, Esd_GpuHandle src_handle,
                                   Esd_GpuHandle *out_result);

/* Bitmap mask, applied at child layer finish */
void lv_draw_eve5_apply_bitmap_mask(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                    const lv_draw_image_dsc_t * layer_dsc);

#if LV_DRAW_EVE5_SW_FALLBACK
/* SW fallback */
const void * lv_draw_eve5_sw_get_dsc_cache_data(const lv_draw_task_t * t, uint32_t * out_size);
uint8_t * lv_draw_eve5_sw_render_to_buffer(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                           int32_t buf_w, int32_t buf_h);
Esd_GpuHandle lv_draw_eve5_sw_render_cached(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                            int32_t * out_w, int32_t * out_h,
                                            uint32_t * out_stride, bool *out_from_cache);
void lv_draw_eve5_sw_render_task(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
#endif

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_PRIVATE_H */
