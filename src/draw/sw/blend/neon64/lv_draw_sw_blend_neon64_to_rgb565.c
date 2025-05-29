/**
 * @file lv_draw_sw_blend_neon64_to_rgb565.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_neon64_to_rgb565.h"
#include <src/misc/lv_color.h>
#include <src/misc/lv_types.h>
#include <stdint.h>
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON64

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

static inline uint16x8_t l8_to_rgb565_8(const uint8_t * src);
static inline uint16x4_t l8_to_rgb565_4(const uint8_t * src);
static inline uint32_t l8_to_rgb565_2(const uint8_t * src);
static inline uint16_t l8_to_rgb565_1(const uint8_t * src);

static inline uint16x8_t lv_color_8_16_mix_8(const uint16_t * src, const uint16_t * dst);
static inline uint16x4_t lv_color_8_16_mix_4(const uint16_t * src, const uint16_t * dst);
static inline uint32_t lv_color_8_16_mix_2(const uint16_t * src, const uint16_t * dst);
static inline uint16_t lv_color_8_16_mix_1(const uint16_t * src, const uint16_t * dst);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_draw_sw_blend_neon64_color_to_rgb565(lv_draw_sw_blend_fill_dsc_t * dsc)
{

    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w            = dsc->dest_w;
    const int32_t h            = dsc->dest_h;
    const int32_t dest_stride  = dsc->dest_stride;
    const uint16_t color16     = lv_color_to_u16(dsc->color);
    const uint16x8_t color_vec = vdupq_n_u16(color16);
    uint16_t * dest_buf_u16    = dsc->dest_buf;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * row_ptr = dest_buf_u16;
        int32_t x          = 0;
        /* Handle unaligned pixels at the beginning */
        const size_t offset = ((size_t)row_ptr) & 0xF;
        if(offset != 0) {
            int32_t pixel_alignment = (16 - offset) >> 1;
            pixel_alignment         = (pixel_alignment > w) ? w : pixel_alignment;
            for(; x < pixel_alignment; x++) {
                row_ptr[x] = color16;
            }
        }

        /* Process 8 pixels at a time */
        for(; x < w - 7; x += 8) {
            vst1q_u16(&row_ptr[x], color_vec);
        }

        /* Handle remaining pixels */
        for(; x < w; x++) {
            row_ptr[x] = color16;
        }

        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
    }

    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon64_l8_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    int32_t dest_stride        = dsc->dest_stride;
    const uint8_t * src_buf_l8 = dsc->src_buf;
    int32_t src_stride         = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = src_buf_l8;
        int32_t x               = 0;
        for(; x < w - 7; x += 8) {

            vst1q_u16(&dest_row[x], l8_to_rgb565_8(&src_row[x]));
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&dest_row[x], l8_to_rgb565_4(&src_row[x]));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&dest_row[x] = l8_to_rgb565_2(&src_row[x]);
        }
        for(; x < w - 0; x += 1) {
            dest_row[x] = l8_to_rgb565_1(&src_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_l8 += src_stride;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon64_al88_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                          = dsc->dest_w;
    int32_t h                          = dsc->dest_h;
    uint16_t * dest_buf_u16            = dsc->dest_buf;
    int32_t dest_stride                = dsc->dest_stride;
    const lv_color16a_t * src_buf_al88 = dsc->src_buf;
    int32_t src_stride                 = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint16_t * src_row = (const uint16_t *)src_buf_al88;
        int32_t x                = 0;

        for(; x < w - 7; x += 8) {
            vst1q_u16(&dest_row[x], lv_color_8_16_mix_8(&src_row[x], &dest_row[x]));
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&dest_row[x], lv_color_8_16_mix_4(&src_row[x], &dest_row[x]));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&dest_row[x] = lv_color_8_16_mix_2(&src_row[x], &dest_row[x]);
        }
        for(; x < w - 0; x += 1) {
            dest_row[x] = lv_color_8_16_mix_1(&src_row[x], &dest_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
    }
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline uint16x8_t l8_to_rgb565_8(const uint8_t * src)
{
    uint16x8_t pixels = vmovl_u8(vld1_u8(src));
    uint16x8_t r      = vshlq_n_u16(vandq_u16(pixels, vdupq_n_u16(0xF8)), 8);
    uint16x8_t g      = vshlq_n_u16(vandq_u16(pixels, vdupq_n_u16(0xFC)), 3);
    uint16x8_t b      = vshrq_n_u16(vandq_u16(pixels, vdupq_n_u16(0xF8)), 3);
    return vorrq_u16(vorrq_u16(r, g), b);
}

static inline uint16x4_t l8_to_rgb565_4(const uint8_t * src)
{
    const uint8_t tmp[8]    = {src[0], src[1], src[2], src[3]};
    const uint16x8_t p      = vmovl_u8(vld1_u8(tmp));
    const uint16x4_t pixels = vget_low_u16(p);

    const uint16x4_t r = vshl_n_u16(vand_u16(pixels, vdup_n_u16(0xF8)), 8);
    const uint16x4_t g = vshl_n_u16(vand_u16(pixels, vdup_n_u16(0xFC)), 3);
    const uint16x4_t b = vshr_n_u16(vand_u16(pixels, vdup_n_u16(0xF8)), 3);
    return vorr_u16(vorr_u16(r, g), b);
}

static inline uint32_t l8_to_rgb565_2(const uint8_t * src)
{
    return ((uint32_t)l8_to_rgb565_1(src + 1) << 16) | l8_to_rgb565_1(src);
}
static inline uint16_t l8_to_rgb565_1(const uint8_t * src)
{
    return ((src[0] & 0xF8) << 8) + ((src[0] & 0xFC) << 3) + ((src[0] & 0xF8) >> 3);
}

static inline uint16x8_t lv_color_8_16_mix_8(const uint16_t * src, const uint16_t * dst)
{
    const uint16x8_t src_mix_pixels = vld1q_u16(src);
    const uint16x8_t dst_pixels     = vld1q_u16(dst);
    const uint16x8_t src_pixels     = vandq_u16(src_mix_pixels, vdupq_n_u16(0xFF));
    const uint16x8_t mix_pixels     = vshrq_n_u16(src_mix_pixels, 8);

    const uint16x8_t mix_zero_mask = vceqq_u16(mix_pixels, vdupq_n_u16(0));
    const uint16x8_t mix_full_mask = vceqq_u16(mix_pixels, vdupq_n_u16(255));
    /* Prepare result in case mix == 255 */
    const uint16x8_t src_r565   = vshlq_n_u16(vandq_u16(src_pixels, vdupq_n_u16(0xF8)), 8);
    const uint16x8_t src_g565   = vshlq_n_u16(vandq_u16(src_pixels, vdupq_n_u16(0xFC)), 3);
    const uint16x8_t src_b565   = vshrq_n_u16(vandq_u16(src_pixels, vdupq_n_u16(0xF8)), 3);
    const uint16x8_t src_rgb565 = vaddq_u16(vaddq_u16(src_r565, src_g565), src_b565);

    /* Do the actual blending */
    const uint16x8_t mix_inv_16 = vsubq_u16(vdupq_n_u16(255), mix_pixels);

    /* Red: ((c1 >> 3) * mix + ((c2 >> 11) & 0x1F) * mix_inv) << 3) & 0xF800 */
    const uint16x8_t src_r = vmulq_u16(vshrq_n_u16(src_pixels, 3), mix_pixels);
    const uint16x8_t dst_r = vandq_u16(vshrq_n_u16(dst_pixels, 11), vdupq_n_u16(0x1F));
    uint16x8_t blended_r   = vmlaq_u16(src_r, dst_r, mix_inv_16);
    blended_r              = vandq_u16(vshlq_n_u16(blended_r, 3), vdupq_n_u16(0xF800));

    /* Green: ((c1 >> 2) * mix + ((c2 >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0 */
    const uint16x8_t src_g = vmulq_u16(vshrq_n_u16(src_pixels, 2), mix_pixels);
    const uint16x8_t dst_g = vandq_u16(vshrq_n_u16(dst_pixels, 5), vdupq_n_u16(0x3F));
    uint16x8_t blended_g   = vmlaq_u16(src_g, dst_g, mix_inv_16);
    blended_g              = vandq_u16(vshrq_n_u16(blended_g, 3), vdupq_n_u16(0x07E0));

    /* Blue: ((c1 >> 3) * mix + (c2 & 0x1F) * mix_inv) >> 8 */
    const uint16x8_t src_b = vmulq_u16(vshrq_n_u16(src_pixels, 3), mix_pixels);
    const uint16x8_t dst_b = vandq_u16(dst_pixels, vdupq_n_u16(0x1F));
    uint16x8_t blended_b   = vmlaq_u16(src_b, dst_b, mix_inv_16);
    blended_b              = vshrq_n_u16(blended_b, 8);

    /* Select what value to take for each pixel depending on original mix value */
    const uint16x8_t blended_result = vorrq_u16(vorrq_u16(blended_r, blended_g), blended_b);
    const uint16x8_t result         = vbslq_u16(mix_zero_mask, dst_pixels, blended_result);
    return vbslq_u16(mix_full_mask, src_rgb565, result);
}

static inline uint16x4_t lv_color_8_16_mix_4(const uint16_t * src, const uint16_t * dst)
{
    const uint16x4_t src_mix_pixels = vld1_u16(src);
    const uint16x4_t dst_pixels     = vld1_u16(dst);
    const uint16x4_t src_pixels     = vand_u16(src_mix_pixels, vdup_n_u16(0xFF));
    const uint16x4_t mix_pixels     = vshr_n_u16(src_mix_pixels, 8);
    const uint16x4_t mix_zero_mask  = vceq_u16(mix_pixels, vdup_n_u16(0));
    const uint16x4_t mix_full_mask  = vceq_u16(mix_pixels, vdup_n_u16(255));

    /* Prepare result in case mix == 255 */
    const uint16x4_t src_r565   = vshl_n_u16(vand_u16(src_pixels, vdup_n_u16(0xF8)), 8);
    const uint16x4_t src_g565   = vshl_n_u16(vand_u16(src_pixels, vdup_n_u16(0xFC)), 3);
    const uint16x4_t src_b565   = vshr_n_u16(vand_u16(src_pixels, vdup_n_u16(0xF8)), 3);
    const uint16x4_t src_rgb565 = vadd_u16(vadd_u16(src_r565, src_g565), src_b565);
    const uint16x4_t mix_inv_16 = vsub_u16(vdup_n_u16(255), mix_pixels);

    /* Red: ((c1 >> 3) * mix + ((c2 >> 11) & 0x1F) * mix_inv) << 3) & 0xF800 */
    const uint16x4_t src_r = vmul_u16(vshr_n_u16(src_pixels, 3), mix_pixels);
    const uint16x4_t dst_r = vand_u16(vshr_n_u16(dst_pixels, 11), vdup_n_u16(0x1F));
    uint16x4_t blended_r   = vmla_u16(src_r, dst_r, mix_inv_16);
    blended_r              = vand_u16(vshl_n_u16(blended_r, 3), vdup_n_u16(0xF800));

    /* Green: ((c1 >> 2) * mix + ((c2 >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0 */
    const uint16x4_t src_g = vmul_u16(vshr_n_u16(src_pixels, 2), mix_pixels);
    const uint16x4_t dst_g = vand_u16(vshr_n_u16(dst_pixels, 5), vdup_n_u16(0x3F));
    uint16x4_t blended_g   = vmla_u16(src_g, dst_g, mix_inv_16);
    blended_g              = vand_u16(vshr_n_u16(blended_g, 3), vdup_n_u16(0x07E0));

    /* Blue: ((c1 >> 3) * mix + (c2 & 0x1F) * mix_inv) >> 8 */
    const uint16x4_t src_b = vmul_u16(vshr_n_u16(src_pixels, 3), mix_pixels);
    const uint16x4_t dst_b = vand_u16(dst_pixels, vdup_n_u16(0x1F));
    uint16x4_t blended_b   = vmla_u16(src_b, dst_b, mix_inv_16);
    blended_b              = vshr_n_u16(blended_b, 8);

    const uint16x4_t blended_result = vorr_u16(vorr_u16(blended_r, blended_g), blended_b);
    const uint16x4_t result         = vbsl_u16(mix_zero_mask, dst_pixels, blended_result);
    return vbsl_u16(mix_full_mask, src_rgb565, result);
}

static inline uint32_t lv_color_8_16_mix_2(const uint16_t * src, const uint16_t * dst)
{
    return ((uint32_t)lv_color_8_16_mix_1(src + 1, dst + 1) << 16) | lv_color_8_16_mix_1(src, dst);
}

static inline uint16_t lv_color_8_16_mix_1(const uint16_t * src, const uint16_t * dst)
{
    const uint8_t c1  = *src & 0xFF;
    const uint8_t mix = (*src & 0xFF00) >> 8;
    const uint16_t c2 = *dst;

    if(mix == 0) {
        return c2;
    } else if(mix == 255) {
        return ((c1 & 0xF8) << 8) + ((c1 & 0xFC) << 3) + ((c1 & 0xF8) >> 3);
    } else {
        uint8_t mix_inv = 255 - mix;
        return ((((c1 >> 3) * mix + ((c2 >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
               ((((c1 >> 2) * mix + ((c2 >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
               (((c1 >> 3) * mix + (c2 & 0x1F) * mix_inv) >> 8);
    }
}

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}
#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON64 */
