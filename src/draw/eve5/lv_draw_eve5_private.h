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
#include "../lv_draw_label_private.h"
#include "../lv_image_decoder_private.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
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
#define EVE5_FONT_WHOLE_MAX_BYTES       (RAM_G_SIZE > (1024 * 1024) ? (256 * 1024) : (64 * 1024)) /* Max GPU allocation size */
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

/* Render opaque canvas layers into YCBCR render targets (BT820+ only).
 * YCBCR is a 2x2-pixel block format (4 bytes per block: line stride is
 * 2 bytes/pixel, each stored line covers two display rows) — 1 byte per
 * pixel total. Large opaque canvases — e.g. the photo slides
 * lv_demo_high_res preloads — shrink to half of RGB565 and a third of
 * RGB8, at photographic quality. Side effects: the canvas direct-image
 * shortcut is disabled for opaque canvases (an adopted decoder allocation
 * would be RGB/paletted, so their content always re-encodes through the
 * render pipeline; canvases with alpha keep zero-copy adoption), and SW
 * migration/download of a YCBCR canvas is not supported. */
#ifndef LV_DRAW_EVE5_OPAQUE_CANVAS_YCBCR
#define LV_DRAW_EVE5_OPAQUE_CANVAS_YCBCR 0
#endif

/* Pre-BT820 scratch ring for gradient bitmaps. The pre-BT820 GPU allocator
 * (the variant of EVE_GpuAlloc used on FT80X..BT81X) caps live allocation
 * handles at 64; a frame with many gradients trips it. BT820 uses EVE_GpuAlloc5
 * which doesn't have that ceiling. The ring takes one allocator handle up
 * front and serves an unbounded number of frame-local gradient uploads from
 * it; read/write cursors advance with CMD_SWAP so the section being scanned
 * out isn't overwritten while we build the next frame's DL.
 *
 * Sized for ~32 small gradients per frame at ~64 B each (a 16-pixel pre-
 * rendered strip is the common case), with two-frame in-flight headroom. */
#ifndef LV_DRAW_EVE5_RING_SIZE
#define LV_DRAW_EVE5_RING_SIZE 4096
#endif

/*********************
 * DEFINES
 *********************/
#define DRAW_UNIT_ID_EVE5 82

/* Debug logging toggle. Set to 1 to enable verbose task logging. */
#ifndef EVE5_DEBUG_LOG
#define EVE5_DEBUG_LOG 0
#endif

#if EVE5_DEBUG_LOG
#define EVE5_LOG(...) LV_LOG_INFO(__VA_ARGS__)
#else
#define EVE5_LOG(...) do {} while(0)
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
    EVE_GpuHandle gpu_handle;       /**< Whole-font: single GPU allocation. Per-glyph: GA_HANDLE_INVALID */
    uint32_t * glyph_offsets;       /**< Whole-font: per-gid byte offset into gpu_handle. Per-glyph: NULL */
    EVE_GpuHandle * glyph_handles;  /**< Per-glyph: per-gid handle array. Whole-font: NULL */
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
    EVE_GpuHandle handle;
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
    EVE_GpuHandle corner_handle;
    EVE_GpuHandle edge_handle;
} lv_draw_eve5_shadow_slot_t;

/* Pre-BT820 transient-allocation ring (currently used only for gradient bitmaps).
 * Three cursors using absolute (unwrapped) positions: write_abs advances on
 * each allocation, curr_start snapshots write_abs at every CMD_SWAP, and
 * safe_until lags one swap behind curr_start — anything before safe_until is
 * no longer being scanned out and is safe to overwrite. The free byte count
 * is ring_size - (write_abs - safe_until); allocation fails when a request
 * would push that delta past ring_size. base == GA_INVALID on chips that
 * don't use the ring (BT820+). */
typedef struct {
    EVE_GpuHandle handle;
    uint32_t base;
    uint32_t size;
    uint32_t write_abs;
    uint32_t curr_start;
    uint32_t safe_until;
} lv_draw_eve5_ring_t;

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
    EVE_GpuHandle prev_handle;  /**< Previous slice render output (GA_HANDLE_INVALID = none) */
    bool isolated;              /**< Force clear to (0,0,0,0): ignore canvas content and prev_handle */
    /* Format of the prev_handle bitmap when blitted as the new slice's base. When 0,
     * matches the new slice's render-target format. Used for full-mode screen slicing
     * where the prev slice was rendered to an ARGB8 temp but the final tail slice
     * targets SWAPCHAIN_0 in RGB8. */
    uint16_t prev_eve_format;
    uint32_t prev_stride;       /**< Stride of prev_handle in bytes (only used when prev_eve_format != 0) */
} lv_draw_eve5_slice_t;

/* Bitmap handle pool with LRU eviction.
 *
 * Tracks owner identity per handle so callers can detect eviction —
 * handle_check(handle, owner) returns true only if the caller still owns
 * the slot. Allocation: a non-0xFF `preferred` argument force-claims that
 * specific handle (evicting whatever was there); 0xFF picks the LRU tail.
 * Ownership is just pointer equality — pass anything stable per logical
 * binding (e.g., the address of a per-rom-font slot, or an lv_font_t *).
 *
 * Reserved slots are excluded from the LRU list and never returned by
 * alloc: the EVE_CO_SCRATCH_HANDLE (trashed on coprocessor command entry)
 * and any slot index >= the runtime BITMAP_HANDLE_MASK + 1.
 */
typedef struct {
    void * owner;       /**< NULL = unowned. Owner identity tested by handle_check. */
    uint8_t lru_prev;   /**< 0xFF = none (head sentinel) */
    uint8_t lru_next;   /**< 0xFF = none (tail sentinel) */
    bool reserved;      /**< true = excluded from LRU; never returned by alloc */
} lv_draw_eve5_handle_slot_t;

/* Compile-time slot array size. BT820 has 64 bitmap handles, FT81X-class
 * chips have 32. In MULTI builds we always need 64; for single-target
 * non-BT820 builds the high slots get reserved at init (small overhead). */
#if defined(BT_82X_ENABLE) || defined(EVE_MULTI_GRAPHICS_TARGET)
#define LV_DRAW_EVE5_HANDLE_CAP 64
#else
#define LV_DRAW_EVE5_HANDLE_CAP 32
#endif

typedef struct {
    lv_draw_eve5_handle_slot_t slots[LV_DRAW_EVE5_HANDLE_CAP];
    uint8_t lru_head;   /**< MRU end; 0xFF if pool is empty */
    uint8_t lru_tail;   /**< LRU end (next alloc target); 0xFF if pool is empty */
} lv_draw_eve5_handle_pool_t;

/* ROM font cache entry, indexed by (rom_idx - LV_DRAW_EVE5_ROM_FONT_MIN).
 * The slot's address serves as a stable per-rom_idx owner pointer for the
 * handle pool. `handle` records the last-allocated handle so resolve() can
 * fast-path ownership-checked reuse without re-running the alloc.
 *
 * Range macros mirror Esd_BitmapHandle.c: MIN inclusive, CAP/MAX exclusive.
 * CAP is compile-time (sizes the array); MAX(phost) is runtime (== CAP on
 * single-target builds, runtime-checked via EVE_Hal_supportLargeFont in
 * MULTI builds). LARGEFONT covers rom indices 32..34 (FT810+ excluding
 * BT88X). */
typedef struct {
    uint8_t handle;     /**< Last-allocated bitmap handle, or 0xFF if unallocated */
} lv_draw_eve5_rom_font_slot_t;

#define LV_DRAW_EVE5_ROM_FONT_MIN 16UL /* inclusive */
#ifdef EVE_SUPPORT_LARGEFONT
#define LV_DRAW_EVE5_ROM_FONT_CAP 35UL /* exclusive — matches ESD_ROMFONT_CAP */
#else
#define LV_DRAW_EVE5_ROM_FONT_CAP 32UL
#endif
#define LV_DRAW_EVE5_ROM_FONT_NBCAP (LV_DRAW_EVE5_ROM_FONT_CAP - LV_DRAW_EVE5_ROM_FONT_MIN)

/** Runtime exclusive upper bound: 35 if the chip ships large fonts, else 32. */
static inline uint8_t lv_draw_eve5_rom_font_max(EVE_HalContext * phost)
{
    return EVE_Hal_supportLargeFont(phost) ? 35 : 32;
}

/* EVE5 draw unit */
typedef struct {
    lv_draw_unit_t base_unit;
    EVE_HalContext * hal;
    EVE_GpuAlloc * allocator;

    /* Asset caches */
    lv_draw_eve5_font_vram_t * font_list; /**< Head of intrusive list of resident fonts */
#if LV_DRAW_EVE5_SW_FALLBACK
    lv_draw_eve5_sw_cache_t sw_cache;
#endif

    /* Bitmap handle pool: LRU allocator over the chip's bitmap handles, with
     * owner-identity tracking for eviction detection. CO scratch is reserved. */
    lv_draw_eve5_handle_pool_t handle_pool;

    /* ROM font cache. Each slot tracks the last bitmap handle the matching
     * rom_idx claimed from handle_pool. Sized at compile-time CAP; init walks
     * the runtime MAX. */
    lv_draw_eve5_rom_font_slot_t rom_font_slots[LV_DRAW_EVE5_ROM_FONT_NBCAP];

    /* Re-entrancy guard for SW fallback */
    bool rendering_in_progress;

    /* Dispatch-scoped hint: the next vram_alloc_cb is allocating a canvas
     * layer's buffer (draw_buf-backed, parentless, not the screen). Set
     * around lv_draw_layer_alloc_buf in dispatch so the opaque-canvas
     * YCBCR policy can pick the right format on first allocation. */
    bool alloc_canvas_hint;

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

    /* Pre-BT820 transient ring (gradients). Unused on BT820+. */
    lv_draw_eve5_ring_t scratch_ring;
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

static inline void eve5_vram_res_resolve(EVE_GpuAlloc *alloc, const lv_eve5_vram_res_t * vr,
                                         uint32_t * out_addr, uint32_t * out_palette_addr)
{
    uint32_t base = EVE_GpuAlloc_Get(alloc, vr->gpu_handle);
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

/**
 * Returns true if the EVE bitmap format carries a per-pixel alpha channel
 * that may vary across pixels — i.e. sampling the texture as `SRC_ALPHA`
 * gives values other than constant 1. RGB-only formats return false.
 *
 * Formats classed as "has alpha":
 *   - ARGB1555/2/4/6/8, ARGB6, PALETTED, PALETTED4444, PALETTED8,
 *     PALETTEDARGB8, LA1/2/4/8 (any palette/explicit alpha channel)
 *   - L1/L2/L4/L8 (luminance is sampled as alpha by EVE — varies per pixel)
 *
 * Formats classed as "no alpha" (sampled alpha == 1 everywhere):
 *   - RGB332, RGB565, PALETTED565, RGB6, RGB8, YCBCR
 *
 * Specialty formats (TEXT8X8, TEXTVGA, BARGRAPH, GLFORMAT) fall into the
 * default and return true; they don't appear in image-source paths. YCBCR
 * does appear as a source (opaque canvas render targets) and is opaque.
 */
static inline bool lv_draw_eve5_format_has_alpha(uint16_t eve_format)
{
    if(eve_format == RGB332 || eve_format == RGB565 || eve_format == PALETTED565)
        return false;
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
    if(eve_format == RGB8 || eve_format == RGB6 || eve_format == YCBCR)
        return false;
#endif
    return true;
}

static inline void set_palette_if_needed(EVE_HalContext *phost, uint16_t eve_format, uint32_t palette_addr)
{
    /* PALETTEDARGB8 is BT820-only (EVE5). The macro itself is undefined on
     * pre-BT820 single-target builds, hence the compile-time guard; the
     * runtime EVE_GEN check covers multi-target builds running on older
     * hardware where the format identifier is reserved. */
#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
    if(EVE_GEN >= EVE5 && eve_format == PALETTEDARGB8 && palette_addr != GA_INVALID) {
        EVE_CoDl_paletteSource(phost, palette_addr);
    }
#else
    (void)phost;
    (void)eve_format;
    (void)palette_addr;
#endif
}

/** Check if an EVE bitmap format requires GLFORMAT + BITMAP_EXT_FORMAT mode.
 *  Legacy formats (ARGB8, RGB565, L8, etc.) fit in the 5-bit layout field (0-30).
 *  Extended formats (ASTC, etc.) exceed this range and need GLFORMAT (31). */
static inline bool eve5_is_extended_format(uint16_t eve_format)
{
    return eve_format > GLFORMAT;
}

/** Byte size of a render-target surface for a given line stride and
 *  16-aligned height. YCBCR stores 4 bytes per 2x2 pixel quad: the line
 *  stride is 2 bytes per pixel, but each stored line covers two display
 *  rows, so the surface totals 1 byte per pixel. All other formats are
 *  plain stride * height. */
static inline uint32_t eve5_rt_surface_size(uint16_t eve_format, uint32_t stride, uint32_t aligned_h)
{
#if (EVE_SUPPORT_CHIPID >= EVE_BT820) || defined(EVE_MULTI_GRAPHICS_TARGET)
    if(eve_format == YCBCR) return stride * aligned_h / 2;
#endif
    return stride * aligned_h;
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

/**
 * Image-source bitmap layout with luminance-vs-alpha compensation for L# sources.
 *
 * When @p sample_as_luminance is set and @p eve_format is L1 / L2 / L4 / L8, the
 * chip is put in GLFORMAT mode with BITMAP_SWIZZLE(ALPHA, ALPHA, ALPHA, ONE) —
 * the stored value, which EVE delivers in the sample's ALPHA channel (RGB is a
 * constant 255 for the L# formats — reading RED gives 1, not the value), is
 * routed to all three RGB channels with alpha forced to 1. Restores
 * luminance-as-RGB (A=255) semantics on top of EVE's default L# sampling
 * (alpha-only with white RGB). Requires BITMAP_SWIZZLE (BT815+). On older
 * chips the flag is ignored and the source renders with the alpha-as-luminance
 * behavior.
 *
 * LVGL only has L8 as a luminance format (no L1/L2/L4), so the sub-byte branches
 * are reachable only via non-LVGL sources (e.g. an .esdm-described raw L# asset
 * authored as luminance, once the loader propagates that intent). LVGL A1-A8
 * sources stay unflagged and render correctly through the default L# path
 * (alpha-only matches EVE L# sampling).
 *
 * Every other case (extended formats, RGB / ARGB / paletted sources, or
 * sample_as_luminance=false) falls through to eve5_set_bitmap_layout.
 */
static inline void eve5_set_image_bitmap_layout(EVE_HalContext *phost,
                                                uint16_t eve_format,
                                                int32_t stride, int32_t height,
                                                bool sample_as_luminance)
{
#if (EVE_SUPPORT_CHIPID >= EVE_BT815) || defined(EVE_MULTI_GRAPHICS_TARGET)
    if(sample_as_luminance && EVE_CHIPID >= EVE_BT815
       && (eve_format == L8 || eve_format == L4 || eve_format == L2 || eve_format == L1)) {
        EVE_CoDl_bitmapLayout(phost, GLFORMAT, stride, height);
        EVE_CoDl_bitmapExtFormat(phost, eve_format);
        EVE_CoDl_bitmapSwizzle(phost, ALPHA, ALPHA, ALPHA, ONE);
        return;
    }
#else
    (void)sample_as_luminance;
#endif
    eve5_set_bitmap_layout(phost, eve_format, stride, height);
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

/* Bitmap handle pool. Init builds the LRU list across all non-reserved
 * handles. alloc(owner, preferred) takes ownership: a non-0xFF preferred
 * forces that handle (evicting current owner); 0xFF picks the LRU tail.
 * touch() marks a handle MRU without changing ownership. check() returns
 * true only if `owner` still matches — callers use this to detect eviction
 * before assuming their previous handle is still valid. invalidate_all()
 * drops all owner references (e.g. after a coprocessor reset); reserved
 * slots stay reserved. */
void lv_draw_eve5_handle_pool_init(lv_draw_eve5_unit_t * u);
uint8_t lv_draw_eve5_handle_alloc(lv_draw_eve5_unit_t * u, void * owner, uint8_t preferred);
void lv_draw_eve5_handle_touch(lv_draw_eve5_unit_t * u, uint8_t handle);
bool lv_draw_eve5_handle_check(const lv_draw_eve5_unit_t * u, uint8_t handle, const void * owner);
void lv_draw_eve5_handle_invalidate_all(lv_draw_eve5_unit_t * u);

/* ROM font cache. init pre-binds rom fonts to identity handles (rom_idx ==
 * handle) where the rom_idx fits in the bitmap handle range; out-of-range
 * rom indices (32..34 on FT81X-class chips) bind lazily through the LRU
 * pool. resolve() returns a usable handle for a given rom_idx, re-binding
 * via CMD_ROMFONT if the cached handle was evicted. invalidate() drops all
 * cached bindings (call after a coprocessor reset). */
void lv_draw_eve5_rom_font_init(lv_draw_eve5_unit_t * u);
uint8_t lv_draw_eve5_rom_font_resolve(lv_draw_eve5_unit_t * u, uint8_t rom_idx);
void lv_draw_eve5_rom_font_invalidate(lv_draw_eve5_unit_t * u);

/* Asset font binding (BT820 .reloc fonts loaded via CMD_LOADASSET).
 * Shares the bitmap handle pool with ROM fonts; the asset font's dsc
 * pointer is the stable owner identity. Returns 0xFF if @p font isn't an
 * asset font, the font's RAM_G allocation was reclaimed, or the chip
 * doesn't support CMD_SETFONT2. */
uint8_t lv_draw_eve5_asset_font_resolve(lv_draw_eve5_unit_t * u, const lv_font_t * font);

#if LV_DRAW_EVE5_SW_FALLBACK
/* SW cache */
void lv_draw_eve5_sw_cache_init(lv_draw_eve5_unit_t * u);
void lv_draw_eve5_sw_cache_deinit(lv_draw_eve5_unit_t * u);
void lv_draw_eve5_sw_cache_new_frame(lv_draw_eve5_unit_t * u);
bool lv_draw_eve5_sw_cache_lookup(lv_draw_eve5_unit_t * u, lv_draw_task_type_t type,
                                  int32_t w, int32_t h, const void * dsc_data, uint32_t dsc_size,
                                  EVE_GpuHandle *out_handle, uint32_t * out_stride);
void lv_draw_eve5_sw_cache_insert(lv_draw_eve5_unit_t * u, lv_draw_task_type_t type,
                                  int32_t w, int32_t h, const void * dsc_data, uint32_t dsc_size,
                                  EVE_GpuHandle handle, uint32_t eve_stride);
void lv_draw_eve5_sw_cache_drop(lv_draw_eve5_unit_t * u, lv_draw_task_type_t type,
                                int32_t w, int32_t h, const void * dsc_data, uint32_t dsc_size);
#endif

/**********************
 * VRAM & FORMAT API
 **********************/

void lv_draw_eve5_register_vram_callbacks(lv_draw_eve5_unit_t * u);
bool lv_draw_eve5_get_render_target_format(EVE_HalContext *hal, lv_color_format_t lv_cf,
                                           uint16_t * eve_fmt, uint8_t * bpp);
bool lv_draw_eve5_get_eve_format_info(EVE_HalContext *hal, lv_color_format_t src_cf,
                                      uint16_t * eve_format, uint8_t * bits_per_pixel,
                                      bool *needs_conversion);
bool lv_draw_eve5_download_image(lv_draw_eve5_unit_t * u, lv_draw_buf_t * buf, const lv_eve5_vram_res_t * vr);

/* Convert one row of pixel data from LVGL color format to EVE bitmap format.
 * Handles per-pixel conversions (no palette, no separate alpha plane).
 * Returns true if the (lv_cf, eve_fmt) pair is supported. */
bool lv_draw_eve5_convert_row(lv_color_format_t lv_cf, uint16_t eve_fmt,
                              const uint8_t * src_row, uint8_t * dst_row, int32_t w);

/* Scratch ring (pre-BT820 only). init/deinit are no-ops on BT820+ where
 * EVE_GpuAlloc handles short-lived allocations directly. */
void lv_draw_eve5_ring_init(lv_draw_eve5_unit_t * u);
void lv_draw_eve5_ring_deinit(lv_draw_eve5_unit_t * u);
/** Allocate `size` bytes from the ring with `align`-byte alignment.
 *  Returns RAM_G address, or GA_INVALID when the ring is unused or full. */
uint32_t lv_draw_eve5_ring_alloc(lv_draw_eve5_unit_t * u, uint32_t size, uint32_t align);
/** Advance the in-use boundary; called once per CMD_SWAP. */
void lv_draw_eve5_ring_swap(lv_draw_eve5_unit_t * u);

#if LV_DRAW_EVE5_SW_FALLBACK
/**********************
 * TEXTURE API
 **********************/

EVE_GpuHandle lv_draw_eve5_hal_upload_texture(lv_draw_eve5_unit_t * u, const uint8_t * buf_data,
                                              int32_t buf_w, int32_t buf_h, uint32_t * out_stride);
void lv_draw_eve5_hal_draw_texture(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                   uint32_t ram_g_addr, int32_t tex_w, int32_t tex_h,
                                   uint32_t eve_stride, const lv_area_t * draw_area);
bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t * u, EVE_GpuHandle handle);
#endif

/**********************
 * IMAGE LOADING API
 **********************/

bool lv_draw_eve5_resolve_image_source(const void * src, eve5_resolved_image_t * resolved,
                                       lv_draw_unit_t * draw_unit);
void lv_draw_eve5_release_image_source(eve5_resolved_image_t * resolved);
lv_eve5_vram_res_t * lv_draw_eve5_upload_image_to_gpu(lv_draw_eve5_unit_t * u,
                                                      LV_IMAGE_DSC_CONST lv_image_dsc_t * img_dsc);
lv_eve5_vram_res_t * lv_draw_eve5_resolve_to_gpu(lv_draw_eve5_unit_t * u, const void * src);

#if EVE5_HW_IMAGE_DECODE
void lv_draw_eve5_register_image_decoder(lv_draw_eve5_unit_t * unit);
bool lv_draw_eve5_try_load_file_image(lv_draw_eve5_unit_t * u, const void * src,
                                      uint32_t * ram_g_addr, uint16_t * eve_format,
                                      int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                      EVE_GpuHandle *out_handle, uint32_t * out_palette_addr);
/* Load a raw/deflate/asset bitmap described by a ".esdm" metadata sidecar.
 * Dispatches to the EVE SD card (raw coprocessor FS commands) or the host
 * lv_fs by path. Returns false when no usable sidecar is present. */
bool lv_draw_eve5_try_load_esdm_image(lv_draw_eve5_unit_t * u, const void * src,
                                      uint32_t * ram_g_addr, uint16_t * eve_format,
                                      int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                      EVE_GpuHandle *out_handle, uint32_t * out_palette_addr);
/* Load an LVGL `.bin` image to RAM_G. Two paths:
 *  - SD bins whose CF is direct-copy (LVGL body bytes already match the EVE
 *    bitmap format) and whose LVGL stride matches the EVE-computed stride:
 *    the SD FS driver's pre-load already put the whole file in RAM_G, so we
 *    steal that allocation via lv_eve5_sdcard_steal_ramg and aim the bitmap
 *    source at `base + 12 + palette_bytes`. Zero host bounce, no second SD
 *    read. The 12-byte header sits unused at the start of the allocation.
 *  - Everything else (non-SD paths, conversion-required CFs, stride mismatch,
 *    or a failed steal): the body is read into a host buffer and handed to
 *    lv_draw_eve5_upload_image_to_gpu via a synthetic lv_image_dsc_t. That
 *    reuses every per-CF conversion the upload mapping already implements
 *    (RGB565_SWAPPED byte swap, XRGB8888 X→A, RGB565A8 plane split, I1/I2/I4
 *    palette expansion to PALETTEDARGB8, etc.).
 *
 * Bypasses LVGL's bin decoder entirely on the EVE5 path — works regardless
 * of LV_BIN_DECODER_RAM_LOAD. The caller (decoder_open) builds vram_res
 * around the returned gpu_handle, threading @p *out_lv_cf and
 * @p *out_is_premultiplied through. */
bool lv_draw_eve5_try_load_lvgl_bin_image(lv_draw_eve5_unit_t * u, const void * src,
                                          uint32_t * ram_g_addr, uint16_t * eve_format,
                                          int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                          EVE_GpuHandle *out_handle, uint32_t * out_palette_addr,
                                          lv_color_format_t * out_lv_cf,
                                          bool * out_is_premultiplied);
/* Returns true if the LVGL color format code can be loaded by
 * lv_draw_eve5_try_load_lvgl_bin_image on the current chip. Used by
 * decoder_info to claim or decline .bin files before any I/O. */
bool lv_draw_eve5_lvgl_bin_cf_supported(uint8_t lv_cf);
#endif
#if LV_USE_FS_EVE5_SDCARD
bool lv_draw_eve5_try_load_sdcard_image(lv_draw_eve5_unit_t * u, const void * src,
                                        uint32_t * ram_g_addr, uint16_t * eve_format,
                                        int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                        EVE_GpuHandle *out_handle, uint32_t * out_palette_addr);
#endif
#if LV_USE_FS_EVE5_FLASH
bool lv_draw_eve5_try_load_flash_image(lv_draw_eve5_unit_t * u, const void * src,
                                       uint32_t * ram_g_addr, uint16_t * eve_format,
                                       int32_t * eve_stride, int32_t * src_w, int32_t * src_h,
                                       EVE_GpuHandle *out_handle, uint32_t * out_palette_addr);
#endif

/**********************
 * LAYER MANAGEMENT
 **********************/

void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer, bool is_screen,
                                 const lv_draw_eve5_slice_t * slice);
void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer, bool is_screen,
                                   int rendered_count);

/* L8 render-target alpha pass */
EVE_GpuHandle lv_draw_eve5_hal_init_l8_rendertarget(lv_draw_eve5_unit_t * u,
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
EVE_GpuHandle lv_draw_eve5_render_alpha_to_l8(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
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
                                EVE_GpuHandle dst_handle, const lv_draw_task_t * blur_task);

/* Blend mode support (MULTIPLY, SUBTRACTIVE, DIFFERENCE) */
bool lv_draw_eve5_blend_multiply(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                 EVE_GpuHandle dst_handle, EVE_GpuHandle src_handle,
                                 EVE_GpuHandle *out_result);
bool lv_draw_eve5_blend_subtractive(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                    EVE_GpuHandle dst_handle, EVE_GpuHandle src_handle,
                                    EVE_GpuHandle *out_result);
bool lv_draw_eve5_blend_difference(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                   EVE_GpuHandle dst_handle, EVE_GpuHandle src_handle,
                                   EVE_GpuHandle *out_result);

/* Bitmap mask, applied at child layer finish */
void lv_draw_eve5_apply_bitmap_mask(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                    const lv_draw_image_dsc_t * layer_dsc);

#if LV_DRAW_EVE5_SW_FALLBACK
/* SW fallback */
const void * lv_draw_eve5_sw_get_dsc_cache_data(const lv_draw_task_t * t, uint32_t * out_size);
uint8_t * lv_draw_eve5_sw_render_to_buffer(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                           int32_t buf_w, int32_t buf_h);
EVE_GpuHandle lv_draw_eve5_sw_render_cached(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                            int32_t * out_w, int32_t * out_h,
                                            uint32_t * out_stride, bool *out_from_cache);
void lv_draw_eve5_sw_render_task(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t);
#endif

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_PRIVATE_H */
