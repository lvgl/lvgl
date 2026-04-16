/**
 * @file lv_draw_eve5_gaussian.c
 *
 * EVE5 (BT820) Gaussian Blur via Mipmap Downsampling
 *
 * Separable Gaussian kernel with nearest sampling throughout the
 * pyramid, bilinear only on final upscale.
 *
 * Pipeline:
 * 1. Extract blur region with edge-extended padding
 * 2. Build pyramid: each tier applies H blur+2x downsample then V blur+2x
 *    downsample using the binomial [1,4,6,4,1]/16 kernel (sigma^2 = 1).
 *    Stop before the next tier would overshoot sigma^2_target.
 * 3. Extra pass: one H+V blur at the deepest level (no downsample)
 *    with Gaussian weights computed for sigma^2_remaining. Uses 5-tap,
 *    7-tap, or 9-tap kernel selected at runtime based on sigma^2_local.
 * 4. Single bilinear upscale from the final level back to output.
 *
 * Per-tier contribution to original sigma^2 = 4^tier (8x-scaled in code).
 * Accumulated sigma^2 after n tiers = (4^n - 1) / 3.
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
#define MAX_BLUR_LEVELS   (1 + MAX_PYRAMID_TIERS + 1)  /* extract + pyramid + extra */

/* Binomial kernel [1,4,6,4,1]/16 weights in 8-bit (sum = 256).
 * Exact sigma^2 = 1.0 per pass. */
#define BINOM_W_OUTER  16
#define BINOM_W_INNER  64
#define BINOM_W_CENTER 96

/* Final pass thresholds (table index k = sigma^2_local * 16).
 * FINAL_K_MAX: maximum sigma^2_local the final pass handles (9-tap range).
 *   Pyramid stops when remaining sigma^2 fits within this range.
 * THRESHOLD_5TO7: below this, +-3 taps carry <1% energy — 5-tap saves 4 draws.
 * THRESHOLD_7TO9: above this, 7-tap truncation exceeds ~1% — 9-tap captures the tail. */
#define FINAL_K_MAX             48  /* sigma^2_local = 3.0 */
#define FINAL_K_THRESHOLD_5TO7  16  /* sigma^2_local <= 1.0: use 5-tap */
#define FINAL_K_THRESHOLD_7TO9  36  /* sigma^2_local > 2.25: use 9-tap */

/**********************
 * TYPES
 **********************/
typedef struct {
    Esd_GpuHandle handle;
    int32_t w, h;       /**< Logical pixel dimensions */
    int32_t aw, ah;     /**< 16-byte aligned dimensions for render target */
    int32_t sigma_sq;   /**< Accumulated variance in original pixels, 8x-scaled */
} gauss_level_t;

/**********************
 * 5-TAP GAUSSIAN WEIGHT TABLE
 *
 * Weights for taps at positions [-2,-1,0,+1,+2], indexed by sigma^2
 * in 1/16 steps (k = 0..48, sigma^2 = k/16, range 0..3).
 *
 * Entry format: {w_outer, w_inner}.
 * w_center = 256 - 2*(w_outer + w_inner).
 *
 * Computed from: w(x) = exp(-x^2 / (2*sigma^2)), normalized over 5 taps.
 **********************/
static const uint8_t s_gauss5_weights[49][2] = {
    /* k=0  sigma^2=0.000 */ {  0,   0 },  /* passthrough */
    /* k=1  sigma^2=0.062 */ {  0,   0 },
    /* k=2  sigma^2=0.125 */ {  0,   5 },
    /* k=3  sigma^2=0.188 */ {  0,  16 },
    /* k=4  sigma^2=0.250 */ {  0,  27 },
    /* k=5  sigma^2=0.312 */ {  0,  37 },
    /* k=6  sigma^2=0.375 */ {  1,  44 },
    /* k=7  sigma^2=0.438 */ {  2,  49 },
    /* k=8  sigma^2=0.500 */ {  3,  53 },
    /* k=9  sigma^2=0.562 */ {  4,  56 },
    /* k=10 sigma^2=0.625 */ {  5,  58 },
    /* k=11 sigma^2=0.688 */ {  7,  60 },
    /* k=12 sigma^2=0.750 */ {  8,  61 },
    /* k=13 sigma^2=0.812 */ { 10,  61 },
    /* k=14 sigma^2=0.875 */ { 11,  62 },
    /* k=15 sigma^2=0.938 */ { 13,  62 },
    /* k=16 sigma^2=1.000 */ { 14,  63 },
    /* k=17 sigma^2=1.062 */ { 15,  63 },
    /* k=18 sigma^2=1.125 */ { 17,  63 },
    /* k=19 sigma^2=1.188 */ { 18,  63 },
    /* k=20 sigma^2=1.250 */ { 19,  63 },
    /* k=21 sigma^2=1.312 */ { 20,  62 },
    /* k=22 sigma^2=1.375 */ { 21,  62 },
    /* k=23 sigma^2=1.438 */ { 22,  62 },
    /* k=24 sigma^2=1.500 */ { 23,  62 },
    /* k=25 sigma^2=1.562 */ { 24,  62 },
    /* k=26 sigma^2=1.625 */ { 24,  62 },
    /* k=27 sigma^2=1.688 */ { 25,  61 },
    /* k=28 sigma^2=1.750 */ { 26,  61 },
    /* k=29 sigma^2=1.812 */ { 27,  61 },
    /* k=30 sigma^2=1.875 */ { 27,  61 },
    /* k=31 sigma^2=1.938 */ { 28,  61 },
    /* k=32 sigma^2=2.000 */ { 29,  61 },
    /* k=33 sigma^2=2.062 */ { 29,  60 },
    /* k=34 sigma^2=2.125 */ { 30,  60 },
    /* k=35 sigma^2=2.188 */ { 30,  60 },
    /* k=36 sigma^2=2.250 */ { 31,  60 },
    /* k=37 sigma^2=2.312 */ { 31,  60 },
    /* k=38 sigma^2=2.375 */ { 32,  60 },
    /* k=39 sigma^2=2.438 */ { 32,  59 },
    /* k=40 sigma^2=2.500 */ { 33,  59 },
    /* k=41 sigma^2=2.562 */ { 33,  59 },
    /* k=42 sigma^2=2.625 */ { 33,  59 },
    /* k=43 sigma^2=2.688 */ { 34,  59 },
    /* k=44 sigma^2=2.750 */ { 34,  59 },
    /* k=45 sigma^2=2.812 */ { 34,  59 },
    /* k=46 sigma^2=2.875 */ { 35,  58 },
    /* k=47 sigma^2=2.938 */ { 35,  58 },
    /* k=48 sigma^2=3.000 */ { 35,  58 },
};

/**********************
 * 7-TAP GAUSSIAN WEIGHT TABLE
 *
 * Weights for taps at positions [-3..+3], indexed by sigma^2
 * in 1/16 steps (k = 0..48, sigma^2 = k/16, range 0..3).
 *
 * Entry format: {w_outer3, w_middle2, w_inner1}.
 * w_center = 256 - 2*(w_outer3 + w_middle2 + w_inner1).
 *
 * Computed from: w(x) = exp(-x^2 / (2*sigma^2)), normalized over 7 taps.
 **********************/
static const uint8_t s_gauss7_weights[49][3] = {
    /* k=0  sigma^2=0.000 */ {  0,   0,   0 },  /* passthrough */
    /* k=1  sigma^2=0.063 */ {  0,   0,   0 },
    /* k=2  sigma^2=0.125 */ {  0,   0,   5 },
    /* k=3  sigma^2=0.188 */ {  0,   0,  16 },
    /* k=4  sigma^2=0.250 */ {  0,   0,  27 },
    /* k=5  sigma^2=0.313 */ {  0,   0,  37 },
    /* k=6  sigma^2=0.375 */ {  0,   1,  44 },
    /* k=7  sigma^2=0.438 */ {  0,   2,  49 },
    /* k=8  sigma^2=0.500 */ {  0,   3,  53 },
    /* k=9  sigma^2=0.563 */ {  0,   4,  56 },
    /* k=10 sigma^2=0.625 */ {  0,   5,  58 },
    /* k=11 sigma^2=0.688 */ {  0,   7,  60 },
    /* k=12 sigma^2=0.750 */ {  0,   8,  61 },
    /* k=13 sigma^2=0.813 */ {  0,  10,  61 },
    /* k=14 sigma^2=0.875 */ {  1,  11,  62 },
    /* k=15 sigma^2=0.938 */ {  1,  12,  62 },
    /* k=16 sigma^2=1.000 */ {  1,  14,  62 },
    /* k=17 sigma^2=1.063 */ {  1,  15,  62 },
    /* k=18 sigma^2=1.125 */ {  2,  16,  62 },
    /* k=19 sigma^2=1.188 */ {  2,  17,  62 },
    /* k=20 sigma^2=1.250 */ {  2,  18,  61 },
    /* k=21 sigma^2=1.313 */ {  3,  19,  61 },
    /* k=22 sigma^2=1.375 */ {  3,  20,  61 },
    /* k=23 sigma^2=1.438 */ {  4,  21,  60 },
    /* k=24 sigma^2=1.500 */ {  4,  22,  60 },
    /* k=25 sigma^2=1.563 */ {  5,  23,  60 },
    /* k=26 sigma^2=1.625 */ {  5,  24,  59 },
    /* k=27 sigma^2=1.688 */ {  5,  24,  59 },
    /* k=28 sigma^2=1.750 */ {  6,  25,  58 },
    /* k=29 sigma^2=1.813 */ {  6,  25,  58 },
    /* k=30 sigma^2=1.875 */ {  7,  26,  58 },
    /* k=31 sigma^2=1.938 */ {  7,  26,  57 },
    /* k=32 sigma^2=2.000 */ {  8,  27,  57 },
    /* k=33 sigma^2=2.063 */ {  8,  27,  57 },
    /* k=34 sigma^2=2.125 */ {  9,  28,  56 },
    /* k=35 sigma^2=2.188 */ {  9,  28,  56 },
    /* k=36 sigma^2=2.250 */ {  9,  28,  55 },
    /* k=37 sigma^2=2.313 */ { 10,  29,  55 },
    /* k=38 sigma^2=2.375 */ { 10,  29,  55 },
    /* k=39 sigma^2=2.438 */ { 11,  29,  55 },
    /* k=40 sigma^2=2.500 */ { 11,  30,  54 },
    /* k=41 sigma^2=2.563 */ { 11,  30,  54 },
    /* k=42 sigma^2=2.625 */ { 12,  30,  54 },
    /* k=43 sigma^2=2.688 */ { 12,  31,  53 },
    /* k=44 sigma^2=2.750 */ { 12,  31,  53 },
    /* k=45 sigma^2=2.813 */ { 13,  31,  53 },
    /* k=46 sigma^2=2.875 */ { 13,  31,  53 },
    /* k=47 sigma^2=2.938 */ { 13,  31,  52 },
    /* k=48 sigma^2=3.000 */ { 14,  32,  52 },
};

/**********************
 * 9-TAP GAUSSIAN WEIGHT TABLE
 *
 * Weights for taps at positions [-4..+4], indexed by sigma^2
 * in 1/16 steps (k = 0..48, sigma^2 = k/16, range 0..3).
 *
 * Entry format: {w_outer4, w_middle3, w_inner2, w_inner1}.
 * w_center = 256 - 2*(w_outer4 + w_middle3 + w_inner2 + w_inner1).
 *
 * Computed from: w(x) = exp(-x^2 / (2*sigma^2)), normalized over 9 taps.
 **********************/
static const uint8_t s_gauss9_weights[49][4] = {
    /* k=0  sigma^2=0.000 */ {  0,   0,   0,   0 },  /* passthrough */
    /* k=1  sigma^2=0.062 */ {  0,   0,   0,   0 },
    /* k=2  sigma^2=0.125 */ {  0,   0,   0,   5 },
    /* k=3  sigma^2=0.188 */ {  0,   0,   0,  16 },
    /* k=4  sigma^2=0.250 */ {  0,   0,   0,  27 },
    /* k=5  sigma^2=0.312 */ {  0,   0,   0,  37 },
    /* k=6  sigma^2=0.375 */ {  0,   0,   1,  44 },
    /* k=7  sigma^2=0.438 */ {  0,   0,   2,  49 },
    /* k=8  sigma^2=0.500 */ {  0,   0,   3,  53 },
    /* k=9  sigma^2=0.562 */ {  0,   0,   4,  56 },
    /* k=10 sigma^2=0.625 */ {  0,   0,   5,  58 },
    /* k=11 sigma^2=0.688 */ {  0,   0,   7,  60 },
    /* k=12 sigma^2=0.750 */ {  0,   0,   8,  61 },
    /* k=13 sigma^2=0.812 */ {  0,   0,  10,  61 },
    /* k=14 sigma^2=0.875 */ {  0,   1,  11,  62 },
    /* k=15 sigma^2=0.938 */ {  0,   1,  12,  62 },
    /* k=16 sigma^2=1.000 */ {  0,   1,  14,  62 },
    /* k=17 sigma^2=1.062 */ {  0,   1,  15,  62 },
    /* k=18 sigma^2=1.125 */ {  0,   2,  16,  62 },
    /* k=19 sigma^2=1.188 */ {  0,   2,  17,  62 },
    /* k=20 sigma^2=1.250 */ {  0,   2,  18,  61 },
    /* k=21 sigma^2=1.312 */ {  0,   3,  19,  61 },
    /* k=22 sigma^2=1.375 */ {  0,   3,  20,  61 },
    /* k=23 sigma^2=1.438 */ {  0,   4,  21,  60 },
    /* k=24 sigma^2=1.500 */ {  0,   4,  22,  60 },
    /* k=25 sigma^2=1.562 */ {  0,   5,  23,  59 },
    /* k=26 sigma^2=1.625 */ {  1,   5,  23,  59 },
    /* k=27 sigma^2=1.688 */ {  1,   5,  24,  58 },
    /* k=28 sigma^2=1.750 */ {  1,   6,  25,  58 },
    /* k=29 sigma^2=1.812 */ {  1,   6,  25,  58 },
    /* k=30 sigma^2=1.875 */ {  1,   7,  26,  57 },
    /* k=31 sigma^2=1.938 */ {  1,   7,  26,  57 },
    /* k=32 sigma^2=2.000 */ {  1,   8,  27,  56 },
    /* k=33 sigma^2=2.062 */ {  1,   8,  27,  56 },
    /* k=34 sigma^2=2.125 */ {  2,   8,  27,  55 },
    /* k=35 sigma^2=2.188 */ {  2,   9,  28,  55 },
    /* k=36 sigma^2=2.250 */ {  2,   9,  28,  55 },
    /* k=37 sigma^2=2.312 */ {  2,  10,  28,  54 },
    /* k=38 sigma^2=2.375 */ {  2,  10,  29,  54 },
    /* k=39 sigma^2=2.438 */ {  2,  10,  29,  53 },
    /* k=40 sigma^2=2.500 */ {  3,  11,  29,  53 },
    /* k=41 sigma^2=2.562 */ {  3,  11,  29,  53 },
    /* k=42 sigma^2=2.625 */ {  3,  11,  30,  52 },
    /* k=43 sigma^2=2.688 */ {  3,  12,  30,  52 },
    /* k=44 sigma^2=2.750 */ {  3,  12,  30,  52 },
    /* k=45 sigma^2=2.812 */ {  4,  12,  30,  51 },
    /* k=46 sigma^2=2.875 */ {  4,  13,  30,  51 },
    /* k=47 sigma^2=2.938 */ {  4,  13,  30,  51 },
    /* k=48 sigma^2=3.000 */ {  4,  13,  31,  50 },
};

/**********************
 * STATIC PROTOTYPES
 **********************/
static bool gaussian_5tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal, bool downsample,
                               uint8_t w_outer, uint8_t w_inner, uint8_t w_center);

static bool gaussian_7tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal,
                               uint8_t w_outer, uint8_t w_middle, uint8_t w_inner, uint8_t w_center);

static bool gaussian_9tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal,
                               uint8_t w_o4, uint8_t w_o3, uint8_t w_i2, uint8_t w_i1, uint8_t w_center);

/**********************
 * 5-TAP GAUSSIAN PASS
 **********************/

/**
 * One separable 5-tap Gaussian blur pass with optional 2x downsample.
 *
 * Renders 5 nearest-sampled bitmap draws at integer source offsets
 * [-2,-1,0,+1,+2] with additive blending.
 */
static bool gaussian_5tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal, bool downsample,
                               uint8_t w_outer, uint8_t w_inner, uint8_t w_center)
{
    EVE_HalContext *phost = u->hal;

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

    EVE_CoDl_bitmapTransform_identity(phost);
    if(horizontal) {
        EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_8_8);
    }
    else {
        EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_8_8);
    }

    EVE_CoDl_blendFunc(phost, ONE, ONE);

    const int32_t offsets[5] = { -2, -1, 0, 1, 2 };
    const uint8_t weights[5] = { w_outer, w_inner, w_center, w_inner, w_outer };

    EVE_CoDl_begin(phost, BITMAPS);

    for(int i = 0; i < 5; i++) {
        if(weights[i] == 0) continue;

        EVE_CoDl_colorRgb(phost, weights[i], weights[i], weights[i]);
        EVE_CoDl_colorA(phost, weights[i]);

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
 * 7-TAP GAUSSIAN PASS
 **********************/

/**
 * One separable 7-tap Gaussian blur pass (no downsample).
 * 7 nearest-sampled draws at [-3..+3].
 */
static bool gaussian_7tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal,
                               uint8_t w_outer, uint8_t w_middle, uint8_t w_inner, uint8_t w_center)
{
    EVE_HalContext *phost = u->hal;

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

    EVE_CoDl_bitmapTransform_identity(phost);

    EVE_CoDl_blendFunc(phost, ONE, ONE);

    const int32_t offsets[7] = { -3, -2, -1, 0, 1, 2, 3 };
    const uint8_t weights[7] = { w_outer, w_middle, w_inner, w_center,
                                  w_inner, w_middle, w_outer
                                };

    EVE_CoDl_begin(phost, BITMAPS);

    for(int i = 0; i < 7; i++) {
        if(weights[i] == 0) continue;

        EVE_CoDl_colorRgb(phost, weights[i], weights[i], weights[i]);
        EVE_CoDl_colorA(phost, weights[i]);

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
 * 9-TAP GAUSSIAN PASS
 **********************/

/**
 * One separable 9-tap Gaussian blur pass (no downsample).
 * 9 nearest-sampled draws at [-4..+4]; zero-weight taps are skipped.
 */
static bool gaussian_9tap_pass(lv_draw_eve5_unit_t * u,
                               uint32_t src_addr, int32_t src_stride, int32_t src_h,
                               uint32_t dst_addr, int32_t dst_aw, int32_t dst_ah,
                               int32_t dst_w, int32_t dst_h,
                               bool horizontal,
                               uint8_t w_o4, uint8_t w_o3, uint8_t w_i2, uint8_t w_i1, uint8_t w_center)
{
    EVE_HalContext *phost = u->hal;

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

    EVE_CoDl_bitmapTransform_identity(phost);

    EVE_CoDl_blendFunc(phost, ONE, ONE);

    const int32_t offsets[9] = { -4, -3, -2, -1, 0, 1, 2, 3, 4 };
    const uint8_t weights[9] = { w_o4, w_o3, w_i2, w_i1, w_center,
                                  w_i1, w_i2, w_o3, w_o4
                                };

    EVE_CoDl_begin(phost, BITMAPS);

    for(int i = 0; i < 9; i++) {
        if(weights[i] == 0) continue;

        EVE_CoDl_colorRgb(phost, weights[i], weights[i], weights[i]);
        EVE_CoDl_colorA(phost, weights[i]);

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
 * Gaussian blur of a region within a completed layer buffer.
 * Modifies dst_handle in place.
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

    /* Full blur area in layer-relative coords (for rounded rect geometry) */
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

    /* Convert blur_radius to sigma^2 (8x-scaled for precision).
     * Uses effective_r = R/2 to match LVGL's AUTO quality IIR filter behavior.
     * sigma^2 = (R/2) * (R/2 + 4) / 8 = R*(R+8)/32 -> scaled by 8 = R*(R+8)/4 */
    int32_t sigma_sq_target = blur_radius * (blur_radius + 8) / 4;
    if(sigma_sq_target < 1) sigma_sq_target = 1;

    /* Pre-estimate pyramid depth for padding computation */
    int32_t n_tiers_est = 0;
    {
        int32_t acc = 0, p4 = 1;
        int32_t test_w = bw + 2 * blur_radius;
        int32_t test_h = bh + 2 * blur_radius;
        while(n_tiers_est < MAX_PYRAMID_TIERS) {
            test_w = (test_w + 1) / 2;
            test_h = (test_h + 1) / 2;
            if(test_w < 4 || test_h < 4) break;
            if((sigma_sq_target - acc) * 2 <= FINAL_K_MAX * p4) break;
            acc += p4 * 8;
            p4 *= 4;
            n_tiers_est++;
        }
    }

    /* Padding covers cumulative kernel reach through all tiers plus final pass */
    #define FINAL_HALF_TAP 4
    int32_t scale_n = 1 << n_tiers_est;
    int32_t pyramid_reach = 2 * (scale_n - 1);
    int32_t final_reach = FINAL_HALF_TAP * scale_n;
    int32_t total_reach = pyramid_reach + final_reach;
    if(n_tiers_est == 0) total_reach = FINAL_HALF_TAP * 2;
    int32_t pad = LV_MAX(blur_radius, total_reach);

    /* Padded content dimensions (16px RT alignment) */
    int32_t pw = ALIGN_UP(bw + 2 * pad, 16);
    int32_t ph = ALIGN_UP(bh + 2 * pad, 16);
    pad = (pw - bw) / 2;
    int32_t pad_y = (ph - bh) / 2;
    int32_t paw = ALIGN_UP(pw, 16);
    int32_t pah = ALIGN_UP(ph, 16);

    /* Phase 1: Extract blur region with edge padding */
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

        /* Draw 1: stretch source to fill padded buffer (edge extension) */
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

        /* Rounded corner pre-mask: stencil the rounded rect so draw 2 only
         * writes inside it. Content outside keeps the stretched edge extension,
         * preventing foreign content from contaminating the blur. */
        if(corner_radius > 0) {
            int32_t rx1 = full_x1 - bx1 + pad;
            int32_t ry1 = full_y1 - by1 + pad_y;
            int32_t rx2 = full_x2 - bx1 + pad;
            int32_t ry2 = full_y2 - by1 + pad_y;

            EVE_CoDl_saveContext(phost);

            EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
            EVE_CoDl_stencilOp(phost, KEEP, INCR);
            EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
            lv_draw_eve5_draw_rect(u, rx1, ry1, rx2, ry2, corner_radius, NULL, NULL);

            EVE_CoDl_colorMask(phost, 1, 1, 1, 1);
            EVE_CoDl_stencilFunc(phost, NOTEQUAL, 0, 255);
            EVE_CoDl_stencilOp(phost, KEEP, KEEP);
        }

        /* Draw 2: blit source 1:1 at center, masked to rounded rect when applicable.
         * Re-set bitmap state since draw_rect may have clobbered it. */
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);
        EVE_CoDl_bitmapSource(phost, dst_addr + src_ofs);
        EVE_CoDl_bitmapLayout(phost, ARGB8, layer_stride, layer_h - by1);
        EVE_CoDl_bitmapSize(phost, NEAREST, BORDER, BORDER, bw, bh);
        EVE_CoDl_bitmapTransform_identity(phost);

        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, pad, pad_y);
        EVE_CoDl_end(phost);

        if(corner_radius > 0) {
            EVE_CoDl_restoreContext(phost);
        }

        EVE_CoDl_display(phost);
        EVE_CoCmd_swap(phost);
        EVE_CoCmd_graphicsFinish(phost);
    }

    /* Allocate reusable H-pass temp buffer */
    int32_t temp_aw = (n_tiers_est > 0) ? ALIGN_UP((pw + 1) / 2, 16) : paw;
    int32_t temp_ah = ALIGN_UP(ph, 16);
    uint32_t temp_size = (uint32_t)temp_aw * 4 * (uint32_t)temp_ah;
    Esd_GpuHandle temp_handle = Esd_GpuAlloc_Alloc(u->allocator, temp_size, GA_ALIGN_128);
    if(Esd_GpuAlloc_Get(u->allocator, temp_handle) == GA_INVALID) {
        LV_LOG_WARN("EVE5 gaussian: failed to allocate temp buffer");
        Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
        return true;
    }

    /* Phase 2: Build Gaussian pyramid */
    gauss_level_t levels[MAX_BLUR_LEVELS];
    int32_t n_levels = 0;

    levels[0].handle = extract_handle;
    levels[0].w = pw;
    levels[0].h = ph;
    levels[0].aw = paw;
    levels[0].ah = pah;
    levels[0].sigma_sq = 0;
    n_levels = 1;

    int32_t pow4 = 1;

    for(int32_t tier = 0; tier < MAX_PYRAMID_TIERS; tier++) {
        int32_t remaining_before = sigma_sq_target - levels[n_levels - 1].sigma_sq;
        if(remaining_before * 2 <= FINAL_K_MAX * pow4) break;

        gauss_level_t * prev = &levels[n_levels - 1];

        int32_t next_w = (prev->w + 1) / 2;
        int32_t next_h = (prev->h + 1) / 2;
        if(next_w < 4 || next_h < 4) break;
        if(n_levels >= MAX_BLUR_LEVELS - 1) break;

        int32_t next_aw = ALIGN_UP(next_w, 16);
        int32_t next_ah = ALIGN_UP(next_h, 16);

        uint32_t level_size = (uint32_t)next_aw * 4 * (uint32_t)next_ah;
        Esd_GpuHandle level_handle = Esd_GpuAlloc_Alloc(u->allocator, level_size, GA_ALIGN_128);
        if(Esd_GpuAlloc_Get(u->allocator, level_handle) == GA_INVALID) {
            LV_LOG_WARN("EVE5 gaussian: failed to allocate pyramid level %"PRId32, tier);
            break;
        }

        int32_t h_w = next_w;
        int32_t h_h = prev->h;
        int32_t h_aw = ALIGN_UP(h_w, 16);
        int32_t h_ah = ALIGN_UP(h_h, 16);

        uint32_t prev_addr = Esd_GpuAlloc_Get(u->allocator, prev->handle);
        uint32_t t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
        uint32_t lev_addr = Esd_GpuAlloc_Get(u->allocator, level_handle);
        if(prev_addr == GA_INVALID || t_addr == GA_INVALID || lev_addr == GA_INVALID) break;

        /* H blur + 2x downsample */
        gaussian_5tap_pass(u, prev_addr, prev->aw * 4, prev->h,
                           t_addr, h_aw, h_ah, h_w, h_h,
                           true, true,
                           BINOM_W_OUTER, BINOM_W_INNER, BINOM_W_CENTER);

        t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
        lev_addr = Esd_GpuAlloc_Get(u->allocator, level_handle);
        if(t_addr == GA_INVALID || lev_addr == GA_INVALID) break;

        /* V blur + 2x downsample */
        gaussian_5tap_pass(u, t_addr, h_aw * 4, h_h,
                           lev_addr, next_aw, next_ah, next_w, next_h,
                           false, true,
                           BINOM_W_OUTER, BINOM_W_INNER, BINOM_W_CENTER);

        levels[n_levels].handle = level_handle;
        levels[n_levels].w = next_w;
        levels[n_levels].h = next_h;
        levels[n_levels].aw = next_aw;
        levels[n_levels].ah = next_ah;
        levels[n_levels].sigma_sq = prev->sigma_sq + pow4 * 8;
        n_levels++;
        pow4 *= 4;
    }

    /* Phase 3: Final blur pass for remaining sigma^2 */
    {
        int32_t remaining = sigma_sq_target - levels[n_levels - 1].sigma_sq;

        if(remaining > 0 && n_levels < MAX_BLUR_LEVELS) {
            gauss_level_t * prev = &levels[n_levels - 1];

            if(prev->w >= 4 && prev->h >= 4) {

                int32_t k = remaining * 2 / pow4;
                if(k > 48) k = 48;
                if(k < 0) k = 0;

                bool use_5tap = (k <= FINAL_K_THRESHOLD_5TO7);
                bool use_9tap = (k > FINAL_K_THRESHOLD_7TO9);

                uint8_t ew_outer = s_gauss5_weights[k][0];
                uint8_t ew_inner = s_gauss5_weights[k][1];

                uint8_t ew_outer3 = s_gauss7_weights[k][0];
                uint8_t ew_middle = s_gauss7_weights[k][1];
                uint8_t ew_inner7 = s_gauss7_weights[k][2];
                uint8_t ew_center = (uint8_t)(256 - 2 * (ew_outer3 + ew_middle + ew_inner7));
                bool has_blur = (ew_outer > 0 || ew_inner > 0 || ew_outer3 > 0 || ew_middle > 0 || ew_inner7 > 0);

                uint8_t ew_o4 = 0, ew_o3 = 0, ew_i2 = 0, ew_i1 = 0;
                if(use_9tap) {
                    ew_o4 = s_gauss9_weights[k][0];
                    ew_o3 = s_gauss9_weights[k][1];
                    ew_i2 = s_gauss9_weights[k][2];
                    ew_i1 = s_gauss9_weights[k][3];
                    ew_center = (uint8_t)(256 - 2 * (ew_o4 + ew_o3 + ew_i2 + ew_i1));
                    has_blur = true;
                }

                if(has_blur) {
                    uint32_t extra_size = (uint32_t)prev->aw * 4 * (uint32_t)prev->ah;
                    Esd_GpuHandle extra_handle = Esd_GpuAlloc_Alloc(u->allocator, extra_size, GA_ALIGN_128);

                    if(Esd_GpuAlloc_Get(u->allocator, extra_handle) != GA_INVALID) {
                        uint32_t prev_addr = Esd_GpuAlloc_Get(u->allocator, prev->handle);
                        uint32_t t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
                        uint32_t ex_addr = Esd_GpuAlloc_Get(u->allocator, extra_handle);

                        if(prev_addr != GA_INVALID && t_addr != GA_INVALID && ex_addr != GA_INVALID) {
                            bool final_ok = false;

                            if(use_9tap) {
                                gaussian_9tap_pass(u, prev_addr, prev->aw * 4, prev->h,
                                                   t_addr, prev->aw, prev->ah, prev->w, prev->h,
                                                   true,
                                                   ew_o4, ew_o3, ew_i2, ew_i1, ew_center);

                                t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
                                ex_addr = Esd_GpuAlloc_Get(u->allocator, extra_handle);

                                if(t_addr != GA_INVALID && ex_addr != GA_INVALID) {
                                    gaussian_9tap_pass(u, t_addr, prev->aw * 4, prev->h,
                                                       ex_addr, prev->aw, prev->ah, prev->w, prev->h,
                                                       false,
                                                       ew_o4, ew_o3, ew_i2, ew_i1, ew_center);
                                    final_ok = true;
                                }
                            }
                            else if(!use_5tap) {
                                gaussian_7tap_pass(u, prev_addr, prev->aw * 4, prev->h,
                                                   t_addr, prev->aw, prev->ah, prev->w, prev->h,
                                                   true,
                                                   ew_outer3, ew_middle, ew_inner7, ew_center);

                                t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
                                ex_addr = Esd_GpuAlloc_Get(u->allocator, extra_handle);

                                if(t_addr != GA_INVALID && ex_addr != GA_INVALID) {
                                    gaussian_7tap_pass(u, t_addr, prev->aw * 4, prev->h,
                                                       ex_addr, prev->aw, prev->ah, prev->w, prev->h,
                                                       false,
                                                       ew_outer3, ew_middle, ew_inner7, ew_center);
                                    final_ok = true;
                                }
                            }
                            else {
                                ew_center = (uint8_t)(256 - 2 * ew_outer - 2 * ew_inner);
                                gaussian_5tap_pass(u, prev_addr, prev->aw * 4, prev->h,
                                                   t_addr, prev->aw, prev->ah, prev->w, prev->h,
                                                   true, false,
                                                   ew_outer, ew_inner, ew_center);

                                t_addr = Esd_GpuAlloc_Get(u->allocator, temp_handle);
                                ex_addr = Esd_GpuAlloc_Get(u->allocator, extra_handle);

                                if(t_addr != GA_INVALID && ex_addr != GA_INVALID) {
                                    gaussian_5tap_pass(u, t_addr, prev->aw * 4, prev->h,
                                                       ex_addr, prev->aw, prev->ah, prev->w, prev->h,
                                                       false, false,
                                                       ew_outer, ew_inner, ew_center);
                                    final_ok = true;
                                }
                            }

                            if(final_ok) {
                                levels[n_levels].handle = extra_handle;
                                levels[n_levels].w = prev->w;
                                levels[n_levels].h = prev->h;
                                levels[n_levels].aw = prev->aw;
                                levels[n_levels].ah = prev->ah;
                                levels[n_levels].sigma_sq = sigma_sq_target;
                                n_levels++;
                            }
                        }
                    }
                }
            }
        }
    }

    if(n_levels < 1) {
        Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
        Esd_GpuAlloc_PendingFree(u->allocator, temp_handle);
        return true;
    }

    /* Phase 4: Composite back to layer (bilinear upscale from deepest level) */
    int32_t final_idx = n_levels - 1;

    dst_addr = Esd_GpuAlloc_Get(u->allocator, dst_handle);
    if(dst_addr == GA_INVALID) goto cleanup;

    {
        gauss_level_t * final_lev = &levels[final_idx];
        uint32_t final_addr = Esd_GpuAlloc_Get(u->allocator, final_lev->handle);
        if(final_addr == GA_INVALID) goto cleanup;

        uint32_t scale_x = (uint32_t)final_lev->w * 256 / (uint32_t)pw;
        uint32_t scale_y = (uint32_t)final_lev->h * 256 / (uint32_t)ph;

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

        EVE_CoDl_scissorXY(phost, bx1, by1);
        EVE_CoDl_scissorSize(phost, bw, bh);

        EVE_CoDl_bitmapHandle(phost, phost->CoScratchHandle);

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

        /* Bilinear upscale from final level */
        EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
        EVE_CoDl_blendFunc(phost, ONE, ZERO);
        EVE_CoDl_bitmapSource(phost, final_addr);
        EVE_CoDl_bitmapLayout(phost, ARGB8, final_lev->aw * 4, final_lev->h);
        EVE_CoDl_bitmapSize(phost, BILINEAR, BORDER, BORDER, pw, ph);
        EVE_CoDl_bitmapTransform_identity(phost);
        if(scale_x != 0x0100 || scale_y != 0x0100) {
            EVE_CoDl_bitmapTransformA_ex(phost, 0, scale_x);
            EVE_CoDl_bitmapTransformE_ex(phost, 0, scale_y);
        }
        EVE_CoDl_begin(phost, BITMAPS);
        EVE_CoDl_vertex2f_0(phost, bx1 - pad, by1 - pad_y);
        EVE_CoDl_end(phost);

        if(corner_radius > 0) {
            EVE_CoDl_restoreContext(phost);
        }

        EVE_CoDl_display(phost);
        EVE_CoCmd_swap(phost);
        EVE_CoCmd_graphicsFinish(phost);
    }

cleanup:
    Esd_GpuAlloc_PendingFree(u->allocator, extract_handle);
    Esd_GpuAlloc_PendingFree(u->allocator, temp_handle);
    for(int32_t i = 1; i < n_levels; i++) {
        Esd_GpuAlloc_PendingFree(u->allocator, levels[i].handle);
    }

    return true;
}

#endif /* LV_USE_DRAW_EVE5 */
