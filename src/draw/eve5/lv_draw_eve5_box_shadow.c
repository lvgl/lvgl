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
 * == Corner Texture Layout ==
 *
 *     (0,0) transparent -----> X
 *       |
 *       |     blur region
 *       |         |
 *       v         v
 *       Y    +---------+
 *            |    .--' |  <- Gaussian falloff
 *            |  .'     |
 *            | |  solid|  <- solid quarter-circle
 *            +---------+
 *                    (SIZE-1, SIZE-1) = alpha 255
 *
 * == Gaussian Math ==
 *
 * For each texel:
 *   signed_dist = distance_to_corner - solid_radius
 *   alpha = 0.5 × (1 - erf(signed_dist / (sigma × √2)))
 *
 * Produces smooth falloff: 255 inside, ~128 at boundary, 0 far outside.
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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void generate_corner_texture(uint8_t * buf, int32_t tex_size, int32_t solid_radius_idx);
static void generate_edge_texture(uint8_t * buf, int32_t tex_size, int32_t solid_radius_idx);
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

/**
 * Generate 2D corner texture for Gaussian shadow falloff.
 * Solid quarter-circle at (tex_size-1, tex_size-1), blur toward (0, 0).
 */
static void generate_corner_texture(uint8_t * buf, int32_t tex_size, int32_t solid_radius_idx)
{
#if LV_DRAW_EVE5_NO_FLOAT
    int32_t solid_radius_256 = (int32_t)((int64_t)solid_radius_idx * tex_size * 256 / (SHADOW_TEX_SIZE - 1));

    int32_t blur_region_256 = tex_size * 256 - solid_radius_256;
    if(blur_region_256 < 256) blur_region_256 = 256;

    int32_t sigma_256 = blur_region_256 * 45 / 100;
    if(sigma_256 < 128) sigma_256 = 128;

    int32_t sigma_sqrt2_256 = sigma_256 * 1414 / 1000;
    if(sigma_sqrt2_256 < 1) sigma_sqrt2_256 = 1;

    for(int32_t y = 0; y < tex_size; y++) {
        for(int32_t x = 0; x < tex_size; x++) {
            int32_t dx = tex_size - 1 - x;
            int32_t dy = tex_size - 1 - y;
            int32_t dist_256 = lv_sqrt32((uint32_t)(dx * dx + dy * dy) << 16);
            int32_t signed_dist_256 = dist_256 - solid_radius_256;

            buf[y * tex_size + x] = gauss_cdf_lookup(signed_dist_256, sigma_sqrt2_256);
        }
    }
#else
    float solid_radius = (float)solid_radius_idx * tex_size / (SHADOW_TEX_SIZE - 1);

    float blur_region = tex_size - solid_radius;
    if(blur_region < 1.0f) blur_region = 1.0f;

    float sigma = blur_region * 0.45f;
    if(sigma < 0.5f) sigma = 0.5f;

    float inv_sigma_sqrt2 = 1.0f / (sigma * 1.41421356f);

    for(int32_t y = 0; y < tex_size; y++) {
        for(int32_t x = 0; x < tex_size; x++) {
            float dx = (float)(tex_size - 1 - x);
            float dy = (float)(tex_size - 1 - y);
            float dist = sqrtf(dx * dx + dy * dy);
            float signed_dist = dist - solid_radius;
            float alpha = 0.5f * (1.0f - fast_erf(signed_dist * inv_sigma_sqrt2));

            buf[y * tex_size + x] = (uint8_t)(alpha * 255.0f + 0.5f);
        }
    }
#endif
}

/**
 * Generate 1D edge texture for straight shadow edges.
 * Solid at x = tex_size-1, transparent at x = 0.
 */
static void generate_edge_texture(uint8_t * buf, int32_t tex_size, int32_t solid_radius_idx)
{
#if LV_DRAW_EVE5_NO_FLOAT
    int32_t solid_width_256 = (int32_t)((int64_t)solid_radius_idx * tex_size * 256 / (SHADOW_TEX_SIZE - 1));
    int32_t blur_region_256 = tex_size * 256 - solid_width_256;
    if(blur_region_256 < 256) blur_region_256 = 256;

    int32_t sigma_256 = blur_region_256 * 45 / 100;
    if(sigma_256 < 128) sigma_256 = 128;

    int32_t sigma_sqrt2_256 = sigma_256 * 1414 / 1000;
    if(sigma_sqrt2_256 < 1) sigma_sqrt2_256 = 1;

    for(int32_t x = 0; x < tex_size; x++) {
        int32_t signed_dist_256 = (tex_size - 1 - x) * 256 - solid_width_256;
        buf[x] = gauss_cdf_lookup(signed_dist_256, sigma_sqrt2_256);
    }
#else
    float solid_width = (float)solid_radius_idx * tex_size / (SHADOW_TEX_SIZE - 1);

    float blur_region = tex_size - solid_width;
    if(blur_region < 1.0f) blur_region = 1.0f;

    float sigma = blur_region * 0.45f;
    if(sigma < 0.5f) sigma = 0.5f;

    float inv_sigma_sqrt2 = 1.0f / (sigma * 1.41421356f);

    for(int32_t x = 0; x < tex_size; x++) {
        float dist_from_solid = (float)(tex_size - 1 - x);
        float signed_dist = dist_from_solid - solid_width;
        float alpha = 0.5f * (1.0f - fast_erf(signed_dist * inv_sigma_sqrt2));

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

    /* Corner texture (64×64 L8 = 4KB) */
    uint32_t corner_addr = Esd_GpuAlloc_Get(u->allocator, slot->corner_handle);
    if(corner_addr == GA_INVALID) {
        uint32_t corner_bytes = SHADOW_TEX_SIZE * SHADOW_TEX_SIZE;

        slot->corner_handle = Esd_GpuAlloc_Alloc(u->allocator, corner_bytes, 0);
        corner_addr = Esd_GpuAlloc_Get(u->allocator, slot->corner_handle);

        if(corner_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate shadow corner texture");
            return false;
        }

        uint8_t * buf = lv_malloc(corner_bytes);
        if(!buf) {
            LV_LOG_ERROR("EVE5: Failed to allocate corner generation buffer");
            Esd_GpuAlloc_Free(u->allocator, slot->corner_handle);
            slot->corner_handle = GA_HANDLE_INVALID;
            return false;
        }

        generate_corner_texture(buf, SHADOW_TEX_SIZE, ratio_idx);
        EVE_Hal_wrMem(u->hal, corner_addr, buf, corner_bytes);
        lv_free(buf);
        EVE_Hal_requestFenceBeforeSwap(u->hal);

        LV_LOG_INFO("EVE5: Generated shadow corner texture for ratio %"PRId32" at 0x%08"PRIx32,
                    ratio_idx, corner_addr);
    }

    /* Edge texture (64×1 L8 = 64 bytes, aligned to 4) */
    uint32_t edge_addr = Esd_GpuAlloc_Get(u->allocator, slot->edge_handle);
    if(edge_addr == GA_INVALID) {
        uint32_t edge_bytes = ALIGN_UP(SHADOW_TEX_SIZE, 4);

        slot->edge_handle = Esd_GpuAlloc_Alloc(u->allocator, edge_bytes, 0);
        edge_addr = Esd_GpuAlloc_Get(u->allocator, slot->edge_handle);

        if(edge_addr == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate shadow edge texture");
            return false;
        }

        uint8_t * buf = lv_malloc(edge_bytes);
        if(!buf) {
            LV_LOG_ERROR("EVE5: Failed to allocate edge generation buffer");
            Esd_GpuAlloc_Free(u->allocator, slot->edge_handle);
            slot->edge_handle = GA_HANDLE_INVALID;
            return false;
        }

        lv_memzero(buf, edge_bytes);
        generate_edge_texture(buf, SHADOW_TEX_SIZE, ratio_idx);
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
 * Render box shadow using 9-slice Gaussian textures.
 */
void lv_draw_eve5_hal_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t)
{
    EVE_HalContext *phost = u->hal;

    lv_layer_t * layer = t->target_layer;
    lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    const lv_area_t * coords = &t->area;

    /* Core area: widget bounds with offset and spread applied */
    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2 + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1 + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2 + dsc->ofs_y + dsc->spread;

    int32_t r_sh = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side / 2) r_sh = short_side / 2;

    int32_t blur_radius = (dsc->width + 1) / 2;
    int32_t corner_size = blur_radius + r_sh;
    if(corner_size <= 0) return;

    int32_t ratio_idx = calc_ratio_index(r_sh, corner_size);

    if(!ensure_shadow_textures(u, ratio_idx)) {
        return;
    }

    lv_draw_eve5_shadow_slot_t * slot = &u->shadow_slots[ratio_idx];
    uint32_t corner_addr = Esd_GpuAlloc_Get(u->allocator, slot->corner_handle);
    uint32_t edge_addr = Esd_GpuAlloc_Get(u->allocator, slot->edge_handle);

    if(corner_addr == GA_INVALID || edge_addr == GA_INVALID) {
        LV_LOG_WARN("EVE5: Shadow textures evicted unexpectedly");
        return;
    }

    /* Shadow bounding box: core expanded by blur_radius */
    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - blur_radius;
    shadow_area.x2 = core_area.x2 + blur_radius;
    shadow_area.y1 = core_area.y1 - blur_radius;
    shadow_area.y2 = core_area.y2 + blur_radius;

    /* Convert to layer-local coordinates */
    int32_t lx = layer->buf_area.x1;
    int32_t ly = layer->buf_area.y1;

    int32_t sx1 = shadow_area.x1 - lx;
    int32_t sy1 = shadow_area.y1 - ly;
    int32_t sx2 = shadow_area.x2 - lx;
    int32_t sy2 = shadow_area.y2 - ly;

    int32_t shadow_w = sx2 - sx1 + 1;
    int32_t shadow_h = sy2 - sy1 + 1;

    /* Clamp render corner size to prevent overlap on pill/circle shapes */
    int32_t render_corner_w = LV_MIN(corner_size, shadow_w / 2);
    int32_t render_corner_h = LV_MIN(corner_size, shadow_h / 2);

    /* Texture scale: maps output pixels to texture coordinates (s8.8 format) */
    int32_t scale = (SHADOW_TEX_SIZE * 256) / corner_size;
    int32_t tex_max = (SHADOW_TEX_SIZE - 1) * 256;

    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_saveContext(phost);
    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorRgb(phost, dsc->color.red, dsc->color.green, dsc->color.blue);
    EVE_CoDl_colorA(phost, dsc->opa);

    /*
     * CORNERS: Texture has solid at (SIZE-1, SIZE-1). Each corner is flipped
     * so the solid region points toward the widget center.
     */

    EVE_CoDl_bitmapHandle(phost, SHADOW_BITMAP_HANDLE);
    EVE_CoDl_bitmapSource(phost, corner_addr);
    EVE_CoDl_bitmapLayout(phost, L8, SHADOW_TEX_SIZE, SHADOW_TEX_SIZE);
    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, render_corner_w, render_corner_h);

    EVE_CoDl_begin(phost, BITMAPS);

    /* Top-left: identity */
    EVE_CoDl_bitmapTransformA(phost, scale);
    EVE_CoDl_bitmapTransformB(phost, 0);
    EVE_CoDl_bitmapTransformC(phost, 0);
    EVE_CoDl_bitmapTransformD(phost, 0);
    EVE_CoDl_bitmapTransformE(phost, scale);
    EVE_CoDl_bitmapTransformF(phost, 0);
    EVE_CoDl_vertex2f_0(phost, sx1, sy1);

    /* Top-right: flip X */
    EVE_CoDl_bitmapTransformA(phost, -scale);
    EVE_CoDl_bitmapTransformC(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy1);

    /* Bottom-left: flip Y */
    EVE_CoDl_bitmapTransformA(phost, scale);
    EVE_CoDl_bitmapTransformC(phost, 0);
    EVE_CoDl_bitmapTransformE(phost, -scale);
    EVE_CoDl_bitmapTransformF(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx1, sy2 + 1 - render_corner_h);

    /* Bottom-right: flip both */
    EVE_CoDl_bitmapTransformA(phost, -scale);
    EVE_CoDl_bitmapTransformC(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy2 + 1 - render_corner_h);

    EVE_CoDl_end(phost);

    /*
     * EDGES: 1D texture with solid at x=SIZE-1, transparent at x=0.
     * Rotated 90° for horizontal edges. Flipped so solid faces widget.
     */

    int32_t edge_h_len = shadow_w - 2 * render_corner_w;
    int32_t edge_v_len = shadow_h - 2 * render_corner_h;
    int32_t edge_scale = (SHADOW_TEX_SIZE * 256) / corner_size;
    int32_t edge_tex_max = (SHADOW_TEX_SIZE - 1) * 256;

    EVE_CoDl_bitmapSource(phost, edge_addr);
    EVE_CoDl_bitmapLayout(phost, L8, SHADOW_TEX_SIZE, 1);

    /* Horizontal edges */
    if(edge_h_len > 0) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, edge_h_len, render_corner_h);

        /* Top edge */
        EVE_CoDl_bitmapTransformA(phost, 0);
        EVE_CoDl_bitmapTransformB(phost, edge_scale);
        EVE_CoDl_bitmapTransformC(phost, 0);
        EVE_CoDl_bitmapTransformD(phost, 0);
        EVE_CoDl_bitmapTransformE(phost, 0);
        EVE_CoDl_bitmapTransformF(phost, 0);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1 + render_corner_w, sy1);
        EVE_CoDl_end(phost);

        /* Bottom edge */
        EVE_CoDl_bitmapTransformB(phost, -edge_scale);
        EVE_CoDl_bitmapTransformC(phost, edge_tex_max);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1 + render_corner_w, sy2 + 1 - render_corner_h);
        EVE_CoDl_end(phost);
    }

    /* Vertical edges */
    if(edge_v_len > 0) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, render_corner_w, edge_v_len);

        /* Left edge */
        EVE_CoDl_bitmapTransformA(phost, edge_scale);
        EVE_CoDl_bitmapTransformB(phost, 0);
        EVE_CoDl_bitmapTransformC(phost, 0);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1, sy1 + render_corner_h);
        EVE_CoDl_end(phost);

        /* Right edge */
        EVE_CoDl_bitmapTransformA(phost, -edge_scale);
        EVE_CoDl_bitmapTransformC(phost, edge_tex_max);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy1 + render_corner_h);
        EVE_CoDl_end(phost);
    }

    /* CENTER: solid fill between edge regions */
    int32_t cx1 = sx1 + render_corner_w;
    int32_t cy1 = sy1 + render_corner_h;
    int32_t cx2 = sx2 + 1 - render_corner_w;
    int32_t cy2 = sy2 + 1 - render_corner_h;

    if(cx2 > cx1 && cy2 > cy1) {
        /* Use scissored rect to avoid EVE RECTS alpha artifacts */
        lv_area_t center_screen;
        center_screen.x1 = cx1 + layer->buf_area.x1;
        center_screen.y1 = cy1 + layer->buf_area.y1;
        center_screen.x2 = cx2 + layer->buf_area.x1 - 1;
        center_screen.y2 = cy2 + layer->buf_area.y1 - 1;

        lv_area_t center_scissor;
        if(lv_area_intersect(&center_scissor, &center_screen, &t->clip_area)) {
            lv_draw_eve5_set_scissor(u, &center_scissor, &layer->buf_area);

            int32_t radius = 1;
            EVE_CoDl_lineWidth(phost, radius * 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, cx1 - 1, cy1 - 1);
            EVE_CoDl_vertex2f_0(phost, cx2, cy2);
            EVE_CoDl_end(phost);

            lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
        }
    }

    EVE_CoDl_restoreContext(phost);
}

/**********************
 * ALPHA PASS
 **********************/

/**
 * Draw box shadow alpha coverage for alpha recovery passes.
 *
 * BT820 L8 decodes as (R=255, G=255, B=255, A=L), so luminance maps to alpha.
 * During RGB render, the varying A modulates the shadow color via blend.
 *
 * For alpha_to_rgb (L8 render-target): draw flat rect at dsc->opa since the
 * L8 render target captures luminance, not the source texture's L8 gradient.
 *
 * For direct-to-alpha: redraw 9-slice with L8 textures. Since A=L, the
 * Gaussian gradient is captured as varying alpha, modulated by colorA(dsc->opa).
 */
void lv_draw_eve5_alpha_draw_box_shadow(lv_draw_eve5_unit_t * u, const lv_draw_task_t * t, bool alpha_to_rgb)
{
    EVE_HalContext *phost = u->hal;
    lv_layer_t * layer = t->target_layer;
    const lv_draw_box_shadow_dsc_t * dsc = t->draw_dsc;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width <= 0) return;

    const lv_area_t * coords = &t->area;
    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2 + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1 + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2 + dsc->ofs_y + dsc->spread;

    int32_t r_sh = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side / 2) r_sh = short_side / 2;

    int32_t blur_radius = (dsc->width + 1) / 2;
    int32_t corner_size = blur_radius + r_sh;
    if(corner_size <= 0) return;

    int32_t lx = layer->buf_area.x1;
    int32_t ly = layer->buf_area.y1;
    int32_t sx1 = core_area.x1 - blur_radius - lx;
    int32_t sy1 = core_area.y1 - blur_radius - ly;
    int32_t sx2 = core_area.x2 + blur_radius - lx;
    int32_t sy2 = core_area.y2 + blur_radius - ly;

    if(alpha_to_rgb) {
        /* L8 render-target: flat rect at dsc->opa */
        lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);
        EVE_CoDl_colorA(phost, dsc->opa);
        EVE_CoDl_lineWidth(phost, 16);
        EVE_CoDl_begin(phost, RECTS);
        EVE_CoDl_vertex2f_0(phost, sx1, sy1);
        EVE_CoDl_vertex2f_0(phost, sx2, sy2);
        EVE_CoDl_end(phost);
        return;
    }

    /* Direct-to-alpha: redraw 9-slice */
    int32_t ratio_idx = (r_sh * (EVE5_SHADOW_TEX_SIZE - 1)) / corner_size;
    if(ratio_idx < 0) ratio_idx = 0;
    if(ratio_idx >= EVE5_SHADOW_TEX_SIZE) ratio_idx = EVE5_SHADOW_TEX_SIZE - 1;

    lv_draw_eve5_shadow_slot_t * slot = &u->shadow_slots[ratio_idx];
    uint32_t corner_addr = Esd_GpuAlloc_Get(u->allocator, slot->corner_handle);
    uint32_t edge_addr = Esd_GpuAlloc_Get(u->allocator, slot->edge_handle);

    if(corner_addr == GA_INVALID || edge_addr == GA_INVALID) return;

    int32_t shadow_w = sx2 - sx1 + 1;
    int32_t shadow_h = sy2 - sy1 + 1;

    int32_t render_corner_w = LV_MIN(corner_size, shadow_w / 2);
    int32_t render_corner_h = LV_MIN(corner_size, shadow_h / 2);

    int32_t scale = (EVE5_SHADOW_TEX_SIZE * 256) / corner_size;
    int32_t tex_max = (EVE5_SHADOW_TEX_SIZE - 1) * 256;

    lv_draw_eve5_set_scissor(u, &t->clip_area, &layer->buf_area);

    EVE_CoDl_colorA(phost, dsc->opa);

    /* Corners */
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, corner_addr);
    EVE_CoDl_bitmapLayout(phost, L8, EVE5_SHADOW_TEX_SIZE, EVE5_SHADOW_TEX_SIZE);
    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, render_corner_w, render_corner_h);

    EVE_CoDl_begin(phost, BITMAPS);

    EVE_CoDl_bitmapTransformA(phost, scale);
    EVE_CoDl_bitmapTransformB(phost, 0);
    EVE_CoDl_bitmapTransformC(phost, 0);
    EVE_CoDl_bitmapTransformD(phost, 0);
    EVE_CoDl_bitmapTransformE(phost, scale);
    EVE_CoDl_bitmapTransformF(phost, 0);
    EVE_CoDl_vertex2f_0(phost, sx1, sy1);

    EVE_CoDl_bitmapTransformA(phost, -scale);
    EVE_CoDl_bitmapTransformC(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy1);

    EVE_CoDl_bitmapTransformA(phost, scale);
    EVE_CoDl_bitmapTransformC(phost, 0);
    EVE_CoDl_bitmapTransformE(phost, -scale);
    EVE_CoDl_bitmapTransformF(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx1, sy2 + 1 - render_corner_h);

    EVE_CoDl_bitmapTransformA(phost, -scale);
    EVE_CoDl_bitmapTransformC(phost, tex_max);
    EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy2 + 1 - render_corner_h);

    EVE_CoDl_end(phost);

    /* Edges */
    int32_t edge_h_len = shadow_w - 2 * render_corner_w;
    int32_t edge_v_len = shadow_h - 2 * render_corner_h;
    int32_t edge_scale = (EVE5_SHADOW_TEX_SIZE * 256) / corner_size;
    int32_t edge_tex_max = (EVE5_SHADOW_TEX_SIZE - 1) * 256;

    EVE_CoDl_bitmapSource(phost, edge_addr);
    EVE_CoDl_bitmapLayout(phost, L8, EVE5_SHADOW_TEX_SIZE, 1);

    if(edge_h_len > 0) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, edge_h_len, render_corner_h);

        EVE_CoDl_bitmapTransformA(phost, 0);
        EVE_CoDl_bitmapTransformB(phost, edge_scale);
        EVE_CoDl_bitmapTransformC(phost, 0);
        EVE_CoDl_bitmapTransformD(phost, 0);
        EVE_CoDl_bitmapTransformE(phost, 0);
        EVE_CoDl_bitmapTransformF(phost, 0);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1 + render_corner_w, sy1);
        EVE_CoDl_end(phost);

        EVE_CoDl_bitmapTransformB(phost, -edge_scale);
        EVE_CoDl_bitmapTransformC(phost, edge_tex_max);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1 + render_corner_w, sy2 + 1 - render_corner_h);
        EVE_CoDl_end(phost);
    }

    if(edge_v_len > 0) {
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, render_corner_w, edge_v_len);

        EVE_CoDl_bitmapTransformA(phost, edge_scale);
        EVE_CoDl_bitmapTransformB(phost, 0);
        EVE_CoDl_bitmapTransformC(phost, 0);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx1, sy1 + render_corner_h);
        EVE_CoDl_end(phost);

        EVE_CoDl_bitmapTransformA(phost, -edge_scale);
        EVE_CoDl_bitmapTransformC(phost, edge_tex_max);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, sx2 + 1 - render_corner_w, sy1 + render_corner_h);
        EVE_CoDl_end(phost);
    }

    /* Center */
    int32_t cx1 = sx1 + render_corner_w;
    int32_t cy1 = sy1 + render_corner_h;
    int32_t cx2 = sx2 + 1 - render_corner_w;
    int32_t cy2 = sy2 + 1 - render_corner_h;

    if(cx2 > cx1 && cy2 > cy1) {
        lv_area_t center_screen;
        center_screen.x1 = cx1 + layer->buf_area.x1;
        center_screen.y1 = cy1 + layer->buf_area.y1;
        center_screen.x2 = cx2 + layer->buf_area.x1 - 1;
        center_screen.y2 = cy2 + layer->buf_area.y1 - 1;

        lv_area_t center_scissor;
        if(lv_area_intersect(&center_scissor, &center_screen, &t->clip_area)) {
            lv_draw_eve5_set_scissor(u, &center_scissor, &layer->buf_area);

            int32_t radius = 1;
            EVE_CoDl_lineWidth(phost, radius * 16);
            EVE_CoDl_begin(phost, RECTS);
            EVE_CoDl_vertex2f_0(phost, cx1 - 1, cy1 - 1);
            EVE_CoDl_vertex2f_0(phost, cx2, cy2);
            EVE_CoDl_end(phost);
        }
    }
}

#endif /* LV_USE_DRAW_EVE5 */
