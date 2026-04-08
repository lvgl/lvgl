/**
 * @file lv_draw_eve5_blend.c
 *
 * EVE5 (BT820) Per-Channel Blend Mode Implementation
 *
 * Implements non-standard blend modes (MULTIPLY, SUBTRACTIVE, DIFFERENCE)
 * using multi-pass alpha-channel swizzle techniques. EVE's blend factors
 * are alpha-only ({ZERO, ONE, SRC_ALPHA, DST_ALPHA, ONE_MINUS_*}), so
 * per-channel color math is achieved by routing individual R/G/B channels
 * through the alpha channel via BITMAP_SWIZZLE, performing the blend math
 * with alpha blend factors, then writing the result back via COLOR_MASK.
 *
 * Architecture:
 * - The blend-mode task is rendered in isolation into a clean ARGB8 buffer
 *   (the "src" bitmap) using the normal slice pipeline with isolated=true.
 *   This handles all transforms, colorkey, recolor, etc. automatically.
 * - The accumulated layer content before the blend task is the "dst" bitmap.
 * - Both are completed renders, so they can be sampled as textures.
 * - Per-channel math runs in separate DL cycles producing "temp" bitmaps.
 * - A final composite DL blits dst, then composites the channel result
 *   on top using standard premultiplied blend(ONE, ONE_MINUS_SRC_ALPHA).
 *
 * Per-channel pass counts:
 *   MULTIPLY:    3 draws/channel = 9 + 1 alpha = 10 draws, 2 DLs
 *   SUBTRACTIVE: 5 draws/channel = 15 + 1 alpha = 16 draws, 2 DLs
 *   DIFFERENCE:  SUBTRACTIVE x2 + ADD = 16 + 16 + 10 draws, 4 DLs
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

/**********************
 * STATIC PROTOTYPES
 **********************/

static void setup_swizzled_blit(EVE_HalContext *phost, uint32_t addr,
                                uint32_t stride, int32_t w, int32_t h,
                                uint8_t channel);
static void begin_channel_dl(EVE_HalContext *phost, uint32_t target_addr,
                             int32_t aligned_w, int32_t aligned_h, int32_t w, int32_t h);
static void finish_channel_dl(EVE_HalContext *phost);
static void copy_src_alpha(EVE_HalContext *phost, uint32_t src_addr,
                           uint32_t stride, int32_t w, int32_t h);
static void splat_alpha_to_channel(EVE_HalContext *phost, int32_t w, int32_t h,
                                   uint8_t r_mask, uint8_t g_mask, uint8_t b_mask);
static bool composite_over_dst(lv_draw_eve5_unit_t *u, Esd_GpuHandle *out_result,
                               uint32_t dst_addr, Esd_GpuHandle temp_handle,
                               bool temp_is_premultiplied,
                               int32_t aligned_w, int32_t aligned_h,
                               int32_t w, int32_t h, uint32_t stride, uint32_t buf_size);

/**********************
 * SHARED HELPERS
 **********************/

/**
 * Configure bitmap for blitting with a single channel routed to alpha.
 * GLFORMAT + BITMAP_SWIZZLE routes the selected channel to alpha; RGB zeroed.
 */
static void setup_swizzled_blit(EVE_HalContext *phost, uint32_t addr,
                                uint32_t stride, int32_t w, int32_t h,
                                uint8_t channel)
{
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, addr);
    EVE_CoDl_bitmapLayout(phost, GLFORMAT, stride, h);
    EVE_CoDl_bitmapExtFormat(phost, ARGB8);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, channel);
    EVE_CoDl_bitmapTransform_identity(phost);
}

/** Start a channel-math DL: render target, clear to (0,0,0,0). */
static void begin_channel_dl(EVE_HalContext *phost, uint32_t target_addr,
                             int32_t aligned_w, int32_t aligned_h, int32_t w, int32_t h)
{
    EVE_CoCmd_renderTarget(phost, target_addr, ARGB8, aligned_w, aligned_h);
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, w, h);
    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clearColorA(phost, 0);
    EVE_CoDl_clear(phost, 1, 1, 1);
    EVE_CoDl_vertexFormat(phost, 0);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
}

/** Finish a channel-math DL: display, swap, graphicsFinish. */
static void finish_channel_dl(EVE_HalContext *phost)
{
    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    EVE_CoCmd_graphicsFinish(phost);
}

/** Copy src alpha channel (normal blit, alpha-only). */
static void copy_src_alpha(EVE_HalContext *phost, uint32_t src_addr,
                           uint32_t stride, int32_t w, int32_t h)
{
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, src_addr);
    EVE_CoDl_bitmapLayout(phost, ARGB8, stride, h);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
    EVE_CoDl_bitmapTransform_identity(phost);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);
}

/** Write current alpha value to an RGB channel via colorMask + DST_ALPHA rect. */
static void splat_alpha_to_channel(EVE_HalContext *phost, int32_t w, int32_t h,
                                   uint8_t r_mask, uint8_t g_mask, uint8_t b_mask)
{
    EVE_CoDl_colorMask(phost, r_mask, g_mask, b_mask, 0);
    EVE_CoDl_blendFunc(phost, DST_ALPHA, ZERO);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_lineWidth(phost, 16);
    EVE_CoDl_begin(phost, RECTS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_vertex2f_0(phost, w, h);
    EVE_CoDl_end(phost);
}

/**
 * Composite temp over dst into a new result buffer.
 * Allocates result buffer; caller owns it on success.
 *
 * @param temp_is_premultiplied  true: temp RGB is already scaled by alpha
 *   (MULTIPLY — product of premultiplied src inherits coverage scaling).
 *   Uses blend(ONE, ONE_MINUS_SRC_ALPHA) for exact compositing.
 *   false: temp RGB is straight (SUBTRACTIVE/DIFFERENCE — channel math
 *   produces raw values, alpha copied separately).
 *   Uses blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA) to apply coverage.
 */
static bool composite_over_dst(lv_draw_eve5_unit_t *u, Esd_GpuHandle *out_result,
                               uint32_t dst_addr, Esd_GpuHandle temp_handle,
                               bool temp_is_premultiplied,
                               int32_t aligned_w, int32_t aligned_h,
                               int32_t w, int32_t h, uint32_t stride, uint32_t buf_size)
{
    EVE_HalContext *phost = u->hal;

    uint32_t temp_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
    if(temp_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    Esd_GpuHandle result_handle = Esd_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t result_addr = Esd_GpuAlloc_Get(u->allocator, result_handle);
    if(result_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, result_addr, aligned_w, aligned_h, w, h);
    EVE_CoDl_bitmapTransform_identity(phost);

    /* Blit dst as base */
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, dst_addr);
    EVE_CoDl_bitmapLayout(phost, ARGB8, stride, h);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    /* Composite temp over dst base */
    if(temp_is_premultiplied) {
        /* MULTIPLY: product of premultiplied src is already coverage-scaled.
         * blend(ONE, ONE_MINUS_SRC_ALPHA) for exact premultiplied compositing. */
        EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
    }
    else {
        /* SUBTRACTIVE/DIFFERENCE: channel math produces raw values, alpha separate.
         * blend(SRC_ALPHA, ONE_MINUS_SRC_ALPHA) applies coverage to temp RGB. */
        EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
    }
    EVE_CoDl_bitmapSource(phost, temp_addr);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);

    finish_channel_dl(phost);

    {
        EVE_CmdSync sync = EVE_Cmd_sync(phost);
        Esd_GpuAlloc_DeferredFree(u->allocator, temp_handle, sync);
    }

    *out_result = result_handle;
    return true;
}

/**********************
 * PER-CHANNEL MATH MACROS
 *
 * These emit DL commands for one RGB channel.
 * All operate on the alpha channel as scratch, then splat to the target channel.
 * Caller must have begun a channel DL and set colorArgb to 0xFFFFFFFF.
 **********************/

/** MULTIPLY one channel: alpha = a.c * b.c / 255, splat to channel. (3 draws) */
#define CHANNEL_MULTIPLY(phost, a_addr, b_addr, stride, w, h, channel, r_mask, g_mask, b_mask) \
    do { \
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1); \
        EVE_CoDl_blendFunc(phost, ONE, ZERO); \
        setup_swizzled_blit(phost, a_addr, stride, w, h, channel); \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_end(phost); \
        \
        EVE_CoDl_blendFunc(phost, DST_ALPHA, ZERO); \
        setup_swizzled_blit(phost, b_addr, stride, w, h, channel); \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_end(phost); \
        \
        splat_alpha_to_channel(phost, w, h, r_mask, g_mask, b_mask); \
    } while(0)

/**
 * SUBTRACTIVE one channel: alpha = max(a.c - b.c, 0), splat to channel. (5 draws)
 *
 * Uses complement identity: a - b = 255 - ((255 - a) + b).
 * Hardware clamps the addition to 255, so when b > a the final invert gives 0.
 */
#define CHANNEL_SUBTRACT(phost, a_addr, b_addr, stride, w, h, channel, r_mask, g_mask, b_mask) \
    do { \
        /* 1. alpha = a.c */ \
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1); \
        EVE_CoDl_blendFunc(phost, ONE, ZERO); \
        setup_swizzled_blit(phost, a_addr, stride, w, h, channel); \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_end(phost); \
        \
        /* 2. alpha = 255 - a.c */ \
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF); \
        EVE_CoDl_blendFunc(phost, ONE_MINUS_DST_ALPHA, ZERO); \
        EVE_CoDl_lineWidth(phost, 16); \
        EVE_CoDl_begin(phost, RECTS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_vertex2f_0(phost, w, h); \
        EVE_CoDl_end(phost); \
        \
        /* 3. alpha = (255 - a.c) + b.c, clamped to 255 */ \
        EVE_CoDl_blendFunc(phost, ONE, ONE); \
        setup_swizzled_blit(phost, b_addr, stride, w, h, channel); \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_end(phost); \
        \
        /* 4. alpha = 255 - result = max(a.c - b.c, 0) */ \
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF); \
        EVE_CoDl_blendFunc(phost, ONE_MINUS_DST_ALPHA, ZERO); \
        EVE_CoDl_begin(phost, RECTS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_vertex2f_0(phost, w, h); \
        EVE_CoDl_end(phost); \
        \
        /* 5. splat */ \
        splat_alpha_to_channel(phost, w, h, r_mask, g_mask, b_mask); \
    } while(0)

/** ADD one channel: alpha = min(a.c + b.c, 255), splat to channel. (3 draws) */
#define CHANNEL_ADD(phost, a_addr, b_addr, stride, w, h, channel, r_mask, g_mask, b_mask) \
    do { \
        EVE_CoDl_colorMask(phost, 0, 0, 0, 1); \
        EVE_CoDl_blendFunc(phost, ONE, ZERO); \
        setup_swizzled_blit(phost, a_addr, stride, w, h, channel); \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_end(phost); \
        \
        EVE_CoDl_blendFunc(phost, ONE, ONE); \
        setup_swizzled_blit(phost, b_addr, stride, w, h, channel); \
        EVE_CoDl_begin(phost, BITMAPS); \
        EVE_CoDl_vertex2f_0(phost, 0, 0); \
        EVE_CoDl_end(phost); \
        \
        splat_alpha_to_channel(phost, w, h, r_mask, g_mask, b_mask); \
    } while(0)

/** Expand a per-channel macro for all 3 RGB channels. */
#define FOR_EACH_CHANNEL(MACRO, phost, a_addr, b_addr, stride, w, h) \
    MACRO(phost, a_addr, b_addr, stride, w, h, RED,   1, 0, 0); \
    MACRO(phost, a_addr, b_addr, stride, w, h, GREEN, 0, 1, 0); \
    MACRO(phost, a_addr, b_addr, stride, w, h, BLUE,  0, 0, 1)

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * MULTIPLY: result.c = dst.c * src.c / 255
 * 2 DL cycles: channel math + composite.
 */
bool lv_draw_eve5_blend_multiply(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                 Esd_GpuHandle dst_handle, Esd_GpuHandle src_handle,
                                 Esd_GpuHandle *out_result)
{
    EVE_HalContext *phost = u->hal;
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aw = ALIGN_UP(w, 16);
    int32_t ah = ALIGN_UP(h, 16);
    uint32_t stride = aw * 4;
    uint32_t buf_size = stride * ah;

    uint32_t dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    uint32_t src_addr = Esd_GpuAlloc_Get(u->allocator, src_handle);
    if(dst_addr == GA_INVALID || src_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    Esd_GpuHandle temp = Esd_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp_addr = Esd_GpuAlloc_Get(u->allocator, temp);
    if(temp_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, temp_addr, aw, ah, w, h);
    FOR_EACH_CHANNEL(CHANNEL_MULTIPLY, phost, dst_addr, src_addr, stride, w, h);
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    return composite_over_dst(u, out_result, dst_addr, temp, true, aw, ah, w, h, stride, buf_size);
}

/**
 * SUBTRACTIVE: result.c = max(dst.c - src.c, 0)
 * 2 DL cycles: channel math + composite.
 */
bool lv_draw_eve5_blend_subtractive(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                    Esd_GpuHandle dst_handle, Esd_GpuHandle src_handle,
                                    Esd_GpuHandle *out_result)
{
    EVE_HalContext *phost = u->hal;
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aw = ALIGN_UP(w, 16);
    int32_t ah = ALIGN_UP(h, 16);
    uint32_t stride = aw * 4;
    uint32_t buf_size = stride * ah;

    uint32_t dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    uint32_t src_addr = Esd_GpuAlloc_Get(u->allocator, src_handle);
    if(dst_addr == GA_INVALID || src_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    Esd_GpuHandle temp = Esd_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp_addr = Esd_GpuAlloc_Get(u->allocator, temp);
    if(temp_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, temp_addr, aw, ah, w, h);
    FOR_EACH_CHANNEL(CHANNEL_SUBTRACT, phost, dst_addr, src_addr, stride, w, h);
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    return composite_over_dst(u, out_result, dst_addr, temp, false, aw, ah, w, h, stride, buf_size);
}

/**
 * DIFFERENCE: result.c = abs(dst.c - src.c)
 *           = max(dst.c - src.c, 0) + max(src.c - dst.c, 0)
 *
 * 4 DL cycles: subtract(dst,src) + subtract(src,dst) + add + composite.
 */
bool lv_draw_eve5_blend_difference(lv_draw_eve5_unit_t *u, lv_layer_t *layer,
                                   Esd_GpuHandle dst_handle, Esd_GpuHandle src_handle,
                                   Esd_GpuHandle *out_result)
{
    EVE_HalContext *phost = u->hal;
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aw = ALIGN_UP(w, 16);
    int32_t ah = ALIGN_UP(h, 16);
    uint32_t stride = aw * 4;
    uint32_t buf_size = stride * ah;

    uint32_t dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    uint32_t src_addr = Esd_GpuAlloc_Get(u->allocator, src_handle);
    if(dst_addr == GA_INVALID || src_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    /* DL1: temp1.c = max(dst.c - src.c, 0) */
    Esd_GpuHandle temp1 = Esd_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp1_addr = Esd_GpuAlloc_Get(u->allocator, temp1);
    if(temp1_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, temp1_addr, aw, ah, w, h);
    FOR_EACH_CHANNEL(CHANNEL_SUBTRACT, phost, dst_addr, src_addr, stride, w, h);
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    /* DL2: temp2.c = max(src.c - dst.c, 0) */
    Esd_GpuHandle temp2 = Esd_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp2_addr = Esd_GpuAlloc_Get(u->allocator, temp2);
    if(temp2_addr == GA_INVALID) {
        Esd_GpuAlloc_PendingFree(u->allocator,temp1);
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, temp2_addr, aw, ah, w, h);
    FOR_EACH_CHANNEL(CHANNEL_SUBTRACT, phost, src_addr, dst_addr, stride, w, h);
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    /* Re-resolve addresses after DL2 (allocator may have moved things) */
    temp1_addr = Esd_GpuAlloc_Get(u->allocator, temp1);
    temp2_addr = Esd_GpuAlloc_Get(u->allocator, temp2);
    if(temp1_addr == GA_INVALID || temp2_addr == GA_INVALID) {
        Esd_GpuAlloc_PendingFree(u->allocator,temp1);
        Esd_GpuAlloc_PendingFree(u->allocator,temp2);
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    /* DL3: temp3.c = temp1.c + temp2.c = abs(dst.c - src.c) */
    Esd_GpuHandle temp3 = Esd_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp3_addr = Esd_GpuAlloc_Get(u->allocator, temp3);
    if(temp3_addr == GA_INVALID) {
        Esd_GpuAlloc_PendingFree(u->allocator,temp1);
        Esd_GpuAlloc_PendingFree(u->allocator,temp2);
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, temp3_addr, aw, ah, w, h);
    FOR_EACH_CHANNEL(CHANNEL_ADD, phost, temp1_addr, temp2_addr, stride, w, h);
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    {
        EVE_CmdSync sync = EVE_Cmd_sync(phost);
        Esd_GpuAlloc_DeferredFree(u->allocator, temp1, sync);
        Esd_GpuAlloc_DeferredFree(u->allocator, temp2, sync);
    }

    /* DL4: composite temp3 over dst */
    return composite_over_dst(u, out_result, dst_addr, temp3, false, aw, ah, w, h, stride, buf_size);
}

#endif /* LV_USE_DRAW_EVE5 */
