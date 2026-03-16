/**
 * @file lv_draw_eve5_image.c
 *
 * EVE5 (BT820) Image Shared Helpers
 *
 * Contains shared utilities used by image rendering, alpha correction,
 * and text drawing:
 * - Alpha accuracy predicate (lv_draw_eve5_image_needs_alpha_rendertarget)
 * - Colorkey stencil mask builder (build_colorkey_stencil)
 * - Affine transform computation (compute_image_skew)
 * - Affine transform application (apply_image_skew)
 *
 * The main RGB draw function is in lv_draw_eve5_image_render.c.
 * Alpha correction pass is in lv_draw_eve5_image_alpha.c.
 * Image loading, format conversion, and upload are in lv_draw_eve5_image_load.c.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw.h"
#include "../lv_draw_image.h"
#include "../lv_image_decoder_private.h"
#if LV_USE_OS
#include "../../drivers/display/eve5/lv_eve5.h"
#endif
#if LV_USE_FS_EVE5_SDCARD
#include "../../drivers/display/eve5/lv_eve5_sdcard.h"
#endif

#if !LV_DRAW_EVE5_NO_FLOAT
#include <math.h>
#endif

/**********************
 * STATIC PROTOTYPES
 **********************/

/**********************
 * ALPHA ACCURACY PREDICATE
 *
 * See lv_draw_eve5_private.h for the full contract this function satisfies.
 **********************/

/**
 * IMAGE / LAYER: returns true when clip_radius triggers the alpha-as-scratch
 * clip masking path AND the source has per-pixel alpha (ARGB or mask bitmap).
 *
 * The RGB pass draws rounded-clip images via: (1a) clear bbox alpha=0,
 * (1b) fill rounded rect alpha=255, (1c) multiply by bitmap alpha via
 * blend(ZERO, SRC_ALPHA) when the source has alpha, (2) draw RGB through
 * blend(DST_ALPHA, ONE_MINUS_DST_ALPHA).  This trashes the alpha channel.
 * The direct alpha correction pass uses stencil to approximate recovery of
 * the rounded-clip shape; no exact reconstruction is currently possible.
 * When both an ARGB source and a bitmap mask are present, the stencil recovers
 * the mask shape but the image's own per-pixel alpha is lost entirely.
 *
 * This does NOT cover:
 *   - Colorkey stencil: uses stencil but doesn't trash alpha (6-pass INCR).
 *     The alpha pass handles colorkey in both the standard path (rebuilds
 *     stencil, gates bitmap draw) and the masking fallback paths (gates
 *     clip shape / mask bitmap draw via alpha_pass_build_colorkey_gate).
 *   - Recolor: trashes alpha but doesn't use stencil in the alpha pass.
 *
 * Mirrors: RGB pass in lv_draw_eve5_hal_draw_image (clip_radius masking path
 *          with has_alpha_trashed, specifically the !colorkey + (mask||ARGB)
 *          branch).
 *          Direct alpha pass in lv_draw_eve5_hal_alpha_draw_image (stencil
 *          approximation only — no exact recovery exists).
 */
bool lv_draw_eve5_image_needs_alpha_rendertarget(const lv_draw_task_t *t)
{
    const lv_draw_image_dsc_t *dsc = t->draw_dsc;
    if(dsc->opa <= LV_OPA_MIN) return false;

    /* Clip stencil path: clip_radius + no colorkey + (mask bitmap OR ARGB source).
     * RGB pass trashes alpha via alpha-as-scratch masking for clip_radius. */
    if(dsc->clip_radius > 0 && dsc->colorkey == NULL) {
        if(dsc->bitmap_mask_src != NULL) return true;
        /* LAYER tasks always use ARGB8 render targets */
        if(t->type == LV_DRAW_TASK_TYPE_LAYER) return true;
        /* Check if source image format maps to ARGB8 */
        if(lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE) {
            const lv_image_dsc_t *img_dsc = dsc->src;
            if(img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888 ||
               img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED)
                return true;
        }
    }

    /* Bitmap mask + ARGB source (any clip_radius, any colorkey):
     * The masking path trashes alpha. The alpha pass draws the mask shape
     * at flat opa, losing the source's per-pixel alpha. Only ARGB sources
     * are affected — opaque formats have no per-pixel alpha to lose. */
    if(dsc->bitmap_mask_src != NULL) {
        if(t->type == LV_DRAW_TASK_TYPE_LAYER) return true;
        if(lv_image_src_get_type(dsc->src) == LV_IMAGE_SRC_VARIABLE) {
            const lv_image_dsc_t *img_dsc = dsc->src;
            if(img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888 ||
               img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED)
                return true;
        }
    }

    return false;
}

/**********************
 * SHARED HELPERS
 **********************/

/**
 * Build a colorkey stencil mask using 6-pass BITMAP_SWIZZLE approach.
 * Routes each color channel to alpha via BITMAP_SWIZZLE, then uses
 * ALPHA_FUNC for range testing. After 6 passes (2 per channel),
 * stencil == 6 at pixels matching the colorkey range.
 *
 * BT820 note: BITMAP_SWIZZLE only takes effect in GLFORMAT mode.
 * This function switches the bitmap layout to GLFORMAT + BITMAP_EXT_FORMAT
 * for the stencil passes, then restores the original layout on return.
 *
 * Caller must:
 * - Set up bitmap handle/source/layout/size and any transforms beforehand
 * - Clear stencil (CLEAR(0,1,0)) before calling
 * - Be inside a saveContext block
 *
 * On return: colorMask=(0,0,0,0), stencil accumulation state set,
 * bitmap layout restored to original format.
 */
void build_colorkey_stencil(EVE_HalContext *phost,
                            const lv_image_colorkey_t *colorkey,
                            uint16_t eve_format, int32_t eve_stride, int32_t layout_h,
                            int32_t vx, int32_t vy)
{
    /* Switch to GLFORMAT mode so BITMAP_SWIZZLE takes effect on BT820 */
    EVE_CoDl_bitmapLayout(phost, GLFORMAT, eve_stride, layout_h);
    EVE_CoDl_bitmapExtFormat(phost, eve_format);

    /* Suppress pixel output, accumulate stencil on alpha-test pass */
    EVE_CoDl_colorMask(phost, 0, 0, 0, 0);
    EVE_CoDl_stencilFunc(phost, ALWAYS, 0, 255);
    EVE_CoDl_stencilOp(phost, KEEP, INCR);
    EVE_CoDl_colorA(phost, 255);
    EVE_CoDl_colorRgb(phost, 255, 255, 255);

    /* Red channel: stencil++ where low.red <= R <= high.red */
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, RED);
    EVE_CoDl_alphaFunc(phost, GEQUAL, colorkey->low.red);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
    EVE_CoDl_alphaFunc(phost, LEQUAL, colorkey->high.red);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);

    /* Green channel: stencil++ where low.green <= G <= high.green */
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, GREEN);
    EVE_CoDl_alphaFunc(phost, GEQUAL, colorkey->low.green);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
    EVE_CoDl_alphaFunc(phost, LEQUAL, colorkey->high.green);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);

    /* Blue channel: stencil++ where low.blue <= B <= high.blue */
    EVE_CoDl_bitmapSwizzle(phost, ZERO, ZERO, ZERO, BLUE);
    EVE_CoDl_alphaFunc(phost, GEQUAL, colorkey->low.blue);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);
    EVE_CoDl_alphaFunc(phost, LEQUAL, colorkey->high.blue);
    EVE_CoDl_begin(phost, BITMAPS);
    EVE_CoDl_vertex2f_0(phost, vx, vy);
    EVE_CoDl_end(phost);

    /* Restore original bitmap layout (exits GLFORMAT mode, disables swizzle) */
    EVE_CoDl_bitmapLayout(phost, (uint8_t)eve_format, eve_stride, layout_h);
}

/**
 * Compute inverse affine transform for skewed image rendering.
 * Produces the six bitmap transform coefficients (A-F) and the
 * transformed bounding box needed for BITMAP_SIZE.
 * Returns false if the transform matrix is degenerate.
 */
bool compute_image_skew(image_skew_t *out,
                        int32_t rotation, int32_t scale_x, int32_t scale_y,
                        int32_t skew_x, int32_t skew_y,
                        int32_t pivot_x, int32_t pivot_y,
                        int32_t src_w, int32_t src_h,
                        int32_t img_x, int32_t img_y,
                        int32_t draw_vx, int32_t draw_vy)
{
#if LV_DRAW_EVE5_NO_FLOAT
    /* Integer-only path using lv_trigo_sin/cos (1-degree resolution)
     * and 16.16 fixed-point matrix math. */

    /* Round deci-degrees to nearest integer degree */
    int16_t rot_deg = (int16_t)((rotation + 5) / 10);
    int16_t skx_deg = (int16_t)((skew_x + 5) / 10);
    int16_t sky_deg = (int16_t)((skew_y + 5) / 10);

    /* lv_trigo_sin/cos return Q15 (1.15 format, ±32768).
     * Shift left 1 to get 16.16 fixed-point (±65536 = ±1.0). */
    int32_t cos_r = lv_trigo_cos(rot_deg) << 1;  /* 16.16 */
    int32_t sin_r = lv_trigo_sin(rot_deg) << 1;

    /* tan(skew) = sin/cos in 16.16. Clamp when |cos| < 5% to avoid overflow. */
    int32_t tan_skx, tan_sky;
    {
        int32_t cos_skx = lv_trigo_cos(skx_deg) << 1;
        int32_t sin_skx = lv_trigo_sin(skx_deg) << 1;
        if(LV_ABS(cos_skx) < 3277)  /* ~5% of 65536 */
            tan_skx = (cos_skx >= 0) ? 0x7FFFFFFF : (int32_t)0x80000001;
        else
            tan_skx = (int32_t)(((int64_t)sin_skx << 16) / cos_skx);
    }
    {
        int32_t cos_sky = lv_trigo_cos(sky_deg) << 1;
        int32_t sin_sky = lv_trigo_sin(sky_deg) << 1;
        if(LV_ABS(cos_sky) < 3277)
            tan_sky = (cos_sky >= 0) ? 0x7FFFFFFF : (int32_t)0x80000001;
        else
            tan_sky = (int32_t)(((int64_t)sin_sky << 16) / cos_sky);
    }

    /* Scale: 8.8 to 16.16 */
    int32_t fsx = (int32_t)scale_x << 8;  /* 16.16 */
    int32_t fsy = (int32_t)scale_y << 8;

    /* 16.16 × 16.16 multiply helper macro */
    #define FP_MUL(a, b) ((int32_t)(((int64_t)(a) * (b)) >> 16))

    /* Forward matrix M = Scale * Skew * Rotation (all 16.16) */
    int32_t ma = FP_MUL(fsx, cos_r) + FP_MUL(FP_MUL(tan_skx, fsy), sin_r);
    int32_t mb = FP_MUL(-fsx, sin_r) + FP_MUL(FP_MUL(tan_skx, fsy), cos_r);
    int32_t md = FP_MUL(FP_MUL(tan_sky, fsx), cos_r) + FP_MUL(fsy, sin_r);
    int32_t me = FP_MUL(FP_MUL(-tan_sky, fsx), sin_r) + FP_MUL(fsy, cos_r);

    /* Determinant in 16.16 (result of 16.16 × 16.16) */
    int32_t det = FP_MUL(ma, me) - FP_MUL(mb, md);
    if(LV_ABS(det) < 66)  /* ~1e-6 in 16.16 */
        return false;

    /* Inverse matrix coefficients in 16.16: ia = me/det, etc. */
    out->ia = (int32_t)(((int64_t)me << 16) / det);
    out->ib = (int32_t)(-((int64_t)mb << 16) / det);
    out->i_d = (int32_t)(-((int64_t)md << 16) / det);
    out->ie = (int32_t)(((int64_t)ma << 16) / det);

    /* Translation: C = px - (ia * local_piv_x + ib * local_piv_y)
     * Pivot and offsets are integers, convert to 16.16 for multiply. */
    int32_t ox_fp = (img_x - draw_vx) << 16;
    int32_t local_piv_x_fp = ox_fp + (pivot_x << 16);
    int32_t local_piv_y_fp = ((img_y - draw_vy) << 16) + (pivot_y << 16);
    out->ic = (pivot_x << 16) - (FP_MUL(out->ia, local_piv_x_fp) + FP_MUL(out->ib, local_piv_y_fp));
    out->i_f = (pivot_y << 16) - (FP_MUL(out->i_d, local_piv_x_fp) + FP_MUL(out->ie, local_piv_y_fp));

    /* Format selection: if any |coeff| >= 2.0 (0x20000 in 16.16), use 8.8 */
    out->p = 1;  /* signed 1.15 */
    if(LV_ABS(out->ia) >= 0x20000 || LV_ABS(out->ib) >= 0x20000
       || LV_ABS(out->i_d) >= 0x20000 || LV_ABS(out->ie) >= 0x20000) {
        out->p = 0;  /* unsigned 8.8 */
    }

    /* Bounding box: transform corners with forward matrix (16.16),
     * then shift >> 16 for pixels. */
    int32_t cx[4], cy[4];
    cx[0] = -(pivot_x << 16);              cy[0] = -(pivot_y << 16);
    cx[1] = (src_w - pivot_x) << 16;       cy[1] = -(pivot_y << 16);
    cx[2] = -(pivot_x << 16);              cy[2] = (src_h - pivot_y) << 16;
    cx[3] = (src_w - pivot_x) << 16;       cy[3] = (src_h - pivot_y) << 16;
    int32_t bx_min = 0, bx_max = 0, by_min = 0, by_max = 0;
    for(int i = 0; i < 4; i++) {
        int32_t sx = FP_MUL(ma, cx[i]) + FP_MUL(mb, cy[i]);
        int32_t sy = FP_MUL(md, cx[i]) + FP_MUL(me, cy[i]);
        if(i == 0) { bx_min = bx_max = sx; by_min = by_max = sy; }
        else {
            if(sx < bx_min) bx_min = sx;
            if(sx > bx_max) bx_max = sx;
            if(sy < by_min) by_min = sy;
            if(sy > by_max) by_max = sy;
        }
    }
    out->bmp_w = ((bx_max - bx_min) >> 16) + 2;
    out->bmp_h = ((by_max - by_min) >> 16) + 2;
    if(out->bmp_w > 2048) out->bmp_w = 2048;
    if(out->bmp_h > 2048) out->bmp_h = 2048;

    #undef FP_MUL
#else
    float px = (float)pivot_x;
    float py = (float)pivot_y;
    float rot_rad = rotation * ((float)M_PI / 1800.0f);
    float cos_r = cosf(rot_rad);
    float sin_r = sinf(rot_rad);
    float fsx = scale_x / 256.0f;
    float fsy = scale_y / 256.0f;
    float tan_skx = tanf(skew_x * ((float)M_PI / 1800.0f));
    float tan_sky = tanf(skew_y * ((float)M_PI / 1800.0f));

    float ma = fsx * cos_r + tan_skx * fsy * sin_r;
    float mb = -fsx * sin_r + tan_skx * fsy * cos_r;
    float md = tan_sky * fsx * cos_r + fsy * sin_r;
    float me = -tan_sky * fsx * sin_r + fsy * cos_r;

    float det = ma * me - mb * md;
    if(fabsf(det) < 1e-6f)
        return false;
    float inv_det = 1.0f / det;
    out->ia = me * inv_det;
    out->ib = -mb * inv_det;
    out->i_d = -md * inv_det;
    out->ie = ma * inv_det;

    float ox = (float)(img_x - draw_vx);
    float local_piv_x = ox + px;
    float local_piv_y = (float)(img_y - draw_vy) + py;
    out->ic = px - (out->ia * local_piv_x + out->ib * local_piv_y);
    out->i_f = py - (out->i_d * local_piv_x + out->ie * local_piv_y);

    out->p = 1;
    out->unity = 32768.0f;
    if(fabsf(out->ia) >= 2.0f || fabsf(out->ib) >= 2.0f
       || fabsf(out->i_d) >= 2.0f || fabsf(out->ie) >= 2.0f) {
        out->p = 0;
        out->unity = 256.0f;
    }

    /* Compute transformed bounding box for BITMAP_SIZE */
    float corners_x[4], corners_y[4];
    corners_x[0] = -px;             corners_y[0] = -py;
    corners_x[1] = src_w - px;      corners_y[1] = -py;
    corners_x[2] = -px;             corners_y[2] = src_h - py;
    corners_x[3] = src_w - px;      corners_y[3] = src_h - py;
    float bx_min = 0, bx_max = 0, by_min = 0, by_max = 0;
    for(int i = 0; i < 4; i++) {
        float sx2 = ma * corners_x[i] + mb * corners_y[i];
        float sy2 = md * corners_x[i] + me * corners_y[i];
        if(i == 0) { bx_min = bx_max = sx2; by_min = by_max = sy2; }
        else {
            if(sx2 < bx_min) bx_min = sx2;
            if(sx2 > bx_max) bx_max = sx2;
            if(sy2 < by_min) by_min = sy2;
            if(sy2 > by_max) by_max = sy2;
        }
    }
    out->bmp_w = (int32_t)(bx_max - bx_min + 1.5f);
    out->bmp_h = (int32_t)(by_max - by_min + 1.5f);
    if(out->bmp_w > 2048) out->bmp_w = 2048;
    if(out->bmp_h > 2048) out->bmp_h = 2048;
#endif

    return true;
}

/**
 * Apply computed skew transform: override BITMAP_SIZE for the transformed
 * bounding box (or tile extent) and set BITMAP_TRANSFORM A-F coefficients.
 */
void apply_image_skew(EVE_HalContext *phost, const image_skew_t *skew,
                      uint8_t bmp_filter, int32_t tile_w, int32_t tile_h)
{
    if(tile_w > 0 && tile_h > 0) {
        EVE_CoDl_bitmapSize(phost, bmp_filter, REPEAT, REPEAT, tile_w, tile_h);
    }
    else {
        EVE_CoDl_bitmapSize(phost, bmp_filter, BORDER, BORDER, skew->bmp_w, skew->bmp_h);
    }
#if LV_DRAW_EVE5_NO_FLOAT
    /* Convert 16.16 fixed-point coefficients to EVE register format.
     * Signed 1.15 (p=1): coeff >> 1.  Unsigned 8.8 (p=0): coeff >> 8.
     * C/F translation: always >> 8 for EVE's 8.8 register format. */
    if(skew->p) {
        EVE_CoDl_bitmapTransformA_ex(phost, 1, skew->ia >> 1);
        EVE_CoDl_bitmapTransformB_ex(phost, 1, skew->ib >> 1);
        EVE_CoDl_bitmapTransformD_ex(phost, 1, skew->i_d >> 1);
        EVE_CoDl_bitmapTransformE_ex(phost, 1, skew->ie >> 1);
    }
    else {
        EVE_CoDl_bitmapTransformA_ex(phost, 0, skew->ia >> 8);
        EVE_CoDl_bitmapTransformB_ex(phost, 0, skew->ib >> 8);
        EVE_CoDl_bitmapTransformD_ex(phost, 0, skew->i_d >> 8);
        EVE_CoDl_bitmapTransformE_ex(phost, 0, skew->ie >> 8);
    }
    EVE_CoDl_bitmapTransformC(phost, skew->ic >> 8);
    EVE_CoDl_bitmapTransformF(phost, skew->i_f >> 8);
#else
    EVE_CoDl_bitmapTransformA_ex(phost, skew->p, (int32_t)(skew->ia * skew->unity));
    EVE_CoDl_bitmapTransformB_ex(phost, skew->p, (int32_t)(skew->ib * skew->unity));
    EVE_CoDl_bitmapTransformC(phost, (int32_t)(skew->ic * 256.0f));
    EVE_CoDl_bitmapTransformD_ex(phost, skew->p, (int32_t)(skew->i_d * skew->unity));
    EVE_CoDl_bitmapTransformE_ex(phost, skew->p, (int32_t)(skew->ie * skew->unity));
    EVE_CoDl_bitmapTransformF(phost, (int32_t)(skew->i_f * 256.0f));
#endif
}

/* Main draw function (lv_draw_eve5_hal_draw_image) is in lv_draw_eve5_image_render.c */
/* Alpha pass (lv_draw_eve5_hal_alpha_draw_image) is in lv_draw_eve5_image_alpha.c */

#endif /* LV_USE_DRAW_EVE5 */
