/**
 * @file lv_blend_to_rgb888_neon.h
 *
 */

#ifndef LV_BLEND_TO_RGB888_NEON_H
#define LV_BLEND_TO_RGB888_NEON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#if LV_DRAW_SW_ASM_NEON

/*********************
 *      DEFINES
 *********************/

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, px_size) \
    _lv_color_blend_to_rgb888_neon(dsc, px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, px_size) \
    _lv_color_blend_to_rgb888_with_opa_neon(dsc, px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, px_size) \
    _lv_color_blend_to_rgb888_with_mask_neon(dsc, px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, px_size) \
    _lv_color_blend_to_rgb888_mix_mask_opa_neon(dsc, px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_neon(dsc, px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_with_opa_neon(dsc, px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_with_mask_neon(dsc, px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_neon(dsc, px_size)

/*
 d26 ~ d29 is the source pixel (color or img)
 d18 ~ d21 is the dest pixel
 d17 is the inverse opa
 d16 = 255
 q0 ~ q2 is the premult src color
*/

#define SRC_PREMULT_8_PIXEL_NEON(opa)   \
    "vmull.u8       q0, d26, "#opa"                 \n" /* sb * opa */     \
    "vmull.u8       q1, d27, "#opa"                 \n" /* sg * opa */     \
    "vmull.u8       q2, d28, "#opa"                 \n" /* sr * opa */

#define SRC_PREMULT_8_PIXEL_INV_OPA_NEON(opa)   \
    "vsub.u8        d17, d16, "#opa"                \n" /* inv opa */ \
    "vmull.u8       q0, d26, "#opa"                 \n" /* sb * opa */     \
    "vmull.u8       q1, d27, "#opa"                 \n" /* sg * opa */     \
    "vmull.u8       q2, d28, "#opa"                 \n" /* sr * opa */

#define SRC_OVER_8_PIXEL_NEON   \
    "vmull.u8    q12, d20, d17                  \n" /* dr * inv_a */ \
    "vmull.u8    q11, d19, d17                  \n" /* dg * inv_a */ \
    "vmull.u8    q10, d18, d17                  \n" /* db * inv_a */ \
    "vqadd.u16   q10, q10, q0                   \n" /* (premult b) + (db * inv_a) */ \
    "vqadd.u16   q11, q11, q1                   \n" /* (premult g) + (dg * inv_a) */ \
    "vqadd.u16   q12, q12, q2                   \n" /* (premult r) + (dr * inv_a) */ \
    "vqrshrn.u16 d18, q10, #8                   \n" /* db >>= 8 */ \
    "vqrshrn.u16 d19, q11, #8                   \n" /* dg >>= 8 */ \
    "vqrshrn.u16 d20, q12, #8                   \n" /* dr >>= 8 */ \
    "vmov.u8     d21, #255                      \n"

#define SRC_OVER_8_PIXEL_COVER_NEON   \
    "vmlal.u8    q0, d18, d17                   \n" /* (premult b) + (db * inv_a) */ \
    "vmlal.u8    q1, d19, d17                   \n" /* (premult g) + (dg * inv_a) */ \
    "vmlal.u8    q2, d20, d17                   \n" /* (premult r) + (dr * inv_a) */ \
    "vqrshrn.u16 d18, q0, #8                    \n" /* db >>= 8 */ \
    "vqrshrn.u16 d19, q1, #8                    \n" /* dg >>= 8 */ \
    "vqrshrn.u16 d20, q2, #8                    \n" /* dr >>= 8 */ \

#define LOAD_8_XRGB8888_FROM_SRC   \
    "vld4.u8        {d26, d27, d28, d29}, [%[src]]!         \n"

#define LOAD_8_XRGB8888_FROM_DST   \
    "vld4.u8        {d18, d19, d20, d21}, [r4]!             \n"

#define STORE_8_XRGB8888_TO_DST    \
    "vst4.u8        {d18, d19, d20, d21}, [%[dst]]!         \n"

#define LOAD_8_RGB888_FROM_DST   \
    "vld3.u8        {d18, d19, d20}, [r4]!                  \n"

#define STORE_8_RGB888_TO_DST    \
    "vst3.u8        {d18, d19, d20}, [%[dst]]!              \n"

#define LOAD_4_XRGB8888_FROM_SRC   \
    "vld4.u8        {d26[0], d27[0], d28[0], d29[0]}, [%[src]]!     \n" \
    "vld4.u8        {d26[1], d27[1], d28[1], d29[1]}, [%[src]]!     \n" \
    "vld4.u8        {d26[2], d27[2], d28[2], d29[2]}, [%[src]]!     \n" \
    "vld4.u8        {d26[3], d27[3], d28[3], d29[3]}, [%[src]]!     \n"

#define LOAD_4_XRGB8888_FROM_DST   \
    "vld4.u8        {d18[0], d19[0], d20[0], d21[0]}, [r4]!         \n" \
    "vld4.u8        {d18[1], d19[1], d20[1], d21[1]}, [r4]!         \n" \
    "vld4.u8        {d18[2], d19[2], d20[2], d21[2]}, [r4]!         \n" \
    "vld4.u8        {d18[3], d19[3], d20[3], d21[3]}, [r4]!         \n"

#define STORE_4_XRGB8888_TO_DST    \
    "vst4.u8        {d18[0], d19[0], d20[0], d21[0]}, [%[dst]]!     \n" \
    "vst4.u8        {d18[1], d19[1], d20[1], d21[1]}, [%[dst]]!     \n" \
    "vst4.u8        {d18[2], d19[2], d20[2], d21[2]}, [%[dst]]!     \n" \
    "vst4.u8        {d18[3], d19[3], d20[3], d21[3]}, [%[dst]]!     \n"

#define LOAD_4_RGB888_FROM_DST   \
    "vld3.u8        {d18[0], d19[0], d20[0]}, [r4]!         \n" \
    "vld3.u8        {d18[1], d19[1], d20[1]}, [r4]!         \n" \
    "vld3.u8        {d18[2], d19[2], d20[2]}, [r4]!         \n" \
    "vld3.u8        {d18[3], d19[3], d20[3]}, [r4]!         \n"

#define STORE_4_RGB888_TO_DST    \
    "vst3.u8        {d18[0], d19[0], d20[0]}, [%[dst]]! \n" \
    "vst3.u8        {d18[1], d19[1], d20[1]}, [%[dst]]! \n" \
    "vst3.u8        {d18[2], d19[2], d20[2]}, [%[dst]]! \n" \
    "vst3.u8        {d18[3], d19[3], d20[3]}, [%[dst]]! \n"

#define LOAD_2_XRGB8888_FROM_SRC   \
    "vld4.u8        {d26[0], d27[0], d28[0], d29[0]}, [%[src]]!     \n" \
    "vld4.u8        {d26[1], d27[1], d28[1], d29[1]}, [%[src]]!     \n"

#define LOAD_2_XRGB8888_FROM_DST   \
    "vld4.u8        {d18[0], d19[0], d20[0], d21[0]}, [r4]!         \n" \
    "vld4.u8        {d18[1], d19[1], d20[1], d21[1]}, [r4]!         \n"

#define STORE_2_XRGB8888_TO_DST    \
    "vst4.u8        {d18[0], d19[0], d20[0], d21[0]}, [%[dst]]!     \n" \
    "vst4.u8        {d18[1], d19[1], d20[1], d21[1]}, [%[dst]]!     \n"

#define LOAD_2_RGB888_FROM_DST   \
    "vld3.u8        {d18[0], d19[0], d20[0]}, [r4]!         \n" \
    "vld3.u8        {d18[1], d19[1], d20[1]}, [r4]!         \n"

#define STORE_2_RGB888_TO_DST    \
    "vst3.u8        {d18[0], d19[0], d20[0]}, [%[dst]]!     \n" \
    "vst3.u8        {d18[1], d19[1], d20[1]}, [%[dst]]!     \n"

#define LOAD_1_XRGB8888_FROM_SRC   \
    "vld4.u8        {d26[0], d27[0], d28[0], d29[0]}, [%[src]]!     \n"

#define LOAD_1_XRGB8888_FROM_DST   \
    "vld4.u8        {d18[0], d19[0], d20[0], d21[0]}, [r4]!         \n"

#define STORE_1_XRGB8888_TO_DST    \
    "vst4.u8        {d18[0], d19[0], d20[0], d21[0]}, [%[dst]]!     \n"

#define LOAD_1_RGB888_FROM_DST   \
    "vld3.u8        {d18[0], d19[0], d20[0]}, [r4]!         \n"

#define STORE_1_RGB888_TO_DST    \
    "vst3.u8        {d18[0], d19[0], d20[0]}, [%[dst]]!     \n"

#define LOAD_8_MASK(res) \
    "vld1.u8        {"#res"}, [%[mask]]!                    \n"

#define LOAD_4_MASK(res) \
    "vld1.u8        {"#res"[0]}, [%[mask]]!                 \n" \
    "vld1.u8        {"#res"[1]}, [%[mask]]!                 \n" \
    "vld1.u8        {"#res"[2]}, [%[mask]]!                 \n" \
    "vld1.u8        {"#res"[3]}, [%[mask]]!                 \n"

#define LOAD_2_MASK(res) \
    "vld1.u8        {"#res"[0]}, [%[mask]]!                 \n" \
    "vld1.u8        {"#res"[1]}, [%[mask]]!                 \n"

#define LOAD_1_MASK(res) \
    "vld1.u8        {"#res"[0]}, [%[mask]]!                 \n"

#define MIX_OPA_2(opa1, opa2, res) \
    "vmull.u8       q0, "#opa1", "#opa2"                    \n" /* sa * opa */ \
    "vqrshrn.u16    "#res", q0, #8                          \n" /* (sa * opa) >>= 8 */

#define MIX_OPA_3(opa1, opa2, opa3, res) \
    "vmull.u8       q0, "#opa1", "#opa2"                    \n" /* sa * opa */ \
    "vqrshrn.u16    "#res", q0, #8                          \n" /* (sa * opa) >>= 8 */ \
    "vmull.u8       q0, "#opa3", "#res"                     \n" /* sa * opa */ \
    "vqrshrn.u16    "#res", q0, #8                          \n" /* (sa * opa) >>= 8 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline void lv_color_blend_to_rgb888_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint32_t dst_lineskip)
{

    asm volatile(
        "vld3.u8        {d0[], d1[], d2[]}, [%[pcolor]]     \n"

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"
        "vst3.u8        {d0, d1, d2}, [%[dst]]!             \n"
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"
        "vst3.u8        {d0[0], d1[0], d2[0]}, [%[dst]]!    \n"
        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "bgt            0b                                  \n"

        : [dst] "+r"(dst),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [w] "r"(w),
        [d_lineskip] "r"(dst_lineskip)
        : "cc", "memory", "q0", "q1", "ip");

}

static inline void lv_color_blend_to_xrgb8888_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint32_t dst_lineskip)
{
    asm volatile(
        "vld1.u32       {d0[]}, [%[pcolor]]                 \n"
        "vld1.u32       {d1[]}, [%[pcolor]]                 \n"
        "vld1.u32       {d2[]}, [%[pcolor]]                 \n"
        "vld1.u32       {d3[]}, [%[pcolor]]                 \n"

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"
        "vst4.u32       {d0-d3},   [%[dst]]!                \n"
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"
        "vst2.u32       {d0, d1}, [%[dst]]!                 \n"

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"
        "vst1.u32       {d0}, [%[dst]]!                     \n"

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"
        "vst1.u32       {d0[0]}, [%[dst]]!                  \n"

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "bgt            0b                                  \n"

        : [dst] "+r"(dst),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [w] "r"(w),
        [d_lineskip] "r"(dst_lineskip)
        : "cc", "memory", "q0", "q1", "ip");
}

static inline void lv_color_blend_to_rgb888_with_opa_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint8_t opa, uint32_t dst_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vld3.u8        {d26[], d27[], d28[]}, [%[pcolor]]  \n"
        "vdup.u8        d29, %[opa]                         \n"

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"
        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST

        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"

        "beq            1f                                  \n"

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [opa] "r"(opa),
        [w] "r"(w),
        [d_lineskip] "r"(dst_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_color_blend_to_xrgb8888_with_opa_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint8_t opa, uint32_t dst_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vld3.u8        {d26[], d27[], d28[]}, [%[pcolor]]  \n"
        "vdup.u8        d29, %[opa]                         \n"

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"
        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST

        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"

        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [opa] "r"(opa),
        [w] "r"(w),
        [d_lineskip] "r"(dst_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_color_blend_to_rgb888_with_mask_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint8_t * mask, uint32_t dst_lineskip, uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vld3.u8        {d26[], d27[], d28[]}, [%[pcolor]]  \n"

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST

        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [mask] "+r"(mask),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [w] "r"(w),
        [d_lineskip] "r"(dst_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_color_blend_to_xrgb8888_with_mask_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint8_t * mask, uint32_t dst_lineskip, uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vld3.u8        {d26[], d27[], d28[]}, [%[pcolor]]  \n"

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST

        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_MASK(d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [mask] "+r"(mask),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [w] "r"(w),
        [d_lineskip] "r"(dst_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_color_blend_to_rgb888_mix_mask_opa_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint8_t * mask, uint8_t opa, uint32_t dst_lineskip,
 uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vld3.u8        {d26[], d27[], d28[]}, [%[pcolor]]  \n"
        "vdup.u8        d30, %[opa]                         \n"

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [mask] "+r"(mask),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [w] "r"(w),
        [opa] "r"(opa),
        [d_lineskip] "r"(dst_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip"
    );
}

static inline void lv_color_blend_to_xrgb8888_mix_mask_opa_neon
(uint8_t * pcolor, uint8_t * dst, int32_t w, int32_t h, uint8_t * mask, uint8_t opa, uint32_t dst_lineskip,
 uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vld3.u8        {d26[], d27[], d28[]}, [%[pcolor]]  \n"
        "vdup.u8        d30, %[opa]                         \n"

        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"

        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_MASK(d29)
        MIX_OPA_2(d29, d30, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [mask] "+r"(mask),
        [h] "+r"(h)
        : [pcolor] "r"(pcolor),
        [w] "r"(w),
        [opa] "r"(opa),
        [d_lineskip] "r"(dst_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip"
    );
}

static inline void lv_argb8888_blend_normal_to_rgb888_neon
(uint8_t * src, uint8_t * dst, int32_t w, int32_t h, uint32_t dst_lineskip, uint32_t src_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"

        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [h] "+r"(h)
        :[w] "r"(w),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_xrgb8888_neon
(uint8_t * src, uint8_t * dst, int32_t w, int32_t h, uint32_t dst_lineskip, uint32_t src_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"

        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [h] "+r"(h)
        :[w] "r"(w),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_rgb888_with_opa_neon
(uint8_t * src, uint8_t * dst, int32_t w, int32_t h, uint8_t opa, uint32_t dst_lineskip, uint32_t src_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vdup.u8        d30, %[opa]                         \n"
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"

        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d30)

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [h] "+r"(h)
        :[w] "r"(w),
        [opa] "r"(opa),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_xrgb8888_with_opa_neon
(uint8_t * src, uint8_t * dst, int32_t w, int32_t h, uint8_t opa, uint32_t dst_lineskip, uint32_t src_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vdup.u8        d30, %[opa]                         \n"
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"

        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [h] "+r"(h)
        :[w] "r"(w),
        [opa] "r"(opa),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_rgb888_with_mask_neon(uint8_t * src, uint8_t * dst, int32_t w, int32_t h,
                                                                     uint8_t * mask, uint32_t dst_lineskip, uint32_t src_lineskip, uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        LOAD_8_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        LOAD_4_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        LOAD_2_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        LOAD_1_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [mask] "+r"(mask),
        [h] "+r"(h)
        :[w] "r"(w),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_xrgb8888_with_mask_neon(uint8_t * src, uint8_t * dst, int32_t w,
                                                                       int32_t h,
                                                                       uint8_t * mask, uint32_t dst_lineskip, uint32_t src_lineskip, uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"
        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        LOAD_8_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        LOAD_4_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        LOAD_2_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        LOAD_1_MASK(d30)

        MIX_OPA_2(d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [mask] "+r"(mask),
        [h] "+r"(h)
        :[w] "r"(w),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_neon(uint8_t * src, uint8_t * dst, int32_t w,
                                                                        int32_t h,
                                                                        uint8_t * mask, uint8_t opa, uint32_t dst_lineskip, uint32_t src_lineskip, uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vdup.u8        d30, %[opa]                         \n"
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"

        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        LOAD_8_MASK(d31)

        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_RGB888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        LOAD_4_MASK(d31)

        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_RGB888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        LOAD_2_MASK(d31)
        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_RGB888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        LOAD_1_MASK(d30)
        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_RGB888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_RGB888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [mask] "+r"(mask),
        [h] "+r"(h)
        :[w] "r"(w),
        [opa] "r"(opa),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void lv_argb8888_blend_normal_to_xrgb8888_mix_mask_opa_neon(uint8_t * src, uint8_t * dst, int32_t w,
                                                                          int32_t h,
                                                                          uint8_t * mask, uint8_t opa, uint32_t dst_lineskip, uint32_t src_lineskip, uint32_t mask_lineskip)
{
    asm volatile(
        "vmov.u8        d16, #255                           \n" /* a = 255  */
        "vdup.u8        d30, %[opa]                         \n"
        "0:                                                 \n"
        "movs	        ip, %[w], lsr #3                    \n"
        "mov            r4, %[dst]                          \n"

        "beq            4f                                  \n"
        "8:                                                 \n"
        "subs           ip, ip, #1                          \n"

        LOAD_8_XRGB8888_FROM_SRC

        LOAD_8_MASK(d31)
        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_8_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_8_XRGB8888_TO_DST
        "bgt            8b                                  \n"

        "4:                                                 \n"
        "ands           ip, %[w], #4                        \n"
        "beq            2f                                  \n"

        LOAD_4_XRGB8888_FROM_SRC

        LOAD_4_MASK(d31)
        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_4_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_4_XRGB8888_TO_DST

        "2:                                                 \n"
        "ands           ip, %[w], #2                        \n"
        "beq            1f                                  \n"

        LOAD_2_XRGB8888_FROM_SRC

        LOAD_2_MASK(d31)

        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_2_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_2_XRGB8888_TO_DST

        "1:                                                 \n"
        "ands           ip, %[w], #1                        \n"
        "beq            99f                                 \n"

        LOAD_1_XRGB8888_FROM_SRC

        LOAD_1_MASK(d31)
        MIX_OPA_3(d31, d30, d29, d29)

        SRC_PREMULT_8_PIXEL_INV_OPA_NEON(d29)

        LOAD_1_XRGB8888_FROM_DST

        SRC_OVER_8_PIXEL_NEON

        STORE_1_XRGB8888_TO_DST

        "99:                                                \n"
        "subs           %[h], %[h], #1                      \n"
        "add            %[dst], %[dst], %[d_lineskip]       \n"
        "add            %[src], %[src], %[s_lineskip]       \n"
        "add            %[mask], %[mask], %[m_lineskip]     \n"
        "bgt            0b                                  \n"
        : [dst] "+r"(dst),
        [src] "+r"(src),
        [mask] "+r"(mask),
        [h] "+r"(h)
        :[w] "r"(w),
        [opa] "r"(opa),
        [d_lineskip] "r"(dst_lineskip),
        [s_lineskip] "r"(src_lineskip),
        [m_lineskip] "r"(mask_lineskip)
        : "cc", "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "r4", "ip");
}

static inline void _lv_color_blend_to_rgb888_neon(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    uint32_t color32 = lv_color_to_u32(dsc->color);
    if(dest_px_size == 3) {
        lv_color_blend_to_rgb888_neon((uint8_t *)&color32, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                      (dsc->dest_stride - dsc->dest_w) * dest_px_size);
    }
    if(dest_px_size == 4) {
        lv_color_blend_to_xrgb8888_neon((uint8_t *)&color32, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                        (dsc->dest_stride - dsc->dest_w) * dest_px_size);
    }
}


static inline void _lv_color_blend_to_rgb888_with_opa_neon(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_color_blend_to_rgb888_with_opa_neon((uint8_t *)&dsc->color, dsc->dest_buf, dsc->dest_w, dsc->dest_h, dsc->opa,
                                               (dsc->dest_stride - dsc->dest_w) * dest_px_size);
    }
    if(dest_px_size == 4) {
        lv_color_blend_to_xrgb8888_with_opa_neon((uint8_t *)&dsc->color, dsc->dest_buf, dsc->dest_w, dsc->dest_h, dsc->opa,
                                                 (dsc->dest_stride - dsc->dest_w) * dest_px_size);
    }
}

static inline void _lv_color_blend_to_rgb888_with_mask_neon(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_color_blend_to_rgb888_with_mask_neon((uint8_t *)&dsc->color, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                (uint8_t *)dsc->mask_buf,
                                                (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                dsc->mask_stride - dsc->dest_w);
    }
    if(dest_px_size == 4) {
        lv_color_blend_to_xrgb8888_with_mask_neon((uint8_t *)&dsc->color, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                  (uint8_t *)dsc->mask_buf,
                                                  (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                  dsc->mask_stride - dsc->dest_w);
    }
}

static inline void _lv_color_blend_to_rgb888_mix_mask_opa_neon(_lv_draw_sw_blend_fill_dsc_t * dsc,
                                                               uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_color_blend_to_rgb888_mix_mask_opa_neon((uint8_t *)&dsc->color, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                   (uint8_t *)dsc->mask_buf, dsc->opa,
                                                   (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                   dsc->mask_stride - dsc->dest_w);
    }
    if(dest_px_size == 4) {
        lv_color_blend_to_xrgb8888_mix_mask_opa_neon((uint8_t *)&dsc->color, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                     (uint8_t *)dsc->mask_buf, dsc->opa,
                                                     (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                     dsc->mask_stride - dsc->dest_w);
    }
}

static inline void _lv_argb8888_blend_normal_to_rgb888_neon(_lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                (dsc->src_stride - dsc->dest_w) * 4);
    }
    if(dest_px_size == 4) {
        lv_argb8888_blend_normal_to_xrgb8888_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                  (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                  (dsc->src_stride - dsc->dest_w) * 4);
    }
}

static inline void _lv_argb8888_blend_normal_to_rgb888_with_opa_neon(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_with_opa_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                         dsc->opa,
                                                         (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                         (dsc->src_stride - dsc->dest_w) * 4);
    }
    if(dest_px_size == 4) {
        lv_argb8888_blend_normal_to_xrgb8888_with_opa_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                           dsc->opa,
                                                           (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                           (dsc->src_stride - dsc->dest_w) * 4);
    }
}

static inline void _lv_argb8888_blend_normal_to_rgb888_with_mask_neon(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_with_mask_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                          (uint8_t *)dsc->mask_buf,
                                                          (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                          (dsc->src_stride - dsc->dest_w) * 4,
                                                          dsc->mask_stride - dsc->dest_w);
    }
    if(dest_px_size == 4) {
        lv_argb8888_blend_normal_to_xrgb8888_with_mask_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                            (uint8_t *)dsc->mask_buf,
                                                            (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                            (dsc->src_stride - dsc->dest_w) * 4,
                                                            dsc->mask_stride - dsc->dest_w);
    }
}

static inline void _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_neon(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                         uint32_t dest_px_size)
{
    if(dest_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                             (uint8_t *)dsc->mask_buf, dsc->opa,
                                                             (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                             (dsc->src_stride - dsc->dest_w) * 4,
                                                             dsc->mask_stride - dsc->dest_w);
    }
    if(dest_px_size == 4) {
        lv_argb8888_blend_normal_to_xrgb8888_mix_mask_opa_neon((uint8_t *)dsc->src_buf, dsc->dest_buf, dsc->dest_w, dsc->dest_h,
                                                               (uint8_t *)dsc->mask_buf, dsc->opa,
                                                               (dsc->dest_stride - dsc->dest_w) * dest_px_size,
                                                               (dsc->src_stride - dsc->dest_w) * 4,
                                                               dsc->mask_stride - dsc->dest_w);
    }
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_DRAW_SW_ASM_NEON*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_TO_RGB888_NEON_H*/
