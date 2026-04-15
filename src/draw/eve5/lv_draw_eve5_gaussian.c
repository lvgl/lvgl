/**
 * @file lv_draw_eve5_gaussian.c
 *
 * EVE5 (BT820) High-Quality Gaussian Blur via Mipmap Downsampling
 *
 * Separable 5-tap [1,4,6,4,1]/16 Gaussian kernel with nearest sampling
 * throughout the pyramid, bilinear only on final upscale. Each pyramid
 * tier applies H blur+2x downsample then V blur+2x downsample using
 * 5 nearest-sampled bitmap draws per pass. Optional extra blur passes
 * at the deepest level provide fine-grained sigma control without
 * further resolution loss. Final output is bilinear-upscaled and
 * composited with optional rounded-corner masking.
 *
 * Kernel sigma_local = 1 per pass. Per-tier contribution to original
 * sigma^2 = 4^tier. Accumulated sigma^2 after n tiers = (4^n - 1) / 3:
 *   1 tier:  sigma = 1.0      4 tiers: sigma = 9.2
 *   2 tiers: sigma = 2.2      5 tiers: sigma = 18.5
 *   3 tiers: sigma = 4.6      6 tiers: sigma = 37.0
 *
 * Extra blur passes at the deepest level each add 4^n to sigma^2
 * (same contribution as one more tier, but without halving resolution).
 *
 * 5 nearest fetches per output pixel per pass vs 4 bilinear (16 nearest)
 * for the old mipmap approach -- fewer fetches AND a proper Gaussian kernel.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_blur.h"

/*********************
 * DEFINES
 *********************/
#define MAX_PYRAMID_TIERS 8
#define MAX_EXTRA_PASSES  4
#define MAX_BLUR_LEVELS   (1 + MAX_PYRAMID_TIERS + MAX_EXTRA_PASSES)

/* 5-tap Gaussian weights: [1, 4, 6, 4, 1] / 16
 * Scaled to 8-bit color: 16, 64, 96, 64, 16 (sum = 256 ~ 1.0) */
#define GAUSS_W_OUTER  16
#define GAUSS_W_INNER  64
#define GAUSS_W_CENTER 96

/**********************
 * TYPES
 **********************/
typedef struct {
    Esd_GpuHandle handle;
    int32_t w, h;       /**< Logical pixel dimensions */
    int32_t aw, ah;     /**< 16-byte aligned dimensions for render target */
    int32_t sigma_sq;   /**< Accumulated variance in original pixels */
} gauss_level_t;

/**********************
 * STATIC PROTOTYPES
 **********************/
static bool gaussian_5tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal, bool downsample);

/**********************
 * 5-TAP GAUSSIAN PASS
 **********************/

/**
 * One separable 5-tap Gaussian blur pass with optional 2x downsample.
 *
 * Renders 5 nearest-sampled bitmap draws with additive blending,
 * weighted by the [1,4,6,4,1]/16 binomial kernel. Each draw samples
 * at a different source offset along the blur axis.
 *
 * With downsample=true, the transform scales 2:1 along the blur axis,
 * so each output pixel's 5 taps cover a 5-pixel window in the source
 * and the output is half the source size along that axis.
 *
 * @param horizontal  true for horizontal blur, false for vertical
 * @param downsample  true for 2x downsample along blur axis
 */
static bool gaussian_5tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal, bool downsample)
{
    EVE_HalContext *phost = u->hal;

    /* 2.0 in unsigned 8.8 for downsample, 1.0 for same-resolution blur */
    int32_t scale_8_8 = downsample ? 0x0200 : 0x0100;

    EVE_CoCmd_renderTarget(phost, dst_addr, ARGB8, dst_aw, dst_ah);
    EVE_CoCmd_dlStart(phost);

    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clearColorA(phost, 0);
    EVE_CoDl_clear(phost, 1, 1, 1);

    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, dst_w, dst_h);
    EVE_CoDl_vertexFormat(phost, 0);

    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, src_addr);
    EVE_CoDl_bitmapLayout(phost, ARGB8, src_stride, src_h);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, dst_w, dst_h);

    /* Scale along blur axis, identity on the other.
     * Transform: src_coord = A * out_x + C (horizontal) or E * out_y + F (vertical) */
    EVE_CoDl_bitmapTransform_identity(phost);
    if(horizontal) {
        EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_8_8);
    }
    else {
        EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_8_8);
    }

    /* Additive blend: accumulate weighted taps into cleared-to-zero target */
    EVE_CoDl_blendFunc(phost, ONE, ONE);

    static const int32_t offsets[5] = { -2, -1, 0, 1, 2 };
    static const uint8_t weights[5] = { GAUSS_W_OUTER, GAUSS_W_INNER, GAUSS_W_CENTER,
                                         GAUSS_W_INNER, GAUSS_W_OUTER
                                       };

    EVE_CoDl_begin(phost, BITMAPS);

    for(int i = 0; i < 5; i++) {
        EVE_CoDl_colorRgb(phost, weights[i], weights[i], weights[i]);
        EVE_CoDl_colorA(phost, weights[i]);

        /* Offset in 15.8 fixed point: 1 source pixel = 256 */
        int32_t ofs = offsets[i] * 256;

        if(horizontal) {
            EVE_CoDl_bitmapTransformC(phost, ofs);
        }
        else {
            EVE_CoDl_bitmapTransformF(phost, ofs);
        }

        EVE_CoDl_vertex2f_0(phost, 0, 0);
    }

    EVE_CoDl_end(phost);

    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);

    return true;
}

/**********************
 * GAUSSIAN BLUR ENTRY POINT
 **********************/

/**
 * High-quality Gaussian blur of a region within a completed layer buffer.
 * Modifies dst_handle in place.
 *
 * Pipeline:
 * 1. Extract blur region with edge-extended padding
 * 2. Build Gaussian pyramid (separable 5-tap H+V per tier)
 * 3. Optional extra blur passes at deepest level
 * 4. Bilinear-upscale interpolated result back to layer
 */
bool lv_draw_eve5_gaussian_blur(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                Esd_GpuHandle dst_handle, const lv_draw_task_t * blur_task)
{
    EVE_HalContext *phost = u->hal;
    const lv_draw_blur_dsc_t * dsc = blur_task->draw_dsc;
    const lv_area_t * layer_area = &layer->buf_area;

    int32_t blur_radius = dsc->blur_radius;
    int32_t corner_radius = dsc->corner_radius;

    if(blur_radius <= 0) return true;

    /* Full blur area in layer-relative coords (for rounded rect geometry).
     * Uses FULL area so tiled strips produce consistent rounded rect portions. */
    int32_t full_x1 = blur_task->area.x1 - layer_area->x1;
    int32_t full_y1 = blur_task->area.y1 - layer_area->y1;
    int32_t full_x2 = blur_task->area.x2 - layer_area->x1;
    int32_t full_y2 = blur_task->area.y2 - layer_area->y1;
    int32_t full_w = full_x2 - full_x1 + 1;
    int32_t full_h = full_y2 - full_y1 + 1;

    /* Clipped blur area: what we can actually read/write in this tile */
    lv_area_t blur_clip;
    if(!lv_area_intersect(&blur_clip, &blur_task->area, &blur_task->clip_area)) return true;

    int32_t bx1 = blur_clip.x1 - layer_area->x1;
    int32_t by1 = blur_clip.y1 - layer_area->y1;
    int32_t bx2 = blur_clip.x2 - layer_area->x1;
    int32_t by2 = blur_clip.y2 - layer_area->y1;
    int32_t bw = bx2 - bx1 + 1;
    int32_t bh = by2 - by1 + 1;

    if(bw <= 0 || bh <= 0) return true;

    int32_t short_side = LV_MIN(full_w, full_h);
    if(corner_radius > short_side / 2) corner_radius = short_side / 2;
    if(corner_radius < 0) corner_radius = 0;

    int32_t layer_w = lv_area_get_width(layer_area);
    int32_t layer_h = lv_area_get_height(layer_area);
    int32_t layer_aw = ALIGN_UP(layer_w, 16);
    int32_t layer_ah = ALIGN_UP(layer_h, 16);
    uint32_t layer_stride = (uint32_t)layer_aw * 4;

    uint32_t dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    if(dst_addr == GA_INVALID) return false;

    /* LVGL blur_radius maps to sigma = blur_radius / 2 (matching the SW renderer's
     * convention where blur_radius is the visual extent, roughly 2-3 sigma). */
    int32_t sigma_sq_target = (blur_radius * blur_radius + 2) / 4;
    if(sigma_sq_target < 1) sigma_sq_target = 1;

    /* Pre-estimate pyramid depth for padding computation.
     * sigma^2 after n tiers = (4^n - 1) / 3. Find n where this >= sigma_sq_target. */
    int32_t n_tiers_est = 0;
    {
        int32_t acc = 0, p4 = 1;
        int32_t test_w = bw + 2 * blur_radius;
        int32_t test_h = bh + 2 * blur_radius;
        while(n_tiers_est < MAX_PYRAMID_TIERS) {
            test_w = (test_w + 1) / 2;
            test_h = (test_h + 1) / 2;
            if(test_w < 4 || test_h < 4) break;
            acc += p4;
            p4 *= 4;
            n_tiers_est++;
            if(acc >= sigma_sq_target) break;
        }
    }

    /* Padding must cover the cumulative 5-tap kernel reach through all tiers.
     * At tier k the kernel reaches +/-2 pixels at scale 2^k = +/-2*2^k original pixels.
     * Sum through n tiers: 2*(2^0 + 2^1 + ... + 2^(n-1)) = 2*(2^n - 1).
     * Also keep blur_radius as a floor so the extraction region is visually adequate. */
    int32_t kernel_reach = 2 * ((1 << n_tiers_est) - 1);
    if(n_tiers_est == 0) kernel_reach = 2;  /* single tier: just +/-2 */
    int32_t pad = LV_MAX(blur_radius, kernel_reach);

    /* ========== Padded content dimensions (16px RT alignment) ========== */
    int32_t pw = ALIGN_UP(bw + 2 * pad, 16);
    int32_t ph = ALIGN_UP(bh + 2 * pad, 16);
    pad = (pw - bw) / 2;
    int32_t pad_y = (ph - bh) / 2;
    int32_t paw = ALIGN_UP(pw, 16);
    int32_t pah = ALIGN_UP(ph, 16);

    /* ========== Phase 1: Extract blur region with edge padding ========== */

    uint32_t extract_size = (uint32_t)paw * 4 * (uint32_t)pah;
    Esd_GpuHandle extract_handle = Esd_GpuAlloc_Alloc(u->allocator, extract_size, GA_ALIGN_128);
    uint32_t extract_addr = Esd_GpuAlloc_Get(u->allocator, extract_handle);
    if(extract_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 gaussian: failed to allocate extraction buffer (%"PRIu32" bytes)", extract_size);
        return false;
    }

    {
        uint32_t src_ofs = (uint32_t)by1 * layer_stride + (uint32_t)bx1 * 4;

        EVE_CoCmd_renderTarget(phost, extract_addr, ARGB8, paw, pah);
        EVE_CoCmd_dlStart(phost);

        EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
        EVE_CoDl_clearColorA(phost, 0);
        EVE_CoDl_clear(phost, 1, 1, 1);

        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_bitmapSource(phost, dst_addr + src_ofs);
        EVE_CoDl_bitmapLayout(phost, ARGB8, layer_stride, layer_h - by1);

        /* Draw 1: stretch source to fill padded buffer (extends edges into padding).
         * NEAREST prevents dark fringe from BILINEAR interpolating with BORDER black. */
        {
            uint32_t scale_a = (uint32_t)bw * 256 / (uint32_t)pw;
            uint32_t scale_e = (uint32_t)bh * 256 / (uint32_t)ph;
            int32_t ofs_c = ((int32_t)bw * 256 - (int32_t)(scale_a * (uint32_t)pw)) / 2;
            int32_t ofs_f = ((int32_t)bh * 256 - (int32_t)(scale_e * (uint32_t)ph)) / 2;

            EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, pw, ph);
            EVE_CoDl_bitmapTransform_identity(phost);
            EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_a);
            EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_e);
            EVE_CoDl_bitmapTransformC(phost, ofs_c);
            EVE_CoDl_bitmapTransformF(phost, ofs_f);
            EVE_CoDl_blendFunc(phost, ONE, ZERO);

            EVE_CoDl_begin(phost, BITMAPS);
            EVE_CoDl_vertex2f_0(phost, 0, 0);
            EVE_CoDl_end(phost);
        }

        /* Draw 2: blit source 1:1 at center (exact pixels override stretched edges) */
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, bw, bh);
        EVE_CoDl_bitmapTransform_identity(phost);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, pad, pad_y);
        EVE_CoDl_end(phost);

        EVE_CoDl_display(phost);
        EVE_CoCmd_swap(phost);
        EVE_CoCmd_graphicsFinish(phost);
    }

    /* ========== Allocate reusable H-pass temp buffer ========== */
    /* Max temp size at tier 0: (pw/2 x ph). All deeper tiers and extra
     * passes (at the deepest level's resolution) fit within this. */
    int32_t temp_aw = ALIGN_UP((pw + 1) / 2, 16);
    int32_t temp_ah = ALIGN_UP(ph, 16);
    uint32_t temp_size = (uint32_t)temp_aw * 4 * (uint32_t)temp_ah;
    Esd_GpuHandle temp_handle = Esd_GpuAlloc_Alloc(u->allocator, temp_size, GA_ALIGN_128);
    if(Esd_GpuAlloc_Get(u->allocator, temp_handle) == GA_INVALID) {
        LV_LOG_WARN("EVE5 gaussian: failed to allocate temp buffer");
        Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
        return true;
    }

    /* ========== Phase 2: Build Gaussian pyramid ========== */
    gauss_level_t levels[MAX_BLUR_LEVELS];
    int32_t n_levels = 0;

    /* Level 0: the unblurred padded extraction */
    levels[0].handle = extract_handle;
    levels[0].w = pw;
    levels[0].h = ph;
    levels[0].aw = paw;
    levels[0].ah = pah;
    levels[0].sigma_sq = 0;
    n_levels = 1;

    int32_t pow4 = 1;  /* 4^tier, tracks this tier's sigma^2 contribution */

    for(int32_t tier = 0; tier < MAX_PYRAMID_TIERS; tier++) {
        gauss_level_t * prev = &levels[n_levels - 1];

        int32_t next_w = (prev->w + 1) / 2;
        int32_t next_h = (prev->h + 1) / 2;
        if(next_w < 4 || next_h < 4) break;
        if(n_levels >= MAX_BLUR_LEVELS) break;

        int32_t next_aw = ALIGN_UP(next_w, 16);
        int32_t next_ah = ALIGN_UP(next_h, 16);

        /* Allocate level output */
        uint32_t level_size = (uint32_t)next_aw * 4 * (uint32_t)next_ah;
        Esd_GpuHandle level_handle = Esd_GpuAlloc_Alloc(u->allocator, level_size, GA_ALIGN_128);
        if(Esd_GpuAlloc_Get(u->allocator, level_handle) == GA_INVALID) {
            LV_LOG_WARN("EVE5 gaussian: failed to allocate pyramid level %"PRId32, tier);
            break;
        }

        /* H intermediate dimensions: half-width x full-height */
        int32_t h_w = next_w;
        int32_t h_h = prev->h;
        int32_t h_aw = ALIGN_UP(h_w, 16);
        int32_t h_ah = ALIGN_UP(h_h, 16);

        /* Re-resolve addresses after allocation */
        uint32_t prev_addr = Esd_GpuAlloc_Get(u->allocator, prev->handle);
        uint32_t t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
        uint32_t lev_addr = Esd_GpuAlloc_Get(u->allocator, level_handle);
        if(prev_addr == GA_INVALID || t_addr == GA_INVALID || lev_addr == GA_INVALID) break;

        /* H blur + 2x downsample: prev -> temp */
        gaussian_5tap_pass(u, prev_addr, prev->aw * 4, prev->h,
                           t_addr, h_aw, h_ah, h_w, h_h,
                           true, true);

        /* Re-resolve after graphicsFinish */
        t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
        lev_addr = Esd_GpuAlloc_Get(u->allocator, level_handle);
        if(t_addr == GA_INVALID || lev_addr == GA_INVALID) break;

        /* V blur + 2x downsample: temp -> level */
        gaussian_5tap_pass(u, t_addr, h_aw * 4, h_h,
                           lev_addr, next_aw, next_ah, next_w, next_h,
                           false, true);

        levels[n_levels].handle = level_handle;
        levels[n_levels].w = next_w;
        levels[n_levels].h = next_h;
        levels[n_levels].aw = next_aw;
        levels[n_levels].ah = next_ah;
        levels[n_levels].sigma_sq = prev->sigma_sq + pow4;
        n_levels++;
        pow4 *= 4;

        if(levels[n_levels - 1].sigma_sq >= sigma_sq_target) break;
    }

    /* ========== Phase 3: Extra blur passes at deepest level ========== */
    /* Each extra H+V pass (no downsample) adds pow4 to sigma^2 — same
     * contribution as one more pyramid tier but without halving resolution,
     * so more detail is preserved. */
    int32_t extra_contribution = pow4;

    while(n_levels >= 2 &&
          levels[n_levels - 1].sigma_sq < sigma_sq_target &&
          n_levels < MAX_BLUR_LEVELS) {

        gauss_level_t * prev = &levels[n_levels - 1];
        if(prev->w < 2 || prev->h < 2) break;

        uint32_t extra_buf_size = (uint32_t)prev->aw * 4 * (uint32_t)prev->ah;
        Esd_GpuHandle extra_handle = Esd_GpuAlloc_Alloc(u->allocator, extra_buf_size, GA_ALIGN_128);
        if(Esd_GpuAlloc_Get(u->allocator, extra_handle) == GA_INVALID) break;

        uint32_t prev_addr = Esd_GpuAlloc_Get(u->allocator, prev->handle);
        uint32_t t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
        uint32_t ex_addr = Esd_GpuAlloc_Get(u->allocator, extra_handle);
        if(prev_addr == GA_INVALID || t_addr == GA_INVALID || ex_addr == GA_INVALID) break;

        /* H blur (no downsample): prev -> temp */
        gaussian_5tap_pass(u, prev_addr, prev->aw * 4, prev->h,
                           t_addr, prev->aw, prev->ah, prev->w, prev->h,
                           true, false);

        t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
        ex_addr = Esd_GpuAlloc_Get(u->allocator, extra_handle);
        if(t_addr == GA_INVALID || ex_addr == GA_INVALID) break;

        /* V blur (no downsample): temp -> extra */
        gaussian_5tap_pass(u, t_addr, prev->aw * 4, prev->h,
                           ex_addr, prev->aw, prev->ah, prev->w, prev->h,
                           false, false);

        levels[n_levels].handle = extra_handle;
        levels[n_levels].w = prev->w;
        levels[n_levels].h = prev->h;
        levels[n_levels].aw = prev->aw;
        levels[n_levels].ah = prev->ah;
        levels[n_levels].sigma_sq = prev->sigma_sq + extra_contribution;
        n_levels++;
    }

    if(n_levels < 2) {
        /* Could not build even one blur level — skip blur gracefully */
        Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
        Esd_GpuAlloc_PendingFree(u->allocator, temp_handle);
        return true;
    }

    /* ========== Find bracketing levels for interpolation ========== */
    int32_t level_low = 0;
    int32_t level_high = 1;

    for(int32_t i = 0; i < n_levels - 1; i++) {
        if(levels[i + 1].sigma_sq >= sigma_sq_target) {
            level_low = i;
            level_high = i + 1;
            break;
        }
        /* Target exceeds all built levels: use the deepest */
        level_low = i + 1;
        level_high = i + 1;
    }

    uint8_t frac = 0;
    if(level_low != level_high) {
        int32_t range = levels[level_high].sigma_sq - levels[level_low].sigma_sq;
        int32_t delta = sigma_sq_target - levels[level_low].sigma_sq;
        if(range > 0) {
            frac = (uint8_t)LV_MIN(((int64_t)delta * 255) / range, 255);
        }
    }

    /* ========== Phase 4: Composite back to layer ========== */
    dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    if(dst_addr == GA_INVALID) goto cleanup;

    {
        EVE_CoCmd_renderTarget(phost, dst_addr, ARGB8, layer_aw, layer_ah);
        EVE_CoCmd_dlStart(phost);

        /* Blit entire source layer as background (preserves unchanged areas) */
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_bitmapSource(phost, dst_addr);
        EVE_CoDl_bitmapLayout(phost, ARGB8, layer_stride, layer_h);
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, layer_w, layer_h);
        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_end(phost);

        /* Scissor to clipped blur region */
        EVE_CoDl_scissorXY(phost, bx1, by1);
        EVE_CoDl_scissorSize(phost, bw, bh);

        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);

        /* Rounded corner stencil (optional) */
        if(corner_radius > 0) {
            EVE_CoDl_saveContext(phost);

            EVE_CoDl_clearStencil(phost, 0);
            EVE_CoDl_clear(phost, 0, 1, 0);

            EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, INCR);
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
            lv_draw_eve5_draw_rect(u, full_x1, full_y1, full_x2, full_y2,
                                   corner_radius, NULL, NULL);

            EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
            EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 255);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        }

        /* Composite blurred levels via bilinear upscale.
         * Blend between bracketing levels for smooth radius transitions.
         * Transform maps output coords to source: src = out * (level_dim / padded_dim). */
        {
            uint8_t weight_low = 255 - frac;
            uint8_t weight_high_val = frac;

            /* Draw low-sigma level */
            {
                gauss_level_t * lev = &levels[level_low];
                uint32_t lev_addr = Esd_GpuAlloc_Get(u->allocator, lev->handle);
                if(lev_addr != GA_INVALID) {
                    uint32_t scale_x = (uint32_t)lev->w * 256 / (uint32_t)pw;
                    uint32_t scale_y = (uint32_t)lev->h * 256 / (uint32_t)ph;

                    EVE_CoDl_colorRgb(phost, weight_low, weight_low, weight_low);
                    EVE_CoDl_colorA(phost, weight_low);
                    EVE_CoDl_blendFunc(phost, ONE, ZERO);
                    EVE_CoDl_bitmapSource(phost, lev_addr);
                    EVE_CoDl_bitmapLayout(phost, ARGB8, lev->aw * 4, lev->h);
                    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, pw, ph);
                    EVE_CoDl_bitmapTransform_identity(phost);
                    if(scale_x != 0x0100 || scale_y != 0x0100) {
                        EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_x);
                        EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_y);
                    }
                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, bx1 - pad, by1 - pad_y);
                    EVE_CoDl_end(phost);
                }
            }

            /* Draw high-sigma level (additive blend) */
            if(weight_high_val > 0 && level_high != level_low) {
                gauss_level_t * lev = &levels[level_high];
                uint32_t lev_addr = Esd_GpuAlloc_Get(u->allocator, lev->handle);
                if(lev_addr != GA_INVALID) {
                    uint32_t scale_x = (uint32_t)lev->w * 256 / (uint32_t)pw;
                    uint32_t scale_y = (uint32_t)lev->h * 256 / (uint32_t)ph;

                    EVE_CoDl_colorRgb(phost, weight_high_val, weight_high_val, weight_high_val);
                    EVE_CoDl_colorA(phost, weight_high_val);
                    EVE_CoDl_blendFunc(phost, ONE, ONE);
                    EVE_CoDl_bitmapSource(phost, lev_addr);
                    EVE_CoDl_bitmapLayout(phost, ARGB8, lev->aw * 4, lev->h);
                    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, pw, ph);
                    EVE_CoDl_bitmapTransform_identity(phost);
                    if(scale_x != 0x0100 || scale_y != 0x0100) {
                        EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_x);
                        EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_y);
                    }
                    EVE_CoDl_begin(phost, BITMAPS);
                    EVE_CoDl_vertex2f_0(phost, bx1 - pad, by1 - pad_y);
                    EVE_CoDl_end(phost);
                }
            }
        }

        if(corner_radius > 0) {
            EVE_CoDl_restoreContext(phost);
        }

        EVE_CoDl_display(phost);
        EVE_CoCmd_swap(phost);
        EVE_CoCmd_graphicsFinish(phost);
    }

    /* ========== Cleanup ========== */
cleanup:
    Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
    Esd_GpuAlloc_PendingFree(u->allocator, temp_handle);
    for(int32_t i = 1; i < n_levels; i++) {
        Esd_GpuAlloc_PendingFree(u->allocator, levels[i].handle);
    }

    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
