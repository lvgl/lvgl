/**
 * @file lv_draw_sw_blend_to_i1.c
 *
 */

#include "lv_draw_sw_blend_to_i1.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_I1

#include "lv_draw_sw_blend_private.h"

static inline void /* LV_ATTRIBUTE_FAST_MEM */ lv_color_8_8_mix(const uint8_t src, uint8_t * dest, uint8_t mix);

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    static inline lv_color16_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color16_from_u16(uint16_t raw);
#endif

#define I1_LUM_THRESHOLD LV_DRAW_SW_I1_LUM_THRESHOLD

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1(...)                    LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1_WITH_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1_WITH_OPA(...)           LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1_WITH_MASK
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1_WITH_MASK(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_I1
    #define LV_DRAW_SW_COLOR_BLEND_TO_I1(...)                         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_I1_WITH_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_I1_WITH_OPA(...)                LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_I1_WITH_MASK
    #define LV_DRAW_SW_COLOR_BLEND_TO_I1_WITH_MASK(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_I1_MIX_MASK_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_I1_MIX_MASK_OPA(...)            LV_RESULT_INVALID
#endif

#if LV_DRAW_SW_SUPPORT_RGB565
    #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1
        #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1(...)                 LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1_WITH_OPA
        #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1_WITH_OPA(...)        LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1_WITH_MASK
        #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1_WITH_MASK(...)       LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
        #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)    LV_RESULT_INVALID
    #endif
#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1
        #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1(...)                 LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1_WITH_OPA
        #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1_WITH_OPA(...)        LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1_WITH_MASK
        #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1_WITH_MASK(...)       LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
        #define LV_DRAW_SW_RGB565_SWAPPED_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)    LV_RESULT_INVALID
    #endif
#endif

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888
    #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1
        #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1(...)                 LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1_WITH_OPA
        #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1_WITH_OPA(...)        LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1_WITH_MASK
        #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1_WITH_MASK(...)       LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
        #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)    LV_RESULT_INVALID
    #endif
#endif

#if LV_DRAW_SW_SUPPORT_L8
    #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1
        #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1(...)               LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1_WITH_OPA
        #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1_WITH_OPA(...)      LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1_WITH_MASK
        #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1_WITH_MASK(...)     LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
        #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)  LV_RESULT_INVALID
    #endif
#endif

#if LV_DRAW_SW_SUPPORT_AL88
    #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1
        #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1(...)               LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1_WITH_OPA
        #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1_WITH_OPA(...)      LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1_WITH_MASK
        #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1_WITH_MASK(...)     LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
        #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)  LV_RESULT_INVALID
    #endif
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
    #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1
        #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1(...)               LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1_WITH_OPA
        #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1_WITH_OPA(...)      LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1_WITH_MASK
        #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1_WITH_MASK(...)     LV_RESULT_INVALID
    #endif

    #ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1_MIX_MASK_OPA
        #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(...)  LV_RESULT_INVALID
    #endif
#endif

static inline void LV_ATTRIBUTE_FAST_MEM lv_color_8_8_mix(const uint8_t src, uint8_t * dest, uint8_t mix)
{

    if(mix == 0) return;

    if(mix >= LV_OPA_MAX) {
        *dest = src;
    }
    else {
        lv_opa_t mix_inv = 255 - mix;
        *dest = (uint32_t)((uint32_t)src * mix + dest[0] * mix_inv) >> 8;
    }
}

/* Layout-specific functions keep layout branches out of the pixel loops. */
#if LV_DRAW_SW_I1_HTILE_MSB || LV_DRAW_SW_I1_HTILE_LSB
    #define I1_H_BYTE(base, x, y, stride, bit_ofs) ((base) + (y) * (stride) + (((x) + (bit_ofs)) >> 3))
#endif
#if LV_DRAW_SW_I1_HTILE_MSB
    #define I1_H_MSB_BIT(x, y, stride, bit_ofs) (7 - (((x) + (bit_ofs)) & 7))
#endif
#if LV_DRAW_SW_I1_HTILE_LSB
    #define I1_H_LSB_BIT(x, y, stride, bit_ofs) (((x) + (bit_ofs)) & 7)
#endif
#if LV_DRAW_SW_I1_VTILE_MSB || LV_DRAW_SW_I1_VTILE_LSB
    #define I1_V_BYTE(base, x, y, stride, bit_ofs) ((base) + (((y) + (bit_ofs)) >> 3) * (stride) + (x))
#endif
#if LV_DRAW_SW_I1_VTILE_MSB
    #define I1_V_MSB_BIT(x, y, stride, bit_ofs) (7 - (((y) + (bit_ofs)) & 7))
#endif
#if LV_DRAW_SW_I1_VTILE_LSB
    #define I1_V_LSB_BIT(x, y, stride, bit_ofs) (((y) + (bit_ofs)) & 7)
#endif

/* Preserve legacy non-NORMAL luminance handling for premultiplied ARGB and RGB565. */
typedef struct {
    uint8_t s_normal;
    uint8_t s_nonnormal;
    uint8_t alpha;
} i1_src_px_t;

static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_src_gray(uint8_t v)
{
    i1_src_px_t p = {v, v, LV_OPA_COVER};
    return p;
}

#if LV_DRAW_SW_SUPPORT_RGB565 || LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
static inline uint8_t LV_ATTRIBUTE_FAST_MEM i1_c16_nonnormal_lum(lv_color16_t c)
{
    uint8_t r = (uint8_t)((c.red * 2106u) >> 8);
    uint8_t g = (uint8_t)((c.green * 1037u) >> 8);
    uint8_t b = (uint8_t)((c.blue * 2106u) >> 8);
    return (uint8_t)((77u * r + 151u * g + 28u * b) >> 8);
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB565
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_rgb565(const uint8_t * src, int32_t x)
{
    lv_color16_t c = ((const lv_color16_t *)src)[x];
    i1_src_px_t p = {lv_color16_luminance(c), i1_c16_nonnormal_lum(c), LV_OPA_COVER};
    return p;
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_rgb565_swapped(const uint8_t * src, int32_t x)
{
    lv_color16_t c = lv_color16_from_u16(lv_color_swap_16(((const uint16_t *)src)[x]));
    i1_src_px_t p = {lv_color16_luminance(c), i1_c16_nonnormal_lum(c), LV_OPA_COVER};
    return p;
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB888
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_rgb888(const uint8_t * src, int32_t x)
{
    return i1_src_gray(lv_color24_luminance(src + x * 3));
}
#endif

#if LV_DRAW_SW_SUPPORT_XRGB8888
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_xrgb8888(const uint8_t * src, int32_t x)
{
    return i1_src_gray(lv_color24_luminance(src + x * 4));
}
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_argb8888(const uint8_t * src, int32_t x)
{
    lv_color32_t c = ((const lv_color32_t *)src)[x];
    uint8_t l = lv_color32_luminance(c);
    i1_src_px_t p = {l, l, c.alpha};
    return p;
}
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_argb8888_premultiplied(const uint8_t * src,
                                                                                 int32_t x)
{
    lv_color32_t c = ((const lv_color32_t *)src)[x];
    i1_src_px_t p = {lv_color32_lumi_of(c, true), lv_color32_luminance(lv_color32_unpremultiply(c)), c.alpha};
    return p;
}
#endif

#if LV_DRAW_SW_SUPPORT_L8
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_l8(const uint8_t * src, int32_t x)
{
    return i1_src_gray(src[x]);
}
#endif

#if LV_DRAW_SW_SUPPORT_AL88
static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_al88(const uint8_t * src, int32_t x)
{
    const lv_color16a_t * p = &((const lv_color16a_t *)src)[x];
    i1_src_px_t r = {p->lumi, p->lumi, p->alpha};
    return r;
}
#endif

/* Decode I1 using its tiling, bit order, and sub-byte source offset. */
static inline uint8_t LV_ATTRIBUTE_FAST_MEM i1_src_bit(const uint8_t * base, int32_t x, int32_t y,
                                                       const lv_draw_sw_blend_image_dsc_t * dsc)
{
    if(dsc->src_vtiled) {
        int32_t idx = y + dsc->src_ybit;
        uint32_t byte_off = ((uint32_t)(idx >> 3)) * dsc->src_stride + (uint32_t)x;
        int32_t bit = dsc->src_lsb_first ? (idx & 7) : 7 - (idx & 7);
        return (base[byte_off] >> bit) & 1;
    }
    else {
        const uint8_t * row = base + (uint32_t)y * dsc->src_stride;
        int32_t idx = x + dsc->src_xbit;
        uint32_t byte_off = (uint32_t)(idx >> 3);
        int32_t bit = dsc->src_lsb_first ? (idx & 7) : 7 - (idx & 7);
        return (row[byte_off] >> bit) & 1;
    }
}

static inline i1_src_px_t LV_ATTRIBUTE_FAST_MEM i1_source_i1(const uint8_t * base, int32_t x,
                                                             int32_t y,
                                                             const lv_draw_sw_blend_image_dsc_t * dsc)
{
    return i1_src_gray(i1_src_bit(base, x, y, dsc) * 255);
}

/* Fill keeps the legacy /255 threshold semantics, distinct from image blending. */
#define I1_DEFINE_GENERIC_FILL(name, byte_expr, bit_expr)                                        \
    static void LV_ATTRIBUTE_FAST_MEM name(lv_draw_sw_blend_fill_dsc_t * dsc)                   \
    {                                                                                             \
        int32_t w = dsc->dest_w;                                                                  \
        int32_t h = dsc->dest_h;                                                                  \
        uint8_t src01 = lv_color_luminance(dsc->color) / (I1_LUM_THRESHOLD + 1);                 \
        int32_t bit_ofs = dsc->dest_vtiled ? (dsc->relative_area.y1 % 8) : (dsc->relative_area.x1 % 8); \
        uint8_t * dest = dsc->dest_buf;                                                           \
        const lv_opa_t * mask = dsc->mask_buf;                                                    \
        int32_t mask_stride = dsc->mask_stride;                                                   \
        lv_opa_t opa = dsc->opa;                                                                  \
        \
        if(mask == NULL && opa >= LV_OPA_MAX) {                                                   \
            for(int32_t y = 0; y < h; y++) {                                                      \
                for(int32_t x = 0; x < w; x++) {                                                  \
                    uint8_t * b = byte_expr(dest, x, y, dsc->dest_stride, bit_ofs);               \
                    uint8_t sh = (uint8_t)(1U << bit_expr(x, y, dsc->dest_stride, bit_ofs));      \
                    if(src01) *b |= sh; else *b &= (uint8_t)~sh;                                  \
                }                                                                                 \
            }                                                                                     \
        }                                                                                         \
        else if(mask == NULL && opa < LV_OPA_MAX) {                                               \
            for(int32_t y = 0; y < h; y++) {                                                      \
                for(int32_t x = 0; x < w; x++) {                                                  \
                    uint8_t * b = byte_expr(dest, x, y, dsc->dest_stride, bit_ofs);               \
                    uint8_t sh = (uint8_t)(1U << bit_expr(x, y, dsc->dest_stride, bit_ofs));      \
                    uint8_t cur = (*b >> bit_expr(x, y, dsc->dest_stride, bit_ofs)) & 1;          \
                    uint8_t nb = (opa * src01 + (255 - opa) * cur) / 255;                         \
                    if(nb) *b |= sh; else *b &= (uint8_t)~sh;                                     \
                }                                                                                 \
            }                                                                                     \
        }                                                                                         \
        else if(mask != NULL && opa >= LV_OPA_MAX) {                                              \
            for(int32_t y = 0; y < h; y++) {                                                      \
                for(int32_t x = 0; x < w; x++) {                                                  \
                    uint8_t mv = mask[x];                                                         \
                    if(mv == LV_OPA_TRANSP) continue;                                             \
                    uint8_t * b = byte_expr(dest, x, y, dsc->dest_stride, bit_ofs);               \
                    uint8_t sh = (uint8_t)(1U << bit_expr(x, y, dsc->dest_stride, bit_ofs));      \
                    if(mv == LV_OPA_COVER) { if(src01) *b |= sh; else *b &= (uint8_t)~sh; }       \
                    else {                                                                        \
                        uint8_t cur = (*b >> bit_expr(x, y, dsc->dest_stride, bit_ofs)) & 1;      \
                        uint8_t nb = (mv * src01 + (255 - mv) * cur) / 255;                       \
                        if(nb) *b |= sh; else *b &= (uint8_t)~sh;                                 \
                    }                                                                             \
                }                                                                                 \
                mask += mask_stride;                                                              \
            }                                                                                     \
        }                                                                                         \
        else {                                                                                    \
            for(int32_t y = 0; y < h; y++) {                                                      \
                for(int32_t x = 0; x < w; x++) {                                                  \
                    uint8_t mv = mask[x];                                                         \
                    if(mv == LV_OPA_TRANSP) continue;                                             \
                    uint8_t * b = byte_expr(dest, x, y, dsc->dest_stride, bit_ofs);               \
                    uint8_t sh = (uint8_t)(1U << bit_expr(x, y, dsc->dest_stride, bit_ofs));      \
                    uint8_t cur = (*b >> bit_expr(x, y, dsc->dest_stride, bit_ofs)) & 1;          \
                    uint8_t bo = (mv * opa) / 255;                                                \
                    uint8_t nb = (bo * src01 + (255 - bo) * cur) / 255;                           \
                    if(nb) *b |= sh; else *b &= (uint8_t)~sh;                                     \
                }                                                                                 \
                mask += mask_stride;                                                              \
            }                                                                                     \
        }                                                                                         \
    }

/* Generate source/layout-specialized loops without per-pixel layout branches. */
#define I1_NORMAL_PIXEL(SRC_READ, BE, BT, MIX_EXPR)                                              \
    i1_src_px_t sp = (SRC_READ);                                                                 \
    uint8_t * b = BE(dest, x, y, dsc->dest_stride, bit_ofs);                                     \
    uint8_t sh = (uint8_t)(1U << BT(x, y, dsc->dest_stride, bit_ofs));                           \
    uint8_t D = (uint8_t)((*b >> BT(x, y, dsc->dest_stride, bit_ofs)) & 1u) * 255u;              \
    uint8_t mix = (MIX_EXPR);                                                                    \
    uint8_t nv = D;                                                                              \
    lv_color_8_8_mix(sp.s_normal, &nv, mix);                                                     \
    if(nv > I1_LUM_THRESHOLD) *b |= sh; else *b &= (uint8_t)~sh;

#define I1_NONNORMAL_PIXEL(SRC_READ, BE, BT, MIX_EXPR)                                           \
    i1_src_px_t sp = (SRC_READ);                                                                 \
    uint8_t * b = BE(dest, x, y, dsc->dest_stride, bit_ofs);                                     \
    uint8_t sh = (uint8_t)(1U << BT(x, y, dsc->dest_stride, bit_ofs));                           \
    uint8_t D = (uint8_t)((*b >> BT(x, y, dsc->dest_stride, bit_ofs)) & 1u) * 255u;              \
    uint8_t res;                                                                                 \
    switch(dsc->blend_mode) {                                                                    \
    case LV_BLEND_MODE_ADDITIVE: res = (D + sp.s_nonnormal) > 255 ? 255 : (D + sp.s_nonnormal); break; \
    case LV_BLEND_MODE_SUBTRACTIVE: res = D > sp.s_nonnormal ? D - sp.s_nonnormal : 0; break; \
    case LV_BLEND_MODE_MULTIPLY: res = (uint8_t)(((uint32_t)D * sp.s_nonnormal) >> 8); break; \
    case LV_BLEND_MODE_DIFFERENCE: res = D > sp.s_nonnormal ? D - sp.s_nonnormal : sp.s_nonnormal - D; break; \
    default: res = D; break;                                                                 \
    }                                                                                            \
    uint8_t mix = (MIX_EXPR);                                                                    \
    uint8_t nv = D;                                                                              \
    lv_color_8_8_mix(res, &nv, mix);                                                             \
    if(nv > I1_LUM_THRESHOLD) *b |= sh; else *b &= (uint8_t)~sh;

#define I1_DEFINE_GENERIC_IMAGE(name, byte_expr, bit_expr, SRC_READ, SRC_ADV, HAS_ALPHA)          \
    static void LV_ATTRIBUTE_FAST_MEM name(lv_draw_sw_blend_image_dsc_t * dsc)                   \
    {                                                                                             \
        int32_t w = dsc->dest_w;                                                                  \
        int32_t h = dsc->dest_h;                                                                  \
        lv_opa_t opa = dsc->opa;                                                                  \
        int32_t bit_ofs = dsc->dest_vtiled ? (dsc->relative_area.y1 % 8) : (dsc->relative_area.x1 % 8); \
        uint8_t * dest = dsc->dest_buf;                                                           \
        const uint8_t * src = dsc->src_buf;                                                      \
        const lv_opa_t * mask = dsc->mask_buf;                                                   \
        int32_t mask_stride = dsc->mask_stride;                                                  \
        \
        if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {                                             \
            if(mask == NULL && opa >= LV_OPA_MAX) {                                               \
                for(int32_t y = 0; y < h; y++) {                                                  \
                    for(int32_t x = 0; x < w; x++) { I1_NORMAL_PIXEL(SRC_READ, byte_expr, bit_expr, HAS_ALPHA ? sp.alpha : 255) } \
                    SRC_ADV;                                                                      \
                }                                                                                 \
            }                                                                                     \
            else if(mask == NULL && opa < LV_OPA_MAX) {                                           \
                for(int32_t y = 0; y < h; y++) {                                                  \
                    for(int32_t x = 0; x < w; x++) { I1_NORMAL_PIXEL(SRC_READ, byte_expr, bit_expr, HAS_ALPHA ? LV_OPA_MIX2(sp.alpha, opa) : opa) } \
                    SRC_ADV;                                                                      \
                }                                                                                 \
            }                                                                                     \
            else if(mask != NULL && opa >= LV_OPA_MAX) {                                          \
                for(int32_t y = 0; y < h; y++) {                                                  \
                    for(int32_t x = 0; x < w; x++) { uint8_t mv = mask[x]; I1_NORMAL_PIXEL(SRC_READ, byte_expr, bit_expr, HAS_ALPHA ? LV_OPA_MIX2(sp.alpha, mv) : mv) } \
                    SRC_ADV; mask += mask_stride;                                                 \
                }                                                                                 \
            }                                                                                     \
            else {                                                                                \
                for(int32_t y = 0; y < h; y++) {                                                  \
                    for(int32_t x = 0; x < w; x++) { uint8_t mv = mask[x]; I1_NORMAL_PIXEL(SRC_READ, byte_expr, bit_expr, HAS_ALPHA ? LV_OPA_MIX3(sp.alpha, mv, opa) : LV_OPA_MIX2(mv, opa)) } \
                    SRC_ADV; mask += mask_stride;                                                 \
                }                                                                                 \
            }                                                                                     \
        }                                                                                         \
        else {                                                                                    \
            if(mask == NULL) {                                                                    \
                for(int32_t y = 0; y < h; y++) {                                                  \
                    for(int32_t x = 0; x < w; x++) { I1_NONNORMAL_PIXEL(SRC_READ, byte_expr, bit_expr, HAS_ALPHA ? LV_OPA_MIX2(sp.alpha, opa) : opa) } \
                    SRC_ADV;                                                                      \
                }                                                                                 \
            }                                                                                     \
            else {                                                                                \
                for(int32_t y = 0; y < h; y++) {                                                  \
                    for(int32_t x = 0; x < w; x++) { uint8_t mv = mask[x]; I1_NONNORMAL_PIXEL(SRC_READ, byte_expr, bit_expr, HAS_ALPHA ? LV_OPA_MIX3(sp.alpha, mv, opa) : LV_OPA_MIX2(mv, opa)) } \
                    SRC_ADV; mask += mask_stride;                                                 \
                }                                                                                 \
            }                                                                                     \
        }                                                                                         \
    }

#if LV_DRAW_SW_I1_HTILE_MSB
    I1_DEFINE_GENERIC_FILL(i1_fill_hmsb, I1_H_BYTE, I1_H_MSB_BIT)
#endif
#if LV_DRAW_SW_I1_HTILE_LSB
    I1_DEFINE_GENERIC_FILL(i1_fill_hlsb, I1_H_BYTE, I1_H_LSB_BIT)
#endif
#if LV_DRAW_SW_I1_VTILE_MSB
    I1_DEFINE_GENERIC_FILL(i1_fill_vmsb, I1_V_BYTE, I1_V_MSB_BIT)
#endif
#if LV_DRAW_SW_I1_VTILE_LSB
    I1_DEFINE_GENERIC_FILL(i1_fill_vlsb, I1_V_BYTE, I1_V_LSB_BIT)
#endif

#define I1_ROW_ADV(src) (src += dsc->src_stride)

/* I1 sources handle their own row and layout offsets; other formats advance by stride. */
#if LV_DRAW_SW_I1_HTILE_MSB
#define I1_IMG_HMSB(suffix, SRC_READ, HAS_ALPHA) \
    I1_DEFINE_GENERIC_IMAGE(i1_image_hmsb_##suffix, I1_H_BYTE, I1_H_MSB_BIT, SRC_READ, I1_ROW_ADV(src), HAS_ALPHA)
#define I1_I1_HMSB() I1_DEFINE_GENERIC_IMAGE(i1_image_hmsb_i1, I1_H_BYTE, I1_H_MSB_BIT, i1_source_i1(src, x, y, dsc), ((void)0), 0)
#endif
#if LV_DRAW_SW_I1_HTILE_LSB
#define I1_IMG_HLSB(suffix, SRC_READ, HAS_ALPHA) \
    I1_DEFINE_GENERIC_IMAGE(i1_image_hlsb_##suffix, I1_H_BYTE, I1_H_LSB_BIT, SRC_READ, I1_ROW_ADV(src), HAS_ALPHA)
#define I1_I1_HLSB() I1_DEFINE_GENERIC_IMAGE(i1_image_hlsb_i1, I1_H_BYTE, I1_H_LSB_BIT, i1_source_i1(src, x, y, dsc), ((void)0), 0)
#endif
#if LV_DRAW_SW_I1_VTILE_MSB
#define I1_IMG_VMSB(suffix, SRC_READ, HAS_ALPHA) \
    I1_DEFINE_GENERIC_IMAGE(i1_image_vmsb_##suffix, I1_V_BYTE, I1_V_MSB_BIT, SRC_READ, I1_ROW_ADV(src), HAS_ALPHA)
#define I1_I1_VMSB() I1_DEFINE_GENERIC_IMAGE(i1_image_vmsb_i1, I1_V_BYTE, I1_V_MSB_BIT, i1_source_i1(src, x, y, dsc), ((void)0), 0)
#endif
#if LV_DRAW_SW_I1_VTILE_LSB
#define I1_IMG_VLSB(suffix, SRC_READ, HAS_ALPHA) \
    I1_DEFINE_GENERIC_IMAGE(i1_image_vlsb_##suffix, I1_V_BYTE, I1_V_LSB_BIT, SRC_READ, I1_ROW_ADV(src), HAS_ALPHA)
#define I1_I1_VLSB() I1_DEFINE_GENERIC_IMAGE(i1_image_vlsb_i1, I1_V_BYTE, I1_V_LSB_BIT, i1_source_i1(src, x, y, dsc), ((void)0), 0)
#endif

#if LV_DRAW_SW_I1_HTILE_MSB
    #if LV_DRAW_SW_SUPPORT_RGB565
        I1_IMG_HMSB(rgb565, i1_source_rgb565(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        I1_IMG_HMSB(rgb565_swapped, i1_source_rgb565_swapped(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB888
        I1_IMG_HMSB(rgb888, i1_source_rgb888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_XRGB8888
        I1_IMG_HMSB(xrgb8888, i1_source_xrgb8888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888
        I1_IMG_HMSB(argb8888, i1_source_argb8888(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        I1_IMG_HMSB(argb8888_premultiplied, i1_source_argb8888_premultiplied(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_L8
        I1_IMG_HMSB(l8, i1_source_l8(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_AL88
        I1_IMG_HMSB(al88, i1_source_al88(src, x), 1)
    #endif
    I1_I1_HMSB()
#endif

#if LV_DRAW_SW_I1_HTILE_LSB
    #if LV_DRAW_SW_SUPPORT_RGB565
        I1_IMG_HLSB(rgb565, i1_source_rgb565(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        I1_IMG_HLSB(rgb565_swapped, i1_source_rgb565_swapped(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB888
        I1_IMG_HLSB(rgb888, i1_source_rgb888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_XRGB8888
        I1_IMG_HLSB(xrgb8888, i1_source_xrgb8888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888
        I1_IMG_HLSB(argb8888, i1_source_argb8888(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        I1_IMG_HLSB(argb8888_premultiplied, i1_source_argb8888_premultiplied(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_L8
        I1_IMG_HLSB(l8, i1_source_l8(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_AL88
        I1_IMG_HLSB(al88, i1_source_al88(src, x), 1)
    #endif
    I1_I1_HLSB()
#endif

#if LV_DRAW_SW_I1_VTILE_MSB
    #if LV_DRAW_SW_SUPPORT_RGB565
        I1_IMG_VMSB(rgb565, i1_source_rgb565(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        I1_IMG_VMSB(rgb565_swapped, i1_source_rgb565_swapped(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB888
        I1_IMG_VMSB(rgb888, i1_source_rgb888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_XRGB8888
        I1_IMG_VMSB(xrgb8888, i1_source_xrgb8888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888
        I1_IMG_VMSB(argb8888, i1_source_argb8888(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        I1_IMG_VMSB(argb8888_premultiplied, i1_source_argb8888_premultiplied(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_L8
        I1_IMG_VMSB(l8, i1_source_l8(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_AL88
        I1_IMG_VMSB(al88, i1_source_al88(src, x), 1)
    #endif
    I1_I1_VMSB()
#endif

#if LV_DRAW_SW_I1_VTILE_LSB
    #if LV_DRAW_SW_SUPPORT_RGB565
        I1_IMG_VLSB(rgb565, i1_source_rgb565(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        I1_IMG_VLSB(rgb565_swapped, i1_source_rgb565_swapped(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_RGB888
        I1_IMG_VLSB(rgb888, i1_source_rgb888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_XRGB8888
        I1_IMG_VLSB(xrgb8888, i1_source_xrgb8888(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888
        I1_IMG_VLSB(argb8888, i1_source_argb8888(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        I1_IMG_VLSB(argb8888_premultiplied, i1_source_argb8888_premultiplied(src, x), 1)
    #endif
    #if LV_DRAW_SW_SUPPORT_L8
        I1_IMG_VLSB(l8, i1_source_l8(src, x), 0)
    #endif
    #if LV_DRAW_SW_SUPPORT_AL88
        I1_IMG_VLSB(al88, i1_source_al88(src, x), 1)
    #endif
    I1_I1_VLSB()
#endif

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_i1(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    /* Keep accelerator hooks for the legacy htile-MSB layout. */
    if(!dsc->dest_vtiled && !dsc->dest_lsb_first) {
        if(dsc->mask_buf == NULL && dsc->opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID != LV_DRAW_SW_COLOR_BLEND_TO_I1(dsc)) return;
        }
        else if(dsc->mask_buf == NULL && dsc->opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID != LV_DRAW_SW_COLOR_BLEND_TO_I1_WITH_OPA(dsc)) return;
        }
        else if(dsc->mask_buf != NULL && dsc->opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID != LV_DRAW_SW_COLOR_BLEND_TO_I1_WITH_MASK(dsc)) return;
        }
        else {
            if(LV_RESULT_INVALID != LV_DRAW_SW_COLOR_BLEND_TO_I1_MIX_MASK_OPA(dsc)) return;
        }
    }

    if(dsc->dest_vtiled) {
        if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
            i1_fill_vlsb(dsc);
#endif
        }
        else {
#if LV_DRAW_SW_I1_VTILE_MSB
            i1_fill_vmsb(dsc);
#endif
        }
    }
    else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
        i1_fill_hlsb(dsc);
#endif
    }
    else {
#if LV_DRAW_SW_I1_HTILE_MSB
        i1_fill_hmsb(dsc);
#endif
    }
}

/* Try the legacy htile-MSB NORMAL accelerator hook for this source format. */
#define I1_ACCEL_TRY(FMT)                                                               \
    if(dsc->mask_buf == NULL && dsc->opa >= LV_OPA_MAX) {                               \
        if(LV_RESULT_INVALID != LV_DRAW_SW_##FMT##_BLEND_NORMAL_TO_I1(dsc)) return;     \
    }                                                                                   \
    else if(dsc->mask_buf == NULL && dsc->opa < LV_OPA_MAX) {                           \
        if(LV_RESULT_INVALID != LV_DRAW_SW_##FMT##_BLEND_NORMAL_TO_I1_WITH_OPA(dsc)) return;  \
    }                                                                                   \
    else if(dsc->mask_buf != NULL && dsc->opa >= LV_OPA_MAX) {                          \
        if(LV_RESULT_INVALID != LV_DRAW_SW_##FMT##_BLEND_NORMAL_TO_I1_WITH_MASK(dsc)) return;  \
    }                                                                                   \
    else {                                                                              \
        if(LV_RESULT_INVALID != LV_DRAW_SW_##FMT##_BLEND_NORMAL_TO_I1_MIX_MASK_OPA(dsc)) return;  \
    }

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_i1(lv_draw_sw_blend_image_dsc_t * dsc)
{
    /* Accelerators support only clean htile-MSB NORMAL sources. */
    if(!dsc->dest_vtiled && !dsc->dest_lsb_first &&
       dsc->blend_mode == LV_BLEND_MODE_NORMAL &&
       !dsc->src_vtiled && !dsc->src_lsb_first && dsc->src_xbit == 0) {
        switch(dsc->src_color_format) {
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                I1_ACCEL_TRY(RGB565);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
            case LV_COLOR_FORMAT_RGB565_SWAPPED:
                I1_ACCEL_TRY(RGB565_SWAPPED);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                I1_ACCEL_TRY(RGB888);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
                I1_ACCEL_TRY(RGB888);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
            case LV_COLOR_FORMAT_ARGB8888:
                I1_ACCEL_TRY(ARGB8888);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
            case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
                I1_ACCEL_TRY(ARGB8888);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                I1_ACCEL_TRY(L8);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_AL88
            case LV_COLOR_FORMAT_AL88:
                I1_ACCEL_TRY(AL88);
                break;
#endif
            case LV_COLOR_FORMAT_I1:
                I1_ACCEL_TRY(I1);
                break;
            default:
                break;
        }
    }

    switch(dsc->src_color_format) {
#if LV_DRAW_SW_SUPPORT_RGB565
        case LV_COLOR_FORMAT_RGB565:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_rgb565(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_rgb565(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_rgb565(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_rgb565(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_rgb565_swapped(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_rgb565_swapped(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_rgb565_swapped(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_rgb565_swapped(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
        case LV_COLOR_FORMAT_RGB888:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_rgb888(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_rgb888(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_rgb888(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_rgb888(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_XRGB8888
        case LV_COLOR_FORMAT_XRGB8888:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_xrgb8888(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_xrgb8888(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_xrgb8888(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_xrgb8888(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
        case LV_COLOR_FORMAT_ARGB8888:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_argb8888(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_argb8888(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_argb8888(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_argb8888(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_argb8888_premultiplied(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_argb8888_premultiplied(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_argb8888_premultiplied(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_argb8888_premultiplied(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_L8
        case LV_COLOR_FORMAT_L8:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_l8(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_l8(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_l8(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_l8(dsc);
#endif
            }
            break;
#endif
#if LV_DRAW_SW_SUPPORT_AL88
        case LV_COLOR_FORMAT_AL88:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_al88(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_al88(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_al88(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_al88(dsc);
#endif
            }
            break;
#endif
        case LV_COLOR_FORMAT_I1:
            if(dsc->dest_vtiled) {
                if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_VTILE_LSB
                    i1_image_vlsb_i1(dsc);
#endif
                }
                else {
#if LV_DRAW_SW_I1_VTILE_MSB
                    i1_image_vmsb_i1(dsc);
#endif
                }
            }
            else if(dsc->dest_lsb_first) {
#if LV_DRAW_SW_I1_HTILE_LSB
                i1_image_hlsb_i1(dsc);
#endif
            }
            else {
#if LV_DRAW_SW_I1_HTILE_MSB
                i1_image_hmsb_i1(dsc);
#endif
            }
            break;
        default:
            break;
    }
}

#undef I1_ACCEL_TRY
#undef I1_NORMAL_PIXEL
#undef I1_NONNORMAL_PIXEL
#undef I1_ROW_ADV
#undef I1_DEFINE_GENERIC_IMAGE
#undef I1_DEFINE_GENERIC_FILL
#undef I1_IMG_HMSB
#undef I1_IMG_HLSB
#undef I1_IMG_VMSB
#undef I1_IMG_VLSB
#undef I1_I1_HMSB
#undef I1_I1_HLSB
#undef I1_I1_VMSB
#undef I1_I1_VLSB
#undef I1_V_LSB_BIT
#undef I1_V_MSB_BIT
#undef I1_V_BYTE
#undef I1_H_LSB_BIT
#undef I1_H_MSB_BIT
#undef I1_H_BYTE

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
static inline lv_color16_t LV_ATTRIBUTE_FAST_MEM lv_color16_from_u16(uint16_t raw)
{
    lv_color16_t c;
    c.red = (raw >> 11) & 0x1F;
    c.green = (raw >> 5) & 0x3F;
    c.blue = raw & 0x1F;
    return c;
}
#endif

#endif /* LV_DRAW_SW_SUPPORT_I1 */

#endif
