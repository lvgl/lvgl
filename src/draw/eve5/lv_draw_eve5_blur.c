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

    /* 4 draws at half-pixel offsets in output space.
     * vertex2f_1 = 1/2 pixel units, so +/-1 = +/-0.5 output pixels = +/-1 source pixel. */
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_1(phost, -1, -1);
    EVE_CoDl_vertex2f_1(phost, +1, -1);
    EVE_CoDl_vertex2f_1(phost, -1, +1);
    EVE_CoDl_vertex2f_1(phost, +1, +1);
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

    /* Compute blur region in layer-relative coordinates */
    lv_area_t blur_clip;
    if(!lv_area_intersect(&blur_clip, &blur_task->area, &blur_task->clip_area)) return true;

    int32_t bx1 = blur_clip.x1 - layer_area->x1;
    int32_t by1 = blur_clip.y1 - layer_area->y1;
    int32_t bx2 = blur_clip.x2 - layer_area->x1;
    int32_t by2 = blur_clip.y2 - layer_area->y1;
    int32_t bw = bx2 - bx1 + 1;
    int32_t bh = by2 - by1 + 1;

    if(bw <= 0 || bh <= 0) return true;

    /* Clamp corner radius */
    int32_t short_side = LV_MIN(bw, bh);
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

    /* Aligned blur region dimensions */
    int32_t baw = ALIGN_UP(bw, 16);
    int32_t bah = ALIGN_UP(bh, 16);

    /* ========== Phase 1: Extract blur region ========== */

    uint32_t extract_size = (uint32_t)baw * 4 * (uint32_t)bah;
    Esd_GpuHandle extract_handle = Esd_GpuAlloc_Alloc(u->allocator, extract_size, GA_ALIGN_128);
    uint32_t extract_addr = Esd_GpuAlloc_Get(u->allocator, extract_handle);
    if(extract_addr == GA_INVALID) {
        LV_LOG_ERROR("EVE5 blur: failed to allocate extraction buffer (%"PRIu32" bytes)", extract_size);
        return false;
    }

    /* Source offset: point bitmap source at (bx1, by1) in the layer buffer */
    uint32_t src_ofs = (uint32_t)by1 * layer_stride + (uint32_t)bx1 * 4;

    EVE_CoCmd_renderTarget(phost, extract_addr, ARGB8, baw, bah);
    EVE_CoCmd_dlStart(phost);

    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clearColorA(phost, 0);
    EVE_CoDl_clearStencil(phost, 0);
    EVE_CoDl_clear(phost, 1, 1, 1);

    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
    EVE_CoDl_bitmapSource(phost, dst_addr + src_ofs);
    EVE_CoDl_bitmapLayout(phost, ARGB8, layer_stride, layer_h - by1);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, bw, bh);
    EVE_CoDl_bitmapTransform_identity(phost);

    if(corner_radius > 0) {
        EVE_CoDl_saveContext(phost);

        /* Build rounded rect stencil */
        EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(phost, KEEP, INCR);
        EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
        lv_draw_eve5_draw_rect(u, 0, 0, bw - 1, bh - 1, corner_radius, NULL, NULL);

        /* Blit through stencil */
        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 255);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_end(phost);

        EVE_CoDl_restoreContext(phost);
    }
    else {
        /* Simple rect extraction */
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, 0, 0);
        EVE_CoDl_end(phost);
    }

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

    int32_t prev_w = bw;
    int32_t prev_h = bh;
    int32_t prev_aw = baw;
    int32_t prev_ah = bah;
    uint32_t prev_addr = extract_addr;
    int32_t prev_stride = baw * 4;

    while(num_levels < MAX_BLUR_LEVELS) {
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

    /* Level n has effective blur radius ~2^n */
    int32_t level_low = 0;
    int32_t radius_low = 1;
    while(level_low < num_levels - 1 && radius_low * 2 <= blur_radius) {
        level_low++;
        radius_low *= 2;
    }

    int32_t level_high = level_low + 1;
    int32_t radius_high = radius_low * 2;

    uint8_t frac = 0;
    bool need_two_levels = false;

    if(level_high < num_levels && blur_radius > radius_low) {
        frac = (uint8_t)LV_MIN(((blur_radius - radius_low) * 255) / (radius_high - radius_low), 255);
        need_two_levels = (frac > 0);
    }

    /* ========== Phase 4: Composite back into dst_handle ========== */

    EVE_CoCmd_renderTarget(phost, dst_addr, ARGB8, layer_aw, layer_ah);
    EVE_CoCmd_dlStart(phost);

    /* Scissor to blur region */
    EVE_CoDl_scissorXY(phost, bx1, by1);
    EVE_CoDl_scissorSize(phost, bw, bh);

    EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);

    if(corner_radius > 0) {
        EVE_CoDl_saveContext(phost);

        /* Build rounded rect stencil at blur region position */
        EVE_CoDl_clearStencil(phost, 0);
        EVE_CoDl_clear(phost, 0, 1, 0);

        EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
        EVE_CoDl_stencilOp(phost, KEEP, INCR);
        EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
        lv_draw_eve5_draw_rect(u, bx1, by1, bx2, by2, corner_radius, NULL, NULL);

        EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
        EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 255);
        EVE_CoDl_stencilOp(phost, KEEP, KEEP);
    }

    /* Draw level_low upsampled at (1 - frac) weight.
     * Upsample transform: A = mip_w / bw, E = mip_h / bh in unsigned 8.8. */
    {
        int32_t lw = mip_w[level_low];
        int32_t lh = mip_h[level_low];
        int32_t law = mip_aw[level_low];
        uint32_t laddr = Esd_GpuAlloc_Get(u->allocator, mip_handles[level_low]);

        uint32_t scale_a = (uint32_t)lw * 256 / (uint32_t)bw;
        uint32_t scale_e = (uint32_t)lh * 256 / (uint32_t)bh;

        uint8_t weight = need_two_levels ? (255 - frac) : 255;

        EVE_CoDl_colorRgb(phost, weight, weight, weight);
        EVE_CoDl_colorA(phost, weight);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);

        EVE_CoDl_bitmapSource(phost, laddr);
        EVE_CoDl_bitmapLayout(phost, ARGB8, law * 4, lh);
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, bw, bh);
        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_a);
        EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_e);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, bx1, by1);
        EVE_CoDl_end(phost);
    }

    /* Draw level_high upsampled at frac weight (additive) */
    if(need_two_levels) {
        int32_t lw = mip_w[level_high];
        int32_t lh = mip_h[level_high];
        int32_t law = mip_aw[level_high];
        uint32_t laddr = Esd_GpuAlloc_Get(u->allocator, mip_handles[level_high]);

        uint32_t scale_a = (uint32_t)lw * 256 / (uint32_t)bw;
        uint32_t scale_e = (uint32_t)lh * 256 / (uint32_t)bh;

        EVE_CoDl_colorRgb(phost, frac, frac, frac);
        EVE_CoDl_colorA(phost, frac);
        EVE_CoDl_blendFunc(phost, ONE, ONE);

        EVE_CoDl_bitmapSource(phost, laddr);
        EVE_CoDl_bitmapLayout(phost, ARGB8, law * 4, lh);
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, bw, bh);
        EVE_CoDl_bitmapTransform_identity(phost);
        EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_a);
        EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_e);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, bx1, by1);
        EVE_CoDl_end(phost);
    }

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
