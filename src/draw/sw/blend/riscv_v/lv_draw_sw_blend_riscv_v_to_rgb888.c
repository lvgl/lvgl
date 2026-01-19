/**
 * @file lv_draw_sw_blend_riscv_v_to_rgb888.c
 * RGB888/XRGB8888 blend implementation for RISC-V Vector Extension (RVV 1.0)
 *
 * Supports both dest_px_size=3 (RGB888) and dest_px_size=4 (XRGB8888)
 * Reference: lv_draw_sw_blend_neon_to_rgb888.c
 *
 * NOTE: All RVV blend logic is inlined to avoid passing vuint32m4_t as function
 * parameters, which causes complex stack operations that can corrupt the stack.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_riscv_v_to_rgb888.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V

#include "../../../../misc/lv_color.h"
#include "../../../../misc/lv_types.h"
#include "../lv_draw_sw_blend_private.h"
#include "lv_blend_riscv_v_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Fill with solid color (no blending needed, opa >= 255)
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf        = dsc->dest_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* RGB888: 3 bytes per pixel (B, G, R) - use RVV segmented store */
        /* Initialize color vectors once with max vl */
        size_t vlmax = __riscv_vsetvlmax_e8m2();
        vuint8m2_t v_b = __riscv_vmv_v_x_u8m2(dsc->color.blue, vlmax);
        vuint8m2_t v_g = __riscv_vmv_v_x_u8m2(dsc->color.green, vlmax);
        vuint8m2_t v_r = __riscv_vmv_v_x_u8m2(dsc->color.red, vlmax);

        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV using segmented store for 3-byte pixels */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    else { /* dest_px_size == 4 */
        /* XRGB8888: 4 bytes per pixel */
        const uint32_t color32 = 0xFF000000 | ((uint32_t)dsc->color.red << 16) |
                                 ((uint32_t)dsc->color.green << 8) | dsc->color.blue;

        /* Initialize color vector once with max vl */
        size_t vlmax = __riscv_vsetvlmax_e32m4();
        vuint32m4_t v_color = __riscv_vmv_v_x_u32m4(color32, vlmax);

        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV - use m4 to reduce register pressure */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e32m4(w - x);
                __riscv_vse32_v_u32m4((uint32_t *)(dest_buf + x * 4), v_color, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * Fill with color and opacity (opa < 255)
 * blend formula: result = (fg * opa + bg * (255 - opa)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa         = dsc->opa;
    const uint8_t opa_inv     = 255 - opa;
    const uint16_t fg_b_opa   = (uint16_t)dsc->color.blue * opa;
    const uint16_t fg_g_opa   = (uint16_t)dsc->color.green * opa;
    const uint16_t fg_r_opa   = (uint16_t)dsc->color.red * opa;
    uint8_t * dest_buf        = dsc->dest_buf;
    size_t vl;

    /* Early exit if fully transparent */
    if(opa == 0) return LV_RESULT_OK;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV using segmented load/store for 3-byte pixels */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Load destination B, G, R channels using segmented load */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_VLSEG3E8_U8M2(dest_buf + x * 3, vl, v_dst_b, v_dst_g, v_dst_r);

                /* Blend solid color with destination */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                            fg_r_opa, fg_g_opa, fg_b_opa, opa_inv,
                                            v_r, v_g, v_b, vl);

                /* Store result using segmented store */
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Load destination B, G, R, X channels using segmented load */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r, v_dst_x;
                LV_RVV_VLSEG4E8_U8M2(dest_buf + x * 4, vl, v_dst_b, v_dst_g, v_dst_r, v_dst_x);
                /* v_dst_x is X/Alpha, ignored for input */
                (void)v_dst_x;

                /* Blend solid color with destination */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                            fg_r_opa, fg_g_opa, fg_b_opa, opa_inv,
                                            v_r, v_g, v_b, vl);

                vuint8m2_t v_x = __riscv_vmv_v_x_u8m2(0xFF, vl);  /* Alpha = 0xFF */

                /* Store result using segmented store */
                LV_RVV_VSSEG4E8_U8M2(dest_buf + x * 4, v_b, v_g, v_r, v_x, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * Fill with color and per-pixel mask (opa >= 255)
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf  = dsc->mask_buf;
    const uint8_t fg_b        = dsc->color.blue;
    const uint8_t fg_g        = dsc->color.green;
    const uint8_t fg_r        = dsc->color.red;
    uint8_t * dest_buf        = dsc->dest_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* RGB888: 3 bytes per pixel - use RVV for blending with mask */
        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV using segmented load/store for 3-byte pixels */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Load mask values */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Load destination B, G, R channels using segmented load */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_VLSEG3E8_U8M2(dest_buf + x * 3, vl, v_dst_b, v_dst_g, v_dst_r);

                /* Blend solid color with mask */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mask8,
                                                  v_r, v_g, v_b, vl);

                /* Optional: Handle special cases for mask == 0 or mask >= 255.
                 * Without this, max error is ±1 (e.g., (x*255)>>8 ≈ x*0.996).
                 * For graphics rendering, ±1 error is typically acceptable.
                 * Uncomment below if exact values are required. */
                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mask8, vl);
                /* Store result using segmented store */
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        /* XRGB8888: 4 bytes per pixel - use segmented load/store like RGB888 */
        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV using segmented load/store for 4-byte pixels */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Load mask values */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Load destination B, G, R, X channels using segmented load */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r, v_dst_x;
                LV_RVV_VLSEG4E8_U8M2(dest_buf + x * 4, vl, v_dst_b, v_dst_g, v_dst_r, v_dst_x);
                /* v_dst_x is X/Alpha, ignored for input */
                (void)v_dst_x;

                /* Blend solid color with mask */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mask8,
                                                  v_r, v_g, v_b, vl);
                vuint8m2_t v_x = __riscv_vmv_v_x_u8m2(0xFF, vl);  /* Alpha = 0xFF */

                /* Optional: Handle special cases for mask == 0 or mask >= 255.
                 * Without this, max error is ±1. Uncomment if exact values required. */
                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mask8, vl);

                /* Store result using segmented store */
                LV_RVV_VSSEG4E8_U8M2(dest_buf + x * 4, v_b, v_g, v_r, v_x, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * Fill with color, opacity, and per-pixel mask
 * Effective mix = (mask * opa) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                   uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w           = dsc->dest_w;
    const int32_t h           = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa         = dsc->opa;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf  = dsc->mask_buf;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t fg_b = dsc->color.blue;
    const uint8_t fg_g = dsc->color.green;
    const uint8_t fg_r = dsc->color.red;
    size_t vl;

    /* Early exit if fully transparent */
    if(opa == 0) return LV_RESULT_OK;

    if(dest_px_size == 3) {
        /* RGB888: 3 bytes per pixel - use RVV for blending with opa and mask */

        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV using segmented load/store for 3-byte pixels */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Load mask values */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Load destination B, G, R channels using segmented load */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_VLSEG3E8_U8M2(dest_buf + x * 3, vl, v_dst_b, v_dst_g, v_dst_r);

                /* Compute mix = (mask * opa) >> 8 using widening multiply then narrow */
                vuint16m4_t v_mix16 = __riscv_vwmulu_vx_u16m4(v_mask8, opa, vl);
                vuint8m2_t v_mix8 = __riscv_vnsrl_wx_u8m2(v_mix16, 8, vl);

                /* Blend solid color with mix (mask * opa) */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mix8,
                                                  v_r, v_g, v_b, vl);

                /* Optional: Handle special cases for mix == 0 or mix >= 255.
                 * Without this, max error is ±1. Uncomment if exact values required. */

                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mix8, vl);

                /* Store result using segmented store */
                LV_RVV_VSSEG3E8_U8M2(dest_buf + x * 3, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        /* XRGB8888: 4 bytes per pixel - use segmented load/store like RGB888 */


        for(int32_t y = 0; y < h; y++) {
            /* Process with RVV using segmented load/store for 4-byte pixels */
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m2(w - x);

                /* Load mask values */
                vuint8m2_t v_mask8 = __riscv_vle8_v_u8m2(&mask_buf[x], vl);

                /* Compute mix = (mask * opa) >> 8 using widening multiply */
                vuint16m4_t v_mix16 = __riscv_vsrl_vx_u16m4(
                                          __riscv_vwmulu_vx_u16m4(v_mask8, opa, vl), 8, vl);
                vuint8m2_t v_mix8 = __riscv_vnsrl_wx_u8m2(v_mix16, 0, vl);

                /* Load destination B, G, R, X channels using segmented load */
                vuint8m2_t v_dst_b, v_dst_g, v_dst_r, v_dst_x;
                LV_RVV_VLSEG4E8_U8M2(dest_buf + x * 4, vl, v_dst_b, v_dst_g, v_dst_r, v_dst_x);
                (void)v_dst_x;  /* v_dst_x is X/Alpha, ignored for input */

                /* Blend solid color with mix (mask * opa) */
                vuint8m2_t v_b, v_g, v_r;
                LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b,
                                                  fg_r, fg_g, fg_b, v_mix8,
                                                  v_r, v_g, v_b, vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b,
                                                       fg_r, fg_g, fg_b,
                                                       v_dst_r, v_dst_g, v_dst_b,
                                                       v_mix8, vl);

                vuint8m2_t v_x = __riscv_vmv_v_x_u8m2(0xFF, vl);  /* Alpha = 0xFF */

                /* Store result using segmented store */
                LV_RVV_VSSEG4E8_U8M2(dest_buf + x * 4, v_b, v_g, v_r, v_x, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *  RGB565 TO RGB888 BLEND FUNCTIONS
 **********************/

/**
 * RGB565 to RGB888/XRGB8888 simple copy (no blending, opa >= 255)
 * RGB565 format: RRRRRGGGGGGBBBBB (5-6-5 bits)
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e16m2(w - x);

                /* Load RGB565 pixels */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Extract R5, G6, B5 components */
                vuint16m2_t v_r5 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 11, vl), 0x1F, vl);
                vuint16m2_t v_g6 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 5, vl), 0x3F, vl);
                vuint16m2_t v_b5 = __riscv_vand_vx_u16m2(v_rgb565, 0x1F, vl);

                /* Convert to 8-bit: R8 = (R5 * 2106) >> 8, G8 = (G6 * 1037) >> 8, B8 = (B5 * 2106) >> 8 */
                vuint16m2_t v_r8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_r5, 2106, vl), 8, vl);
                vuint16m2_t v_g8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_g6, 1037, vl), 8, vl);
                vuint16m2_t v_b8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_b5, 2106, vl), 8, vl);

                /* Narrow to 8-bit */
                vuint8m1_t v_r = __riscv_vnsrl_wx_u8m1(v_r8_16, 0, vl);
                vuint8m1_t v_g = __riscv_vnsrl_wx_u8m1(v_g8_16, 0, vl);
                vuint8m1_t v_b = __riscv_vnsrl_wx_u8m1(v_b8_16, 0, vl);

                /* Store using stride store for RGB888 (3 bytes per pixel) */
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e16m2(w - x);

                /* Load RGB565 pixels */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Extract R5, G6, B5 components */
                vuint16m2_t v_r5 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 11, vl), 0x1F, vl);
                vuint16m2_t v_g6 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2(v_rgb565, 5, vl), 0x3F, vl);
                vuint16m2_t v_b5 = __riscv_vand_vx_u16m2(v_rgb565, 0x1F, vl);

                /* Convert to 8-bit */
                vuint16m2_t v_r8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_r5, 2106, vl), 8, vl);
                vuint16m2_t v_g8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_g6, 1037, vl), 8, vl);
                vuint16m2_t v_b8_16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_b5, 2106, vl), 8, vl);

                /* Narrow to 8-bit */
                vuint8m1_t v_r = __riscv_vnsrl_wx_u8m1(v_r8_16, 0, vl);
                vuint8m1_t v_g = __riscv_vnsrl_wx_u8m1(v_g8_16, 0, vl);
                vuint8m1_t v_b = __riscv_vnsrl_wx_u8m1(v_b8_16, 0, vl);
                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                /* Store using stride store for XRGB8888 (4 bytes per pixel) */
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB565 to RGB888/XRGB8888 with opacity
 * blend formula: result = (src * opa + dst * (255 - opa)) >> 8
 * Optimized using vwmaccu for blend calculation
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Load RGB565 source pixels */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Convert RGB565 to RGB888 using macro, then narrow to 8-bit */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Load destination RGB888 using stride load */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Blend using vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b,
                                      v_dst_r, v_dst_g, v_dst_b,
                                      opa, v_r, v_g, v_b, vl);

                /* Store result */
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Load RGB565 source pixels */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Convert RGB565 to RGB888 using macro, then narrow to 8-bit */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Load destination XRGB8888 using stride load */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Blend using vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b,
                                      v_dst_r, v_dst_g, v_dst_b,
                                      opa, v_r, v_g, v_b, vl);
                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                /* Store result */
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB565 to RGB888/XRGB8888 with per-pixel mask
 * blend formula: result = (src * mask + dst * (255 - mask)) >> 8
 * Optimized using vwmaccu for blend calculation
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Load mask */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);

                /* Load RGB565 source */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Convert RGB565 to RGB888 and narrow to 8-bit */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Load destination */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Blend with mask using vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mask, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Load mask */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);

                /* Load RGB565 source */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Convert RGB565 to RGB888 and narrow to 8-bit */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Load destination */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Blend with mask using vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mask, v_r, v_g, v_b, vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask, vl);

                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB565 to RGB888/XRGB8888 with opacity and per-pixel mask
 * effective mix = (mask * opa) >> 8
 * blend formula: result = (src * mix + dst * (255 - mix)) >> 8
 *
 * Note: with_opa_mask needs 16-bit intermediate for mix calculation,
 * so we cannot directly use the vwmaccu optimization for this case.
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint16_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Load mask and compute effective mix = (mask * opa) >> 8 */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);

                /* Load RGB565 source */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Convert RGB565 to RGB888 and narrow to 8-bit */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Load destination */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Blend with effective mix using vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mix, v_r, v_g, v_b, vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mix, vl);

                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else { /* dest_px_size == 4 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);

                /* Load mask and compute effective mix */
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);

                /* Load RGB565 source */
                vuint16m2_t v_rgb565 = __riscv_vle16_v_u16m2(&src_buf[x], vl);

                /* Convert RGB565 to RGB888 and narrow to 8-bit */
                vuint16m2_t v_src_r16, v_src_g16, v_src_b16;
                LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_src_r16, v_src_g16, v_src_b16, vl);
                vuint8m1_t v_src_r = __riscv_vnsrl_wx_u8m1(v_src_r16, 0, vl);
                vuint8m1_t v_src_g = __riscv_vnsrl_wx_u8m1(v_src_g16, 0, vl);
                vuint8m1_t v_src_b = __riscv_vnsrl_wx_u8m1(v_src_b16, 0, vl);

                /* Load destination */
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);

                /* Blend with effective mix using vwmaccu */
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_mix, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mix, vl);
                vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, vl);

                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }

            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *  RGB888/XRGB8888 TO RGB888/XRGB8888 BLEND FUNCTIONS
 **********************/

/**
 * RGB888/XRGB8888 to RGB888/XRGB8888 simple copy (no blending, opa >= 255)
 * src_px_size: 3 for RGB888, 4 for XRGB8888
 * dest_px_size: 3 for RGB888, 4 for XRGB8888
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                      uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    /* Fast path: same pixel size, use RVV memcpy */
    if(src_px_size == dest_px_size) {
        const int32_t row_bytes = w * dest_px_size;
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < row_bytes; x += vl) {
                vl = __riscv_vsetvl_e8m8(row_bytes - x);
                vuint8m8_t v_data = __riscv_vle8_v_u8m8(src_buf + x, vl);
                __riscv_vse8_v_u8m8(dest_buf + x, v_data, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
        return LV_RESULT_OK;
    }

    /* Different pixel sizes: need per-pixel conversion */
    if(dest_px_size == 3) {
        /* Source: XRGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r;
                LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_src_b, v_src_g, v_src_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* Destination: XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);

        /* Source: RGB888 -> XRGB8888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r;
                LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_src_b, v_src_g, v_src_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB888/XRGB8888 to RGB888/XRGB8888 with opacity
 * blend formula: result = (src * opa + dst * (255 - opa)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        if(src_px_size == 3) {
            /* RGB888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
        else {
            /* XRGB8888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
    }
    else {
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        if(src_px_size == 3) {
            /* RGB888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                uint8_t * dest_row = dest_buf;
                const uint8_t * src_row = src_buf;
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_row, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_row, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_row, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
        else {
            /* XRGB8888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, opa, v_r, v_g, v_b, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
            }
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB888/XRGB8888 to RGB888/XRGB8888 with per-pixel mask
 * blend formula: result = (src * mask + dst * (255 - mask)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        if(src_px_size == 3) {
            /* RGB888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask,
                                                v_r, v_g, v_b,
                                                vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);

                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask,
                                                v_r, v_g, v_b,
                                                vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else {
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        if(src_px_size == 3) {
            /* RGB888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask,
                                                v_r, v_g, v_b,
                                                vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mask, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mask, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

/**
 * RGB888/XRGB8888 to RGB888/XRGB8888 with opacity and per-pixel mask
 * effective mix = (mask * opa) >> 8
 * blend formula: result = (src * mix + dst * (255 - mix)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        if(src_px_size == 3) {
            /* RGB888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> RGB888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else {
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        if(src_px_size == 3) {
            /* RGB888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_RGB888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
        else {
            /* XRGB8888 -> XRGB8888 */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x += vl) {
                    vl = __riscv_vsetvl_e8m1(w - x);
                    vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                    vuint16m2_t v_mask16 = __riscv_vzext_vf2_u16m2(v_mask, vl);
                    vuint16m2_t v_mix16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(v_mask16, opa, vl), 8, vl);
                    vuint8m1_t v_mix = __riscv_vnsrl_wx_u8m1(v_mix16, 0, vl);
                    vuint8m1_t v_src_b, v_src_g, v_src_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, vl);
                    vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                    LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                    vuint8m1_t v_r, v_g, v_b;
                    LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_mix, v_r, v_g, v_b, vl);
                    LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                    v_src_r, v_src_g, v_src_b,
                                                    v_dst_r, v_dst_g, v_dst_b,
                                                    v_mix, vl);
                    LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                src_buf = drawbuf_next_row(src_buf, src_stride);
                mask_buf += mask_stride;
            }
        }
    }

    return LV_RESULT_OK;
}

/**********************
 *  ARGB8888 TO RGB888/XRGB8888 BLEND FUNCTIONS
 **********************/

/**
 * ARGB8888 to RGB888/XRGB8888 blend using source alpha
 * blend formula: result = (src * src_alpha + dst * (255 - src_alpha)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                        uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_src_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_src_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * ARGB8888 to RGB888/XRGB8888 with global opacity
 * effective_alpha = (src_alpha * opa) >> 8
 * blend formula: result = (src * effective_alpha + dst * (255 - effective_alpha)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_OPA_U8M1(v_src_a, opa, v_eff_a, vl);
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_OPA_U8M1(v_src_a, opa, v_eff_a, vl);
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

/**
 * ARGB8888 to RGB888/XRGB8888 with per-pixel mask
 * effective_alpha = (src_alpha * mask) >> 8
 * blend formula: result = (src * effective_alpha + dst * (255 - effective_alpha)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_U8M1(v_src_a, v_mask, v_eff_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_U8M1(v_src_a, v_mask, v_eff_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * ARGB8888 to RGB888/XRGB8888 with opacity and per-pixel mask
 * effective_alpha = (src_alpha * mask * opa) >> 16
 * blend formula: result = (src * effective_alpha + dst * (255 - effective_alpha)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    const uint8_t * mask_buf = dsc->mask_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_OPA_U8M1(v_src_a, v_mask, opa, v_eff_a, vl);
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b,
                                            v_dst_r, v_dst_g, v_dst_b,
                                            v_eff_a,
                                            v_r, v_g, v_b,
                                            vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }
    else {
        /* ARGB8888 -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_mask = __riscv_vle8_v_u8m1(&mask_buf[x], vl);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_eff_a;
                LV_RVV_CALC_EFF_ALPHA_MASK_OPA_U8M1(v_src_a, v_mask, opa, v_eff_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_r, v_g, v_b;
                LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, v_eff_a, v_r, v_g, v_b, vl);
                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_eff_a, vl);
                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
            mask_buf += mask_stride;
        }
    }

    return LV_RESULT_OK;
}

/**
 * ARGB8888 premultiplied to RGB888/XRGB8888
 * For premultiplied alpha, source RGB is already multiplied by alpha:
 *   src_premul = src * src_alpha / 255
 * blend formula: result = src_premul + dst * (255 - src_alpha) / 255
 *              = src_premul + (dst * (255 - src_alpha)) >> 8
 */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);

    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    uint8_t * dest_buf = dsc->dest_buf;
    const uint8_t * src_buf = dsc->src_buf;
    size_t vl;

    if(dest_px_size == 3) {
        /* ARGB8888 premultiplied -> RGB888 */
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_RGB888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_inv_a = __riscv_vrsub_vx_u8m1(v_src_a, 255, vl);
                vuint16m2_t v_dst_r16 = __riscv_vzext_vf2_u16m2(v_dst_r, vl);
                vuint16m2_t v_dst_g16 = __riscv_vzext_vf2_u16m2(v_dst_g, vl);
                vuint16m2_t v_dst_b16 = __riscv_vzext_vf2_u16m2(v_dst_b, vl);
                vuint16m2_t v_inv_a16 = __riscv_vzext_vf2_u16m2(v_inv_a, vl);
                vuint16m2_t v_r16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_r16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_g16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_g16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_b16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_b16, v_inv_a16, vl), 8, vl);
                vuint8m1_t v_r = __riscv_vadd_vv_u8m1(v_src_r, __riscv_vnsrl_wx_u8m1(v_r16, 0, vl), vl);
                vuint8m1_t v_g = __riscv_vadd_vv_u8m1(v_src_g, __riscv_vnsrl_wx_u8m1(v_g16, 0, vl), vl);
                vuint8m1_t v_b = __riscv_vadd_vv_u8m1(v_src_b, __riscv_vnsrl_wx_u8m1(v_b16, 0, vl), vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);

                LV_RVV_STORE_RGB888_U8M1(dest_buf, x, v_b, v_g, v_r, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
    else {
        /* ARGB8888 premultiplied -> XRGB8888 */
        size_t max_vl = __riscv_vsetvlmax_e8m1();
        vuint8m1_t v_a = __riscv_vmv_v_x_u8m1(0xFF, max_vl);
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x += vl) {
                vl = __riscv_vsetvl_e8m1(w - x);
                vuint8m1_t v_src_b, v_src_g, v_src_r, v_src_a;
                LV_RVV_LOAD_ARGB8888_U8M1(src_buf, x, v_src_b, v_src_g, v_src_r, v_src_a, vl);
                vuint8m1_t v_dst_b, v_dst_g, v_dst_r;
                LV_RVV_LOAD_XRGB8888_U8M1(dest_buf, x, v_dst_b, v_dst_g, v_dst_r, vl);
                vuint8m1_t v_inv_a = __riscv_vrsub_vx_u8m1(v_src_a, 255, vl);
                vuint16m2_t v_dst_r16 = __riscv_vzext_vf2_u16m2(v_dst_r, vl);
                vuint16m2_t v_dst_g16 = __riscv_vzext_vf2_u16m2(v_dst_g, vl);
                vuint16m2_t v_dst_b16 = __riscv_vzext_vf2_u16m2(v_dst_b, vl);
                vuint16m2_t v_inv_a16 = __riscv_vzext_vf2_u16m2(v_inv_a, vl);
                vuint16m2_t v_r16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_r16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_g16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_g16, v_inv_a16, vl), 8, vl);
                vuint16m2_t v_b16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(v_dst_b16, v_inv_a16, vl), 8, vl);
                vuint8m1_t v_r = __riscv_vadd_vv_u8m1(v_src_r, __riscv_vnsrl_wx_u8m1(v_r16, 0, vl), vl);
                vuint8m1_t v_g = __riscv_vadd_vv_u8m1(v_src_g, __riscv_vnsrl_wx_u8m1(v_g16, 0, vl), vl);
                vuint8m1_t v_b = __riscv_vadd_vv_u8m1(v_src_b, __riscv_vnsrl_wx_u8m1(v_b16, 0, vl), vl);

                LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b,
                                                v_src_r, v_src_g, v_src_b,
                                                v_dst_r, v_dst_g, v_dst_b,
                                                v_src_a, vl);

                LV_RVV_STORE_XRGB8888_U8M1(dest_buf, x, v_b, v_g, v_r, v_a, vl);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }

    return LV_RESULT_OK;
}

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V */