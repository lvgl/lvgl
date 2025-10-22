/**
 * @file lv_draw_sw_blend_neon_to_rgb888.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_neon_to_rgb888.h"
#include <stdint.h>
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON

#include "../../../../misc/lv_color.h"
#include "../../../../misc/lv_types.h"
#include "../lv_draw_sw_blend_private.h"
#include <arm_neon.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride);

static inline uint32x4_t rgb565_xrgb_mix_4_internal(uint32x4_t src, uint32x4_t dst, uint32x4_t mix);
static inline uint32x2_t rgb565_xrgb_mix_2_internal(uint32x2_t src, uint32x2_t dst, uint32x2_t mix);

static inline uint8x16x3_t lv_color_to_rgb888_16(const lv_color_t * color);
static inline uint8x8x3_t lv_color_to_rgb888_8(const lv_color_t * color);

static inline uint8x16x4_t lv_color_to_xrgb888_16(const lv_color_t * color);
static inline uint8x8x4_t lv_color_to_xrgb888_8(const lv_color_t * color);
static inline uint8x16_t lv_color_to_xrgb888_4(const lv_color_t * color);
static inline uint8x8_t lv_color_to_xrgb888_2(const lv_color_t * color);
static inline uint8x8x3_t lv_rgb565_to_rgb888_8(const uint16_t * color);
static inline uint8x8x4_t lv_rgb565_to_xrgb8888_8(const uint16_t * color);


static inline uint8x8x3_t rgb565_rgb888_mix_8_internal(uint16x8_t src, uint8x8x3_t dst, uint8x8_t mix);

static inline void lv_color_24_24_mix_1(const uint8_t * src, uint8_t * dest, uint8_t mix);

static inline void rgb565_mix_1(const uint16_t * src, uint8_t * dest, uint8_t mix);

static inline uint32x4_t argb_rgb_mix_4(const uint32_t * src, const uint32_t * dst);
static inline uint32x2_t argb_rgb_mix_2(const uint32_t * src, const uint32_t * dst);

static inline uint32x4_t argb_rgb_mix_4_with_opa(const uint32_t * src, const uint32_t * dst, uint32x4_t mix);
static inline uint32x2_t argb_rgb_mix_2_with_opa(const uint32_t * src, const uint32_t * dst, uint32x2_t mix);

static inline uint32x4_t argb_rgb_mix_4_with_opa_mask(const uint32_t * src, const uint32_t * dst, uint32x4_t opa,
                                                      uint32x4_t mask);
static inline uint32x2_t argb_rgb_mix_2_with_opa_mask(const uint32_t * src, const uint32_t * dst, uint32x2_t opa,
                                                      uint32x2_t mask);

static inline uint32x4_t lv_color_24_24_mix_4_internal(uint32x4_t src, uint32x4_t dst, uint32x4_t mix);
static inline uint32x2_t lv_color_24_24_mix_2_internal(uint32x2_t src, uint32x2_t dst, uint32x2_t mix);

static inline uint32x4_t lv_color_24_24_mix_4_premul_internal(uint32x4_t src, uint32x4_t dst, uint32x4_t mix);
static inline uint32x2_t lv_color_24_24_mix_2_premul_internal(uint32x2_t src, uint32x2_t dst, uint32x2_t mix);
static inline void lv_color_24_24_mix_premult(const uint8_t * src, uint8_t * dest, uint8_t mix);

static inline uint8x16x3_t rgb_mix_8_internal(uint8x16x3_t src, uint8x16x3_t dst, uint8x16_t mix);
static inline uint8x8x3_t rgb_mix_4_internal(uint8x8x3_t src, uint8x8x3_t dst, uint8x8_t mix);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_draw_sw_blend_neon_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    uint8_t * dest_buf_8      = dsc->dest_buf;

    if(dest_px_size == 3) {
        const int32_t byte_w = w * 3;
        const uint8x16x3_t vec_16 = lv_color_to_rgb888_16(&dsc->color);
        const uint8x8x3_t vec_8 = lv_color_to_rgb888_8(&dsc->color);
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_byte_ptr = dest_buf_8;
            int32_t x = 0;
            for(; x < byte_w - 47; x += 48) {
                vst3q_u8(&row_byte_ptr[x], vec_16);
            }
            for(; x < byte_w - 23; x += 24) {
                vst3_u8(&row_byte_ptr[x], vec_8);
            }
            for(; x < byte_w; x += 3) {
                row_byte_ptr[x + 0] = dsc->color.blue;
                row_byte_ptr[x + 1] = dsc->color.green;
                row_byte_ptr[x + 2] = dsc->color.red;
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
        }
    }
    else if(dest_px_size == 4) {
        const int32_t byte_w = w * 4;
        const uint8x16x4_t vec_16 = lv_color_to_xrgb888_16(&dsc->color);
        const uint8x8x4_t vec_8 = lv_color_to_xrgb888_8(&dsc->color);
        const uint8x16_t vec_4 = lv_color_to_xrgb888_4(&dsc->color);
        const uint8x8_t vec_2 = lv_color_to_xrgb888_2(&dsc->color);

        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_byte_ptr = dest_buf_8;
            int32_t x = 0;
            for(; x < byte_w - 63; x += 64) {
                vst4q_u8(&row_byte_ptr[x], vec_16);
            }
            for(; x < byte_w - 31; x += 32) {
                vst4_u8(&row_byte_ptr[x], vec_8);
            }
            for(; x < byte_w - 15; x += 16) {
                vst1q_u8(&row_byte_ptr[x], vec_4);
            }
            for(; x < byte_w - 7; x += 8) {
                vst1_u8(&row_byte_ptr[x], vec_2);
            }
            for(; x < byte_w; x += 4) {
                row_byte_ptr[x + 0] = dsc->color.blue;
                row_byte_ptr[x + 1] = dsc->color.green;
                row_byte_ptr[x + 2] = dsc->color.red;
                row_byte_ptr[x + 3] = 0xFF;
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
        }
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa    = dsc->opa;

    if(dest_px_size == 3) {
        const uint8x16x3_t color_16 = {{vdupq_n_u8(dsc->color.blue), vdupq_n_u8(dsc->color.green), vdupq_n_u8(dsc->color.red)}};
        const uint8x8x3_t color_8 = {{vdup_n_u8(dsc->color.blue), vdup_n_u8(dsc->color.green), vdup_n_u8(dsc->color.red)}};
        const uint8x16_t opa_16 = vdupq_n_u8(dsc->opa);
        const uint8x8_t opa_8 = vdup_n_u8(dsc->opa);

        uint8_t * dest_buf_8 = dsc->dest_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            int32_t x = 0;
            for(; x < w - 15; x += 16) {
                uint8x16x3_t dst = vld3q_u8(&row_ptr[x * 3]);
                vst3q_u8(&row_ptr[x * 3], rgb_mix_8_internal(color_16, dst, opa_16));
            }
            for(; x < w - 7; x += 8) {
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb_mix_4_internal(color_8, dst, opa_8));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((const uint8_t *)&dsc->color, &row_ptr[x * 3], opa);
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
        }

    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint32_t color = dsc->color.red << 16 | dsc->color.green << 8 | dsc->color.blue;
        const uint32x4_t vec_4 = vdupq_n_u32(color);
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t vec_2 = vdup_n_u32(color);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                vst1q_u32(&row_ptr[x], lv_color_24_24_mix_4_internal(vec_4, dst, opa_4));
            }
            for(; x < w - 1; x += 2) {
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                vst1_u32(&row_ptr[x], lv_color_24_24_mix_2_internal(vec_2, dst, opa_2));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((const uint8_t *)&dsc->color, (uint8_t *)&row_ptr[x], opa);
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
        }
    }

    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    if(dest_px_size == 3) {
        const uint8x16x3_t color_16 = {{vdupq_n_u8(dsc->color.blue), vdupq_n_u8(dsc->color.green), vdupq_n_u8(dsc->color.red)}};
        const uint8x8x3_t color_8 = {{vdup_n_u8(dsc->color.blue), vdup_n_u8(dsc->color.green), vdup_n_u8(dsc->color.red)}};

        uint8_t * dest_buf_8 = dsc->dest_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint8_t * mask_row_ptr = mask_buf_8;
            int32_t x = 0;
            for(; x < w - 15; x += 16) {
                const uint8x16_t mask_16 = vld1q_u8(&mask_row_ptr[x]);
                uint8x16x3_t dst = vld3q_u8(&row_ptr[x * 3]);
                vst3q_u8(&row_ptr[x * 3], rgb_mix_8_internal(color_16, dst, mask_16));
            }
            for(; x < w - 7; x += 8) {
                const uint8x8_t mask_8 = vld1_u8(&mask_row_ptr[x]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb_mix_4_internal(color_8, dst, mask_8));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((const uint8_t *)&dsc->color, &row_ptr[x * 3], mask_row_ptr[x]);
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint32_t color = dsc->color.red << 16 | dsc->color.green << 8 | dsc->color.blue;
        const uint32x4_t vec_4 = vdupq_n_u32(color);
        const uint32x2_t vec_2 = vdup_n_u32(color);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint8_t * mask_row_ptr = mask_buf_8;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                const uint32x4_t mix_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                const uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                vst1q_u32(&row_ptr[x], lv_color_24_24_mix_4_internal(vec_4, dst, mix_4));
            }
            for(; x < w - 1; x += 2) {
                const uint32x2_t mix_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                const uint32x2_t dst = vld1_u32(&row_ptr[x]);
                vst1_u32(&row_ptr[x], lv_color_24_24_mix_2_internal(vec_2, dst, mix_2));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((const uint8_t *)&dsc->color, (uint8_t *)&row_ptr[x], mask_row_ptr[x]);
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_color_to_rgb888_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa = dsc->opa;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    if(dest_px_size == 3) {
        const uint8x16x3_t color_16 = {{vdupq_n_u8(dsc->color.blue), vdupq_n_u8(dsc->color.green), vdupq_n_u8(dsc->color.red)}};
        const uint8x8x3_t color_8 = {{vdup_n_u8(dsc->color.blue), vdup_n_u8(dsc->color.green), vdup_n_u8(dsc->color.red)}};
        const uint8x16_t opa_16 = vdupq_n_u8(dsc->opa);
        const uint8x8_t opa_8 = vdup_n_u8(dsc->opa);

        uint8_t * dest_buf_8 = dsc->dest_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint8_t * mask_row_ptr = mask_buf_8;
            int32_t x = 0;
            for(; x < w - 15; x += 16) {
                const uint8x16_t mask_16 = vld1q_u8(&mask_row_ptr[x]);
                const uint8x8_t opa_lo = vget_low_u8(opa_16);
                const uint8x8_t opa_hi = vget_high_u8(opa_16);
                const uint8x8_t mask_lo = vget_low_u8(mask_16);
                const uint8x8_t mask_hi = vget_high_u8(mask_16);
                const uint16x8_t result_lo_wide = vmull_u8(opa_lo, mask_lo);
                const uint16x8_t result_hi_wide = vmull_u8(opa_hi, mask_hi);
                const uint8x8_t result_lo = vshrn_n_u16(result_lo_wide, 8);
                const uint8x8_t result_hi = vshrn_n_u16(result_hi_wide, 8);
                const uint8x16_t mix = vcombine_u8(result_lo, result_hi);
                uint8x16x3_t dst = vld3q_u8(&row_ptr[x * 3]);
                vst3q_u8(&row_ptr[x * 3], rgb_mix_8_internal(color_16, dst, mix));
            }
            for(; x < w - 7; x += 8) {
                const uint8x8_t mask_8 = vld1_u8(&mask_row_ptr[x]);
                const uint16x8_t mult = vmull_u8(opa_8, mask_8);
                const uint8x8_t mix = vshrn_n_u16(mult, 8);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb_mix_4_internal(color_8, dst, mix));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((const uint8_t *)&dsc->color, &row_ptr[x * 3], LV_OPA_MIX2(opa, mask_row_ptr[x]));
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    else if(dest_px_size == 4) {
        const int32_t mask_stride = dsc->mask_stride;
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint8_t * mask_buf_8 = dsc->mask_buf;
        const uint32_t color = dsc->color.red << 16 | dsc->color.green << 8 | dsc->color.blue;
        const uint32x4_t vec_4 = vdupq_n_u32(color);
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t vec_2 = vdup_n_u32(color);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint8_t * mask_row_ptr = mask_buf_8;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                const uint32x4_t mix_4 = vshrq_n_u32(vmulq_u32(opa_4, mask_4), 8);
                const uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                vst1q_u32(&row_ptr[x], lv_color_24_24_mix_4_internal(vec_4, dst, mix_4));
            }
            for(; x < w - 1; x += 2) {
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                const uint32x2_t mix_2 = vshr_n_u32(vmul_u32(opa_2, mask_2), 8);
                const uint32x2_t dst = vld1_u32(&row_ptr[x]);
                vst1_u32(&row_ptr[x], lv_color_24_24_mix_2_internal(vec_2, dst, mix_2));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((const uint8_t *)&dsc->color, (uint8_t *)&row_ptr[x], LV_OPA_MIX2(opa, mask_row_ptr[x]));
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}

#ifdef __aarch64__ /* vqtbl1q_u8 is only available on arm64 */
lv_result_t lv_draw_sw_blend_neon_l8_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf_8 = dsc->dest_buf;
    const uint8_t * src_buf_l8 = dsc->src_buf;

    if(dest_px_size == 3) {
        static const uint8x16_t selector_mask = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 0};
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_byte_ptr = dest_buf_8;
            const uint8_t * src_row = src_buf_l8;
            int32_t dest_x = 0;
            int32_t src_x = 0;
            for(; src_x < w - 8; src_x += 5, dest_x += 15) {
                uint8x8_t gray_pixels = vld1_u8(&src_row[src_x]);
                uint8x16_t gray_extended = vcombine_u8(gray_pixels, vdup_n_u8(0));
                uint8x16_t rgb_result = vqtbl1q_u8(gray_extended, selector_mask);
                vst1q_u8(&row_byte_ptr[dest_x], rgb_result);
            }
            for(; src_x < w; src_x++, dest_x += 3) {
                row_byte_ptr[dest_x + 0] = src_row[src_x];
                row_byte_ptr[dest_x + 1] = src_row[src_x];
                row_byte_ptr[dest_x + 2] = src_row[src_x];
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
        }
    }
    else if(dest_px_size == 4) {
        static const uint8x16_t selector_mask = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_byte_ptr = dest_buf_8;
            const uint8_t * src_row = src_buf_l8;
            int32_t dest_x = 0;
            int32_t src_x = 0;
            for(; src_x < w - 8; src_x += 4, dest_x += 16) {
                uint8x8_t gray_pixels = vld1_u8(&src_row[src_x]);
                uint8x16_t gray_extended = vcombine_u8(gray_pixels, vdup_n_u8(0));
                uint8x16_t rgb_result = vqtbl1q_u8(gray_extended, selector_mask);
                vst1q_u8(&row_byte_ptr[dest_x], rgb_result);
            }
            for(; src_x < w; src_x++, dest_x += 4) {
                row_byte_ptr[dest_x + 0] = src_row[src_x];
                row_byte_ptr[dest_x + 1] = src_row[src_x];
                row_byte_ptr[dest_x + 2] = src_row[src_x];
                row_byte_ptr[dest_x + 3] = src_row[src_x];
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
        }
    }
    return LV_RESULT_OK;
}
#endif

lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                    = dsc->dest_w;
    int32_t h                    = dsc->dest_h;
    uint8_t * dest_buf_u8      = dsc->dest_buf;
    int32_t dest_stride          = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    int32_t src_stride           = dsc->src_stride;

    if(dest_px_size == 3) {
        for(int32_t y = 0; y < h; y++) {
            uint8_t * dest_row      = dest_buf_u8;
            const uint16_t * src_row = (const uint16_t *)src_buf_u16;
            int32_t src_x = 0;
            int32_t dest_x = 0;

            for(; src_x < w - 7; src_x += 8, dest_x += 24) {
                uint8x8x3_t rgb = lv_rgb565_to_rgb888_8(&src_row[src_x]);
                vst3_u8(&dest_row[dest_x], rgb);
            }
            for(; src_x < w; src_x++, dest_x += 3) {
                const uint8_t r = (((src_row[src_x] & 0xF800) >> 11) * 2106) >> 8;
                const uint8_t g = (((src_row[src_x] & 0x07E0) >> 5) * 1037) >> 8;
                const uint8_t b = (((src_row[src_x] & 0x001F)) * 2106) >> 8;
                dest_row[dest_x + 0] = b;
                dest_row[dest_x + 1] = g;
                dest_row[dest_x + 2] = r;
            }
            dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
            src_buf_u16  = drawbuf_next_row(src_buf_u16, src_stride);
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            uint8_t * dest_row      = dest_buf_u8;
            const uint16_t * src_row = (const uint16_t *)src_buf_u16;
            int32_t src_x = 0;
            int32_t dest_x = 0;

            for(; src_x < w - 7; src_x += 8, dest_x += 32) {
                uint8x8x4_t xrgb = lv_rgb565_to_xrgb8888_8(&src_row[src_x]);
                vst4_u8(&dest_row[dest_x], xrgb);
            }
            for(; src_x < w; src_x++, dest_x += 4) {
                const uint8_t r = (((src_row[src_x] & 0xF800) >> 11) * 2106) >> 8;
                const uint8_t g = (((src_row[src_x] & 0x07E0) >> 5) * 1037) >> 8;
                const uint8_t b = (((src_row[src_x] & 0x001F)) * 2106) >> 8;
                dest_row[dest_x + 0] = b;
                dest_row[dest_x + 1] = g;
                dest_row[dest_x + 2] = r;
            }
            dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
            src_buf_u16  = drawbuf_next_row(src_buf_u16, src_stride);
        }
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{

    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint8_t opa = dsc->opa;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;

    if(dest_px_size == 3) {
        const uint8x8_t opa_8 = vdup_n_u8(dsc->opa);
        uint8_t * dest_buf_8 = dsc->dest_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint16_t * src_row_ptr = src_buf_u16;
            int32_t x = 0;
            for(; x < w - 7; x += 8) {
                uint16x8_t src = vld1q_u16(&src_row_ptr[x]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb565_rgb888_mix_8_internal(src, dst, opa_8));
            }
            for(; x < w; x++) {
                rgb565_mix_1(&src_row_ptr[x], &row_ptr[x * 3], opa);
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
        }
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint16_t * src_buf_u16 = dsc->src_buf;
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint16_t * src_row_ptr = src_buf_u16;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                const uint16x4_t vec_4 = vld1_u16(&src_row_ptr[x]);
                uint32x4_t src = vmovl_u16(vec_4);
                vst1q_u32(&row_ptr[x], rgb565_xrgb_mix_4_internal(src, dst, opa_4));
            }
            for(; x < w - 1; x += 2) {
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                uint32x2_t src = {src_row_ptr[x], src_row_ptr[x + 1]};
                vst1_u32(&row_ptr[x], rgb565_xrgb_mix_2_internal(src, dst, opa_2));
            }
            for(; x < w; x++) {
                rgb565_mix_1(&src_row_ptr[x], (uint8_t *)&row_ptr[x], opa);
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
        }
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride = dsc->mask_stride;

    if(dest_px_size == 3) {
        uint8_t * dest_buf_8 = dsc->dest_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint16_t * src_row_ptr = src_buf_u16;
            const uint8_t * mask_row_ptr = mask_buf_u8;
            int32_t x = 0;
            for(; x < w - 7; x += 8) {
                uint16x8_t src = vld1q_u16(&src_row_ptr[x]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                uint8x8_t mask = vld1_u8(&mask_row_ptr[x]);
                vst3_u8(&row_ptr[x * 3], rgb565_rgb888_mix_8_internal(src, dst, mask));
            }
            for(; x < w; x++) {
                rgb565_mix_1(&src_row_ptr[x], &row_ptr[x * 3], mask_row_ptr[x]);
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
            mask_buf_u8 = drawbuf_next_row(mask_buf_u8, mask_stride);
        }
    }
    else if(dest_px_size == 4) {
        /* The algorithm below might look okay but the demo render test fails with it so it's disabled */
        return LV_RESULT_INVALID;
#if 0 /* TODO: Figure out the problem with the algorithm below */
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint16_t * src_buf_u16 = dsc->src_buf;

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint16_t * src_row_ptr = src_buf_u16;
            const uint8_t * mask_row_ptr = mask_buf_u8;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                const uint16x4_t vec_4 = vld1_u16(&src_row_ptr[x]);
                uint32x4_t src = vmovl_u16(vec_4);
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};

                vst1q_u32(&row_ptr[x], rgb565_xrgb_mix_4_internal(src, dst, mask_4));
            }
            for(; x < w - 1; x += 2) {
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                uint32x2_t src = {src_row_ptr[x], src_row_ptr[x + 1]};
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                vst1_u32(&row_ptr[x], rgb565_xrgb_mix_2_internal(src, dst, mask_2));
            }
            for(; x < w; x++) {
                rgb565_mix_1(&src_row_ptr[x], (uint8_t *)&row_ptr[x],  mask_row_ptr[x]);
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
            mask_buf_u8 = drawbuf_next_row(mask_buf_u8, mask_stride);
        }
#endif
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t * mask_buf_u8  = dsc->mask_buf;
    const int32_t mask_stride    = dsc->mask_stride;
    const uint8_t opa    = dsc->opa;

    if(dest_px_size == 3) {
        uint8_t * dest_buf_8 = dsc->dest_buf;
        const uint8x8_t opa_8 = vdup_n_u8(dsc->opa);
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint16_t * src_row_ptr = src_buf_u16;
            const uint8_t * mask_row_ptr = mask_buf_u8;
            int32_t x = 0;
            for(; x < w - 7; x += 8) {
                uint16x8_t src = vld1q_u16(&src_row_ptr[x]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                uint8x8_t mask = vld1_u8(&mask_row_ptr[x]);
                const uint16x8_t mult = vmull_u8(opa_8, mask);
                const uint8x8_t mix = vshrn_n_u16(mult, 8);
                vst3_u8(&row_ptr[x * 3], rgb565_rgb888_mix_8_internal(src, dst, mix));
            }
            for(; x < w; x++) {
                rgb565_mix_1(&src_row_ptr[x], &row_ptr[x * 3], LV_OPA_MIX2(mask_row_ptr[x], opa));
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
            mask_buf_u8 = drawbuf_next_row(mask_buf_u8, mask_stride);
        }
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint16_t * src_buf_u16 = dsc->src_buf;
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint16_t * src_row_ptr = src_buf_u16;
            const uint8_t * mask_row_ptr = mask_buf_u8;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                const uint16x4_t vec_4 = vld1_u16(&src_row_ptr[x]);
                uint32x4_t src = vmovl_u16(vec_4);
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                const uint32x4_t mix_4 = vshrq_n_u32(vmulq_u32(opa_4, mask_4), 8);

                vst1q_u32(&row_ptr[x], rgb565_xrgb_mix_4_internal(src, dst, mix_4));
            }
            for(; x < w - 1; x += 2) {
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                uint32x2_t src = {src_row_ptr[x], src_row_ptr[x + 1]};
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                const uint32x2_t mix_2 = vshr_n_u32(vmul_u32(opa_2, mask_2), 8);
                vst1_u32(&row_ptr[x], rgb565_xrgb_mix_2_internal(src, dst, mix_2));
            }
            for(; x < w; x++) {
                rgb565_mix_1(&src_row_ptr[x], (uint8_t *)&row_ptr[x], LV_OPA_MIX2(opa, mask_row_ptr[x]));
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
            mask_buf_u8 = drawbuf_next_row(mask_buf_u8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                   uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;

    /* Fallback to sw implementation*/
    if(src_px_size != dest_px_size) {
        return LV_RESULT_INVALID;
    }

    for(int32_t y = 0; y < h; y++) {
        uint8_t * dest_row = dest_buf_u8;
        const uint8_t * src_row = src_buf_u8;
        int32_t byte_w = w * src_px_size;
        int32_t x = 0;
        for(; x < byte_w - 15; x += 16) {
            vst1q_u8(&dest_row[x], vld1q_u8(&src_row[x]));
        }
        for(; x < byte_w - 7; x += 8) {
            vst1_u8(&dest_row[x], vld1_u8(&src_row[x]));
        }
        for(; x < byte_w; x++) {
            dest_row[x] = src_row[x];
        }
        dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
        src_buf_u8  = drawbuf_next_row(src_buf_u8, src_stride);
    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                            uint32_t src_px_size)
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

    /* Fallback to sw implementation*/
    if(dest_px_size != src_px_size) {
        return LV_RESULT_INVALID;
    }

    if(dest_px_size == 3) {
        const uint8x16_t opa_16 = vdupq_n_u8(dsc->opa);
        const uint8x8_t opa_8 = vdup_n_u8(dsc->opa);
        uint8_t * dest_buf_8 = dsc->dest_buf;
        const uint8_t * src_buf_8 = dsc->src_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint8_t * src_row_ptr = src_buf_8;

            int32_t x = 0;
            for(; x < w - 15; x += 16) {
                uint8x16x3_t src = vld3q_u8(&src_row_ptr[x * 3]);
                uint8x16x3_t dst = vld3q_u8(&row_ptr[x * 3]);
                vst3q_u8(&row_ptr[x * 3], rgb_mix_8_internal(src, dst, opa_16));
            }
            for(; x < w - 7; x += 8) {
                uint8x8x3_t src = vld3_u8(&src_row_ptr[x * 3]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb_mix_4_internal(src, dst, opa_8));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1(&src_row_ptr[x * 3], &row_ptr[x * 3], opa);
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_8 = drawbuf_next_row(src_buf_8, src_stride);
        }

    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint32_t * src_buf_32 = dsc->src_buf;
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint32_t * src_row_ptr = src_buf_32;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                uint32x4_t src = vld1q_u32(&src_row_ptr[x]);
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                vst1q_u32(&row_ptr[x], lv_color_24_24_mix_4_internal(src, dst, opa_4));
            }
            for(; x < w - 1; x += 2) {
                uint32x2_t src = vld1_u32(&src_row_ptr[x]);
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                vst1_u32(&row_ptr[x], lv_color_24_24_mix_2_internal(src, dst, opa_2));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((uint8_t *)&src_row_ptr[x], (uint8_t *)&row_ptr[x], opa);
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            src_buf_32 = drawbuf_next_row(src_buf_32, src_stride);
        }
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                             uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->mask_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    /* Fallback to sw implementation*/
    if(dest_px_size != src_px_size) {
        return LV_RESULT_INVALID;
    }

    if(dest_px_size == 3) {
        uint8_t * dest_buf_8 = dsc->dest_buf;
        const uint8_t * src_buf_8 = dsc->src_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint8_t * src_row_ptr = src_buf_8;
            const uint8_t * mask_row_ptr = mask_buf_8;

            int32_t x = 0;
            for(; x < w - 15; x += 16) {
                const uint8x16_t mask_16 = vld1q_u8(&mask_row_ptr[x]);
                uint8x16x3_t src = vld3q_u8(&src_row_ptr[x * 3]);
                uint8x16x3_t dst = vld3q_u8(&row_ptr[x * 3]);
                vst3q_u8(&row_ptr[x * 3], rgb_mix_8_internal(src, dst, mask_16));
            }
            for(; x < w - 7; x += 8) {
                const uint8x8_t mask = vld1_u8(&mask_row_ptr[x]);
                uint8x8x3_t src = vld3_u8(&src_row_ptr[x * 3]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb_mix_4_internal(src, dst, mask));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1(&src_row_ptr[x * 3], &row_ptr[x * 3], mask_row_ptr[x]);
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_8 = drawbuf_next_row(src_buf_8, src_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }

    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint32_t * src_buf_32 = dsc->src_buf;

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint32_t * src_row_ptr = src_buf_32;
            const uint8_t * mask_row_ptr = mask_buf_8;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                uint32x4_t src = vld1q_u32(&src_row_ptr[x]);
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                vst1q_u32(&row_ptr[x], lv_color_24_24_mix_4_internal(src, dst, mask_4));
            }
            for(; x < w - 1; x += 2) {
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                uint32x2_t src = vld1_u32(&src_row_ptr[x]);
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                vst1_u32(&row_ptr[x], lv_color_24_24_mix_2_internal(src, dst, mask_2));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((uint8_t *)&src_row_ptr[x], (uint8_t *)&row_ptr[x], mask_row_ptr[x]);
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            src_buf_32 = drawbuf_next_row(src_buf_32, src_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size, uint32_t src_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(src_px_size == 3 || src_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const int32_t mask_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;
    const uint8_t * mask_buf_8 = dsc->mask_buf;

    /* Fallback to sw implementation*/
    if(dest_px_size != src_px_size) {
        return LV_RESULT_INVALID;
    }

    if(dest_px_size == 3) {
        const uint8x16_t opa_16 = vdupq_n_u8(dsc->opa);
        const uint8x8_t opa_8 = vdup_n_u8(dsc->opa);
        uint8_t * dest_buf_8 = dsc->dest_buf;
        const uint8_t * src_buf_8 = dsc->src_buf;
        for(int32_t y = 0; y < h; y++) {
            uint8_t * row_ptr = dest_buf_8;
            const uint8_t * src_row_ptr = src_buf_8;
            const uint8_t * mask_row_ptr = mask_buf_8;

            int32_t x = 0;
            for(; x < w - 15; x += 16) {
                const uint8x16_t mask_16 = vld1q_u8(&mask_row_ptr[x]);
                const uint8x8_t opa_lo = vget_low_u8(opa_16);
                const uint8x8_t opa_hi = vget_high_u8(opa_16);
                const uint8x8_t mask_lo = vget_low_u8(mask_16);
                const uint8x8_t mask_hi = vget_high_u8(mask_16);
                const uint16x8_t result_lo_wide = vmull_u8(opa_lo, mask_lo);
                const uint16x8_t result_hi_wide = vmull_u8(opa_hi, mask_hi);
                const uint8x8_t result_lo = vshrn_n_u16(result_lo_wide, 8);
                const uint8x8_t result_hi = vshrn_n_u16(result_hi_wide, 8);
                const uint8x16_t mix = vcombine_u8(result_lo, result_hi);
                uint8x16x3_t src = vld3q_u8(&src_row_ptr[x * 3]);
                uint8x16x3_t dst = vld3q_u8(&row_ptr[x * 3]);
                vst3q_u8(&row_ptr[x * 3], rgb_mix_8_internal(src, dst, mix));
            }
            for(; x < w - 7; x += 8) {
                const uint8x8_t mask = vld1_u8(&mask_row_ptr[x]);
                const uint16x8_t mult = vmull_u8(opa_8, mask);
                const uint8x8_t mix = vshrn_n_u16(mult, 8);
                uint8x8x3_t src = vld3_u8(&src_row_ptr[x * 3]);
                uint8x8x3_t dst = vld3_u8(&row_ptr[x * 3]);
                vst3_u8(&row_ptr[x * 3], rgb_mix_4_internal(src, dst, mix));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1(&src_row_ptr[x * 3], &row_ptr[x * 3], LV_OPA_MIX2(opa, mask_row_ptr[x]));
            }
            dest_buf_8 = drawbuf_next_row(dest_buf_8, dest_stride);
            src_buf_8 = drawbuf_next_row(src_buf_8, src_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }

    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_32 = dsc->dest_buf;
        const uint32_t * src_buf_32 = dsc->src_buf;
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);

        for(int32_t y = 0; y < h; y++) {
            uint32_t * row_ptr = dest_buf_32;
            const uint32_t * src_row_ptr = src_buf_32;
            const uint8_t * mask_row_ptr = mask_buf_8;
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                const uint32x4_t mix_4 = vshrq_n_u32(vmulq_u32(opa_4, mask_4), 8);
                uint32x4_t src = vld1q_u32(&src_row_ptr[x]);
                uint32x4_t dst = vld1q_u32(&row_ptr[x]);
                vst1q_u32(&row_ptr[x], lv_color_24_24_mix_4_internal(src, dst, mix_4));
            }
            for(; x < w - 1; x += 2) {
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                const uint32x2_t mix_2 = vshr_n_u32(vmul_u32(opa_2, mask_2), 8);
                uint32x2_t src = vld1_u32(&src_row_ptr[x]);
                uint32x2_t dst = vld1_u32(&row_ptr[x]);
                vst1_u32(&row_ptr[x], lv_color_24_24_mix_2_internal(src, dst, mix_2));
            }
            for(; x < w; x ++) {
                lv_color_24_24_mix_1((uint8_t *)&src_row_ptr[x], (uint8_t *)&row_ptr[x], LV_OPA_MIX2(opa, mask_row_ptr[x]));
            }
            dest_buf_32 = drawbuf_next_row(dest_buf_32, dest_stride);
            src_buf_32 = drawbuf_next_row(src_buf_32, src_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    int32_t dest_stride        = dsc->dest_stride;
    int32_t src_stride         = dsc->src_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_u32 = dsc->dest_buf;
        const uint32_t * src_buf_u32 = dsc->src_buf;
        for(int32_t y = 0; y < h; y++) {
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                vst1q_u32(&dest_buf_u32[x], argb_rgb_mix_4(&src_buf_u32[x], &dest_buf_u32[x]));
            }
            for(; x < w - 1; x += 2) {
                vst1_u32(&dest_buf_u32[x], argb_rgb_mix_2(&src_buf_u32[x], &dest_buf_u32[x]));
            }
            for(; x < w; x++) {
                lv_color_24_24_mix_1((uint8_t *)&src_buf_u32[x], (uint8_t *)&dest_buf_u32[x],
                                     ((uint8_t *)src_buf_u32)[x * sizeof(uint32_t) + 3]);
            }
            src_buf_u32 = drawbuf_next_row(src_buf_u32, src_stride);
            dest_buf_u32 = drawbuf_next_row(dest_buf_u32, dest_stride);
        }

    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t opa = dsc->opa;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_u32 = dsc->dest_buf;
        const uint32_t * src_buf_u32 = dsc->src_buf;
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);
        for(int32_t y = 0; y < h; y++) {
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                vst1q_u32(&dest_buf_u32[x], argb_rgb_mix_4_with_opa(&src_buf_u32[x], &dest_buf_u32[x], opa_4));
            }
            for(; x < w - 1; x += 2) {
                vst1_u32(&dest_buf_u32[x], argb_rgb_mix_2_with_opa(&src_buf_u32[x], &dest_buf_u32[x], opa_2));
            }
            for(; x < w; x++) {
                uint8_t alpha = ((uint8_t *)src_buf_u32)[x * sizeof(uint32_t) + 3];
                lv_color_24_24_mix_1((uint8_t *)&src_buf_u32[x], (uint8_t *)&dest_buf_u32[x],
                                     LV_OPA_MIX2(alpha, opa));
            }
            src_buf_u32 = drawbuf_next_row(src_buf_u32, src_stride);
            dest_buf_u32 = drawbuf_next_row(dest_buf_u32, dest_stride);
        }
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    const int32_t mask_stride = dsc->mask_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_u32 = dsc->dest_buf;
        const uint32_t * src_buf_u32 = dsc->src_buf;
        const uint8_t * mask_row_ptr = mask_buf_8;
        for(int32_t y = 0; y < h; y++) {
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                vst1q_u32(&dest_buf_u32[x], argb_rgb_mix_4_with_opa(&src_buf_u32[x], &dest_buf_u32[x], mask_4));
            }
            for(; x < w - 1; x += 2) {
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                vst1_u32(&dest_buf_u32[x], argb_rgb_mix_2_with_opa(&src_buf_u32[x], &dest_buf_u32[x], mask_2));
            }
            for(; x < w; x++) {
                uint8_t alpha = ((uint8_t *)src_buf_u32)[x * sizeof(uint32_t) + 3];
                lv_color_24_24_mix_1((uint8_t *)&src_buf_u32[x], (uint8_t *)&dest_buf_u32[x],
                                     LV_OPA_MIX2(alpha, mask_row_ptr[x]));
            }
            src_buf_u32 = drawbuf_next_row(src_buf_u32, src_stride);
            dest_buf_u32 = drawbuf_next_row(dest_buf_u32, dest_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT_NULL(dsc->mask_buf);
    const int32_t w = dsc->dest_w;
    const int32_t h = dsc->dest_h;
    const int32_t dest_stride = dsc->dest_stride;
    const int32_t src_stride = dsc->src_stride;
    const uint8_t * mask_buf_8 = dsc->mask_buf;
    const int32_t mask_stride = dsc->mask_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_u32 = dsc->dest_buf;
        const uint32_t * src_buf_u32 = dsc->src_buf;
        const uint8_t * mask_row_ptr = mask_buf_8;
        const uint32x4_t opa_4 = vdupq_n_u32((uint32_t)dsc->opa);
        const uint32x2_t opa_2 = vdup_n_u32((uint32_t)dsc->opa);
        for(int32_t y = 0; y < h; y++) {
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                const uint32x4_t mask_4 = {mask_row_ptr[x], mask_row_ptr[x + 1],  mask_row_ptr[x + 2], mask_row_ptr[x + 3]};
                vst1q_u32(&dest_buf_u32[x], argb_rgb_mix_4_with_opa_mask(&src_buf_u32[x], &dest_buf_u32[x], opa_4, mask_4));
            }
            for(; x < w - 1; x += 2) {
                const uint32x2_t mask_2 = {mask_row_ptr[x], mask_row_ptr[x + 1]};
                vst1_u32(&dest_buf_u32[x], argb_rgb_mix_2_with_opa_mask(&src_buf_u32[x], &dest_buf_u32[x], opa_2, mask_2));
            }
            for(; x < w; x++) {
                uint8_t alpha = ((uint8_t *)src_buf_u32)[x * sizeof(uint32_t) + 3];
                lv_color_24_24_mix_1((uint8_t *)&src_buf_u32[x], (uint8_t *)&dest_buf_u32[x],
                                     LV_OPA_MIX2(alpha, mask_row_ptr[x]));
            }
            src_buf_u32 = drawbuf_next_row(src_buf_u32, src_stride);
            dest_buf_u32 = drawbuf_next_row(dest_buf_u32, dest_stride);
            mask_buf_8 = drawbuf_next_row(mask_buf_8, mask_stride);
        }
    }
    return LV_RESULT_OK;
}

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
lv_result_t lv_draw_sw_blend_neon_argb888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    LV_ASSERT(dest_px_size == 3 || dest_px_size == 4);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    int32_t dest_stride        = dsc->dest_stride;
    int32_t src_stride         = dsc->src_stride;

    /* Fallback to sw implementation*/
    if(dest_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    else if(dest_px_size == 4) {
        uint32_t * dest_buf_u32 = dsc->dest_buf;
        const uint32_t * src_buf_u32 = dsc->src_buf;
        for(int32_t y = 0; y < h; y++) {
            int32_t x = 0;
            for(; x < w - 3; x += 4) {
                uint32x4_t src_pixels = vld1q_u32(&src_buf_u32[x]);
                uint32x4_t dst_pixels = vld1q_u32(&dest_buf_u32[x]);
                uint32x4_t alpha = vshrq_n_u32(src_pixels, 24);
                vst1q_u32(&dest_buf_u32[x], lv_color_24_24_mix_4_premul_internal(src_pixels, dst_pixels, alpha));
            }
            for(; x < w - 1; x += 2) {
                uint32x2_t src_pixels = vld1_u32(&src_buf_u32[x]);
                uint32x2_t dst_pixels = vld1_u32(&dest_buf_u32[x]);
                uint32x2_t alpha = vshr_n_u32(src_pixels, 24);
                vst1_u32(&dest_buf_u32[x], lv_color_24_24_mix_2_premul_internal(src_pixels, dst_pixels, alpha));
            }
            for(; x < w; x++) {
                lv_color_24_24_mix_premult((uint8_t *)&src_buf_u32[x], (uint8_t *)&dest_buf_u32[x],
                                           ((uint8_t *)src_buf_u32)[x * sizeof(uint32_t) + 3]);
            }
            src_buf_u32 = drawbuf_next_row(src_buf_u32, src_stride);
            dest_buf_u32 = drawbuf_next_row(dest_buf_u32, dest_stride);
        }

    }
    return LV_RESULT_OK;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/


static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}

static inline uint8x16x3_t lv_color_to_rgb888_16(const lv_color_t * color)
{
    const uint8x16_t r_vec = vdupq_n_u8(color->red);
    const uint8x16_t g_vec = vdupq_n_u8(color->green);
    const uint8x16_t b_vec = vdupq_n_u8(color->blue);
    uint8x16x3_t rgb_data;
    rgb_data.val[0] = b_vec;
    rgb_data.val[1] = g_vec;
    rgb_data.val[2] = r_vec;
    return rgb_data;
}

static inline uint8x8x3_t lv_color_to_rgb888_8(const lv_color_t * color)
{
    const uint8x8_t r_vec = vdup_n_u8(color->red);
    const uint8x8_t g_vec = vdup_n_u8(color->green);
    const uint8x8_t b_vec = vdup_n_u8(color->blue);
    uint8x8x3_t rgb_data;
    rgb_data.val[0] = b_vec;
    rgb_data.val[1] = g_vec;
    rgb_data.val[2] = r_vec;
    return rgb_data;
}

static inline uint8x16x4_t lv_color_to_xrgb888_16(const lv_color_t * color)
{
    const uint8x16_t r_vec = vdupq_n_u8(color->red);
    const uint8x16_t g_vec = vdupq_n_u8(color->green);
    const uint8x16_t b_vec = vdupq_n_u8(color->blue);
    const uint8x16_t a_vec = vdupq_n_u8(0xFF);
    uint8x16x4_t rgb_data;
    rgb_data.val[0] = b_vec;
    rgb_data.val[1] = g_vec;
    rgb_data.val[2] = r_vec;
    rgb_data.val[3] = a_vec;
    return rgb_data;
}

static inline uint8x8x4_t lv_color_to_xrgb888_8(const lv_color_t * color)
{
    const uint8x8_t r_vec = vdup_n_u8(color->red);
    const uint8x8_t g_vec = vdup_n_u8(color->green);
    const uint8x8_t b_vec = vdup_n_u8(color->blue);
    const uint8x8_t a_vec = vdup_n_u8(0xFF);
    uint8x8x4_t rgb_data;
    rgb_data.val[0] = b_vec;
    rgb_data.val[1] = g_vec;
    rgb_data.val[2] = r_vec;
    rgb_data.val[3] = a_vec;
    return rgb_data;
}

static inline uint8x16_t lv_color_to_xrgb888_4(const lv_color_t * color)
{
    uint32_t pixel_value = (0xFF << 24) | (color->red << 16) | (color->green << 8) | color->blue;
    return vreinterpretq_u8_u32(vdupq_n_u32(pixel_value));
}

static inline uint8x8_t lv_color_to_xrgb888_2(const lv_color_t * color)
{
    uint32_t pixel_value = (0xFF << 24) | (color->red << 16) | (color->green << 8) | color->blue;
    return vreinterpret_u8_u32(vdup_n_u32(pixel_value));
}

static inline uint8x8x3_t lv_rgb565_to_rgb888_8(const uint16_t * color)
{
    const uint16x8_t pixels = vld1q_u16(color);

    const uint16x8_t blue_mask = vdupq_n_u16(0x001F);
    const uint16x8_t green_mask = vdupq_n_u16(0x07E0);
    const uint16x8_t red_mask = vdupq_n_u16(0xF800);

    const uint16x8_t rb_multiplier = vdupq_n_u16(2106);
    const uint16x8_t g_multiplier = vdupq_n_u16(1037);

    const uint16x8_t r5 = vshrq_n_u16(vandq_u16(pixels, red_mask), 11);
    const uint16x8_t g6 = vshrq_n_u16(vandq_u16(pixels, green_mask), 5);
    const uint16x8_t b5 = vandq_u16(pixels, blue_mask);

    const uint8x8_t r8 = vmovn_u16(vshrq_n_u16(vmulq_u16(r5, rb_multiplier), 8));
    const uint8x8_t g8 = vmovn_u16(vshrq_n_u16(vmulq_u16(g6, g_multiplier), 8));
    const uint8x8_t b8 = vmovn_u16(vshrq_n_u16(vmulq_u16(b5, rb_multiplier), 8));

    uint8x8x3_t rgb;
    rgb.val[0] = b8;
    rgb.val[1] = g8;
    rgb.val[2] = r8;
    return rgb;
}
static inline uint8x8x4_t lv_rgb565_to_xrgb8888_8(const uint16_t * color)
{
    const uint16x8_t pixels = vld1q_u16(color);

    const uint16x8_t blue_mask = vdupq_n_u16(0x001F);
    const uint16x8_t green_mask = vdupq_n_u16(0x07E0);
    const uint16x8_t red_mask = vdupq_n_u16(0xF800);

    const uint16x8_t rb_multiplier = vdupq_n_u16(2106);
    const uint16x8_t g_multiplier = vdupq_n_u16(1037);

    const uint16x8_t r5 = vshrq_n_u16(vandq_u16(pixels, red_mask), 11);
    const uint16x8_t g6 = vshrq_n_u16(vandq_u16(pixels, green_mask), 5);
    const uint16x8_t b5 = vandq_u16(pixels, blue_mask);

    const uint8x8_t r8 = vmovn_u16(vshrq_n_u16(vmulq_u16(r5, rb_multiplier), 8));
    const uint8x8_t g8 = vmovn_u16(vshrq_n_u16(vmulq_u16(g6, g_multiplier), 8));
    const uint8x8_t b8 = vmovn_u16(vshrq_n_u16(vmulq_u16(b5, rb_multiplier), 8));

    uint8x8x4_t rgb;
    rgb.val[0] = b8;
    rgb.val[1] = g8;
    rgb.val[2] = r8;
    return rgb;
}

static inline void rgb565_mix_1(const uint16_t * src, uint8_t * dest, uint8_t mix)
{
    if(mix == 0) {
        return;
    }
    const uint32_t b = ((src[0] & 0x001F) * 2106) >> 8;
    const uint32_t g = (((src[0] & 0x07E0) >> 5) * 1037) >> 8;
    const uint32_t r = (((src[0] & 0xF800) >> 11) * 2106) >> 8;
    if(mix >= LV_OPA_MAX) {
        dest[0] = b;
        dest[1] = g;
        dest[2] = r;
        return;
    }
    const lv_opa_t mix_inv = 255 - mix;
    dest[0] = (uint32_t)(b * mix + dest[0] * mix_inv) >> 8;
    dest[1] = (uint32_t)(g * mix + dest[1] * mix_inv) >> 8;
    dest[2] = (uint32_t)(r * mix + dest[2] * mix_inv) >> 8;


}

static inline void lv_color_24_24_mix_1(const uint8_t * src, uint8_t * dest, uint8_t mix)
{
    if(mix == 0) {
        return;
    }

    if(mix >= LV_OPA_MAX) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        return;
    }
    const lv_opa_t mix_inv = 255 - mix;
    dest[0] = (uint32_t)((uint32_t)src[0] * mix + dest[0] * mix_inv) >> 8;
    dest[1] = (uint32_t)((uint32_t)src[1] * mix + dest[1] * mix_inv) >> 8;
    dest[2] = (uint32_t)((uint32_t)src[2] * mix + dest[2] * mix_inv) >> 8;
}

static inline void lv_color_24_24_mix_premult(const uint8_t * src, uint8_t * dest, uint8_t mix)
{
    if(mix == 0) return;

    if(mix >= LV_OPA_MAX) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
    }
    else {
        lv_opa_t mix_inv = 255 - mix;
        dest[0] = (uint32_t)src[0] + ((uint32_t)(dest[0] * mix_inv) >> 8);
        dest[1] = (uint32_t)src[1] + ((uint32_t)(dest[1] * mix_inv) >> 8);
        dest[2] = (uint32_t)src[2] + ((uint32_t)(dest[2] * mix_inv) >> 8);
    }
}


static inline uint32x4_t argb_rgb_mix_4(const uint32_t * src, const uint32_t * dst)
{
    uint32x4_t src_pixels = vld1q_u32(src);
    uint32x4_t dst_pixels = vld1q_u32(dst);

    uint32x4_t alpha = vshrq_n_u32(src_pixels, 24);
    return lv_color_24_24_mix_4_internal(src_pixels, dst_pixels, alpha);
}
static inline uint32x2_t argb_rgb_mix_2(const uint32_t * src, const uint32_t * dst)
{
    uint32x2_t src_pixels = vld1_u32(src);
    uint32x2_t dst_pixels = vld1_u32(dst);
    uint32x2_t alpha = vshr_n_u32(src_pixels, 24);
    return lv_color_24_24_mix_2_internal(src_pixels, dst_pixels, alpha);
}

static inline uint32x4_t argb_rgb_mix_4_with_opa(const uint32_t * src, const uint32_t * dst, uint32x4_t opa)
{
    uint32x4_t src_pixels = vld1q_u32(src);
    uint32x4_t dst_pixels = vld1q_u32(dst);

    uint32x4_t alpha = vshrq_n_u32(src_pixels, 24);
    uint32x4_t mix = vshrq_n_u32(vmulq_u32(alpha, opa), 8);
    return lv_color_24_24_mix_4_internal(src_pixels, dst_pixels, mix);
}
static inline uint32x2_t argb_rgb_mix_2_with_opa(const uint32_t * src, const uint32_t * dst, uint32x2_t opa)
{
    uint32x2_t src_pixels = vld1_u32(src);
    uint32x2_t dst_pixels = vld1_u32(dst);
    uint32x2_t alpha = vshr_n_u32(src_pixels, 24);
    uint32x2_t mix = vshr_n_u32(vmul_u32(alpha, opa), 8);
    return lv_color_24_24_mix_2_internal(src_pixels, dst_pixels, mix);
}
static inline uint32x4_t argb_rgb_mix_4_with_opa_mask(const uint32_t * src, const uint32_t * dst, uint32x4_t opa,
                                                      uint32x4_t mask)
{
    uint32x4_t src_pixels = vld1q_u32(src);
    uint32x4_t dst_pixels = vld1q_u32(dst);

    uint32x4_t alpha = vshrq_n_u32(src_pixels, 24);
    uint32x4_t mix = vshrq_n_u32(vmulq_u32(alpha, vmulq_u32(opa, mask)), 16);
    return lv_color_24_24_mix_4_internal(src_pixels, dst_pixels, mix);
}

static inline uint32x2_t argb_rgb_mix_2_with_opa_mask(const uint32_t * src, const uint32_t * dst, uint32x2_t opa,
                                                      uint32x2_t mask)
{
    uint32x2_t src_pixels = vld1_u32(src);
    uint32x2_t dst_pixels = vld1_u32(dst);
    uint32x2_t alpha = vshr_n_u32(src_pixels, 24);
    uint32x2_t mix = vshr_n_u32(vmul_u32(alpha, vmul_u32(opa, mask)), 16);
    return lv_color_24_24_mix_2_internal(src_pixels, dst_pixels, mix);
}

static inline uint8x8x3_t rgb565_rgb888_mix_8_internal(uint16x8_t src, uint8x8x3_t dst, uint8x8_t mix)
{
    uint8x8_t zero_mask = vceq_u8(mix, vdup_n_u8(0));
    uint8x8_t full_mask = vcge_u8(mix, vdup_n_u8(LV_OPA_MAX));
    uint8x8_t inv_mix = vsub_u8(vdup_n_u8(255), mix);

    uint8x8_t src_r = vqmovn_u16(vshrq_n_u16(vmulq_u16(vshrq_n_u16(vandq_u16(src, vdupq_n_u16(0xF800)), 11),
                                                       vdupq_n_u16(2106)), 8));
    uint8x8_t src_g = vqmovn_u16(vshrq_n_u16(vmulq_u16(vshrq_n_u16(vandq_u16(src, vdupq_n_u16(0x07E0)), 5),
                                                       vdupq_n_u16(1037)), 8));
    uint8x8_t src_b = vqmovn_u16(vshrq_n_u16(vmulq_u16(vandq_u16(src, vdupq_n_u16(0x001F)), vdupq_n_u16(2106)), 8));

    uint8x8_t dst_b = dst.val[0];
    uint8x8_t dst_g = dst.val[1];
    uint8x8_t dst_r = dst.val[2];

    uint16x8_t blend_r_wide = vaddq_u16(vmull_u8(src_r, mix), vmull_u8(dst_r, inv_mix));
    uint16x8_t blend_g_wide = vaddq_u16(vmull_u8(src_g, mix), vmull_u8(dst_g, inv_mix));
    uint16x8_t blend_b_wide = vaddq_u16(vmull_u8(src_b, mix), vmull_u8(dst_b, inv_mix));

    uint8x8_t blend_r = vshrn_n_u16(blend_r_wide, 8);
    uint8x8_t blend_g = vshrn_n_u16(blend_g_wide, 8);
    uint8x8_t blend_b = vshrn_n_u16(blend_b_wide, 8);

    uint8x8_t result_r = vbsl_u8(zero_mask, dst_r, blend_r);
    result_r = vbsl_u8(full_mask, src_r, result_r);

    uint8x8_t result_g = vbsl_u8(zero_mask, dst_g, blend_g);
    result_g = vbsl_u8(full_mask, src_g, result_g);

    uint8x8_t result_b = vbsl_u8(zero_mask, dst_b, blend_b);
    result_b = vbsl_u8(full_mask, src_b, result_b);

    uint8x8x3_t result = {{ result_b, result_g, result_r }};
    return result;
}

static inline uint8x16x3_t rgb_mix_8_internal(uint8x16x3_t src, uint8x16x3_t dst, uint8x16_t mix)
{
    uint8x16_t zero_mask = vceqq_u8(mix, vdupq_n_u8(0));
    uint8x16_t full_mask = vcgeq_u8(mix, vdupq_n_u8(LV_OPA_MAX));
    uint8x16_t inv_mix = vsubq_u8(vdupq_n_u8(255), mix);

    uint8x8_t mix_lo = vget_low_u8(mix);
    uint8x8_t mix_hi = vget_high_u8(mix);
    uint8x8_t inv_mix_lo = vget_low_u8(inv_mix);
    uint8x8_t inv_mix_hi = vget_high_u8(inv_mix);

    uint8x8_t src_r_lo = vget_low_u8(src.val[0]);
    uint8x8_t src_r_hi = vget_high_u8(src.val[0]);
    uint8x8_t dst_r_lo = vget_low_u8(dst.val[0]);
    uint8x8_t dst_r_hi = vget_high_u8(dst.val[0]);

    uint16x8_t blend_r_wide_lo = vaddq_u16(vmull_u8(src_r_lo, mix_lo), vmull_u8(dst_r_lo, inv_mix_lo));
    uint16x8_t blend_r_wide_hi = vaddq_u16(vmull_u8(src_r_hi, mix_hi), vmull_u8(dst_r_hi, inv_mix_hi));

    uint8x8_t blend_r_lo = vshrn_n_u16(blend_r_wide_lo, 8);
    uint8x8_t blend_r_hi = vshrn_n_u16(blend_r_wide_hi, 8);
    uint8x16_t blend_r = vcombine_u8(blend_r_lo, blend_r_hi);

    uint8x8_t src_g_lo = vget_low_u8(src.val[1]);
    uint8x8_t src_g_hi = vget_high_u8(src.val[1]);
    uint8x8_t dst_g_lo = vget_low_u8(dst.val[1]);
    uint8x8_t dst_g_hi = vget_high_u8(dst.val[1]);

    uint16x8_t blend_g_wide_lo = vaddq_u16(vmull_u8(src_g_lo, mix_lo), vmull_u8(dst_g_lo, inv_mix_lo));
    uint16x8_t blend_g_wide_hi = vaddq_u16(vmull_u8(src_g_hi, mix_hi), vmull_u8(dst_g_hi, inv_mix_hi));

    uint8x8_t blend_g_lo = vshrn_n_u16(blend_g_wide_lo, 8);
    uint8x8_t blend_g_hi = vshrn_n_u16(blend_g_wide_hi, 8);
    uint8x16_t blend_g = vcombine_u8(blend_g_lo, blend_g_hi);

    uint8x8_t src_b_lo = vget_low_u8(src.val[2]);
    uint8x8_t src_b_hi = vget_high_u8(src.val[2]);
    uint8x8_t dst_b_lo = vget_low_u8(dst.val[2]);
    uint8x8_t dst_b_hi = vget_high_u8(dst.val[2]);

    uint16x8_t blend_b_wide_lo = vaddq_u16(vmull_u8(src_b_lo, mix_lo), vmull_u8(dst_b_lo, inv_mix_lo));
    uint16x8_t blend_b_wide_hi = vaddq_u16(vmull_u8(src_b_hi, mix_hi), vmull_u8(dst_b_hi, inv_mix_hi));

    uint8x8_t blend_b_lo = vshrn_n_u16(blend_b_wide_lo, 8);
    uint8x8_t blend_b_hi = vshrn_n_u16(blend_b_wide_hi, 8);
    uint8x16_t blend_b = vcombine_u8(blend_b_lo, blend_b_hi);

    uint8x16_t result_r = vbslq_u8(zero_mask, dst.val[0], blend_r);
    result_r = vbslq_u8(full_mask, src.val[0], result_r);

    uint8x16_t result_g = vbslq_u8(zero_mask, dst.val[1], blend_g);
    result_g = vbslq_u8(full_mask, src.val[1], result_g);

    uint8x16_t result_b = vbslq_u8(zero_mask, dst.val[2], blend_b);
    result_b = vbslq_u8(full_mask, src.val[2], result_b);

    uint8x16x3_t result = {{result_r, result_g, result_b}};
    return result;
}

static inline uint8x8x3_t rgb_mix_4_internal(uint8x8x3_t src, uint8x8x3_t dst, uint8x8_t mix)
{
    uint8x8_t zero_mask = vceq_u8(mix, vdup_n_u8(0));
    uint8x8_t full_mask = vcge_u8(mix, vdup_n_u8(LV_OPA_MAX));
    uint8x8_t inv_mix = vsub_u8(vdup_n_u8(255), mix);

    uint16x8_t blend_r_wide = vaddq_u16(vmull_u8(src.val[0], mix), vmull_u8(dst.val[0], inv_mix));
    uint8x8_t blend_r = vshrn_n_u16(blend_r_wide, 8);

    uint16x8_t blend_g_wide = vaddq_u16(vmull_u8(src.val[1], mix), vmull_u8(dst.val[1], inv_mix));
    uint8x8_t blend_g = vshrn_n_u16(blend_g_wide, 8);

    uint16x8_t blend_b_wide = vaddq_u16(vmull_u8(src.val[2], mix), vmull_u8(dst.val[2], inv_mix));
    uint8x8_t blend_b = vshrn_n_u16(blend_b_wide, 8);

    uint8x8_t result_r = vbsl_u8(zero_mask, dst.val[0], blend_r);
    result_r = vbsl_u8(full_mask, src.val[0], result_r);

    uint8x8_t result_g = vbsl_u8(zero_mask, dst.val[1], blend_g);
    result_g = vbsl_u8(full_mask, src.val[1], result_g);

    uint8x8_t result_b = vbsl_u8(zero_mask, dst.val[2], blend_b);
    result_b = vbsl_u8(full_mask, src.val[2], result_b);

    uint8x8x3_t result = {{result_r, result_g, result_b}};
    return result;
}

static inline uint32x4_t lv_color_24_24_mix_4_internal(uint32x4_t src, uint32x4_t dst, uint32x4_t mix)
{
    uint32x4_t zero_mask = vceqq_u32(mix, vdupq_n_u32(0));
    uint32x4_t full_mask = vcgeq_u32(mix, vdupq_n_u32(LV_OPA_MAX));

    uint32x4_t src_r = vandq_u32(vshrq_n_u32(src, 16), vdupq_n_u32(0xFF));
    uint32x4_t src_g = vandq_u32(vshrq_n_u32(src, 8), vdupq_n_u32(0xFF));
    uint32x4_t src_b = vandq_u32(src, vdupq_n_u32(0xFF));

    uint32x4_t dst_r = vandq_u32(vshrq_n_u32(dst, 16), vdupq_n_u32(0xFF));
    uint32x4_t dst_g = vandq_u32(vshrq_n_u32(dst, 8), vdupq_n_u32(0xFF));
    uint32x4_t dst_b = vandq_u32(dst, vdupq_n_u32(0xFF));

    uint32x4_t inv_mix = vsubq_u32(vdupq_n_u32(255), mix);
    uint32x4_t blend_r = vshrq_n_u32(vaddq_u32(vmulq_u32(src_r, mix), vmulq_u32(dst_r, inv_mix)), 8);
    uint32x4_t blend_g = vshrq_n_u32(vaddq_u32(vmulq_u32(src_g, mix), vmulq_u32(dst_g, inv_mix)), 8);
    uint32x4_t blend_b = vshrq_n_u32(vaddq_u32(vmulq_u32(src_b, mix), vmulq_u32(dst_b, inv_mix)), 8);

    uint32x4_t blended = vorrq_u32(vorrq_u32(vshlq_n_u32(blend_r, 16), vshlq_n_u32(blend_g, 8)), blend_b);
    uint32x4_t result = vbslq_u32(zero_mask, dst, blended);
    return vbslq_u32(full_mask, src, result);
}

static inline uint32x2_t lv_color_24_24_mix_2_internal(uint32x2_t src, uint32x2_t dst, uint32x2_t mix)
{
    uint32x2_t zero_mask = vceq_u32(mix, vdup_n_u32(0));
    uint32x2_t full_mask = vcge_u32(mix, vdup_n_u32(LV_OPA_MAX));

    uint32x2_t src_r = vand_u32(vshr_n_u32(src, 16), vdup_n_u32(0xFF));
    uint32x2_t src_g = vand_u32(vshr_n_u32(src, 8), vdup_n_u32(0xFF));
    uint32x2_t src_b = vand_u32(src, vdup_n_u32(0xFF));

    uint32x2_t dst_r = vand_u32(vshr_n_u32(dst, 16), vdup_n_u32(0xFF));
    uint32x2_t dst_g = vand_u32(vshr_n_u32(dst, 8), vdup_n_u32(0xFF));
    uint32x2_t dst_b = vand_u32(dst, vdup_n_u32(0xFF));

    uint32x2_t inv_mix = vsub_u32(vdup_n_u32(255), mix);
    uint32x2_t blend_r = vshr_n_u32(vadd_u32(vmul_u32(src_r, mix), vmul_u32(dst_r, inv_mix)), 8);
    uint32x2_t blend_g = vshr_n_u32(vadd_u32(vmul_u32(src_g, mix), vmul_u32(dst_g, inv_mix)), 8);
    uint32x2_t blend_b = vshr_n_u32(vadd_u32(vmul_u32(src_b, mix), vmul_u32(dst_b, inv_mix)), 8);

    uint32x2_t blended = vorr_u32(vorr_u32(vshl_n_u32(blend_r, 16), vshl_n_u32(blend_g, 8)), blend_b);
    uint32x2_t result = vbsl_u32(zero_mask, dst, blended);
    return vbsl_u32(full_mask, src, result);
}
static inline uint32x4_t lv_color_24_24_mix_4_premul_internal(uint32x4_t src, uint32x4_t dst, uint32x4_t mix)
{
    uint32x4_t zero_mask = vceqq_u32(mix, vdupq_n_u32(0));
    uint32x4_t full_mask = vcgeq_u32(mix, vdupq_n_u32(LV_OPA_MAX));

    uint32x4_t src_r = vandq_u32(vshrq_n_u32(src, 16), vdupq_n_u32(0xFF));
    uint32x4_t src_g = vandq_u32(vshrq_n_u32(src, 8), vdupq_n_u32(0xFF));
    uint32x4_t src_b = vandq_u32(src, vdupq_n_u32(0xFF));

    uint32x4_t dst_r = vandq_u32(vshrq_n_u32(dst, 16), vdupq_n_u32(0xFF));
    uint32x4_t dst_g = vandq_u32(vshrq_n_u32(dst, 8), vdupq_n_u32(0xFF));
    uint32x4_t dst_b = vandq_u32(dst, vdupq_n_u32(0xFF));

    uint32x4_t inv_mix = vsubq_u32(vdupq_n_u32(255), mix);

    uint32x4_t blend_r = vaddq_u32(src_r, vshrq_n_u32(vmulq_u32(inv_mix, dst_r), 8));
    uint32x4_t blend_g = vaddq_u32(src_g, vshrq_n_u32(vmulq_u32(inv_mix, dst_g), 8));
    uint32x4_t blend_b = vaddq_u32(src_b, vshrq_n_u32(vmulq_u32(inv_mix, dst_b), 8));

    uint32x4_t blended = vorrq_u32(vorrq_u32(vshlq_n_u32(blend_r, 16), vshlq_n_u32(blend_g, 8)), blend_b);
    uint32x4_t result = vbslq_u32(zero_mask, dst, blended);
    return vbslq_u32(full_mask, src, result);
}

static inline uint32x2_t lv_color_24_24_mix_2_premul_internal(uint32x2_t src, uint32x2_t dst, uint32x2_t mix)
{
    uint32x2_t zero_mask = vceq_u32(mix, vdup_n_u32(0));
    uint32x2_t full_mask = vcge_u32(mix, vdup_n_u32(LV_OPA_MAX));

    uint32x2_t src_r = vand_u32(vshr_n_u32(src, 16), vdup_n_u32(0xFF));
    uint32x2_t src_g = vand_u32(vshr_n_u32(src, 8), vdup_n_u32(0xFF));
    uint32x2_t src_b = vand_u32(src, vdup_n_u32(0xFF));

    uint32x2_t dst_r = vand_u32(vshr_n_u32(dst, 16), vdup_n_u32(0xFF));
    uint32x2_t dst_g = vand_u32(vshr_n_u32(dst, 8), vdup_n_u32(0xFF));
    uint32x2_t dst_b = vand_u32(dst, vdup_n_u32(0xFF));

    uint32x2_t inv_mix = vsub_u32(vdup_n_u32(255), mix);

    uint32x2_t blend_r = vadd_u32(src_r, vshr_n_u32(vmul_u32(inv_mix, dst_r), 8));
    uint32x2_t blend_g = vadd_u32(src_g, vshr_n_u32(vmul_u32(inv_mix, dst_g), 8));
    uint32x2_t blend_b = vadd_u32(src_b, vshr_n_u32(vmul_u32(inv_mix, dst_b), 8));

    uint32x2_t blended = vorr_u32(vorr_u32(vshl_n_u32(blend_r, 16), vshl_n_u32(blend_g, 8)), blend_b);
    uint32x2_t result = vbsl_u32(zero_mask, dst, blended);
    return vbsl_u32(full_mask, src, result);
}

static inline uint32x4_t rgb565_xrgb_mix_4_internal(uint32x4_t src, uint32x4_t dst, uint32x4_t mix)
{
    uint32x4_t zero_mask = vceqq_u32(mix, vdupq_n_u32(0));
    uint32x4_t full_mask = vcgeq_u32(mix, vdupq_n_u32(LV_OPA_MAX));

    uint32x4_t src_r = vshrq_n_u32(vmulq_u32(vshrq_n_u32(vandq_u32(src, vdupq_n_u32(0xF800)), 11), vdupq_n_u32(2106)), 8);
    uint32x4_t src_g = vshrq_n_u32(vmulq_u32(vshrq_n_u32(vandq_u32(src, vdupq_n_u32(0x07E0)), 5), vdupq_n_u32(1037)), 8);
    uint32x4_t src_b = vshrq_n_u32(vmulq_u32(vandq_u32(src, vdupq_n_u32(0x001F)), vdupq_n_u32(2106)), 8);

    uint32x4_t dst_r = vandq_u32(vshrq_n_u32(dst, 16), vdupq_n_u32(0xFF));
    uint32x4_t dst_g = vandq_u32(vshrq_n_u32(dst, 8), vdupq_n_u32(0xFF));
    uint32x4_t dst_b = vandq_u32(dst, vdupq_n_u32(0xFF));

    uint32x4_t inv_mix = vsubq_u32(vdupq_n_u32(255), mix);
    uint32x4_t blend_r = vshrq_n_u32(vaddq_u32(vmulq_u32(src_r, mix), vmulq_u32(dst_r, inv_mix)), 8);
    uint32x4_t blend_g = vshrq_n_u32(vaddq_u32(vmulq_u32(src_g, mix), vmulq_u32(dst_g, inv_mix)), 8);
    uint32x4_t blend_b = vshrq_n_u32(vaddq_u32(vmulq_u32(src_b, mix), vmulq_u32(dst_b, inv_mix)), 8);

    uint32x4_t blended = vorrq_u32(vorrq_u32(vshlq_n_u32(blend_r, 16), vshlq_n_u32(blend_g, 8)), blend_b);
    uint32x4_t result = vbslq_u32(zero_mask, dst, blended);
    return vbslq_u32(full_mask, src, result);
}

static inline uint32x2_t rgb565_xrgb_mix_2_internal(uint32x2_t src, uint32x2_t dst, uint32x2_t mix)
{
    uint32x2_t zero_mask = vceq_u32(mix, vdup_n_u32(0));
    uint32x2_t full_mask = vcge_u32(mix, vdup_n_u32(LV_OPA_MAX));

    uint32x2_t src_r = vshr_n_u32(vmul_u32(vshr_n_u32(vand_u32(src, vdup_n_u32(0xF800)), 11), vdup_n_u32(2106)), 8);
    uint32x2_t src_g = vshr_n_u32(vmul_u32(vshr_n_u32(vand_u32(src, vdup_n_u32(0x07E0)), 5), vdup_n_u32(1037)), 8);
    uint32x2_t src_b = vshr_n_u32(vmul_u32(vand_u32(src, vdup_n_u32(0x001F)), vdup_n_u32(2106)), 8);

    uint32x2_t dst_r = vand_u32(vshr_n_u32(dst, 16), vdup_n_u32(0xFF));
    uint32x2_t dst_g = vand_u32(vshr_n_u32(dst, 8), vdup_n_u32(0xFF));
    uint32x2_t dst_b = vand_u32(dst, vdup_n_u32(0xFF));

    uint32x2_t inv_mix = vsub_u32(vdup_n_u32(255), mix);
    uint32x2_t blend_r = vshr_n_u32(vadd_u32(vmul_u32(src_r, mix), vmul_u32(dst_r, inv_mix)), 8);
    uint32x2_t blend_g = vshr_n_u32(vadd_u32(vmul_u32(src_g, mix), vmul_u32(dst_g, inv_mix)), 8);
    uint32x2_t blend_b = vshr_n_u32(vadd_u32(vmul_u32(src_b, mix), vmul_u32(dst_b, inv_mix)), 8);

    uint32x2_t blended = vorr_u32(vorr_u32(vshl_n_u32(blend_r, 16), vshl_n_u32(blend_g, 8)), blend_b);
    uint32x2_t result = vbsl_u32(zero_mask, dst, blended);
    return vbsl_u32(full_mask, src, result);
}
#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON*/
