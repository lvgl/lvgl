/**
 * @file lv_draw_eve5_box_shadow.c
 *
 * Box shadow rendering for EVE5/BT820 using pre-generated Gaussian textures.
 *
 * == 9-Slice Shadow Layout ==
 *
 * The shadow is rendered as 9 regions around the widget:
 *
 *     +------+----------------+------+
 *     |  TL  |      TOP       |  TR  |  <- corner_size height
 *     +------+----------------+------+
 *     |      |                |      |
 *     | LEFT |     CENTER     | RIGHT|  <- solid fill
 *     |      |                |      |
 *     +------+----------------+------+
 *     |  BL  |     BOTTOM     |  BR  |  <- corner_size height
 *     +------+----------------+------+
 *        ^                        ^
 *     corner_size              corner_size
 *
 * - Corners use a 2D texture (64x64 L8) for rounded Gaussian falloff
 * - Edges use a 1D texture (64x1 L8) stretched along the edge
 * - Center is a solid fill at full opacity
 *
 * == Shape ==
 *
 * As in LVGL's software renderer, the shadow is the core rectangle (the
 * widget grown by the spread and moved by the offset, with the radius)
 * blurred over the shadow width: half on the core's edge, fading out to
 * blur = width / 2 outside it and in to full blur inside it. The corner
 * and edge slices are corner_size = blur + blur + radius wide, from the
 * shadow's outer edge to where the shadow is solid; a shadow narrower than
 * two slices is split between them.
 *
 * == Texture Layout ==
 *
 *     (0,0) transparent -----> X
 *       |
 *       v    +---------+
 *       Y    |    .--' |  <- Gaussian falloff around the core's edge,
 *            |  .'     |     blur texels in from (0,0)
 *            | |  solid|
 *            +---------+
 *                    (SIZE-1, SIZE-1) = alpha 255
 *
 * == Gaussian Math ==
 *
 * For each texel:
 *   signed_dist = distance to the core's rounded rectangle (negative inside)
 *   alpha = 0.5 × (1 - erf(signed_dist / (sigma × √2)))
 *
 * sigma = 0.41 × blur, the deviation of the two box blurs of the software
 * renderer, each blur wide.
 *
 * == Texture Caching ==
 *
 * Textures are cached by ratio_idx = (radius × 63) / corner_size, giving
 * 64 possible texture pairs. Same ratio = same visual proportions at any size.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#if !LV_DRAW_EVE5_NO_FLOAT
    #include <math.h>
#endif

/*********************
 *      DEFINES
 *********************/

#define SHADOW_TEX_SIZE      EVE5_SHADOW_TEX_SIZE
#define SHADOW_BITMAP_HANDLE EVE_CO_SCRATCH_HANDLE
#define SHADOW_SIGMA_PCT     41  /* sigma in percent of the blur */

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void generate_corner_texture(uint8_t * buf, int32_t ratio_idx);
static void generate_edge_texture(uint8_t * buf, int32_t ratio_idx);
static bool ensure_shadow_textures(lv_draw_eve5_unit_t * u, int32_t ratio_idx);
static int32_t calc_ratio_index(int32_t radius, int32_t corner_size);

/**********************
 *   TEXTURE GENERATION
 **********************/

static int32_t calc_ratio_index(int32_t radius, int32_t corner_size)
{
    if(corner_size <= 0) return 0;
    if(radius <= 0) return 0;
    if(radius >= corner_size) return SHADOW_TEX_SIZE - 1;

    return (radius * (SHADOW_TEX_SIZE - 1)) / corner_size;
}

#if LV_DRAW_EVE5_NO_FLOAT

/**
 * Precomputed Gaussian CDF lookup table for integer-only platforms.
 * Maps erf input x ∈ [-4.0, +4.0] to alpha ∈ [0, 255].
 * Index: i = (x × 32) + 128, clamped to [0, 255]
 */
static const uint8_t s_gauss_cdf[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254,
    254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 253, 253, 252, 252, 252, 251,
    251, 250, 250, 249, 248, 248, 247, 246, 245, 244, 243, 242, 241, 239, 238, 237,
    235, 233, 231, 230, 227, 225, 223, 221, 218, 216, 213, 210, 207, 204, 201, 197,
    194, 190, 187, 183, 179, 175, 171, 167, 163, 158, 154, 150, 145, 141, 136, 132,
    128, 123, 119, 114, 110, 105, 101,  97,  92,  88,  84,  80,  76,  72,  68,  65,
    61,  58,  54,  51,  48,  45,  42,  39,  37,  34,  32,  30,  28,  25,  24,  22,
    20,  18,  17,  16,  14,  13,  12,  11,  10,   9,   8,   7,   7,   6,   5,   5,
    4,   4,   3,   3,   3,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,
    1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static inline uint8_t gauss_cdf_lookup(int32_t signed_dist_256, int32_t sigma_sqrt2_256)
{
    int32_t idx = (int32_t)(((int64_t)signed_dist_256 * 32) / sigma_sqrt2_256) + 128;
    if(idx < 0) idx = 0;
    if(idx > 255) idx = 255;
    return s_gauss_cdf[idx];
}

#else

/**
 * Fast erf approximation using Abramowitz & Stegun formula.
 * Maximum error ~1.5×10⁻⁷.
 */
static float fast_erf(float x)
{
    float a1 =  0.254829592f;
    float a2 = -0.284496736f;
    float a3 =  1.421413741f;
    float a4 = -1.453152027f;
    float a5 =  1.061405429f;
    float p  =  0.3275911f;

    int sign = (x >= 0) ? 1 : -1;
    x = fabsf(x);

    float t = 1.0f / (1.0f + p * x);
    float y = 1.0f - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * expf(-x * x);

    return sign * y;
}

#endif

/* The textures span corner_size pixels in SHADOW_TEX_SIZE texels. In texels:
 * the radius, the blur on each side of the core's edge (which is blur in from
 * the outer end), and the corner's centre at blur + radius. */
#if LV_DRAW_EVE5_NO_FLOAT

static void texture_shape_256(int32_t ratio_idx, int32_t * radius_256, int32_t * blur_256,
                              int32_t * sigma_sqrt2_256)
{
    *radius_256 = ratio_idx * SHADOW_TEX_SIZE * 256 / (SHADOW_TEX_SIZE - 1);
    *blur_256 = (SHADOW_TEX_SIZE * 256 - *radius_256) / 2;
    int32_t sigma_256 = *blur_256 * SHADOW_SIGMA_PCT / 100;
    if(sigma_256 < 128) sigma_256 = 128;
    *sigma_sqrt2_256 = sigma_256 * 1414 / 1000;
}

#else

static float texture_shape(int32_t ratio_idx, float * radius, float * blur)
{
    *radius = (float)ratio_idx * SHADOW_TEX_SIZE / (SHADOW_TEX_SIZE - 1);
    *blur = (SHADOW_TEX_SIZE - *radius) * 0.5f;
    float sigma = *blur * (SHADOW_SIGMA_PCT / 100.0f);
    if(sigma < 0.5f) sigma = 0.5f;
    return 1.0f / (sigma * 1.41421356f);
}

#endif

/**
 * Generate 2D corner texture for Gaussian shadow falloff: the outer corner at
 * (0, 0), the corner's centre toward (SIZE-1, SIZE-1). Texels hold the
 * shadow at their centres, which the slices sample at the pixel centres.
 */
static void generate_corner_texture(uint8_t * buf, int32_t ratio_idx)
{
#if LV_DRAW_EVE5_NO_FLOAT
    int32_t radius_256, blur_256, sigma_sqrt2_256;
    texture_shape_256(ratio_idx, &radius_256, &blur_256, &sigma_sqrt2_256);
    int32_t centre_256 = blur_256 + radius_256;

    for(int32_t y = 0; y < SHADOW_TEX_SIZE; y++) {
        for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
            /* Signed distance to the rounded rectangle, positive outside */
            int32_t dx = centre_256 - (x * 256 + 128);
            int32_t dy = centre_256 - (y * 256 + 128);
            int32_t ox = LV_MAX(dx, 0);
            int32_t oy = LV_MAX(dy, 0);
            int32_t dist_256 = (int32_t)lv_sqrt32((uint32_t)(ox * ox + oy * oy))
                               + LV_MIN(LV_MAX(dx, dy), 0) - radius_256;
            buf[y * SHADOW_TEX_SIZE + x] = gauss_cdf_lookup(dist_256, sigma_sqrt2_256);
        }
    }
#else
    float radius, blur;
    float inv_sigma_sqrt2 = texture_shape(ratio_idx, &radius, &blur);
    float centre = blur + radius;

    for(int32_t y = 0; y < SHADOW_TEX_SIZE; y++) {
        for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
            /* Signed distance to the rounded rectangle, positive outside */
            float dx = centre - ((float)x + 0.5f);
            float dy = centre - ((float)y + 0.5f);
            float ox = dx > 0.0f ? dx : 0.0f;
            float oy = dy > 0.0f ? dy : 0.0f;
            float inside = dx > dy ? dx : dy;
            if(inside > 0.0f) inside = 0.0f;
            float dist = sqrtf(ox * ox + oy * oy) + inside - radius;
            float alpha = 0.5f * (1.0f - fast_erf(dist * inv_sigma_sqrt2));

            buf[y * SHADOW_TEX_SIZE + x] = (uint8_t)(alpha * 255.0f + 0.5f);
        }
    }
#endif
}

/**
 * Generate 1D edge texture for straight shadow edges: transparent at x = 0,
 * the core's edge blur in, solid at x = SIZE-1.
 */
static void generate_edge_texture(uint8_t * buf, int32_t ratio_idx)
{
#if LV_DRAW_EVE5_NO_FLOAT
    int32_t radius_256, blur_256, sigma_sqrt2_256;
    texture_shape_256(ratio_idx, &radius_256, &blur_256, &sigma_sqrt2_256);

    for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
        buf[x] = gauss_cdf_lookup(blur_256 - (x * 256 + 128), sigma_sqrt2_256);
    }
#else
    float radius, blur;
    float inv_sigma_sqrt2 = texture_shape(ratio_idx, &radius, &blur);

    for(int32_t x = 0; x < SHADOW_TEX_SIZE; x++) {
        float dist = blur - ((float)x + 0.5f);
        float alpha = 0.5f * (1.0f - fast_erf(dist * inv_sigma_sqrt2));

        buf[x] = (uint8_t)(alpha * 255.0f + 0.5f);
    }
#endif
}

/**
 * Ensure shadow textures exist for the given ratio index.
 * Generates and uploads if not present or if evicted by GPU allocator.
 */
static bool ensure_shadow_textures(lv_draw_eve5_unit_t * u, int32_t ratio_idx)
{
    if(ratio_idx < 0 || ratio_idx >= SHADOW_TEX_SIZE) {
        LV_LOG_ERROR("EVE5: Invalid shadow ratio index %"PRId32, ratio_idx);
        return false;
    }

    lv_draw_eve5_shadow_slot_t * slot = &u->shadow_slots[ratio_idx];

    /* Corner texture (64×64 L8 = 4KB). GC-flagged: this function regenerates
     * the texture whenever the handle goes invalid (sweep on pre-BT820,
     * pressure eviction on BT820+). */
    uint32_t corner_addr = EVE_GpuAlloc_Get(u->allocator, slot->corner_handle);
    if(corner_addr == GA_INVALID) {
        uint32_t corner_bytes = SHADOW_TEX_SIZE * SHADOW_TEX_SIZE;

        slot->corner_handle = EVE_GpuAlloc_Alloc(u->allocator, corner_bytes, GA_GC_FLAG);
        corner_addr = EVE_GpuAlloc_Get(u->allocator, slot->corner_handle);

        if(corner_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate shadow corner texture");
            return false;
        }

        uint8_t * buf = lv_malloc(corner_bytes);
        if(!buf) {
            LV_LOG_ERROR("EVE5: Failed to allocate corner generation buffer");
            EVE_GpuAlloc_Free(u->allocator, slot->corner_handle);
            slot->corner_handle = GA_HANDLE_INVALID;
            return false;
        }

        generate_corner_texture(buf, ratio_idx);
        EVE_Hal_wrMem(u->hal, corner_addr, buf, corner_bytes);
        lv_free(buf);
        EVE_Hal_requestFenceBeforeSwap(u->hal);

        LV_LOG_INFO("EVE5: Generated shadow corner texture for ratio %"PRId32" at 0x%08"PRIx32,
                    ratio_idx, corner_addr);
    }

    /* Edge texture (64×1 L8 = 64 bytes, aligned to 4). GC-flagged like the
     * corner texture above. */
    uint32_t edge_addr = EVE_GpuAlloc_Get(u->allocator, slot->edge_handle);
    if(edge_addr == GA_INVALID) {
        uint32_t edge_bytes = ALIGN_UP(SHADOW_TEX_SIZE, 4);

        slot->edge_handle = EVE_GpuAlloc_Alloc(u->allocator, edge_bytes, GA_GC_FLAG);
        edge_addr = EVE_GpuAlloc_Get(u->allocator, slot->edge_handle);

        if(edge_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate shadow edge texture");
            return false;
        }

        uint8_t * buf = lv_malloc(edge_bytes);
        if(!buf) {
            LV_LOG_ERROR("EVE5: Failed to allocate edge generation buffer");
            EVE_GpuAlloc_Free(u->allocator, slot->edge_handle);
            slot->edge_handle = GA_HANDLE_INVALID;
            return false;
        }

        lv_memzero(buf, edge_bytes);
        generate_edge_texture(buf, ratio_idx);
        EVE_Hal_wrMem(u->hal, edge_addr, buf, edge_bytes);
        lv_free(buf);
        EVE_Hal_requestFenceBeforeSwap(u->hal);

        LV_LOG_INFO("EVE5: Generated shadow edge texture for ratio %"PRId32" at 0x%08"PRIx32,
                    ratio_idx, edge_addr);
    }

    return true;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve5_box_shadow_init(lv_draw_eve5_unit_t * u)
{
    for(int32_t i = 0; i < SHADOW_TEX_SIZE; i++) {
        u->shadow_slots[i].corner_handle = GA_HANDLE_INVALID;
        u->shadow_slots[i].edge_handle = GA_HANDLE_INVALID;
    }
}

/**
 * LVGL draws a shadow only outside its widget, which shows where the widget's
 * background doesn't cover it. Marks the widget's rounded rectangle, one pixel
 * smaller so the shadow reaches under its antialiased edge as in the software
 * renderer, in the stencil over the shadow's area and leaves the stencil test
 * drawing where it is clear. The caller restores the stencil state with its
 * saved context.
 */
static void exclude_widget_area(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t,
                                const lv_draw_box_shadow_dsc_t * dsc)
{
    EVE_HalContext * phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    lv_area_t bg = t->area;
    lv_area_increase(&bg, -1, -1);
    int32_t lx = layer->buf_area.x1;
    int32_t ly = layer->buf_area.y1;

    int32_t r_bg = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&bg), lv_area_get_height(&bg));
    if(r_bg > short_side / 2) r_bg = short_side / 2;

    lv_draw_eve5_clear_stencil(u, t->_real_area.x1 - lx, t->_real_area.y1 - ly,
                               t->_real_area.x2 - lx, t->_real_area.y2 - ly,
                               &t->clip_area, &layer->buf_area);
    EVE_CoDl_saveContext(phost);
    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
    EVE_CoDl_stencilOp(phost, KEEP, REPLACE);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 255, 255);
    lv_draw_eve5_draw_rect(u, bg.x1 - lx, bg.y1 - ly, bg.x2 - lx, bg.y2 - ly, r_bg,
                           &t->clip_area, &layer->buf_area);
    EVE_CoDl_restoreContext(phost);
    EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    EVE_CoDl_stencilFunc(phost, NOTEQUAL, 255, 255);
}

/* The 9 slices of a shadow, layer-local */
typedef struct {
    int32_t sx1, sy1, sx2, sy2;             /**< Shadow bounds, inclusive */
    int32_t corner_size;                    /**< Pixels the textures span */
    int32_t left, right, top, bottom;       /**< Widths of the slices on each side */
    int32_t ratio_idx;
} shadow_slices_t;

static bool shadow_slices(const lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, shadow_slices_t * sl)
{
    const lv_area_t * coords = &t->area;
    const lv_area_t * layer_area = &t->target_layer->buf_area;

    /* Core area: widget bounds with offset and spread applied */
    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2 + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1 + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2 + dsc->ofs_y + dsc->spread;

    int32_t r_sh = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side / 2) r_sh = short_side / 2;
    if(r_sh < 0) r_sh = 0;

    int32_t blur = (dsc->width + 1) / 2;
    sl->corner_size = 2 * blur + r_sh;
    if(sl->corner_size <= 0) return false;
    sl->ratio_idx = calc_ratio_index(r_sh, sl->corner_size);

    sl->sx1 = core_area.x1 - blur - layer_area->x1;
    sl->sy1 = core_area.y1 - blur - layer_area->y1;
    sl->sx2 = core_area.x2 + blur - layer_area->x1;
    sl->sy2 = core_area.y2 + blur - layer_area->y1;

    /* A shadow narrower than two slices is split between them */
    int32_t w = sl->sx2 - sl->sx1 + 1;
    int32_t h = sl->sy2 - sl->sy1 + 1;
    sl->left = LV_MIN(sl->corner_size, w / 2);
    sl->right = LV_MIN(sl->corner_size, w - sl->left);
    sl->top = LV_MIN(sl->corner_size, h / 2);
    sl->bottom = LV_MIN(sl->corner_size, h - sl->top);
    return true;
}

/* One slice of the bitmap set up, with its texture transform (s8.8, 15.8) */
static void shadow_slice(EVE_HalContext * phost, int32_t x, int32_t y, int32_t w, int32_t h,
                         int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f)
{
    if(w <= 0 || h <= 0) return;
    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, w, h);
    EVE_CoDl_bitmapTransformA(phost, a);
    EVE_CoDl_bitmapTransformB(phost, b);
    EVE_CoDl_bitmapTransformC(phost, c);
    EVE_CoDl_bitmapTransformD(phost, d);
    EVE_CoDl_bitmapTransformE(phost, e);
    EVE_CoDl_bitmapTransformF(phost, f);
    EVE_CoDl_vertex2f_0(phost, x, y);
}

/**
 * Draw the slices in the current color and opacity, within the scissor set to
 * the clip area. The textures are sampled from the shadow's outer edge, and
 * mirrored for the right and bottom slices.
 */
static void draw_shadow_slices(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, const shadow_slices_t * sl,
                               uint32_t corner_addr, uint32_t edge_addr)
{
    EVE_HalContext * phost = u->hal;
    lv_layer_t * layer = t->target_layer;

    int32_t scale = (SHADOW_TEX_SIZE * 256) / sl->corner_size;
    int32_t xl = sl->sx1 + sl->left;           /* Right of the left slices */
    int32_t xr = sl->sx2 + 1 - sl->right;      /* Left of the right slices */
    int32_t yt = sl->sy1 + sl->top;            /* Bottom of the top slices */
    int32_t yb = sl->sy2 + 1 - sl->bottom;     /* Top of the bottom slices */
    /* Pixel and texel centres are both at integer coordinates: the mirrored
     * slices start at the texture coordinate of their outermost pixel */
    int32_t cr = scale * (sl->right - 1);
    int32_t cb = scale * (sl->bottom - 1);

    /* CORNERS */
    EVE_CoDl_bitmapHandle(phost, SHADOW_BITMAP_HANDLE);
    EVE_CoDl_bitmapSource(phost, corner_addr);
    EVE_CoDl_bitmapLayout(phost, L8, SHADOW_TEX_SIZE, SHADOW_TEX_SIZE);
    EVE_CoDl_begin(phost, BITMAPS);
    shadow_slice(phost, sl->sx1, sl->sy1, sl->left, sl->top, scale, 0, 0, 0, scale, 0);
    shadow_slice(phost, xr, sl->sy1, sl->right, sl->top, -scale, 0, cr, 0, scale, 0);
    shadow_slice(phost, sl->sx1, yb, sl->left, sl->bottom, scale, 0, 0, 0, -scale, cb);
    shadow_slice(phost, xr, yb, sl->right, sl->bottom, -scale, 0, cr, 0, -scale, cb);

    /* EDGES: the one-row texture across the edge, rotated for the top and
     * bottom */
    EVE_CoDl_bitmapSource(phost, edge_addr);
    EVE_CoDl_bitmapLayout(phost, L8, SHADOW_TEX_SIZE, 1);
    shadow_slice(phost, xl, sl->sy1, xr - xl, sl->top, 0, scale, 0, 0, 0, 0);
    shadow_slice(phost, xl, yb, xr - xl, sl->bottom, 0, -scale, cb, 0, 0, 0);
    shadow_slice(phost, sl->sx1, yt, sl->left, yb - yt, scale, 0, 0, 0, 0, 0);
    shadow_slice(phost, xr, yt, sl->right, yb - yt, -scale, 0, cr, 0, 0, 0);
    EVE_CoDl_end(phost);

    /* CENTER: solid, as a scissored rect to avoid EVE RECTS alpha artifacts */
    if(xr > xl && yb > yt) {
        lv_area_t center_screen;
        center_screen.x1 = xl + layer->buf_area.x1;
        center_screen.y1 = yt + layer->buf_area.y1;
        center_screen.x2 = xr + layer->buf_area.x1 - 1;
        center_screen.y2 = yb + layer->buf_area.y1 - 1;

        lv_area_t center_scissor;
        if(lv_area_intersect(&center_scissor, &center_screen, &t->clip_area)) {
            lv_draw_eve5_set_scissor(u, &center_scissor, &layer->buf_area);
            EVE_CoDl_lineWidth(phost, 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, xl - 1, yt - 1);
            EVE_CoDl_vertex2f_0(phost, xr, yb);
            EVE_CoDl_end(phost);
            lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
        }
    }
}

/* Textures of a shadow's slices, made when missing */
static bool shadow_textures(lv_draw_eve5_unit_t * u, int32_t ratio_idx, uint32_t * corner_addr, uint32_t * edge_addr)
{
    if(!ensure_shadow_textures(u, ratio_idx)) return false;

    lv_draw_eve5_shadow_slot_t * slot = &u->shadow_slots[ratio_idx];
    *corner_addr = EVE_GpuAlloc_Get(u->allocator, slot->corner_handle);
    *edge_addr = EVE_GpuAlloc_Get(u->allocator, slot->edge_handle);
    if(*corner_addr == GA_INVALID || *edge_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Shadow textures evicted unexpectedly");
        return false;
    }
    return true;
}

/**
 * Render box shadow using 9-slice Gaussian textures.
 */
void lv_draw_eve5_hal_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    EVE_HalContext * phost = u->hal;
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    shadow_slices_t sl;
    uint32_t corner_addr, edge_addr;
    if(!shadow_slices(t, dsc, &sl)) return;
    if(!shadow_textures(u, sl.ratio_idx, &corner_addr, &edge_addr)) return;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);
    if(!dsc->bg_cover) exclude_widget_area(u, t, dsc);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &t->target_layer->buf_area);

    EVE_CoDl_colorRgb(phost, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(phost, dsc->opa);
    draw_shadow_slices(u, t, &sl, corner_addr, edge_addr);

    EVE_CoDl_restoreContext(phost);
}

/**********************
 * ALPHA PASS
 **********************/

/**
 * Draw box shadow alpha coverage for alpha recovery passes: the same slices.
 * L8 decodes as (R=255, G=255, B=255, A=L), so the shadow is the source
 * alpha: in white, the L8 render target captures it as luminance, and the
 * direct-to-alpha pass, which only writes alpha, as alpha.
 */
void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    EVE_HalContext * phost = u->hal;
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    shadow_slices_t sl;
    uint32_t corner_addr, edge_addr;
    if(!shadow_slices(t, dsc, &sl)) return;
    /* The L8 render target pass runs before the RGB pass that would make them */
    if(!shadow_textures(u, sl.ratio_idx, &corner_addr, &edge_addr)) return;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);
    if(!dsc->bg_cover) exclude_widget_area(u, t, dsc);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &t->target_layer->buf_area);

    if(alpha_to_rgb) EVE_CoDl_colorRgb(phost, 255, 255, 255);
    EVE_CoDl_colorA(phost, dsc->opa);
    draw_shadow_slices(u, t, &sl, corner_addr, edge_addr);

    EVE_CoDl_restoreContext(phost);
}

#endif /* LV_USE_DRAW_EVE5 */
