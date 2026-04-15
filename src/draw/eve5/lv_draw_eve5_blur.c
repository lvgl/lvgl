/**
 * @file lv_draw_eve5_blur.c
 *
 * EVE5 (BT820) Hardware Blur Implementation
 *
 * Mipmap-style downsample chain using bilinear-filtered offset draws.
 * Each level: 4 bilinear draws at half-pixel offsets, additive blend at 1/4 weight.
 * Output: interpolation between bracketing mipmap levels, composited back
 * into the layer buffer with optional rounded-corner masking.
 *
 * See LVGL_BLUR.md for algorithm details and cost analysis.
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
#define MAX_BLUR_LEVELS 8
#define BLUR_R0 4  /* Effective blur radius of mipmap level 0 (calibration vs SW renderer) */

/**********************
 * STATIC PROTOTYPES
 **********************/
static bool downsample_level(lv_draw_eve5_unit_t * u,
                             uint32_t src_addr, int32_t src_w, int32_t src_h, int32_t src_stride,
                             uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                             int32_t dst_w, int32_t dst_h);

/**********************
 * MIPMAP DOWNSAMPLE
 **********************/

/**
 * One mipmap downsample step: 4 bilinear draws at half-pixel offsets.
 *
 * Each draw samples a 2x2 block (bilinear) shifted by 1 source pixel.
 * Together the 4 draws cover a 3x3 source region with Gaussian-like falloff.
 * Additive blend with 1/4 weight per draw (premultiplied-safe).
 */
static bool downsample_level(lv_draw_eve5_unit_t * u,
                             uint32_t src_addr, int32_t src_w, int32_t src_h, int32_t src_stride,
                             uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                             int32_t dst_w, int32_t dst_h)
{
    EVE_HalContext *phost = u->hal;

    EVE_CoCmd_renderTarget(phost, dst_addr, ARGB8, dst_aw, dst_ah);
    EVE_CoCmd_dlStart(phost);

    /* Clear to transparent black */
    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clearColorA(phost, 0);
    EVE_CoDl_clear(phost, 1, 1, 1);

    /* Scissor to content area */
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, dst_w, dst_h);

    /* Bitmap: 2x downsample transform (A=E=2.0 maps 2 source pixels to 1 output pixel) */
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, src_addr);
    EVE_CoDl_bitmapLayout(phost, ARGB8, src_stride, src_h);
    EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, dst_w + 2, dst_h + 2);
    EVE_CoDl_bitmapTransform_identity(phost);
    EVE_CoDl_bitmapTransformA_ex(phost, 0, 0x0200);  /* 2.0 in unsigned 8.8 */
    EVE_CoDl_bitmapTransformE_ex(phost, 0, 0x0200);

    /* Additive blend, 1/4 weight per draw (premultiplied: scale all channels equally) */
    EVE_CoDl_colorRgb(phost, 64, 64, 64);
    EVE_CoDl_colorA(phost, 64);
    EVE_CoDl_blendFunc(phost, ONE, ONE);

    /* 4 draws at quarter-pixel offsets in output space.
     * vertex2f_2 = 1/4 pixel units, so +/-1 = +/-0.25 output pixels = +/-0.5 source pixels.
     * This places each bilinear sample at half-integer source coordinates,
     * averaging a 2x2 block. The 4 draws overlap in a 3x3 region with weights:
     *   1 2 1
     *   2 4 2  (Gaussian-like, no gaps)
     *   1 2 1                                                                    */
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_2(phost, -1, -1);
    EVE_CoDl_vertex2f_2(phost, +1, -1);
    EVE_CoDl_vertex2f_2(phost, -1, +1);
    EVE_CoDl_vertex2f_2(phost, +1, +1);
    EVE_CoDl_end(phost);

    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);

    return true;
}

/**********************
 * BLUR ENTRY POINT
 **********************/

/**
 * Hardware-accelerated blur of a region within a completed layer buffer.
 *
 * Called from the slice-splitting loop in eve5_render_layer() when a
 * LV_DRAW_TASK_TYPE_BLUR task is encountered. Modifies dst_handle in place.
 *
 * @param u           draw unit
 * @param layer       target layer (for buf_area dimensions)
 * @param dst_handle  completed layer content (modified in place)
 * @param blur_task   blur task with descriptor
 * @return            true on success
 */
bool lv_draw_eve5_blur(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                       Esd_GpuHandle dst_handle, const lv_draw_task_t * blur_task)
{
    EVE_HalContext *phost = u->hal;
    const lv_draw_blur_dsc_t * dsc = blur_task->draw_dsc;
    const lv_area_t * layer_area = &layer->buf_area;

    int32_t blur_radius = dsc->blur_radius;
    int32_t corner_radius = dsc->corner_radius;

    if(blur_radius <= 0) return true;

    /* Full blur area in layer-relative coordinates (for rounded rect geometry).
     * The display driver tiles the screen into strips — each tile processes the
     * blur task clipped to its strip. The rounded rect must use the FULL area
     * so tiles produce consistent portions of the same shape. */
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

    /* Clamp corner radius to the FULL blur area */
    int32_t short_side = LV_MIN(full_w, full_h);
    if(corner_radius > short_side / 2) corner_radius = short_side / 2;
    if(corner_radius < 0) corner_radius = 0;

    /* Layer dimensions */
    int32_t layer_w = lv_area_get_width(layer_area);
    int32_t layer_h = lv_area_get_height(layer_area);
    int32_t layer_aw = ALIGN_UP(layer_w, 16);
    int32_t layer_ah = ALIGN_UP(layer_h, 16);
    uint32_t layer_stride = (uint32_t)layer_aw * 4;

    uint32_t dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    if(dst_addr == GA_INVALID) return false;

    /* Padding must survive halvings down to the deepest level we'll actually USE
     * (not the maximum buildable). For blur_radius R with BLUR_R0 calibration:
     *   R < R0: need level 0 only → 1 halving → pad = 2
     *   R < 2*R0: need levels 0-1 → 2 halvings → pad = 4
     *   etc: pad = 2^(needed_levels)  */
    int32_t needed_levels;
    {
        needed_levels = 1;
        if(blur_radius >= BLUR_R0) {
            int32_t r = BLUR_R0;
            while(r * 2 <= blur_radius && needed_levels < MAX_BLUR_LEVELS - 1) {
                needed_levels++;
                r *= 2;
            }
            if(r < blur_radius) {
                needed_levels++;  /* +1 only when level_high is actually needed for interpolation */
            }
        }
    }
    int32_t pad = blur_radius;

    /* Padded content dimensions: RT stride alignment only (16px).
     * Padding is blur_radius (continuous) rather than 2^needed_levels
     * (which doubled at power-of-two boundaries, causing visible jumps).
     * The 2x downsample and 1/2^N upsample scales are exact regardless
     * of whether pw is power-of-two — content maps back correctly. */
    int32_t pw = ALIGN_UP(bw + 2 * pad, 16);
    int32_t ph = ALIGN_UP(bh + 2 * pad, 16);
    pad = (pw - bw) / 2;
    int32_t pad_y = (ph - bh) / 2;
    int32_t paw = ALIGN_UP(pw, 16);  /* RT stride alignment */
    int32_t pah = ALIGN_UP(ph, 16);

    /* ========== Phase 1: Extract blur region with edge padding ========== */

    uint32_t extract_size = (uint32_t)paw * 4 * (uint32_t)pah;
    Esd_GpuHandle extract_handle = Esd_GpuAlloc_Alloc(u->allocator, extract_size, GA_ALIGN_128);
    uint32_t extract_addr = Esd_GpuAlloc_Get(u->allocator, extract_handle);
    if(extract_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 blur: failed to allocate extraction buffer (%"PRIu32" bytes)", extract_size);
        return false;
    }

    /* Source offset: point bitmap source at (bx1, by1) in the layer buffer */
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

    /* Draw 1: scale source to fill entire padded buffer (stretches edges into padding).
     * NEAREST so edge pixels repeat cleanly — BILINEAR would interpolate between
     * the last source texel and BORDER black, creating a dark fringe.
     * Scale rounds UP to ensure full coverage, centered via C/F transform offset. */
    {
        uint32_t scale_a = (uint32_t)bw * 256 / (uint32_t)pw;  /* floor: source stretches to fill padding */
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

#if 1 /* Set to 0 to see padding only (debug) */
    /* Draw 2: blit source 1:1 at center (overwrites with exact pixels) */
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, bw, bh);
    EVE_CoDl_bitmapTransform_identity(phost);

    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, pad, pad_y);
    EVE_CoDl_end(phost);
#endif

    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);

    /* ========== Phase 2: Build mipmap chain ========== */

    Esd_GpuHandle mip_handles[MAX_BLUR_LEVELS];
    int32_t mip_w[MAX_BLUR_LEVELS];
    int32_t mip_h[MAX_BLUR_LEVELS];
    int32_t mip_aw[MAX_BLUR_LEVELS];
    int32_t mip_ah[MAX_BLUR_LEVELS];
    int32_t num_levels = 0;

    int32_t prev_w = pw;
    int32_t prev_h = ph;
    int32_t prev_aw = paw;
    int32_t prev_ah = pah;
    uint32_t prev_addr = extract_addr;
    int32_t prev_stride = paw * 4;

    while(num_levels < needed_levels) {
        int32_t next_w = (prev_w + 1) / 2;
        int32_t next_h = (prev_h + 1) / 2;
        int32_t next_aw = ALIGN_UP(next_w, 16);
        int32_t next_ah = ALIGN_UP(next_h, 16);

        /* Stop: content too small for useful blur work */
        if(next_w <= 4 && next_h <= 4) break;

        /* Stop: alignment floor, no actual size reduction */
        if(next_aw >= prev_aw && next_ah >= prev_ah) break;

        uint32_t mip_size = (uint32_t)next_aw * 4 * (uint32_t)next_ah;
        mip_handles[num_levels] = Esd_GpuAlloc_Alloc(u->allocator, mip_size, GA_ALIGN_128);
        uint32_t mip_addr = Esd_GpuAlloc_Get(u->allocator, mip_handles[num_levels]);
        if(mip_addr == GA_INVALID) {
            LV_LOG_WARN("EVE5 blur: failed to allocate mipmap level %"PRId32, num_levels);
            break;
        }

        downsample_level(u, prev_addr, prev_w, prev_h, prev_stride,
                         mip_addr, next_aw, next_ah, next_w, next_h);

        mip_w[num_levels] = next_w;
        mip_h[num_levels] = next_h;
        mip_aw[num_levels] = next_aw;
        mip_ah[num_levels] = next_ah;

        prev_w = next_w;
        prev_h = next_h;
        prev_aw = next_aw;
        prev_ah = next_ah;
        prev_addr = mip_addr;
        prev_stride = next_aw * 4;

        num_levels++;
    }

    if(num_levels == 0) {
        Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
        return true;
    }

    /* ========== Phase 3: Find bracketing levels ========== */

    /* Each mipmap level's effective blur radius is R0 * 2^n, where R0 is an
     * empirical calibration factor matching the SW renderer's IIR intensity.
     * For blur_radius < R0, blend between extraction (unblurred) and mip[0].
     * For blur_radius >= R0, blend between the two bracketing mip levels. */

    int32_t level_low = -1;    /* -1 = extraction (unblurred) */
    int32_t level_high = 0;
    uint8_t frac;

    if(blur_radius < BLUR_R0) {
        /* Below level 0: blend extraction with mip[0] */
        frac = (uint8_t)(blur_radius * 255 / BLUR_R0);
    }
    else {
        level_low = 0;
        int32_t radius_low = BLUR_R0;
        while(level_low < num_levels - 1 && radius_low * 2 <= blur_radius) {
            level_low++;
            radius_low *= 2;
        }
        level_high = level_low + 1;
        int32_t radius_high = radius_low * 2;

        if(level_high < num_levels && blur_radius > radius_low) {
            frac = (uint8_t)LV_MIN(((blur_radius - radius_low) * 255) / (radius_high - radius_low), 255);
        }
        else {
            frac = 0;
        }
    }

    /* ========== Phase 4: Composite back ========== */
    /* The render target starts from nothing — blit dst_handle as the
     * background first, then draw the blurred region on top.
     * Two weighted draws produce: low * (1-frac) + high * frac */

    EVE_CoCmd_renderTarget(phost, dst_addr, ARGB8, layer_aw, layer_ah);
    EVE_CoCmd_dlStart(phost);

    /* Blit entire source as background */
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

    /* Scissor to blur region for the blurred overlay */
    EVE_CoDl_scissorXY(phost, bx1, by1);
    EVE_CoDl_scissorSize(phost, bw, bh);

    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);

    if(corner_radius > 0) {
        EVE_CoDl_saveContext(phost);

        /* Build rounded rect stencil using FULL blur area coordinates.
         * Scissor clips to the tile — each tile gets the correct portion
         * of the same rounded rect. */
        EVE_CoDl_clearStencil(phost, 0);
        EVE_CoDl_clear(phost, 0, 1, 0);

        EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(phost, KEEP, INCR);
        EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
        lv_draw_eve5_draw_rect(u, full_x1, full_y1, full_x2, full_y2, corner_radius, NULL, NULL);

        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 255);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    }

    /* Upsample helper: blit a padded mip level back to the blur region.
     * Each downsample uses exactly 2.0 transform, so the upsample reverses
     * with exactly 1/2^(level+1). The (n+1)/2 buffer rounding only adds
     * extra edge pixels that are never sampled — content maps back correctly.
     * Vertex at (bx1 - pad) so the 1:1 center aligns with the blur region. */
#define BLIT_LEVEL(addr_, src_h_, src_aw_, scale_, weight_, blend_src_, blend_dst_) \
    do { \
        EVE_CoDl_colorRgb(phost, (weight_), (weight_), (weight_)); \
        EVE_CoDl_colorA(phost, (weight_)); \
        EVE_CoDl_blendFunc(phost, (blend_src_), (blend_dst_)); \
        EVE_CoDl_bitmapSource(phost, (addr_)); \
        EVE_CoDl_bitmapLayout(phost, ARGB8, (src_aw_) * 4, (src_h_)); \
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, pw, ph); \
        EVE_CoDl_bitmapTransform_identity(phost); \
        if((scale_) != 0x0100) { \
            EVE_CoDl_bitmapTransformA_ex(phost, 0, (scale_)); \
            EVE_CoDl_bitmapTransformE_ex(phost, 0, (scale_)); \
        } \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, bx1 - pad, by1 - pad_y); \
        EVE_CoDl_end(phost); \
    } while(0)

    /* Draw low source at (1 - frac) weight */
    {
        uint8_t weight_low = 255 - frac;
        if(level_low < 0) {
            /* Extraction (unblurred): identity scale */
            BLIT_LEVEL(extract_addr, ph, paw, 0x0100, weight_low, ONE, ZERO);
        }
        else {
            uint32_t la = Esd_GpuAlloc_Get(u->allocator, mip_handles[level_low]);
            uint32_t scale = 256 >> (level_low + 1);
            BLIT_LEVEL(la, mip_h[level_low], mip_aw[level_low], scale, weight_low, ONE, ZERO);
        }
    }

    /* Draw high source at frac weight (additive) */
    if(frac > 0 && level_high < num_levels) {
        uint32_t la = Esd_GpuAlloc_Get(u->allocator, mip_handles[level_high]);
        uint32_t scale = 256 >> (level_high + 1);
        BLIT_LEVEL(la, mip_h[level_high], mip_aw[level_high], scale, frac, ONE, ONE);
    }

#undef BLIT_LEVEL

    if(corner_radius > 0) {
        EVE_CoDl_restoreContext(phost);
    }

    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);

    /* ========== Cleanup ========== */

    Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
    for(int32_t i = 0; i < num_levels; i++) {
        Esd_GpuAlloc_PendingFree(u->allocator, mip_handles[i]);
    }

    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
