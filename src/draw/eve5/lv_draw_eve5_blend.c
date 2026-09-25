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
 *   Its RGB is premultiplied: P = s*a for straight color s and coverage a.
 * - The accumulated layer content before the blend task is the "dst" bitmap d.
 * - Both are completed renders, so they can be sampled as textures.
 * - Per-channel math runs in separate DL cycles producing "temp" bitmaps.
 * - A final composite DL blits dst, then composites the premultiplied
 *   channel result on top with blend(ONE, ONE_MINUS_SRC_ALPHA).
 *
 * LVGL's software renderer computes the mode's result f(d, s) from the
 * straight source color and mixes it over dst by the coverage:
 * out = f(d, s)*a + d*(1-a). The premultiplied term f(d, s)*a follows from
 * P and d*a, with no division by a:
 *   MULTIPLY:    d*s*a         = d*P
 *   SUBTRACTIVE: max(d-s, 0)*a = max(d*a - P, 0)
 *   DIFFERENCE:  |d-s|*a       = max(d*a - P, 0) + max(P - d*a, 0)
 *
 * On an L8 layer, LVGL applies the mode to the luminances. The math then runs
 * once, on the luminance an L8 render target stores ((r + g + b) / 3), and
 * writes the result to all three channels.
 *
 * Per-channel pass counts:
 *   MULTIPLY:    3 draws/channel = 9 + 1 alpha = 10 draws, 2 DLs
 *   SUBTRACTIVE: 6 draws/channel = 18 + 1 alpha = 19 draws, 2 DLs
 *   DIFFERENCE:  d*a (2 draws), then 10 draws/channel = 30 + 1 alpha, 3 DLs
 *   Luminance: 2 more draws for each read of a bitmap, for one channel only
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5
#ifdef EVE_SUPPORT_RENDERTARGET

/* This whole file targets render-target compositing — every path here
 * allocates an intermediate ARGB8 RT, renders into it via CMD_RENDERTARGET,
 * and re-blits the result. None of that is reachable on chips without
 * render-target support, and the symbols (CMD_RENDERTARGET, ARGB8, the
 * BT820+ alignment-flag encoding) don't exist there either. */

/**********************
 *      DEFINES
 **********************/

/** Pseudo channel for the per-channel math: the luminance an L8 render
 *  target stores, (r + g + b) / 3 */
#define LUMINANCE 0xFF

/**********************
 *      TYPEDEFS
 **********************/

/** A channel the math runs on, and the RGB channels its result goes to */
typedef struct {
    uint8_t channel;
    uint8_t r_mask, g_mask, b_mask;
} blend_channel_t;

/**********************
 *  STATIC VARIABLES
 **********************/

static const blend_channel_t rgb_channels[3] = {
    { RED, 1, 0, 0 },
    { GREEN, 0, 1, 0 },
    { BLUE, 0, 0, 1 },
};

static const blend_channel_t luminance_channel[1] = {
    { LUMINANCE, 1, 1, 1 },
};

/**********************
 * STATIC PROTOTYPES
 **********************/

static void setup_blit(EVE_HalContext *phost, uint32_t addr,
                       uint32_t stride, int32_t w, int32_t h);
static void setup_swizzled_blit(EVE_HalContext *phost, uint32_t addr,
                                uint32_t stride, int32_t w, int32_t h,
                                uint8_t channel);
static void draw_bitmap(EVE_HalContext *phost);
static void draw_rect(EVE_HalContext *phost, int32_t w, int32_t h);
static void begin_channel_dl(EVE_HalContext *phost, uint32_t target_addr,
                             int32_t aligned_w, int32_t aligned_h, int32_t w, int32_t h);
static void finish_channel_dl(EVE_HalContext *phost);
static void copy_src_alpha(EVE_HalContext *phost, uint32_t src_addr,
                           uint32_t stride, int32_t w, int32_t h);
static void draw_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                         int32_t w, int32_t h, uint8_t channel, bool add);
static void load_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                         int32_t w, int32_t h, uint8_t channel);
static void add_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                        int32_t w, int32_t h, uint8_t channel);
static void multiply_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                             int32_t w, int32_t h, uint8_t channel);
static void subtract_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                             int32_t w, int32_t h, uint8_t channel);
static void store_alpha(EVE_HalContext *phost, int32_t w, int32_t h,
                        uint8_t r_mask, uint8_t g_mask, uint8_t b_mask, bool add);
static bool composite_over_dst(lv_draw_eve5_unit_t * u, EVE_GpuHandle *out_result,
                               uint32_t dst_addr, EVE_GpuHandle temp_handle,
                               int32_t aligned_w, int32_t aligned_h,
                               int32_t w, int32_t h, uint32_t stride, uint32_t buf_size);

/**********************
 * SHARED HELPERS
 **********************/

/** Configure bitmap for a plain ARGB8 blit. */
static void setup_blit(EVE_HalContext *phost, uint32_t addr,
                       uint32_t stride, int32_t w, int32_t h)
{
    EVE_CoDl_bitmapHandle(phost, EVE_CO_SCRATCH_HANDLE);
    EVE_CoDl_bitmapSource(phost, addr);
    EVE_CoDl_bitmapLayout(phost, ARGB8, stride, h);
    EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, w, h);
    EVE_CoDl_bitmapTransform_identity(phost);
}

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

/** Draw the bitmap on the scratch handle at the origin. */
static void draw_bitmap(EVE_HalContext *phost)
{
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_end(phost);
}

/** Draw a white rectangle over the whole area. */
static void draw_rect(EVE_HalContext *phost, int32_t w, int32_t h)
{
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_lineWidth(phost, 16);
    EVE_CoDl_begin(phost, RECTS);
    EVE_CoDl_vertex2f_0(phost, 0, 0);
    EVE_CoDl_vertex2f_0(phost, w, h);
    EVE_CoDl_end(phost);
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
    setup_blit(phost, src_addr, stride, w, h);
    draw_bitmap(phost);
}

/**********************
 * PER-CHANNEL MATH
 *
 * These use the alpha channel as scratch: they read one channel of a bitmap
 * into alpha, operate on it, and store alpha to an RGB channel.
 * Caller must have begun a channel DL.
 **********************/

/**
 * alpha = bitmap.channel, or (add) min(alpha + bitmap.channel, 255).
 * LUMINANCE sums the three channels, each scaled by a third through COLOR_A.
 * The rounding of each third leaves it within 1 of the L8 render target's
 * (r + g + b) / 3.
 */
static void draw_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                         int32_t w, int32_t h, uint8_t channel, bool add)
{
    if(channel == LUMINANCE) {
        EVE_CoDl_colorArgb_ex(phost, 0x55FFFFFF);
        for(int i = 0; i < 3; i++) {
            EVE_CoDl_blendFunc(phost, ONE, (add || i > 0) ? ONE : ZERO);
            setup_swizzled_blit(phost, addr, stride, w, h, rgb_channels[i].channel);
            draw_bitmap(phost);
        }
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    }
    else {
        EVE_CoDl_blendFunc(phost, ONE, add ? ONE : ZERO);
        setup_swizzled_blit(phost, addr, stride, w, h, channel);
        draw_bitmap(phost);
    }
}

/** alpha = bitmap.channel */
static void load_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                         int32_t w, int32_t h, uint8_t channel)
{
    EVE_CoDl_colorMask(phost, 0, 0, 0, 1);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    draw_channel(phost, addr, stride, w, h, channel, false);
}

/** alpha = min(alpha + bitmap.channel, 255) */
static void add_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                        int32_t w, int32_t h, uint8_t channel)
{
    draw_channel(phost, addr, stride, w, h, channel, true);
}

/** alpha = alpha * bitmap.channel / 255 (a real channel, not LUMINANCE) */
static void multiply_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                             int32_t w, int32_t h, uint8_t channel)
{
    EVE_CoDl_blendFunc(phost, DST_ALPHA, ZERO);
    setup_swizzled_blit(phost, addr, stride, w, h, channel);
    draw_bitmap(phost);
}

/**
 * alpha = max(alpha - bitmap.channel, 0)
 * The additions clamp at 255 as they go, which gives the same result as
 * clamping LUMINANCE's sum once.
 *
 * Uses complement identity: a - b = 255 - ((255 - a) + b).
 * Hardware clamps the addition to 255, so when b > a the final invert gives 0.
 */
static void subtract_channel(EVE_HalContext *phost, uint32_t addr, uint32_t stride,
                             int32_t w, int32_t h, uint8_t channel)
{
    EVE_CoDl_blendFunc(phost, ONE_MINUS_DST_ALPHA, ZERO);
    draw_rect(phost, w, h);
    add_channel(phost, addr, stride, w, h, channel);
    EVE_CoDl_blendFunc(phost, ONE_MINUS_DST_ALPHA, ZERO);
    draw_rect(phost, w, h);
}

/** Write alpha to an RGB channel (add: channel = min(channel + alpha, 255)). */
static void store_alpha(EVE_HalContext *phost, int32_t w, int32_t h,
                        uint8_t r_mask, uint8_t g_mask, uint8_t b_mask, bool add)
{
    EVE_CoDl_colorMask(phost, r_mask, g_mask, b_mask, 0);
    EVE_CoDl_blendFunc(phost, DST_ALPHA, add ? ONE : ZERO);
    draw_rect(phost, w, h);
}

/**
 * Composite temp over dst into a new result buffer.
 * Temp holds the premultiplied result f(d, s)*a and the coverage a, so
 * blend(ONE, ONE_MINUS_SRC_ALPHA) gives f(d, s)*a + d*(1-a).
 * Allocates result buffer; caller owns it on success. Consumes temp_handle:
 * scoped-freed on success and on result-allocation failure.
 */
static bool composite_over_dst(lv_draw_eve5_unit_t * u, EVE_GpuHandle *out_result,
                               uint32_t dst_addr, EVE_GpuHandle temp_handle,
                               int32_t aligned_w, int32_t aligned_h,
                               int32_t w, int32_t h, uint32_t stride, uint32_t buf_size)
{
    EVE_HalContext *phost = u->hal;

    uint32_t temp_addr = EVE_GpuAlloc_Get(u->allocator, temp_handle);
    if(temp_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    EVE_GpuHandle result_handle = EVE_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t result_addr = EVE_GpuAlloc_Get(u->allocator, result_handle);
    if(result_addr == GA_INVALID) {
        EVE_GpuAlloc_ScopedFree(u->allocator, temp_handle);
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    begin_channel_dl(phost, result_addr, aligned_w, aligned_h, w, h);

    /* Blit dst as base */
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    setup_blit(phost, dst_addr, stride, w, h);
    draw_bitmap(phost);

    /* Composite temp over dst base */
    EVE_CoDl_blendFunc(phost, ONE, ONE_MINUS_SRC_ALPHA);
    EVE_CoDl_bitmapSource(phost, temp_addr);
    draw_bitmap(phost);

    finish_channel_dl(phost);

    /* Released at the enclosing op scope's close sync */
    EVE_GpuAlloc_ScopedFree(u->allocator, temp_handle);

    *out_result = result_handle;
    return true;
}

/**********************
 * GLOBAL FUNCTIONS
 **********************/

/**
 * MULTIPLY: temp.c = d.c * P.c
 * @param luminance  true: run on the luminances (L8 layers)
 * 2 DL cycles: channel math + composite.
 */
bool lv_draw_eve5_blend_multiply(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                 EVE_GpuHandle dst_handle, EVE_GpuHandle src_handle,
                                 bool luminance, EVE_GpuHandle *out_result)
{
    EVE_HalContext *phost = u->hal;
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aw = ALIGN_UP(w, 16);
    int32_t ah = ALIGN_UP(h, 16);
    uint32_t stride = aw * 4;
    uint32_t buf_size = stride * ah;

    uint32_t dst_addr = EVE_GpuAlloc_Get(u->allocator, dst_handle);
    uint32_t src_addr = EVE_GpuAlloc_Get(u->allocator, src_handle);
    if(dst_addr == GA_INVALID || src_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    EVE_GpuHandle temp = EVE_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp_addr = EVE_GpuAlloc_Get(u->allocator, temp);
    if(temp_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    /* One epoch scope covers all of this op's DL segments; the temps'
     * stamps gate their scoped frees on the op's close sync. */
    EVE_GpuAlloc_OpenScope(u->allocator);

    begin_channel_dl(phost, temp_addr, aw, ah, w, h);
    if(luminance) {
        /* A sum can't scale alpha in one draw: store the dst luminance,
         * then scale the stored value by the src luminance */
        load_channel(phost, dst_addr, stride, w, h, LUMINANCE);
        store_alpha(phost, w, h, 1, 1, 1, false);
        load_channel(phost, src_addr, stride, w, h, LUMINANCE);
        EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
        EVE_CoDl_blendFunc(phost, ZERO, DST_ALPHA);
        draw_rect(phost, w, h);
    }
    else {
        for(int i = 0; i < 3; i++) {
            const blend_channel_t * ch = &rgb_channels[i];
            load_channel(phost, dst_addr, stride, w, h, ch->channel);
            multiply_channel(phost, src_addr, stride, w, h, ch->channel);
            store_alpha(phost, w, h, ch->r_mask, ch->g_mask, ch->b_mask, false);
        }
    }
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    bool ok = composite_over_dst(u, out_result, dst_addr, temp, aw, ah, w, h, stride, buf_size);
    EVE_GpuAlloc_CloseScope(u->allocator, EVE_Cmd_sync(phost));
    return ok;
}

/**
 * SUBTRACTIVE: temp.c = max(d.c * a - P.c, 0)
 * @param luminance  true: run on the luminances (L8 layers)
 * 2 DL cycles: channel math + composite.
 */
bool lv_draw_eve5_blend_subtractive(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                    EVE_GpuHandle dst_handle, EVE_GpuHandle src_handle,
                                    bool luminance, EVE_GpuHandle *out_result)
{
    EVE_HalContext *phost = u->hal;
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aw = ALIGN_UP(w, 16);
    int32_t ah = ALIGN_UP(h, 16);
    uint32_t stride = aw * 4;
    uint32_t buf_size = stride * ah;

    uint32_t dst_addr = EVE_GpuAlloc_Get(u->allocator, dst_handle);
    uint32_t src_addr = EVE_GpuAlloc_Get(u->allocator, src_handle);
    if(dst_addr == GA_INVALID || src_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    EVE_GpuHandle temp = EVE_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp_addr = EVE_GpuAlloc_Get(u->allocator, temp);
    if(temp_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    /* One epoch scope covers all of this op's DL segments */
    EVE_GpuAlloc_OpenScope(u->allocator);

    const blend_channel_t * chs = luminance ? luminance_channel : rgb_channels;
    int n = luminance ? 1 : 3;
    begin_channel_dl(phost, temp_addr, aw, ah, w, h);
    for(int i = 0; i < n; i++) {
        const blend_channel_t * ch = &chs[i];
        load_channel(phost, dst_addr, stride, w, h, ch->channel);
        multiply_channel(phost, src_addr, stride, w, h, ALPHA);
        subtract_channel(phost, src_addr, stride, w, h, ch->channel);
        store_alpha(phost, w, h, ch->r_mask, ch->g_mask, ch->b_mask, false);
    }
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    bool ok = composite_over_dst(u, out_result, dst_addr, temp, aw, ah, w, h, stride, buf_size);
    EVE_GpuAlloc_CloseScope(u->allocator, EVE_Cmd_sync(phost));
    return ok;
}

/**
 * DIFFERENCE: temp.c = |d.c * a - P.c|
 *                    = max(d.c * a - P.c, 0) + max(P.c - d.c * a, 0)
 *
 * The second term adds d.c * a to alpha, which one draw can't do, so d * a
 * is rendered to a bitmap first.
 * @param luminance  true: run on the luminances (L8 layers)
 * 3 DL cycles: d * a + channel math + composite.
 */
bool lv_draw_eve5_blend_difference(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                   EVE_GpuHandle dst_handle, EVE_GpuHandle src_handle,
                                   bool luminance, EVE_GpuHandle *out_result)
{
    EVE_HalContext *phost = u->hal;
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    int32_t aw = ALIGN_UP(w, 16);
    int32_t ah = ALIGN_UP(h, 16);
    uint32_t stride = aw * 4;
    uint32_t buf_size = stride * ah;

    uint32_t dst_addr = EVE_GpuAlloc_Get(u->allocator, dst_handle);
    uint32_t src_addr = EVE_GpuAlloc_Get(u->allocator, src_handle);
    if(dst_addr == GA_INVALID || src_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    EVE_GpuHandle scaled = EVE_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t scaled_addr = EVE_GpuAlloc_Get(u->allocator, scaled);
    if(scaled_addr == GA_INVALID) {
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    /* One epoch scope covers all three of this op's DL segments */
    EVE_GpuAlloc_OpenScope(u->allocator);

    /* DL1: scaled.rgb = d.rgb * a */
    begin_channel_dl(phost, scaled_addr, aw, ah, w, h);
    EVE_CoDl_colorMask(phost, 1, 1, 1, 0);
    EVE_CoDl_blendFunc(phost, ONE, ZERO);
    setup_blit(phost, dst_addr, stride, w, h);
    draw_bitmap(phost);
    EVE_CoDl_blendFunc(phost, ZERO, SRC_ALPHA);
    EVE_CoDl_bitmapSource(phost, src_addr);
    draw_bitmap(phost);
    finish_channel_dl(phost);

    /* DL2: temp.c = max(scaled.c - P.c, 0) + max(P.c - scaled.c, 0) */
    EVE_GpuHandle temp = EVE_GpuAlloc_Alloc(u->allocator, buf_size, GA_ALIGN_128);
    uint32_t temp_addr = EVE_GpuAlloc_Get(u->allocator, temp);
    /* Re-resolve addresses (the allocator may have moved things) */
    scaled_addr = EVE_GpuAlloc_Get(u->allocator, scaled);
    src_addr = EVE_GpuAlloc_Get(u->allocator, src_handle);
    dst_addr = EVE_GpuAlloc_Get(u->allocator, dst_handle);
    if(temp_addr == GA_INVALID || scaled_addr == GA_INVALID ||
       src_addr == GA_INVALID || dst_addr == GA_INVALID) {
        EVE_GpuAlloc_ScopedFree(u->allocator, scaled);
        if(temp_addr != GA_INVALID) EVE_GpuAlloc_ScopedFree(u->allocator, temp);
        EVE_GpuAlloc_CloseScope(u->allocator, EVE_Cmd_sync(phost));
        *out_result = GA_HANDLE_INVALID;
        return false;
    }

    const blend_channel_t * chs = luminance ? luminance_channel : rgb_channels;
    int n = luminance ? 1 : 3;
    begin_channel_dl(phost, temp_addr, aw, ah, w, h);
    for(int i = 0; i < n; i++) {
        const blend_channel_t * ch = &chs[i];
        load_channel(phost, scaled_addr, stride, w, h, ch->channel);
        subtract_channel(phost, src_addr, stride, w, h, ch->channel);
        store_alpha(phost, w, h, ch->r_mask, ch->g_mask, ch->b_mask, false);
        load_channel(phost, src_addr, stride, w, h, ch->channel);
        subtract_channel(phost, scaled_addr, stride, w, h, ch->channel);
        store_alpha(phost, w, h, ch->r_mask, ch->g_mask, ch->b_mask, true);
    }
    copy_src_alpha(phost, src_addr, stride, w, h);
    finish_channel_dl(phost);

    /* Released at the op scope's close sync */
    EVE_GpuAlloc_ScopedFree(u->allocator, scaled);

    /* DL3: composite temp over dst */
    bool ok = composite_over_dst(u, out_result, dst_addr, temp, aw, ah, w, h, stride, buf_size);
    EVE_GpuAlloc_CloseScope(u->allocator, EVE_Cmd_sync(phost));
    return ok;
}

#endif /* EVE_SUPPORT_RENDERTARGET */
#endif /* LV_USE_DRAW_EVE5 */
