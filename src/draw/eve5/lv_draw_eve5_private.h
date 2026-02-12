/**
 * @file lv_draw_eve5_private.h
 *
 * EVE5 (BT820) Draw Unit Private Header
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
#define EVE5_ALPHA_STENCIL_MULTISTEP 1

/* When set to 1, all float usage (sqrtf, sinf, cosf, tanf, expf, fabsf,
 * floorf) is replaced with integer-only alternatives, and <math.h> is
 * not included. This avoids pulling in soft-float library code on
 * embedded targets without an FPU (e.g., FT9XX). */
#ifndef LV_DRAW_EVE5_NO_FLOAT
#define LV_DRAW_EVE5_NO_FLOAT 1
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
    uint8_t eve_format; /* Cached EVE format */
    int32_t eve_stride; /* Cached stride */
    int16_t width;
    int16_t height;
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
} lv_draw_eve5_unit_t;

/**********************
 * CACHE PROTOTYPES
 **********************/

/* Image cache */
void lv_draw_eve5_image_cache_init(lv_draw_eve5_image_cache_t *cache, uint32_t capacity);
void lv_draw_eve5_image_cache_deinit(lv_draw_eve5_image_cache_t *cache);
uint32_t lv_draw_eve5_image_cache_lookup(lv_draw_eve5_unit_t *u,
    const lv_image_dsc_t *img_dsc,
    uint8_t *out_eve_format,
    int32_t *out_eve_stride);
void lv_draw_eve5_image_cache_insert(lv_draw_eve5_unit_t *u,
    const lv_image_dsc_t *img_dsc,
    Esd_GpuHandle handle,
    uint8_t eve_format,
    int32_t eve_stride);

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

/* Shared utilities */
void lv_draw_eve5_set_scissor(lv_draw_eve5_unit_t *u, const lv_area_t *clip, const lv_area_t *layer_area);
void lv_draw_eve5_clear_stencil(lv_draw_eve5_unit_t *u,
                                 int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                 const lv_area_t *clip, const lv_area_t *layer_area);

/* Rounded rectangle primitive (used for alpha masking in fill, border, image) */
void lv_draw_eve5_draw_rect(lv_draw_eve5_unit_t *u, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                             int32_t radius, const lv_area_t *clip_area, const lv_area_t *layer_area);

/* Layer management */
void lv_draw_eve5_hal_init_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen);
void lv_draw_eve5_hal_finish_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer, bool is_screen);

/* Primitive drawing */
void lv_draw_eve5_hal_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

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

/* Bitmap/image drawing */
void lv_draw_eve5_hal_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Label/text drawing */
void lv_draw_eve5_hal_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);
void lv_draw_eve5_hal_draw_letter(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);

/* Utilities */
bool lv_draw_eve5_hal_check_texture(lv_draw_eve5_unit_t *u, Esd_GpuHandle handle);

/* Box shadow drawing */
void lv_draw_eve5_box_shadow_init(lv_draw_eve5_unit_t *u);
void lv_draw_eve5_hal_draw_box_shadow(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Mask rectangle drawing */
void lv_draw_eve5_hal_draw_mask_rect(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

/* Primitive helpers (defined in lv_draw_eve5_primitives.c) */
void draw_circle_subpx(lv_draw_eve5_unit_t *u, int32_t cx2, int32_t cy2, int32_t r16);
void build_triangle_stencil(EVE_HalContext *phost, const lv_point_t p[3]);
bool setup_gradient_bitmap(lv_draw_eve5_unit_t *u, const lv_grad_dsc_t *grad,
                            lv_opa_t opa, int32_t w, int32_t h);

/* Alpha correction pass — per-task drawing (defined in lv_draw_eve5_alpha_pass.c) */
void lv_draw_eve5_alpha_draw_fill(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_fill_with_border(lv_draw_eve5_unit_t *u,
                                               const lv_draw_task_t *fill_task,
                                               const lv_draw_task_t *border_task);
void lv_draw_eve5_alpha_draw_border(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_line(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_triangle(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_hal_alpha_draw_image(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_label(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_letter(lv_draw_eve5_unit_t *u, lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_arc(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);
void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

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
