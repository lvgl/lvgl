/**
 * @file lv_blend_riscv_v_private.h
 * Common macros and utilities for RISC-V Vector Extension (RVV 1.0) blend operations
 *
 * This header provides reusable RVV macros for:
 * - Segmented load/store operations (RGB888/XRGB8888/RGB565)
 * - Alpha blending with scalar or vector alpha
 * - Color format conversions (RGB565 <-> RGB888)
 * - Effective alpha calculations (alpha, mask, opa combinations)
 */

#ifndef LV_BLEND_RISCV_V_PRIVATE_H
#define LV_BLEND_RISCV_V_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V

/* Try to use real RVV, fall back to emulation if not available */
#ifdef __riscv_v
#include <riscv_vector.h>
#else
/* No real RVV available, use emulation */
#include "lv_blend_riscv_vector_emulation.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *  RVV SEGMENTED LOAD/STORE MACROS
 *
 *  Emulate segmented load/store using stride operations.
 *  Compatible with compilers that don't support RVV 1.0 tuple types.
 **********************/

/* RGB888: 3 channels (B,G,R) with stride=3 */
#define LV_RVV_VLSEG3E8_U8M2(base, vl, v_b, v_g, v_r) \
    do { \
        (v_b) = __riscv_vlse8_v_u8m2((base) + 0, 3, (vl)); \
        (v_g) = __riscv_vlse8_v_u8m2((base) + 1, 3, (vl)); \
        (v_r) = __riscv_vlse8_v_u8m2((base) + 2, 3, (vl)); \
    } while(0)

#define LV_RVV_VSSEG3E8_U8M2(base, v_b, v_g, v_r, vl) \
    do { \
        __riscv_vsse8_v_u8m2((base) + 0, 3, (v_b), (vl)); \
        __riscv_vsse8_v_u8m2((base) + 1, 3, (v_g), (vl)); \
        __riscv_vsse8_v_u8m2((base) + 2, 3, (v_r), (vl)); \
    } while(0)

/* XRGB8888/ARGB8888: 4 channels (B,G,R,X) with stride=4 */
#define LV_RVV_VLSEG4E8_U8M2(base, vl, v_b, v_g, v_r, v_x) \
    do { \
        (v_b) = __riscv_vlse8_v_u8m2((base) + 0, 4, (vl)); \
        (v_g) = __riscv_vlse8_v_u8m2((base) + 1, 4, (vl)); \
        (v_r) = __riscv_vlse8_v_u8m2((base) + 2, 4, (vl)); \
        (v_x) = __riscv_vlse8_v_u8m2((base) + 3, 4, (vl)); \
    } while(0)

#define LV_RVV_VSSEG4E8_U8M2(base, v_b, v_g, v_r, v_x, vl) \
    do { \
        __riscv_vsse8_v_u8m2((base) + 0, 4, (v_b), (vl)); \
        __riscv_vsse8_v_u8m2((base) + 1, 4, (v_g), (vl)); \
        __riscv_vsse8_v_u8m2((base) + 2, 4, (v_r), (vl)); \
        __riscv_vsse8_v_u8m2((base) + 3, 4, (v_x), (vl)); \
    } while(0)

/**********************
 *  RGB565 <-> RGB888 CONVERSION MACROS
 *
 *  RGB565 format: RRRRRGGGGGGBBBBB (5-6-5 bits)
 *  Conversion formulas:
 *    R8 = (R5 * 2106) >> 8   (2106 ≈ 255/31 * 256)
 *    G8 = (G6 * 1037) >> 8   (1037 ≈ 255/63 * 256)
 *    B8 = (B5 * 2106) >> 8
 **********************/

/* Extract and convert RGB565 to separate R8, G8, B8 channels (16-bit width) */
#define LV_RVV_RGB565_TO_RGB888_U16M2(v_rgb565, v_r8, v_g8, v_b8, vl) \
    do { \
        vuint16m2_t _r5 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2((v_rgb565), 11, (vl)), 0x1F, (vl)); \
        vuint16m2_t _g6 = __riscv_vand_vx_u16m2(__riscv_vsrl_vx_u16m2((v_rgb565), 5, (vl)), 0x3F, (vl)); \
        vuint16m2_t _b5 = __riscv_vand_vx_u16m2((v_rgb565), 0x1F, (vl)); \
        (v_r8) = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(_r5, 2106, (vl)), 8, (vl)); \
        (v_g8) = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(_g6, 1037, (vl)), 8, (vl)); \
        (v_b8) = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(_b5, 2106, (vl)), 8, (vl)); \
    } while(0)

/* Convert R8, G8, B8 to RGB565 (16-bit) */
#define LV_RVV_RGB888_TO_RGB565_U16M2(v_r8, v_g8, v_b8, v_rgb565, vl) \
    do { \
        vuint16m2_t _r5 = __riscv_vsrl_vx_u16m2((v_r8), 3, (vl)); \
        vuint16m2_t _g6 = __riscv_vsrl_vx_u16m2((v_g8), 2, (vl)); \
        vuint16m2_t _b5 = __riscv_vsrl_vx_u16m2((v_b8), 3, (vl)); \
        (v_rgb565) = __riscv_vor_vv_u16m2(__riscv_vsll_vx_u16m2(_r5, 11, (vl)), \
                                          __riscv_vor_vv_u16m2(__riscv_vsll_vx_u16m2(_g6, 5, (vl)), _b5, (vl)), (vl)); \
    } while(0)

/**********************
 *  ALPHA BLENDING MACROS
 *
 *  Standard blend formula: result = (src * alpha + dst * (255 - alpha)) >> 8
 *
 *  Using vwmaccu (widening multiply-accumulate unsigned):
 *    tmp = dst * (255 - alpha)           // Initialize with dst contribution
 *    tmp = tmp + src * alpha             // vwmaccu adds src contribution
 *    result = tmp >> 8
 *
 *  This reduces operations by combining multiply and add.
 **********************/

/**
 * Blend single channel using vwmaccu (8-bit src/dst -> 16-bit intermediate)
 * LMUL relationship: m1 -> m2, m2 -> m4
 */
#define LV_RVV_BLEND_CHANNEL_U8M1_TO_U16M2(v_src, v_dst, alpha, v_result, vl) \
    do { \
        uint8_t _alpha_inv = 255 - (alpha); \
        vuint16m2_t _tmp = __riscv_vwmulu_vx_u16m2((v_dst), _alpha_inv, (vl)); \
        _tmp = __riscv_vwmaccu_vx_u16m2(_tmp, (alpha), (v_src), (vl)); \
        (v_result) = __riscv_vnsrl_wx_u8m1(_tmp, 8, (vl)); \
    } while(0)

#define LV_RVV_BLEND_CHANNEL_U8M2_TO_U16M4(v_src, v_dst, alpha, v_result, vl) \
    do { \
        uint8_t _alpha_inv = 255 - (alpha); \
        vuint16m4_t _tmp = __riscv_vwmulu_vx_u16m4((v_dst), _alpha_inv, (vl)); \
        _tmp = __riscv_vwmaccu_vx_u16m4(_tmp, (alpha), (v_src), (vl)); \
        (v_result) = __riscv_vnsrl_wx_u8m2(_tmp, 8, (vl)); \
    } while(0)

/**
 * Blend single channel with vector alpha (per-pixel mask)
 */
#define LV_RVV_BLEND_CHANNEL_VMASK_U8M1_TO_U16M2(v_src, v_dst, v_alpha, v_result, vl) \
    do { \
        vuint8m1_t _v_alpha_inv = __riscv_vrsub_vx_u8m1((v_alpha), 255, (vl)); \
        vuint16m2_t _tmp = __riscv_vwmulu_vv_u16m2((v_dst), _v_alpha_inv, (vl)); \
        _tmp = __riscv_vwmaccu_vv_u16m2(_tmp, (v_alpha), (v_src), (vl)); \
        (v_result) = __riscv_vnsrl_wx_u8m1(_tmp, 8, (vl)); \
    } while(0)

#define LV_RVV_BLEND_CHANNEL_VMASK_U8M2_TO_U16M4(v_src, v_dst, v_alpha, v_result, vl) \
    do { \
        vuint8m2_t _v_alpha_inv = __riscv_vrsub_vx_u8m2((v_alpha), 255, (vl)); \
        vuint16m4_t _tmp = __riscv_vwmulu_vv_u16m4((v_dst), _v_alpha_inv, (vl)); \
        _tmp = __riscv_vwmaccu_vv_u16m4(_tmp, (v_alpha), (v_src), (vl)); \
        (v_result) = __riscv_vnsrl_wx_u8m2(_tmp, 8, (vl)); \
    } while(0)

/**
 * Blend RGB channels with scalar alpha (all 3 channels at once)
 * Uses m1->m2 widening
 */
#define LV_RVV_BLEND_RGB_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, \
                              alpha, v_out_r, v_out_g, v_out_b, vl) \
do { \
    LV_RVV_BLEND_CHANNEL_U8M1_TO_U16M2((v_src_r), (v_dst_r), (alpha), (v_out_r), (vl)); \
    LV_RVV_BLEND_CHANNEL_U8M1_TO_U16M2((v_src_g), (v_dst_g), (alpha), (v_out_g), (vl)); \
    LV_RVV_BLEND_CHANNEL_U8M1_TO_U16M2((v_src_b), (v_dst_b), (alpha), (v_out_b), (vl)); \
} while(0)

/**
 * Blend RGB channels with scalar alpha (all 3 channels at once)
 * Uses m2->m4 widening
 */
#define LV_RVV_BLEND_RGB_U8M2(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, \
                              alpha, v_out_r, v_out_g, v_out_b, vl) \
do { \
    LV_RVV_BLEND_CHANNEL_U8M2_TO_U16M4((v_src_r), (v_dst_r), (alpha), (v_out_r), (vl)); \
    LV_RVV_BLEND_CHANNEL_U8M2_TO_U16M4((v_src_g), (v_dst_g), (alpha), (v_out_g), (vl)); \
    LV_RVV_BLEND_CHANNEL_U8M2_TO_U16M4((v_src_b), (v_dst_b), (alpha), (v_out_b), (vl)); \
} while(0)

/**
 * Blend solid color (pre-multiplied) with destination RGB channels
 * fg_color_opa: pre-computed (color * opa) for each channel
 * opa_inv: 255 - opa
 * Formula: result = (dst * opa_inv + fg_color_opa) >> 8
 * Uses m2->m4 widening
 */
#define LV_RVV_BLEND_SOLID_RGB_U8M2(v_dst_r, v_dst_g, v_dst_b, \
                                    fg_r_opa, fg_g_opa, fg_b_opa, opa_inv, \
                                    v_out_r, v_out_g, v_out_b, vl) \
do { \
    vuint16m4_t _v_r16 = __riscv_vwmulu_vx_u16m4((v_dst_r), (opa_inv), (vl)); \
    vuint16m4_t _v_g16 = __riscv_vwmulu_vx_u16m4((v_dst_g), (opa_inv), (vl)); \
    vuint16m4_t _v_b16 = __riscv_vwmulu_vx_u16m4((v_dst_b), (opa_inv), (vl)); \
    _v_r16 = __riscv_vadd_vx_u16m4(_v_r16, (fg_r_opa), (vl)); \
    _v_g16 = __riscv_vadd_vx_u16m4(_v_g16, (fg_g_opa), (vl)); \
    _v_b16 = __riscv_vadd_vx_u16m4(_v_b16, (fg_b_opa), (vl)); \
    (v_out_r) = __riscv_vnsrl_wx_u8m2(_v_r16, 8, (vl)); \
    (v_out_g) = __riscv_vnsrl_wx_u8m2(_v_g16, 8, (vl)); \
    (v_out_b) = __riscv_vnsrl_wx_u8m2(_v_b16, 8, (vl)); \
} while(0)

/**
 * Blend solid color (scalar) with destination RGB channels using vector alpha mask
 * fg_r/g/b: foreground color scalar values
 * v_alpha: per-pixel alpha values (vuint8m2_t)
 * Formula: result = (fg * alpha + dst * (255 - alpha)) >> 8
 * Uses m2->m4 widening with vwmaccu for efficiency
 */
#define LV_RVV_BLEND_SOLID_RGB_VMASK_U8M2(v_dst_r, v_dst_g, v_dst_b, \
                                          fg_r, fg_g, fg_b, v_alpha, \
                                          v_out_r, v_out_g, v_out_b, vl) \
do { \
    vuint8m2_t _v_alpha_inv = __riscv_vrsub_vx_u8m2((v_alpha), 255, (vl)); \
    vuint16m4_t _v_r16 = __riscv_vwmulu_vv_u16m4((v_dst_r), _v_alpha_inv, (vl)); \
    vuint16m4_t _v_g16 = __riscv_vwmulu_vv_u16m4((v_dst_g), _v_alpha_inv, (vl)); \
    vuint16m4_t _v_b16 = __riscv_vwmulu_vv_u16m4((v_dst_b), _v_alpha_inv, (vl)); \
    _v_r16 = __riscv_vwmaccu_vx_u16m4(_v_r16, (fg_r), (v_alpha), (vl)); \
    _v_g16 = __riscv_vwmaccu_vx_u16m4(_v_g16, (fg_g), (v_alpha), (vl)); \
    _v_b16 = __riscv_vwmaccu_vx_u16m4(_v_b16, (fg_b), (v_alpha), (vl)); \
    (v_out_r) = __riscv_vnsrl_wx_u8m2(_v_r16, 8, (vl)); \
    (v_out_g) = __riscv_vnsrl_wx_u8m2(_v_g16, 8, (vl)); \
    (v_out_b) = __riscv_vnsrl_wx_u8m2(_v_b16, 8, (vl)); \
} while(0)

/**
 * Blend RGB channels with vector alpha (per-pixel mask)
 */
#define LV_RVV_BLEND_RGB_VMASK_U8M1(v_src_r, v_src_g, v_src_b, v_dst_r, v_dst_g, v_dst_b, \
                                    v_alpha, v_out_r, v_out_g, v_out_b, vl) \
do { \
    LV_RVV_BLEND_CHANNEL_VMASK_U8M1_TO_U16M2((v_src_r), (v_dst_r), (v_alpha), (v_out_r), (vl)); \
    LV_RVV_BLEND_CHANNEL_VMASK_U8M1_TO_U16M2((v_src_g), (v_dst_g), (v_alpha), (v_out_g), (vl)); \
    LV_RVV_BLEND_CHANNEL_VMASK_U8M1_TO_U16M2((v_src_b), (v_dst_b), (v_alpha), (v_out_b), (vl)); \
} while(0)

/**
 * Optimize blend results for zero and full mask cases (u8m1)
 * When mask is 0, use destination; when mask is >= 255, use source
 */
#define LV_RVV_BLEND_OPTIMIZE_MASK_U8M1(v_r, v_g, v_b, v_src_r, v_src_g, v_src_b, \
                                        v_dst_r, v_dst_g, v_dst_b, v_mask, vl) \
do { \
    vbool8_t _zero_mask = __riscv_vmseq_vx_u8m1_b8((v_mask), 0, (vl)); \
    vbool8_t _full_mask = __riscv_vmsgeu_vx_u8m1_b8((v_mask), LV_OPA_MAX, (vl)); \
    (v_b) = __riscv_vmerge_vvm_u8m1((v_b), (v_dst_b), _zero_mask, (vl)); \
    (v_g) = __riscv_vmerge_vvm_u8m1((v_g), (v_dst_g), _zero_mask, (vl)); \
    (v_r) = __riscv_vmerge_vvm_u8m1((v_r), (v_dst_r), _zero_mask, (vl)); \
    (v_b) = __riscv_vmerge_vvm_u8m1((v_b), (v_src_b), _full_mask, (vl)); \
    (v_g) = __riscv_vmerge_vvm_u8m1((v_g), (v_src_g), _full_mask, (vl)); \
    (v_r) = __riscv_vmerge_vvm_u8m1((v_r), (v_src_r), _full_mask, (vl)); \
} while(0)

/**
 * Optimize blend results for zero and full mask cases (u8m2) with scalar source
 * When mask is 0, use destination; when mask is >= 255, use scalar source
 */
#define LV_RVV_BLEND_OPTIMIZE_MASK_SCALAR_U8M2(v_r, v_g, v_b, src_r, src_g, src_b, \
                                               v_dst_r, v_dst_g, v_dst_b, v_mask, vl) \
do { \
    vbool4_t _zero_mask = __riscv_vmseq_vx_u8m2_b4((v_mask), 0, (vl)); \
    vbool4_t _full_mask = __riscv_vmsgeu_vx_u8m2_b4((v_mask), LV_OPA_MAX, (vl)); \
    (v_b) = __riscv_vmerge_vvm_u8m2((v_b), (v_dst_b), _zero_mask, (vl)); \
    (v_g) = __riscv_vmerge_vvm_u8m2((v_g), (v_dst_g), _zero_mask, (vl)); \
    (v_r) = __riscv_vmerge_vvm_u8m2((v_r), (v_dst_r), _zero_mask, (vl)); \
    (v_b) = __riscv_vmerge_vxm_u8m2((v_b), (src_b), _full_mask, (vl)); \
    (v_g) = __riscv_vmerge_vxm_u8m2((v_g), (src_g), _full_mask, (vl)); \
    (v_r) = __riscv_vmerge_vxm_u8m2((v_r), (src_r), _full_mask, (vl)); \
} while(0)

/**********************
 *  EFFECTIVE ALPHA CALCULATION MACROS
 *
 *  These macros compute the effective alpha from combinations of:
 *    - v_alpha: source alpha channel (per-pixel)
 *    - mask: mask value (per-pixel)
 *    - opa: global opacity (scalar)
 *
 *  Formula: eff_alpha = (alpha * mask * opa) >> 16
 *  Intermediate calculations use 16-bit to prevent overflow.
 **********************/

/**
 * Calculate effective alpha from source alpha and global opa
 */
#define LV_RVV_CALC_EFF_ALPHA_OPA_U8M1(v_src_a, opa, v_eff_a, vl) \
    do { \
        vuint16m2_t _v_alpha16 = __riscv_vzext_vf2_u16m2((v_src_a), (vl)); \
        vuint16m2_t _v_eff16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vx_u16m2(_v_alpha16, (opa), (vl)), 8, (vl)); \
        (v_eff_a) = __riscv_vnsrl_wx_u8m1(_v_eff16, 0, (vl)); \
    } while(0)

/**
 * Calculate effective alpha from source alpha and mask
 */
#define LV_RVV_CALC_EFF_ALPHA_MASK_U8M1(v_src_a, v_mask, v_eff_a, vl) \
    do { \
        vuint16m2_t _v_alpha16 = __riscv_vzext_vf2_u16m2((v_src_a), (vl)); \
        vuint16m2_t _v_mask16 = __riscv_vzext_vf2_u16m2((v_mask), (vl)); \
        vuint16m2_t _v_eff16 = __riscv_vsrl_vx_u16m2(__riscv_vmul_vv_u16m2(_v_alpha16, _v_mask16, (vl)), 8, (vl)); \
        (v_eff_a) = __riscv_vnsrl_wx_u8m1(_v_eff16, 0, (vl)); \
    } while(0)

/**
 * Calculate effective alpha from source alpha, mask, and global opa
 * Formula: eff_alpha = (alpha * mask * opa) >> 16
 * Widen to u32m4 to avoid precision loss from double shift
 */
#define LV_RVV_CALC_EFF_ALPHA_MASK_OPA_U8M1(v_src_a, v_mask, opa, v_eff_a, vl) \
    do { \
        vuint16m2_t _v_alpha16 = __riscv_vzext_vf2_u16m2((v_src_a), (vl)); \
        vuint16m2_t _v_mask16 = __riscv_vzext_vf2_u16m2((v_mask), (vl)); \
        vuint16m2_t _v_prod16 = __riscv_vmul_vv_u16m2(_v_alpha16, _v_mask16, (vl)); \
        vuint32m4_t _v_prod32 = __riscv_vwmulu_vx_u32m4(_v_prod16, (opa), (vl)); \
        vuint16m2_t _v_eff16 = __riscv_vnsrl_wx_u16m2(_v_prod32, 16, (vl)); \
        (v_eff_a) = __riscv_vnsrl_wx_u8m1(_v_eff16, 0, (vl)); \
    } while(0)

/**********************
 *  RGB/ARGB CHANNEL LOAD/STORE MACROS (for ARGB8888/RGB888/XRGB8888)
 *
 *  Load/Store RGB channels to/from memory in different formats using m1 LMUL.
 **********************/

#define LV_RVV_LOAD_ARGB8888_U8M1(ptr, x, v_b, v_g, v_r, v_a, vl) \
    do { \
        (v_b) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 0, 4, (vl)); \
        (v_g) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 1, 4, (vl)); \
        (v_r) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 2, 4, (vl)); \
        (v_a) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 3, 4, (vl)); \
    } while(0)

#define LV_RVV_LOAD_RGB888_U8M1(ptr, x, v_b, v_g, v_r, vl) \
    do { \
        (v_b) = __riscv_vlse8_v_u8m1((ptr) + (x) * 3 + 0, 3, (vl)); \
        (v_g) = __riscv_vlse8_v_u8m1((ptr) + (x) * 3 + 1, 3, (vl)); \
        (v_r) = __riscv_vlse8_v_u8m1((ptr) + (x) * 3 + 2, 3, (vl)); \
    } while(0)

#define LV_RVV_LOAD_XRGB8888_U8M1(ptr, x, v_b, v_g, v_r, vl) \
    do { \
        (v_b) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 0, 4, (vl)); \
        (v_g) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 1, 4, (vl)); \
        (v_r) = __riscv_vlse8_v_u8m1((ptr) + (x) * 4 + 2, 4, (vl)); \
    } while(0)

#define LV_RVV_STORE_RGB888_U8M1(ptr, x, v_b, v_g, v_r, vl) \
    do { \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 3 + 0, 3, (v_b), (vl)); \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 3 + 1, 3, (v_g), (vl)); \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 3 + 2, 3, (v_r), (vl)); \
    } while(0)

#define LV_RVV_STORE_XRGB8888_U8M1(ptr, x, v_b, v_g, v_r, v_a, vl) \
    do { \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 4 + 0, 4, (v_b), (vl)); \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 4 + 1, 4, (v_g), (vl)); \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 4 + 2, 4, (v_r), (vl)); \
        __riscv_vsse8_v_u8m1((ptr) + (x) * 4 + 3, 4, (v_a), (vl)); \
    } while(0)

/**********************
 *      MACROS
 **********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_RISCV_V_PRIVATE_H*/
