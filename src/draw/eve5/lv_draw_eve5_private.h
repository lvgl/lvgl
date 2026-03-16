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
#include "../../font/fmt_txt/lv_font_fmt_txt.h"

/*********************
 * DEFINES
 *********************/
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

/* Fixed point helpers */
#define F16(x) ((int32_t)((x) * 65536L))
#define F16_SCALE_DIV_256(x) ((int32_t)(((x) / 256.0f) * 65536L))
#define DEGREES(a) ((uint16_t)(((a) % 3600) * 65536L / 3600))  /* Input is 0.1 degree units */

/* Nibble extraction for font bitmaps */
#define GET_NIBBLE_HI(w) ((uint8_t)((w) >> 4))
#define GET_NIBBLE_LO(w) ((uint8_t)((w) & 0x0F))

/* Frame allocation tracking for deferred free */
#define EVE5_MAX_FRAME_ALLOCS 256

/* Image cache configuration */
#define EVE5_IMAGE_CACHE_CAPACITY 128

/* Glyph cache configuration */
#define EVE5_GLYPH_CACHE_CAPACITY 256

/* SW cache configuration */
#ifndef EVE5_SW_CACHE_CAPACITY
#define EVE5_SW_CACHE_CAPACITY 32
#endif

/* Shadow texture configuration */
#define EVE5_SHADOW_TEX_SIZE 64

/* Canvas cache configuration - stores persistent GPU textures for canvas layers.
 * When a canvas is rendered, the GPU texture is cached by draw_buf->data pointer.
 * When the draw_buf is later displayed as an image, the cached GPU texture is used
 * directly without re-uploading.
 *
 * NOTE: Currently canvas entries are treated as permanent (no LRU eviction).
 * Proper lifecycle management requires LVGL buffer management hooks to notify
 * the draw unit when a draw_buf is destroyed, so the corresponding GPU allocation
 * can be freed. Until then, canvas GPU memory may leak if canvases are created
 * and destroyed dynamically. */
#ifndef EVE5_CANVAS_CACHE_CAPACITY
#define EVE5_CANVAS_CACHE_CAPACITY 128
#endif

/* Hardware image decode configuration — when enabled, JPEG/PNG files are
 * decoded via CMD_LOADIMAGE using EVE's hardware decoder instead of CPU
 * decoding via lodepng/libjpeg. Significantly faster and avoids host memory
 * allocation for decompressed images.
 *
 * Set to 0 to disable and always use CPU decoding (e.g., for debugging). */
#ifndef EVE5_HW_IMAGE_DECODE
#define EVE5_HW_IMAGE_DECODE 1
#endif

/* Alpha pass stencil approximation — when enabled, uses EVE's stencil buffer
 * to constrain alpha writes for approximate cases (border masking path, etc.).
 * Produces binary edges at stencil boundaries instead of smooth AA, but
 * prevents interior over-coverage for semi-transparent shapes on non-opaque
 * backgrounds. Disabled by default (experimental). */
#define EVE5_ALPHA_STENCIL_APPROX 1

/* Multi-step stencil AA for rounded gradient fills — when enabled alongside
 * EVE5_ALPHA_STENCIL_APPROX, uses 4 concentric stencil boundaries to
 * approximate smooth AA with discrete coverage levels (1/4, 2/4, 3/4, full)
 * instead of a single binary threshold. Better visual quality at the cost
 * of additional display list entries per rounded gradient fill. */
#define EVE5_ALPHA_STENCIL_MULTISTEP 0 /* Not useful when alpha rendertarget is active */

/* Render-target-based alpha recovery — when enabled, layers that contain
 * tasks with inaccurate stencil-based alpha recovery (arcs, diagonal
 * flat-cap lines, rounded gradients with alpha stops, etc.) get a
 * separate L8 render pass that captures exact alpha coverage. The L8
 * result is blitted into the ARGB8 layer's alpha channel, replacing
 * the direct-to-alpha correction pass entirely for that layer. */
#define EVE5_USE_RENDERTARGET_ALPHA 1

/* When set to 1, all float usage (sqrtf, sinf, cosf, tanf, expf, fabsf,
 * floorf) is replaced with integer-only alternatives, and <math.h> is
 * not included. This avoids pulling in soft-float library code on
 * embedded targets without an FPU (e.g., FT9XX). */
#ifndef LV_DRAW_EVE5_NO_FLOAT
#define LV_DRAW_EVE5_NO_FLOAT 1
#endif

/* Force RGB8 format for all opaque (non-alpha) layer render targets.
 * When enabled, non-alpha layers use RGB8 (3bpp) instead of RGB565 (2bpp).
 * Screen layer always uses ARGB8 regardless of this setting. */
#ifndef LV_DRAW_EVE5_OPAQUE_LAYER_RGB8
#define LV_DRAW_EVE5_OPAQUE_LAYER_RGB8 0
#endif

/**********************
 * TYPEDEFS
 **********************/

/**
 * Image cache entry - maps source pointer to GPU allocation
 */
typedef struct
{
    uintptr_t key; /* Source buffer pointer */
    uint32_t key_hash; /* Hash of image data for stale detection */
    Esd_GpuHandle gpu_handle; /* RAM_G allocation */
    uint16_t eve_format; /* Cached EVE format */
    int32_t eve_stride; /* Cached stride */
    int16_t width;
    int16_t height;
    uint16_t palette_size; /* Bytes of palette preceding index data (0 for non-paletted) */
} lv_draw_eve5_image_cache_entry_t;

/**
 * Image cache - index into GPU allocations, allocator owns lifecycle
 */
typedef struct
{
    lv_draw_eve5_image_cache_entry_t *entries;
    uint32_t capacity;
    uint32_t count;
} lv_draw_eve5_image_cache_t;

/**
 * Glyph cache entry - maps glyph bitmap pointer to GPU allocation
 */
typedef struct
{
    uintptr_t key; /* Glyph bitmap pointer */
    Esd_GpuHandle gpu_handle; /* RAM_G allocation */
    uint16_t width;
    uint16_t height;
    uint16_t stride; /* EVE-aligned stride */
} lv_draw_eve5_glyph_cache_entry_t;

/**
 * Glyph cache
 */
typedef struct
{
    lv_draw_eve5_glyph_cache_entry_t *entries;
    uint32_t capacity;
    uint32_t count;
} lv_draw_eve5_glyph_cache_t;

/**
 * SW cache entry - caches SW-rendered textures (box shadows, etc.)
 */
typedef struct {
    void *dsc_data;               /* Copy of descriptor data (excluding base) */
    uint32_t dsc_size;            /* Size of dsc_data */
    lv_draw_task_type_t type;     /* Task type */
    int32_t w;                    /* Texture width */
    int32_t h;                    /* Texture height */
    Esd_GpuHandle handle;         /* GPU memory handle */
    uint32_t eve_stride;          /* Texture stride in RAM_G */
    uint32_t last_used_frame;     /* For LRU eviction */
    bool valid;
} lv_draw_eve5_sw_cache_entry_t;

/**
 * SW cache - caches SW-rendered textures to avoid re-rendering
 */
typedef struct {
    lv_draw_eve5_sw_cache_entry_t entries[EVE5_SW_CACHE_CAPACITY];
    uint32_t frame;
    bool initialized;
} lv_draw_eve5_sw_cache_t;

/**
 * Shadow texture slot - one per ratio index (0 to TEX_SIZE-1)
 */
typedef struct {
    Esd_GpuHandle corner_handle;
    Esd_GpuHandle edge_handle;
} lv_draw_eve5_shadow_slot_t;

/**
 * Canvas cache entry - maps draw_buf->data to persistent GPU allocation.
 * Used for canvas layers rendered by EVE5. When a draw_buf is later displayed
 * as an image, the cached GPU texture is used directly.
 */
typedef struct {
    void *data_ptr;             /* Key: draw_buf->data pointer */
    Esd_GpuHandle gpu_handle;   /* Persistent GPU allocation */
    uint32_t width;
    uint32_t height;
    uint32_t aligned_width;     /* 16-byte aligned width for render target */
    uint32_t stride;            /* Bytes per row (format-dependent) */
    uint32_t last_used_frame;   /* For LRU eviction */
    uint32_t palette_addr;      /* RAM_G address of palette LUT (0 = non-paletted) */
    uint32_t source_offset;     /* Offset from allocation base to bitmap source data (0 for non-paletted) */
    uint16_t eve_format;        /* EVE bitmap format (ARGB8, RGB565, ASTC, etc.) */
    bool valid;
    bool is_premultiplied;      /* True if GPU content is premultiplied alpha */
} lv_draw_eve5_canvas_cache_entry_t;

/**
 * Canvas cache - indexes persistent GPU textures for canvas layers
 */
typedef struct {
    lv_draw_eve5_canvas_cache_entry_t entries[EVE5_CANVAS_CACHE_CAPACITY];
    uint32_t frame;             /* Current frame counter */
    bool initialized;
} lv_draw_eve5_canvas_cache_t;

/**
 * EVE5 draw unit
 */
typedef struct
{
    lv_draw_unit_t base_unit;
    EVE_HalContext *hal;
    Esd_GpuAlloc *allocator;

    /* Asset caches */
    lv_draw_eve5_image_cache_t image_cache;
    lv_draw_eve5_glyph_cache_t glyph_cache;
    lv_draw_eve5_sw_cache_t sw_cache;
    lv_draw_eve5_canvas_cache_t canvas_cache;

    /* Re-entrancy guard for SW fallback */
    bool rendering_in_progress;

    /* Per-frame temporary allocations for deferred free */
    Esd_GpuHandle frame_allocs[EVE5_MAX_FRAME_ALLOCS];
    uint16_t frame_alloc_count;

    /* Box shadow texture cache - indexed by ratio (solid_radius / corner_size) */
    lv_draw_eve5_shadow_slot_t shadow_slots[EVE5_SHADOW_TEX_SIZE];

    /* Per-layer alpha repair tracking (layer-relative coordinates).
     * Some EVE operations use the alpha channel as scratch space (arc stencil,
     * line cap masking), trashing alpha in their bounding box. To repair this,
     * we track the largest fully opaque fill (whose alpha should be 255) and
     * the union of trashed areas. At layer finish, we write alpha=255 into
     * the intersection. */
    lv_area_t alpha_opaque_area;
    int32_t alpha_opaque_radius;
    lv_area_t alpha_trashed_area;
    bool has_alpha_opaque;
    bool has_alpha_trashed;
    bool alpha_needs_rendertarget;

    /* Canvas incremental render: original content reference for alpha compositing.
     * When drawing on an existing canvas, the alpha pass must incorporate the
     * original canvas alpha as the "base layer" before compositing new tasks.
     * Set by hal_init_layer when canvas has existing content. */
    uint32_t canvas_orig_addr;      /* RAM_G address of original content (GA_INVALID if none) */
    uint16_t canvas_orig_format;    /* EVE format (ARGB8, PALETTEDARGB8, RGB565, etc.) */
    uint32_t canvas_orig_stride;    /* Bytes per row */
    uint32_t canvas_orig_palette;   /* RAM_G address of palette LUT (0 for non-paletted) */
    int32_t canvas_orig_w;          /* Content width */
    int32_t canvas_orig_h;          /* Content height */
} lv_draw_eve5_unit_t;

/**********************
 * CACHE PROTOTYPES
 **********************/

/* Image cache */
void lv_draw_eve5_image_cache_init(lv_draw_eve5_image_cache_t *cache, uint32_t capacity);
void lv_draw_eve5_image_cache_deinit(lv_draw_eve5_image_cache_t *cache);
uint32_t lv_draw_eve5_image_cache_lookup(lv_draw_eve5_unit_t *u,
    const lv_image_dsc_t *img_dsc,
    uint16_t *out_eve_format,
    int32_t *out_eve_stride,
    uint16_t *out_palette_size);
void lv_draw_eve5_image_cache_insert(lv_draw_eve5_unit_t *u,
    const lv_image_dsc_t *img_dsc,
    Esd_GpuHandle handle,
    uint16_t eve_format,
    int32_t eve_stride,
    uint16_t palette_size);

/* Raw key-based image cache API (for file path caching) */
uint32_t lv_draw_eve5_image_cache_lookup_raw(lv_draw_eve5_unit_t *u,
    uintptr_t key, uint32_t key_hash,
    uint16_t *out_eve_format,
    int32_t *out_eve_stride,
    int32_t *out_width, int32_t *out_height,
    uint16_t *out_palette_size);
void lv_draw_eve5_image_cache_insert_raw(lv_draw_eve5_unit_t *u,
    uintptr_t key, uint32_t key_hash,
    Esd_GpuHandle handle,
    uint16_t eve_format,
    int32_t eve_stride,
    int16_t width, int16_t height,
    uint16_t palette_size);

/* Glyph cache */
void lv_draw_eve5_glyph_cache_init(lv_draw_eve5_glyph_cache_t *cache, uint32_t capacity);
void lv_draw_eve5_glyph_cache_deinit(lv_draw_eve5_glyph_cache_t *cache);
uint32_t lv_draw_eve5_glyph_cache_lookup(lv_draw_eve5_unit_t *u,
    const uint8_t *glyph_bitmap,
    uint16_t *out_stride);
void lv_draw_eve5_glyph_cache_insert(lv_draw_eve5_unit_t *u,
    const uint8_t *glyph_bitmap,
    Esd_GpuHandle handle,
    uint16_t width,
    uint16_t height,
    uint16_t stride);

/**********************
 * SW CACHE API
 **********************/

void lv_draw_eve5_sw_cache_init(lv_draw_eve5_unit_t *u);
void lv_draw_eve5_sw_cache_deinit(lv_draw_eve5_unit_t *u);
void lv_draw_eve5_sw_cache_new_frame(lv_draw_eve5_unit_t *u);

bool lv_draw_eve5_sw_cache_lookup(lv_draw_eve5_unit_t *u,
                                   lv_draw_task_type_t type,
                                   int32_t w, int32_t h,
                                   const void *dsc_data, uint32_t dsc_size,
                                   Esd_GpuHandle *out_handle,
                                   uint32_t *out_stride);

void lv_draw_eve5_sw_cache_insert(lv_draw_eve5_unit_t *u,
                                   lv_draw_task_type_t type,
                                   int32_t w, int32_t h,
                                   const void *dsc_data, uint32_t dsc_size,
                                   Esd_GpuHandle handle,
                                   uint32_t eve_stride);

void lv_draw_eve5_sw_cache_drop(lv_draw_eve5_unit_t *u,
                                 lv_draw_task_type_t type,
                                 int32_t w, int32_t h,
                                 const void *dsc_data, uint32_t dsc_size);

/**********************
 * CANVAS CACHE API
 **********************/

void lv_draw_eve5_canvas_cache_init(lv_draw_eve5_unit_t *u);
void lv_draw_eve5_canvas_cache_new_frame(lv_draw_eve5_unit_t *u);

/**
 * Look up a canvas GPU texture by draw_buf->data pointer.
 * Returns GPU address if found, GA_INVALID otherwise.
 * Updates last_used_frame on hit.
 */
uint32_t lv_draw_eve5_canvas_cache_lookup(lv_draw_eve5_unit_t *u,
                                           const void *data_ptr,
                                           uint32_t *out_width,
                                           uint32_t *out_height,
                                           uint32_t *out_aligned_width,
                                           uint16_t *out_eve_format,
                                           uint32_t *out_stride,
                                           uint32_t *out_palette_addr);

/**
 * Insert or update a canvas cache entry.
 * If an entry for data_ptr already exists, updates it.
 * If cache is full, evicts the oldest entry.
 */
void lv_draw_eve5_canvas_cache_insert(lv_draw_eve5_unit_t *u,
                                       const void *data_ptr,
                                       Esd_GpuHandle handle,
                                       uint32_t width, uint32_t height,
                                       uint32_t aligned_width,
                                       uint16_t eve_format,
                                       uint32_t stride,
                                       uint32_t palette_addr,
                                       uint32_t source_offset);

/**
 * Get or create a canvas GPU allocation for the given layer.
 * If cached, returns existing allocation. Otherwise allocates new and caches.
 * Returns GPU address, or GA_INVALID on allocation failure.
 * @param target_eve_format  Target EVE format for new allocations (ARGB8, RGB565, etc.)
 * @param target_bpp         Target bytes per pixel for new allocations
 * @param out_aligned_width  Output: 16-byte aligned width
 * @param out_eve_format     Output: EVE bitmap format (may differ from target if cached)
 * @param out_stride         Output: Bytes per row
 * @param out_is_new         Set to true if this is a new allocation (first render),
 *                           false if reusing existing cached allocation (subsequent render).
 * @param out_is_premultiplied  Set to true if the cached content is premultiplied alpha.
 */
uint32_t lv_draw_eve5_canvas_cache_get_or_create(lv_draw_eve5_unit_t *u,
                                                   lv_layer_t *layer,
                                                   uint16_t target_eve_format,
                                                   uint8_t target_bpp,
                                                   uint32_t *out_aligned_width,
                                                   uint16_t *out_eve_format,
                                                   uint32_t *out_stride,
                                                   bool *out_is_new,
                                                   bool *out_is_premultiplied);

/**
 * Set the premultiplied state of a canvas cache entry.
 */
void lv_draw_eve5_canvas_cache_set_premultiplied(lv_draw_eve5_unit_t *u,
                                                   const void *data_ptr,
                                                   bool is_premultiplied);

/**
 * Check if a canvas cache entry is premultiplied.
 */
bool lv_draw_eve5_canvas_cache_is_premultiplied(lv_draw_eve5_unit_t *u,
                                                  const void *data_ptr);

/**
 * Get the EVE format of a canvas cache entry.
 */
uint16_t lv_draw_eve5_canvas_cache_get_format(lv_draw_eve5_unit_t *u,
                                               const void *data_ptr);

/**
 * Update the format, handle, and stride of a canvas cache entry.
 * Used when a direct image load stores a native format instead of ARGB8.
 */
void lv_draw_eve5_canvas_cache_set_format(lv_draw_eve5_unit_t *u,
                                            const void *data_ptr,
                                            Esd_GpuHandle new_handle,
                                            uint16_t eve_format,
                                            uint32_t stride);

/**********************
 * HAL RENDER TARGET
 **********************/

/**
 * Map LVGL color format to EVE render target format and bytes per pixel.
 * Returns true if the format is supported for direct rendering.
 * CMD_RENDERTARGET supports: L8, LA8, RGB565, RGB6, RGB8, ARGB4, ARGB1555, ARGB6, ARGB8, YCBCR
 */
bool lv_draw_eve5_get_render_target_format(lv_color_format_t lv_cf, uint16_t *eve_fmt, uint8_t *bpp);

/**********************
 * HAL TEXTURE API
 **********************/

Esd_GpuHandle lv_draw_eve5_hal_upload_texture(lv_draw_eve5_unit_t *u,
                                               const uint8_t *buf_data,
                                               int32_t buf_w, int32_t buf_h,
                                               uint32_t *out_stride);

void lv_draw_eve5_hal_draw_texture(lv_draw_eve5_unit_t *u,
                                    const lv_draw_task_t *t,
                                    uint32_t ram_g_addr,
                                    int32_t tex_w, int32_t tex_h,
                                    uint32_t eve_stride,
                                    const lv_area_t *draw_area);

/**********************
 * HAL PROTOTYPES
 **********************/

/* Fill+border area matching: LVGL shrinks fill area by up to 2px per edge
 * when border is opaque with radius, so a fill is "paired" with its border
 * when fill is inside border by 0-2px on each edge and radii match. */
static inline bool eve5_fill_border_area_match(const lv_area_t *fill_area,
                                                const lv_area_t *border_area)
{
    int32_t dx1 = fill_area->x1 - border_area->x1;
    int32_t dy1 = fill_area->y1 - border_area->y1;
    int32_t dx2 = fill_area->x2 - border_area->x2;
    int32_t dy2 = fill_area->y2 - border_area->y2;
    return (dx1 >= 0 && dx1 <= 2) &&
           (dy1 >= 0 && dy1 <= 2) &&
           (dx2 <= 0 && dx2 >= -2) &&
           (dy2 <= 0 && dy2 >= -2);
}

/* Shared utilities */
void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t *u, const lv_area_t *clip, const lv_area_t *layer_area);
void lv_draw_eve5_clear_stencil(lv_draw_eve5_unit_t *u,
                                 int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                 const lv_area_t *clip, const lv_area_t *layer_area);

/* Rounded rectangle primitive (used for alpha masking in fill, border, image) */
void lv_draw_eve5_draw_rect(lv_draw_eve5_unit_t *u, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                             int32_t radius, const lv_area_t *clip_area, const lv_area_t *layer_area);

/* Layer management */
void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                  bool is_screen, bool is_canvas);
void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                    bool is_screen, bool is_canvas);

/* L8 alpha render target lifecycle */
Esd_GpuHandle lv_draw_eve5_hal_init_l8_rendertarget(lv_draw_eve5_unit_t *u,
                                                      int32_t aligned_w, int32_t aligned_h,
                                                      int32_t w, int32_t h);
void lv_draw_eve5_hal_finish_l8_rendertarget(lv_draw_eve5_unit_t *u);
void lv_draw_eve5_hal_blit_l8_to_alpha(lv_draw_eve5_unit_t *u, uint32_t l8_addr,
                                        int32_t aligned_w, int32_t aligned_h,
                                        int32_t w, int32_t h);

/* Primitive drawing */
void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb, bool use_hv_opt);
void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb);
void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb);

/* Unified fill+border drawing (optimized when possible) */
void lv_draw_eve5_hal_draw_fill_with_border(lv_draw_eve5_unit_t *u,
	const lv_draw_task_t *fill_task,
	const lv_draw_task_t *border_task);

/* Image transform helpers (defined in lv_draw_eve5_image.c, also used by glyph code) */
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

bool compute_image_skew(image_skew_t *out,
                        int32_t rotation, int32_t scale_x, int32_t scale_y,
                        int32_t skew_x, int32_t skew_y,
                        int32_t pivot_x, int32_t pivot_y,
                        int32_t src_w, int32_t src_h,
                        int32_t img_x, int32_t img_y,
                        int32_t draw_vx, int32_t draw_vy);
void apply_image_skew(EVE_HalContext *phost, const image_skew_t *skew,
                      uint8_t bmp_filter, int32_t tile_w, int32_t tile_h);

/* Frame allocation tracking (defined in lv_draw_eve5_hal.c) */
Esd_GpuHandle track_frame_alloc(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle);

/* Resolved image source — either a direct variable pointer or
 * a decoder session that must be closed after use. */
typedef struct {
    const lv_image_dsc_t *img_dsc;
    lv_image_decoder_dsc_t decoder_dsc;
    bool decoder_open;
} eve5_resolved_image_t;

/* Image format mapping (defined in lv_draw_eve5_image_load.c) */
bool lv_draw_eve5_get_eve_format_info(lv_color_format_t src_cf,
                                 uint16_t *eve_format,
                                 uint8_t *bits_per_pixel,
                                 bool *needs_conversion);

/* Image upload — cached (defined in lv_draw_eve5_image_load.c) */
uint32_t lv_draw_eve5_upload_image(lv_draw_eve5_unit_t *u, const lv_image_dsc_t *img_dsc,
                              uint16_t *out_eve_format, int32_t *out_eve_stride,
                              uint32_t *out_palette_addr);

/* Image source resolution (defined in lv_draw_eve5_image_load.c) */
bool lv_draw_eve5_resolve_image_source(const void *src, eve5_resolved_image_t *resolved);
void lv_draw_eve5_release_image_source(eve5_resolved_image_t *resolved);

/* Hardware image decode paths (defined in lv_draw_eve5_image_load.c) */
#if EVE5_HW_IMAGE_DECODE
bool lv_draw_eve5_try_load_file_image(lv_draw_eve5_unit_t *u, const void *src,
                                 uint32_t *ram_g_addr, uint16_t *eve_format,
                                 int32_t *eve_stride, int32_t *src_w, int32_t *src_h,
                                 Esd_GpuHandle *out_handle, uint32_t *out_palette_addr);
#endif
#if LV_USE_FS_EVE5_SDCARD
bool lv_draw_eve5_try_load_sdcard_image(lv_draw_eve5_unit_t *u, const void *src,
                                   uint32_t *ram_g_addr, uint16_t *eve_format,
                                   int32_t *eve_stride, int32_t *src_w, int32_t *src_h,
                                   Esd_GpuHandle *out_handle);
#endif

/* Image loading - loads image to GPU via best available path (HW decode, SD card, SW decode).
 * Returns GPU address on success, GA_INVALID on failure. Caller owns the handle. */
uint32_t lv_draw_eve5_load_image(lv_draw_eve5_unit_t *u, const void *src,
                                   uint16_t *out_format, int32_t *out_stride,
                                   int32_t *out_w, int32_t *out_h,
                                   Esd_GpuHandle *out_handle,
                                   uint32_t *out_palette_addr);

/* EVE5 image decoder registration (defined in lv_draw_eve5_image_load.c) */
#if EVE5_HW_IMAGE_DECODE
void lv_draw_eve5_register_image_decoder(void);
#endif

/* Colorkey stencil mask (defined in lv_draw_eve5_image.c, also used by image alpha pass) */
void build_colorkey_stencil(EVE_HalContext *phost,
                             const lv_image_colorkey_t *colorkey,
                             uint16_t eve_format, int32_t eve_stride, int32_t layout_h,
                             int32_t vx, int32_t vy);

/* FNV-1a hash for file path strings (used for file image cache keys) */
static inline uint32_t lv_draw_eve5_hash_path(const char *path)
{
    uint32_t h = 2166136261u;
    while(*path) {
        h = (h ^ (uint8_t)*path) * 16777619u;
        path++;
    }
    return h;
}

/* Palette source helper (inline, used by image and image_alpha) */
static inline void set_palette_if_needed(EVE_HalContext *phost,
                                          uint16_t eve_format,
                                          uint32_t palette_addr)
{
    if(eve_format == PALETTEDARGB8 && palette_addr != GA_INVALID) {
        EVE_CoDl_paletteSource(phost, palette_addr);
    }
}

/* Bitmap/image drawing */
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb);

/* Label/text drawing */
void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_letter(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);

/* Utilities */
bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle);

/* Box shadow drawing */
void lv_draw_eve5_box_shadow_init(lv_draw_eve5_unit_t *u);
void lv_draw_eve5_hal_draw_box_shadow(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Canvas layer optimization (defined in lv_draw_eve5_canvas.c) */
bool lv_draw_eve5_try_canvas_direct_image(lv_draw_eve5_unit_t *u, lv_layer_t *layer);

/* Mask rectangle drawing */
void lv_draw_eve5_hal_draw_mask_rect(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* SW fallback helpers (defined in lv_draw_eve5_sw_fallback.c) */
const void *lv_draw_eve5_sw_get_dsc_cache_data(const lv_draw_task_t *t, uint32_t *out_size);
uint8_t *lv_draw_eve5_sw_render_to_buffer(lv_draw_eve5_unit_t *u,
                                     const lv_draw_task_t *t,
                                     int32_t buf_w, int32_t buf_h);
Esd_GpuHandle lv_draw_eve5_sw_render_cached(lv_draw_eve5_unit_t *u,
                                            const lv_draw_task_t *t,
                                            int32_t *out_w, int32_t *out_h,
                                            uint32_t *out_stride,
                                            bool *out_from_cache);
void lv_draw_eve5_sw_render_task(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Render processing loops (defined in lv_draw_eve5_render.c) */
int lv_draw_eve5_render_tasks(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen, bool finish_tasks);
void lv_draw_eve5_opaque_prepass(lv_draw_eve5_unit_t *u, lv_layer_t *layer);
void lv_draw_eve5_alpha_pass(lv_draw_eve5_unit_t *u, lv_layer_t *layer);
#if EVE5_USE_RENDERTARGET_ALPHA
void lv_draw_eve5_check_alpha_recovery(lv_draw_eve5_unit_t *u, lv_layer_t *layer);
Esd_GpuHandle lv_draw_eve5_render_alpha_to_l8(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                              int32_t aligned_w, int32_t aligned_h,
                                              int32_t w, int32_t h);
#endif
bool lv_draw_eve5_is_fully_inside_opaque(lv_draw_eve5_unit_t *u, const lv_area_t *task_area,
                                    const lv_area_t *clip_area, const lv_area_t *layer_area);
bool lv_draw_eve5_line_should_use_hv_opt(const lv_draw_task_t *t, const lv_draw_task_t *prev);

/* Primitive helpers (defined in lv_draw_eve5_primitives.c) */
void draw_circle_subpx(lv_draw_eve5_unit_t *u, int32_t cx2, int32_t cy2, int32_t r16);
void build_triangle_stencil(EVE_HalContext *phost, const lv_point_t p[3]);
bool setup_gradient_bitmap(lv_draw_eve5_unit_t *u, const lv_grad_dsc_t *grad,
                            lv_opa_t opa, int32_t w, int32_t h, bool alpha_to_rgb);

/* Alpha correction pass — per-task drawing (defined in lv_draw_eve5_alpha_pass.c) */
void lv_draw_eve5_alpha_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_fill_with_border(lv_draw_eve5_unit_t *u,
                                               const lv_draw_task_t *fill_task,
                                               const lv_draw_task_t *border_task);
void lv_draw_eve5_alpha_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool use_hv_opt);
void lv_draw_eve5_alpha_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_alpha_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t, bool alpha_to_rgb);
void lv_draw_eve5_alpha_draw_letter(lv_draw_eve5_unit_t *u, lv_draw_task_t *t, bool alpha_to_rgb);
void lv_draw_eve5_alpha_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t, bool alpha_to_rgb);

/* Alpha accuracy predicates — "*_needs_alpha_rendertarget"
 *
 * Each function answers: is the direct-to-alpha correction pass insufficient
 * for this task?  Returns true when BOTH conditions hold:
 *   1. The task requires alpha recovery (either the RGB pass trashes the alpha
 *      channel via alpha-as-scratch masking, or the task's shape produces
 *      squared-alpha on anti-aliased edges from blend(SRC_ALPHA, 1-SRC_ALPHA)).
 *   2. The direct-to-alpha pass cannot accurately reconstruct the alpha — it
 *      would require stencil approximation (binary in/out instead of smooth AA),
 *      or is fully incapable of recovery (e.g. ARGB image with bitmap mask:
 *      the mask shape can be approximated but per-pixel alpha is lost entirely).
 *
 * When any predicate returns true for a layer, eve5_check_alpha_recovery()
 * sets alpha_needs_rendertarget, triggering the L8 render-target alpha path
 * which captures exact alpha coverage instead of the direct-to-alpha pass.
 *
 * IMPORTANT: These predicates must exactly mirror the decision logic in the
 * corresponding RGB draw function (lv_draw_eve5_hal_draw_*) and alpha pass
 * function (lv_draw_eve5_alpha_draw_*). If a rendering path changes — e.g. a
 * new masking technique is added or a path gains exact alpha recovery — the
 * matching predicate MUST be updated to stay in sync.
 *
 * Defined in lv_draw_eve5_alpha_pass.c (fill, border, line, arc)
 * and lv_draw_eve5_image.c (image/layer). */
bool lv_draw_eve5_fill_needs_alpha_rendertarget(const lv_draw_task_t *t);
bool lv_draw_eve5_border_needs_alpha_rendertarget(const lv_draw_task_t *t);
bool lv_draw_eve5_line_needs_alpha_rendertarget(const lv_draw_task_t *t);
bool lv_draw_eve5_arc_needs_alpha_rendertarget(const lv_draw_task_t *t);
bool lv_draw_eve5_image_needs_alpha_rendertarget(const lv_draw_task_t *t);

/* Alpha repair tracking helpers.
 * Call track_alpha_opaque for fully opaque fills (records the largest).
 * Call track_alpha_trashed for operations that use alpha as scratch space.
 * All coordinates are layer-relative. */
static inline void lv_draw_eve5_track_alpha_opaque(lv_draw_eve5_unit_t *u,
                                                     int32_t x1, int32_t y1,
                                                     int32_t x2, int32_t y2,
                                                     int32_t radius)
{
    int64_t new_size = (int64_t)(x2 - x1 + 1) * (y2 - y1 + 1);
    if(!u->has_alpha_opaque) {
        u->alpha_opaque_area = (lv_area_t){ x1, y1, x2, y2 };
        u->alpha_opaque_radius = radius;
        u->has_alpha_opaque = true;
    }
    else {
        int64_t old_size = (int64_t)lv_area_get_width(&u->alpha_opaque_area)
                         * lv_area_get_height(&u->alpha_opaque_area);
        if(new_size > old_size) {
            u->alpha_opaque_area = (lv_area_t){ x1, y1, x2, y2 };
            u->alpha_opaque_radius = radius;
        }
    }
}

static inline void lv_draw_eve5_track_alpha_trashed(lv_draw_eve5_unit_t *u,
                                                      int32_t x1, int32_t y1,
                                                      int32_t x2, int32_t y2)
{
    if(!u->has_alpha_trashed) {
        u->alpha_trashed_area = (lv_area_t){ x1, y1, x2, y2 };
        u->has_alpha_trashed = true;
    }
    else {
        if(x1 < u->alpha_trashed_area.x1) u->alpha_trashed_area.x1 = x1;
        if(y1 < u->alpha_trashed_area.y1) u->alpha_trashed_area.y1 = y1;
        if(x2 > u->alpha_trashed_area.x2) u->alpha_trashed_area.x2 = x2;
        if(y2 > u->alpha_trashed_area.y2) u->alpha_trashed_area.y2 = y2;
    }
}

#endif /* LV_USE_DRAW_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_DRAW_EVE5_PRIVATE_H */
