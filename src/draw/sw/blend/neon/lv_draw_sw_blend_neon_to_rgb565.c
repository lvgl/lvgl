/**
 * @file lv_draw_sw_blend_neon_to_rgb565.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_neon_to_rgb565.h"
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

static inline uint16x8_t l8_to_rgb565_8(const uint8_t * src);
static inline uint16x4_t l8_to_rgb565_4(const uint8_t * src);
static inline uint32_t l8_to_rgb565_2(const uint8_t * src);
static inline uint16_t l8_to_rgb565_1(const uint8_t * src);

static inline uint16x8_t lv_color_8_16_mix_8(const uint16_t * src, const uint16_t * dst);
static inline uint16x4_t lv_color_8_16_mix_4(const uint16_t * src, const uint16_t * dst);

static inline uint16x8_t lv_color_8_16_mix_8_with_opa(const uint16_t * src, const uint16_t * dst, uint8_t opa);
static inline uint16x4_t lv_color_8_16_mix_4_with_opa(const uint16_t * src, const uint16_t * dst, uint8_t opa);
static inline uint16x8_t lv_color_8_16_mix_8_with_mask(const uint16_t * src, const uint16_t * dst,
                                                       const uint8_t * mask);
static inline uint16x4_t lv_color_8_16_mix_4_with_mask(const uint16_t * src, const uint16_t * dst,
                                                       const uint8_t * mask);
static inline uint16x8_t lv_color_8_16_mix_8_with_opa_mask(const uint16_t * src, const uint16_t * dst, uint8_t opa,
                                                           const uint8_t * mask);
static inline uint16x4_t lv_color_8_16_mix_4_with_opa_mask(const uint16_t * src, const uint16_t * dst, uint8_t opa,
                                                           const uint8_t * mask);

static inline uint16x8_t lv_color_8_16_mix_8_internal(uint16x8_t dst_pixels, uint16x8_t src_pixels,
                                                      uint16x8_t mix_pixels);
static inline uint16x4_t lv_color_8_16_mix_4_internal(uint16x4_t dst_pixels, uint16x4_t src_pixels,
                                                      uint16x4_t mix_pixels);

static inline uint32_t lv_color_8_16_mix_2(const uint8_t * src, const uint16_t * dst);
static inline uint32_t lv_color_8_16_mix_2_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa);
static inline uint32_t lv_color_8_16_mix_2_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask);
static inline uint32_t lv_color_8_16_mix_2_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t,
                                                         const uint8_t * mask);

static inline uint16_t lv_color_8_16_mix_1_internal(uint8_t src, uint16_t dst, uint8_t mix);

static inline uint16x8_t lv_color_32_16_mix_8(const uint8_t * src, const uint16_t * dst);
static inline uint16x8_t lv_color_32_16_mix_8_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa);
static inline uint16x8_t lv_color_32_16_mix_8_with_mask(const uint8_t * src, const uint16_t * dst,
                                                        const uint8_t * mask);
static inline uint16x8_t lv_color_32_16_mix_8_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask);

static inline uint16x8_t lv_color_32_16_mix_8_internal(uint16x8_t g_pixels, uint16x8_t r_pixels, uint16x8_t b_pixels,
                                                       uint16x8_t a_pixels, uint16x8_t dst_pixels);

static inline uint16x4_t lv_color_32_16_mix_4(const uint8_t * src, const uint16_t * dst);

static inline uint16x4_t lv_color_32_16_mix_4_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa);
static inline uint16x4_t lv_color_32_16_mix_4_with_mask(const uint8_t * src, const uint16_t * dst,
                                                        const uint8_t * mask);
static inline uint16x4_t lv_color_32_16_mix_4_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask);
static inline uint16x4_t lv_color_32_16_mix_4_internal(uint16x4_t g_pixels, uint16x4_t r_pixels, uint16x4_t b_pixels,
                                                       uint16x4_t a_pixels, uint16x4_t dst_pixels);

static inline uint32_t lv_color_32_16_mix_2(const uint8_t * src, const uint16_t * dst);

static inline uint32_t lv_color_32_16_mix_2_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa);
static inline uint32_t lv_color_32_16_mix_2_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask);
static inline uint32_t lv_color_32_16_mix_2_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                          const uint8_t * mask);

static inline uint32_t lv_color_32_16_mix_1(const uint8_t * src, const uint16_t * dst);
static inline uint16_t lv_color_32_16_mix_1_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa);
static inline uint16_t lv_color_32_16_mix_1_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask);
static inline uint16_t lv_color_32_16_mix_1_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                          const uint8_t * mask);

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
    static inline uint16x8_t lv_color_24_16_mix_premult_8(const uint8_t * src, const uint16_t * dst);
    static inline uint16x4_t lv_color_24_16_mix_premult_4(const uint8_t * src, const uint16_t * dst);
    static inline uint32_t lv_color_24_16_mix_premult_2(const uint8_t * src, const uint16_t * dst);
    static inline uint16_t lv_color_24_16_mix_premult_1(const uint8_t * src, const uint16_t * dst);
#endif

static inline uint16x8_t lv_color_24_16_mix_8_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                       uint8_t src_px_size);
static inline uint16x4_t lv_color_24_16_mix_4_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                       uint8_t src_px_size);
static inline uint32_t lv_color_24_16_mix_2_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                     uint8_t src_px_size);

static inline uint16x8_t lv_color_24_16_mix_8_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask,
                                                        uint8_t src_px_size);
static inline uint16x4_t lv_color_24_16_mix_4_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask,
                                                        uint8_t src_px_size);
static inline uint32_t lv_color_24_16_mix_2_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask,
                                                      uint8_t src_px_size);

static inline uint16x8_t lv_color_24_16_mix_8_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask, uint8_t src_px_size);
static inline uint16x4_t lv_color_24_16_mix_4_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask, uint8_t src_px_size);
static inline uint32_t lv_color_24_16_mix_2_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                          const uint8_t * mask, uint8_t src_px_size);

static inline uint16_t lv_color_24_16_mix_1_internal(const uint8_t * src, const uint16_t * dst, uint8_t mix);
static inline uint16x8_t rgb888_to_rgb565_8(const uint8_t * src, uint8_t src_px_size);
static inline uint16x4_t rgb888_to_rgb565_4(const uint8_t * src, uint8_t src_px_size);
static inline uint32_t rgb888_to_rgb565_2(const uint8_t * src, uint8_t src_px_size);
static inline uint16_t rgb888_to_rgb565_1(const uint8_t * src);

static inline uint16x8_t lv_color_16_16_mix_8_with_opa(const uint16_t * c1, const uint16_t * c2, uint8_t opa);
static inline uint16x8_t lv_color_16_16_mix_8_with_mask(const uint16_t * c1, const uint16_t * c2, const uint8_t * mask);
static inline uint16x8_t lv_color_16_16_mix_8_with_opa_mask(const uint16_t * c1, const uint16_t * c2, uint8_t opa,
                                                            const uint8_t * mask);
static inline uint16x8_t lv_color_16_16_mix_8_internal(uint16x8_t c1_vec, uint16x8_t c2_vec, uint16x8_t mix);

static inline uint16x4_t lv_color_16_16_mix_4_with_opa(const uint16_t * c1, const uint16_t * c2, uint8_t opa);
static inline uint16x4_t lv_color_16_16_mix_4_with_mask(const uint16_t * c1, const uint16_t * c2, const uint8_t * mask);
static inline uint16x4_t lv_color_16_16_mix_4_with_opa_mask(const uint16_t * c1, const uint16_t * c2, uint8_t opa,
                                                            const uint8_t * mask);
static inline uint16x4_t lv_color_16_16_mix_4_internal(uint16x4_t c1_vec, uint16x4_t c2_vec, uint16x4_t mix);

static inline uint32_t lv_color_16_16_mix_2_with_opa(const uint16_t * c1, const uint16_t * c2, uint8_t opa);
static inline uint32_t lv_color_16_16_mix_2_with_mask(const uint16_t * c1, const uint16_t * c2, const uint8_t * mask);
static inline uint32_t lv_color_16_16_mix_2_with_opa_mask(const uint16_t * c1, const uint16_t * c2, uint8_t opa,
                                                          const uint8_t * mask);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_draw_sw_blend_neon_color_to_rgb565(lv_draw_sw_blend_fill_dsc_t * dsc)
{

    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t color16       = lv_color_to_u16(dsc->color);
    const uint16x8_t color_vec_8 = vdupq_n_u16(color16);
    const uint16x4_t color_vec_4 = vdup_n_u16(color16);
    const uint32_t color_vec_2   = color16 << 16 | color16;
    uint16_t * dest_buf_u16      = dsc->dest_buf;

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
        for(; x < w - 7; x += 8) {
            vst1q_u16(&row_ptr[x], color_vec_8);
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&row_ptr[x], color_vec_4);
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&row_ptr[x] = color_vec_2;
        }
        for(; x < w; x++) {
            row_ptr[x] = color16;
        }

        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_color_to_rgb565_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc)
{

    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t color16       = lv_color_to_u16(dsc->color);
    const uint32_t color32       = color16 << 16 | color16;
    const uint16x8_t color_vec_8 = vdupq_n_u16(color16);
    const uint16x4_t color_vec_4 = vdup_n_u16(color16);
    const uint8_t opa            = dsc->opa;
    const uint16x8_t opa_vec_8   = vmovq_n_u16(opa);
    const uint16x4_t opa_vec_4   = vmov_n_u16(opa);
    uint16_t * dest_buf_u16      = dsc->dest_buf;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * row_ptr = dest_buf_u16;
        int32_t x          = 0;
        /* Handle unaligned pixels at the beginning */
        const size_t offset = ((size_t)row_ptr) & 0xF;
        if(offset != 0) {
            int32_t pixel_alignment = (16 - offset) >> 1;
            pixel_alignment         = (pixel_alignment > w) ? w : pixel_alignment;
            for(; x < pixel_alignment; x++) {
                row_ptr[x] = lv_color_16_16_mix(color16, row_ptr[x], opa);
            }
        }
        for(; x < w - 7; x += 8) {
            const uint16x8_t src = vld1q_u16(&row_ptr[x]);
            vst1q_u16(&row_ptr[x], lv_color_16_16_mix_8_internal(color_vec_8, src, opa_vec_8));
        }
        for(; x < w - 3; x += 4) {
            const uint16x4_t src = vld1_u16(&row_ptr[x]);
            vst1_u16(&row_ptr[x], lv_color_16_16_mix_4_internal(color_vec_4, src, opa_vec_4));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&row_ptr[x] = lv_color_16_16_mix_2_with_opa((const uint16_t *)&color32, &row_ptr[x], opa);
        }
        for(; x < w; x++) {
            row_ptr[x] = lv_color_16_16_mix(color16, row_ptr[x], opa);
        }

        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_color_to_rgb565_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t color16       = lv_color_to_u16(dsc->color);
    const uint32_t color32       = color16 << 16 | color16;
    const uint16x8_t color_vec_8 = vdupq_n_u16(color16);
    const uint16x4_t color_vec_4 = vdup_n_u16(color16);
    uint16_t * dest_buf_u16      = dsc->dest_buf;
    const uint8_t * mask_buf_u8  = dsc->mask_buf;
    const int32_t mask_stride    = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * row_ptr       = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        int32_t x                = 0;
        /* Handle unaligned pixels at the beginning */
        const size_t offset = ((size_t)row_ptr) & 0xF;
        if(offset != 0) {
            int32_t pixel_alignment = (16 - offset) >> 1;
            pixel_alignment         = (pixel_alignment > w) ? w : pixel_alignment;
            for(; x < pixel_alignment; x++) {
                row_ptr[x] = lv_color_16_16_mix(color16, row_ptr[x], mask_row[x]);
            }
        }
        for(; x < w - 7; x += 8) {
            const uint16x8_t src  = vld1q_u16(&row_ptr[x]);
            const uint16x8_t mask = vmovl_u8(vld1_u8(&mask_row[x]));
            vst1q_u16(&row_ptr[x], lv_color_16_16_mix_8_internal(color_vec_8, src, mask));
        }
        for(; x < w - 3; x += 4) {
            const uint16x4_t src      = vld1_u16(&row_ptr[x]);
            const uint16x4_t mask_vec = vget_low_u16(vmovl_u8(vld1_u8(&mask_row[x])));
            vst1_u16(&row_ptr[x], lv_color_16_16_mix_4_internal(color_vec_4, src, mask_vec));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&row_ptr[x] =
                lv_color_16_16_mix_2_with_mask((const uint16_t *)&color32, &row_ptr[x], &mask_row[x]);
        }
        for(; x < w; x++) {
            row_ptr[x] = lv_color_16_16_mix(color16, row_ptr[x], mask_row[x]);
        }

        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        mask_buf_u8 += mask_stride;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_color_to_rgb565_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t color16       = lv_color_to_u16(dsc->color);
    const uint32_t color32       = color16 << 16 | color16;
    const uint16x8_t color_vec_8 = vdupq_n_u16(color16);
    const uint16x4_t color_vec_4 = vdup_n_u16(color16);
    const uint8_t opa            = dsc->opa;
    const uint16x8_t opa_vec_8   = vmovq_n_u16(opa);
    const uint16x4_t opa_vec_4   = vmov_n_u16(opa);

    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * row_ptr       = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        int32_t x                = 0;
        /* Handle unaligned pixels at the beginning */
        const size_t offset = ((size_t)row_ptr) & 0xF;
        if(offset != 0) {
            int32_t pixel_alignment = (16 - offset) >> 1;
            pixel_alignment         = (pixel_alignment > w) ? w : pixel_alignment;
            for(; x < pixel_alignment; x++) {
                row_ptr[x] = lv_color_16_16_mix(color16, row_ptr[x], LV_OPA_MIX2(opa, mask_row[x]));
            }
        }
        for(; x < w - 7; x += 8) {
            const uint16x8_t src = vld1q_u16(&row_ptr[x]);
            const uint16x8_t mix = vshrq_n_u16(vmulq_u16(opa_vec_8, vmovl_u8(vld1_u8(&mask_row[x]))), 8);
            vst1q_u16(&row_ptr[x], lv_color_16_16_mix_8_internal(color_vec_8, src, mix));
        }
        for(; x < w - 3; x += 4) {
            const uint16x4_t src = vld1_u16(&row_ptr[x]);
            const uint16x4_t mix = vshr_n_u16(vmul_u16(opa_vec_4, vget_low_u16(vmovl_u8(vld1_u8(&mask_row[x])))), 8);
            vst1_u16(&row_ptr[x], lv_color_16_16_mix_4_internal(color_vec_4, src, mix));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&row_ptr[x] =
                lv_color_16_16_mix_2_with_opa_mask((const uint16_t *)&color32, &row_ptr[x], opa, &mask_row[x]);
        }
        for(; x < w; x++) {
            row_ptr[x] = lv_color_16_16_mix(color16, row_ptr[x], LV_OPA_MIX2(opa, mask_row[x]));
        }

        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        mask_buf_u8 += mask_stride;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_l8_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
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

lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    int32_t dest_stride        = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride         = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;

        for(; x < w - 7; x += 8, src_x += 16) {
            vst1q_u16(&dest_row[x], lv_color_8_16_mix_8((const uint16_t *)&src_row[src_x], &dest_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 8) {
            vst1_u16(&dest_row[x], lv_color_8_16_mix_4((const uint16_t *)&src_row[src_x], &dest_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 4) {
            *(uint32_t *)&dest_row[x] = lv_color_8_16_mix_2(&src_row[src_x], &dest_row[x]);
        }
        for(; x < w - 0; x += 1, src_x += 2) {
            dest_row[x] = lv_color_8_16_mix_1_internal(src_row[src_x], dest_row[x], src_row[src_x + 1]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w            = dsc->dest_w;
    const int32_t h            = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    const int32_t dest_stride  = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    const int32_t src_stride   = dsc->src_stride;
    const uint8_t opa          = dsc->opa;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;

        for(; x < w - 7; x += 8, src_x += 16) {
            vst1q_u16(&dest_row[x], lv_color_8_16_mix_8_with_opa((const uint16_t *)&src_row[src_x], &dest_row[x], opa));
        }
        for(; x < w - 3; x += 4, src_x += 8) {
            vst1_u16(&dest_row[x], lv_color_8_16_mix_4_with_opa((const uint16_t *)&src_row[src_x], &dest_row[x], opa));
        }
        for(; x < w - 1; x += 2, src_x += 4) {
            *(uint32_t *)&dest_row[x] = lv_color_8_16_mix_2_with_opa(&src_row[src_x], &dest_row[x], opa);
        }
        for(; x < w - 0; x += 1, src_x += 2) {
            dest_row[x] =
                lv_color_8_16_mix_1_internal(src_row[src_x], dest_row[x], LV_OPA_MIX2(src_row[src_x + 1], opa));
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const int32_t dest_stride   = dsc->dest_stride;
    const uint8_t * src_buf_u8  = dsc->src_buf;
    const int32_t src_stride    = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * src_row  = (const uint8_t *)src_buf_u8;
        const uint8_t * mask_row = (const uint8_t *)mask_buf_u8;
        int32_t x                = 0;
        int32_t src_x            = 0;

        for(; x < w - 7; x += 8, src_x += 16) {
            vst1q_u16(&dest_row[x],
                      lv_color_8_16_mix_8_with_mask((const uint16_t *)&src_row[src_x], &dest_row[x], &mask_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 8) {
            vst1_u16(&dest_row[x],
                     lv_color_8_16_mix_4_with_mask((const uint16_t *)&src_row[src_x], &dest_row[x], &mask_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 4) {
            *(uint32_t *)&dest_row[x] = lv_color_8_16_mix_2_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x]);
        }
        for(; x < w - 0; x += 1, src_x += 2) {
            dest_row[x] =
                lv_color_8_16_mix_1_internal(src_row[src_x], dest_row[x], LV_OPA_MIX2(src_row[src_x + 1], mask_row[x]));
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
        mask_buf_u8 += mask_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const int32_t dest_stride   = dsc->dest_stride;
    const uint8_t * src_buf_u8  = dsc->src_buf;
    const int32_t src_stride    = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;
    const uint8_t opa           = dsc->opa;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * src_row  = (const uint8_t *)src_buf_u8;
        const uint8_t * mask_row = (const uint8_t *)mask_buf_u8;
        int32_t x                = 0;
        int32_t src_x            = 0;

        for(; x < w - 7; x += 8, src_x += 16) {
            vst1q_u16(&dest_row[x], lv_color_8_16_mix_8_with_opa_mask((const uint16_t *)&src_row[src_x], &dest_row[x],
                                                                      opa, &mask_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 8) {
            vst1_u16(&dest_row[x], lv_color_8_16_mix_4_with_opa_mask((const uint16_t *)&src_row[src_x], &dest_row[x],
                                                                     opa, &mask_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 4) {
            *(uint32_t *)&dest_row[x] =
                lv_color_8_16_mix_2_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x]);
        }
        for(; x < w - 0; x += 1, src_x += 2) {
            dest_row[x] = lv_color_8_16_mix_1_internal(src_row[src_x], dest_row[x],
                                                       LV_OPA_MIX3(src_row[src_x + 1], mask_row[x], opa));
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
        mask_buf_u8 += mask_stride;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{

    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                    = dsc->dest_w;
    int32_t h                    = dsc->dest_h;
    uint16_t * dest_buf_u16      = dsc->dest_buf;
    int32_t dest_stride          = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    int32_t src_stride           = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint16_t * src_row = (const uint16_t *)src_buf_u16;
        int32_t x                = 0;

        for(; x < w - 7; x += 8) {
            vst1q_u16(&dest_row[x], vld1q_u16(&src_row[x]));
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&dest_row[x], vld1_u16(&src_row[x]));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&dest_row[x] = *(uint32_t *)&src_row[x];
        }
        for(; x < w - 0; x += 1) {
            dest_row[x] = src_row[x];
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u16  = drawbuf_next_row(src_buf_u16, src_stride);
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc)
{

    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint16_t * dest_buf_u16      = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t opa            = dsc->opa;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint16_t * src_row = (const uint16_t *)src_buf_u16;
        int32_t x                = 0;

        for(; x < w - 7; x += 8) {
            vst1q_u16(&dest_row[x], lv_color_16_16_mix_8_with_opa(&src_row[x], &dest_row[x], opa));
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&dest_row[x], lv_color_16_16_mix_4_with_opa(&src_row[x], &dest_row[x], opa));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&dest_row[x] = lv_color_16_16_mix_2_with_opa(&src_row[x], &dest_row[x], opa);
        }
        for(; x < w - 0; x += 1) {
            dest_row[x] = lv_color_16_16_mix(src_row[x], dest_row[x], opa);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u16  = drawbuf_next_row(src_buf_u16, src_stride);
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint16_t * dest_buf_u16      = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t * mask_buf_u8  = dsc->mask_buf;
    const int32_t mask_stride    = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        const uint16_t * src_row = (const uint16_t *)src_buf_u16;
        int32_t x                = 0;

        for(; x < w - 7; x += 8) {
            vst1q_u16(&dest_row[x], lv_color_16_16_mix_8_with_mask(&src_row[x], &dest_row[x], &mask_row[x]));
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&dest_row[x], lv_color_16_16_mix_4_with_mask(&src_row[x], &dest_row[x], &mask_row[x]));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&dest_row[x] = lv_color_16_16_mix_2_with_mask(&src_row[x], &dest_row[x], &mask_row[x]);
        }
        for(; x < w - 0; x += 1) {
            dest_row[x] = lv_color_16_16_mix(src_row[x], dest_row[x], mask_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u16  = drawbuf_next_row(src_buf_u16, src_stride);
        mask_buf_u8 += mask_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w              = dsc->dest_w;
    const int32_t h              = dsc->dest_h;
    uint16_t * dest_buf_u16      = dsc->dest_buf;
    const int32_t dest_stride    = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    const int32_t src_stride     = dsc->src_stride;
    const uint8_t * mask_buf_u8  = dsc->mask_buf;
    const uint8_t opa            = dsc->opa;
    const int32_t mask_stride    = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        const uint16_t * src_row = (const uint16_t *)src_buf_u16;
        int32_t x                = 0;

        for(; x < w - 7; x += 8) {
            vst1q_u16(&dest_row[x], lv_color_16_16_mix_8_with_opa_mask(&src_row[x], &dest_row[x], opa, &mask_row[x]));
        }
        for(; x < w - 3; x += 4) {
            vst1_u16(&dest_row[x], lv_color_16_16_mix_4_with_opa_mask(&src_row[x], &dest_row[x], opa, &mask_row[x]));
        }
        for(; x < w - 1; x += 2) {
            *(uint32_t *)&dest_row[x] =
                lv_color_16_16_mix_2_with_opa_mask(&src_row[x], &dest_row[x], opa, &mask_row[x]);
        }
        for(; x < w - 0; x += 1) {
            dest_row[x] = lv_color_16_16_mix(src_row[x], dest_row[x], LV_OPA_MIX2(mask_row[x], opa));
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u16  = drawbuf_next_row(src_buf_u16, src_stride);
        mask_buf_u8 += mask_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc, uint8_t src_px_size)
{

    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    int32_t dest_stride        = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride         = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;
        for(; x < w - 7; x += 8, src_x += src_px_size * 8) {
            vst1q_u16(&dest_row[x], rgb888_to_rgb565_8(&src_row[src_x], src_px_size));
        }
        for(; x < w - 3; x += 4, src_x += src_px_size * 4) {
            vst1_u16(&dest_row[x], rgb888_to_rgb565_4(&src_row[src_x], src_px_size));
        }
        for(; x < w - 1; x += 2, src_x += src_px_size * 2) {
            *(uint32_t *)&dest_row[x] = rgb888_to_rgb565_2(&src_row[src_x], src_px_size);
        }
        for(; x < w - 0; x += 1, src_x += src_px_size * 1) {
            dest_row[x] = rgb888_to_rgb565_1(&src_row[src_x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint8_t src_px_size)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w            = dsc->dest_w;
    const int32_t h            = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    const int32_t dest_stride  = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    const int32_t src_stride   = dsc->src_stride;
    const uint8_t opa          = dsc->opa;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;

        for(; x < w - 7; x += 8, src_x += src_px_size * 8) {
            vst1q_u16(&dest_row[x], lv_color_24_16_mix_8_with_opa(&src_row[src_x], &dest_row[x], opa, src_px_size));
        }
        for(; x < w - 3; x += 4, src_x += src_px_size * 4) {
            vst1_u16(&dest_row[x], lv_color_24_16_mix_4_with_opa(&src_row[src_x], &dest_row[x], opa, src_px_size));
        }
        for(; x < w - 1; x += 2, src_x += src_px_size * 2) {
            *(uint32_t *)&dest_row[x] = lv_color_24_16_mix_2_with_opa(&src_row[src_x], &dest_row[x], opa, src_px_size);
        }
        for(; x < w - 0; x += 1, src_x += src_px_size * 1) {
            dest_row[x] = lv_color_24_16_mix_1_internal(&src_row[src_x], &dest_row[x], opa);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint8_t src_px_size)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const int32_t dest_stride   = dsc->dest_stride;
    const uint8_t * src_buf_u8  = dsc->src_buf;
    const int32_t src_stride    = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        const uint8_t * src_row  = src_buf_u8;
        int32_t x                = 0;
        int32_t src_x            = 0;

        for(; x < w - 7; x += 8, src_x += src_px_size * 8) {
            vst1q_u16(&dest_row[x],
                      lv_color_24_16_mix_8_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x], src_px_size));
        }
        for(; x < w - 3; x += 4, src_x += src_px_size * 4) {
            vst1_u16(&dest_row[x],
                     lv_color_24_16_mix_4_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x], src_px_size));
        }
        for(; x < w - 1; x += 2, src_x += src_px_size * 2) {
            *(uint32_t *)&dest_row[x] =
                lv_color_24_16_mix_2_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x], src_px_size);
        }
        for(; x < w - 0; x += 1, src_x += src_px_size * 1) {
            dest_row[x] = lv_color_24_16_mix_1_internal(&src_row[src_x], &dest_row[x], mask_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
        mask_buf_u8 += mask_stride;
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint8_t src_px_size)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const int32_t dest_stride   = dsc->dest_stride;
    const uint8_t * src_buf_u8  = dsc->src_buf;
    const int32_t src_stride    = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;
    const uint8_t opa           = dsc->opa;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        const uint8_t * src_row  = src_buf_u8;
        int32_t x                = 0;
        int32_t src_x            = 0;

        for(; x < w - 7; x += 8, src_x += src_px_size * 8) {
            vst1q_u16(&dest_row[x], lv_color_24_16_mix_8_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x],
                                                                       src_px_size));
        }
        for(; x < w - 3; x += 4, src_x += src_px_size * 4) {
            vst1_u16(&dest_row[x],
                     lv_color_24_16_mix_4_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x], src_px_size));
        }
        for(; x < w - 1; x += 2, src_x += src_px_size * 2) {
            *(uint32_t *)&dest_row[x] =
                lv_color_24_16_mix_2_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x], src_px_size);
        }
        for(; x < w - 0; x += 1, src_x += src_px_size * 1) {
            dest_row[x] = lv_color_24_16_mix_1_internal(&src_row[src_x], &dest_row[x], LV_OPA_MIX2(mask_row[x], opa));
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
        mask_buf_u8 += mask_stride;
    }

    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    int32_t dest_stride        = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride         = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;

        for(; x < w - 7; x += 8, src_x += 32) {
            vst1q_u16(&dest_row[x], lv_color_32_16_mix_8(&src_row[src_x], &dest_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 16) {
            vst1_u16(&dest_row[x], lv_color_32_16_mix_4(&src_row[src_x], &dest_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 8) {
            *(uint32_t *)&dest_row[x] = lv_color_32_16_mix_2(&src_row[src_x], &dest_row[x]);
        }
        for(; x < w - 0; x += 1, src_x += 4) {
            dest_row[x] = lv_color_32_16_mix_1(&src_row[src_x], &dest_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    const int32_t w            = dsc->dest_w;
    const int32_t h            = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    const int32_t dest_stride  = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    const int32_t src_stride   = dsc->src_stride;
    const uint8_t opa          = dsc->opa;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;

        for(; x < w - 7; x += 8, src_x += 32) {
            vst1q_u16(&dest_row[x], lv_color_32_16_mix_8_with_opa(&src_row[src_x], &dest_row[x], opa));
        }
        for(; x < w - 3; x += 4, src_x += 16) {
            vst1_u16(&dest_row[x], lv_color_32_16_mix_4_with_opa(&src_row[src_x], &dest_row[x], opa));
        }
        for(; x < w - 1; x += 2, src_x += 8) {
            *(uint32_t *)&dest_row[x] = lv_color_32_16_mix_2_with_opa(&src_row[src_x], &dest_row[x], opa);
        }
        for(; x < w - 0; x += 1, src_x += 4) {
            dest_row[x] = lv_color_32_16_mix_1_with_opa(&src_row[src_x], &dest_row[x], opa);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const int32_t dest_stride   = dsc->dest_stride;
    const uint8_t * src_buf_u8  = dsc->src_buf;
    const int32_t src_stride    = dsc->src_stride;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        const uint8_t * src_row  = (const uint8_t *)src_buf_u8;
        int32_t x                = 0;
        int32_t src_x            = 0;

        for(; x < w - 7; x += 8, src_x += 32) {
            vst1q_u16(&dest_row[x], lv_color_32_16_mix_8_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 16) {
            vst1_u16(&dest_row[x], lv_color_32_16_mix_4_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 8) {
            *(uint32_t *)&dest_row[x] = lv_color_32_16_mix_2_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x]);
        }
        for(; x < w - 0; x += 1, src_x += 4) {
            dest_row[x] = lv_color_32_16_mix_1_with_mask(&src_row[src_x], &dest_row[x], &mask_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
        mask_buf_u8 += mask_stride;
    }
    return LV_RESULT_OK;
}
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa < LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf != NULL);
    const int32_t w             = dsc->dest_w;
    const int32_t h             = dsc->dest_h;
    uint16_t * dest_buf_u16     = dsc->dest_buf;
    const int32_t dest_stride   = dsc->dest_stride;
    const uint8_t * src_buf_u8  = dsc->src_buf;
    const int32_t src_stride    = dsc->src_stride;
    const uint8_t opa           = dsc->opa;
    const uint8_t * mask_buf_u8 = dsc->mask_buf;
    const int32_t mask_stride   = dsc->mask_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row      = dest_buf_u16;
        const uint8_t * mask_row = mask_buf_u8;
        const uint8_t * src_row  = (const uint8_t *)src_buf_u8;
        int32_t x                = 0;
        int32_t src_x            = 0;

        for(; x < w - 7; x += 8, src_x += 32) {
            vst1q_u16(&dest_row[x],
                      lv_color_32_16_mix_8_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 16) {
            vst1_u16(&dest_row[x],
                     lv_color_32_16_mix_4_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 8) {
            *(uint32_t *)&dest_row[x] =
                lv_color_32_16_mix_2_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x]);
        }

        for(; x < w - 0; x += 1, src_x += 4) {
            dest_row[x] = lv_color_32_16_mix_1_with_opa_mask(&src_row[src_x], &dest_row[x], opa, &mask_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
        mask_buf_u8 += mask_stride;
    }
    return LV_RESULT_OK;
}

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
lv_result_t lv_draw_sw_blend_neon_argb888_premultiplied_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{
    LV_ASSERT(dsc->opa >= LV_OPA_MAX);
    LV_ASSERT(dsc->mask_buf == NULL);
    int32_t w                  = dsc->dest_w;
    int32_t h                  = dsc->dest_h;
    uint16_t * dest_buf_u16    = dsc->dest_buf;
    int32_t dest_stride        = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride         = dsc->src_stride;

    for(int32_t y = 0; y < h; y++) {
        uint16_t * dest_row     = dest_buf_u16;
        const uint8_t * src_row = (const uint8_t *)src_buf_u8;
        int32_t x               = 0;
        int32_t src_x           = 0;

        for(; x < w - 7; x += 8, src_x += 32) {
            vst1q_u16(&dest_row[x], lv_color_24_16_mix_premult_8(&src_row[src_x], &dest_row[x]));
        }
        for(; x < w - 3; x += 4, src_x += 16) {
            vst1_u16(&dest_row[x], lv_color_24_16_mix_premult_4(&src_row[src_x], &dest_row[x]));
        }
        for(; x < w - 1; x += 2, src_x += 8) {
            *(uint32_t *)&dest_row[x] = lv_color_24_16_mix_premult_2(&src_row[src_x], &dest_row[x]);
        }
        for(; x < w - 0; x += 1, src_x += 4) {
            dest_row[x] = lv_color_24_16_mix_premult_1(&src_row[src_x], &dest_row[x]);
        }
        dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
        src_buf_u8 += src_stride;
    }
    return LV_RESULT_OK;
}
#endif

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

    return lv_color_8_16_mix_8_internal(src_pixels, dst_pixels, mix_pixels);
}

static inline uint16x8_t lv_color_8_16_mix_8_with_opa(const uint16_t * src, const uint16_t * dst, uint8_t opa)
{
    const uint16x8_t src_mix_pixels = vld1q_u16(src);
    const uint16x8_t dst_pixels     = vld1q_u16(dst);
    const uint16x8_t src_pixels     = vandq_u16(src_mix_pixels, vdupq_n_u16(0xFF));
    const uint16x8_t mix_pixels     = vshrq_n_u16(src_mix_pixels, 8);
    const uint16x8_t opa_vec        = vmovq_n_u16(opa);
    const uint16x8_t mix_vec        = vshrq_n_u16(vmulq_u16(opa_vec, mix_pixels), 8);
    return lv_color_8_16_mix_8_internal(src_pixels, dst_pixels, mix_vec);
}

static inline uint16x4_t lv_color_8_16_mix_4_with_opa(const uint16_t * src, const uint16_t * dst, uint8_t opa)
{
    const uint16x4_t src_mix_pixels = vld1_u16(src);
    const uint16x4_t dst_pixels     = vld1_u16(dst);
    const uint16x4_t src_pixels     = vand_u16(src_mix_pixels, vdup_n_u16(0xFF));
    const uint16x4_t mix_pixels     = vshr_n_u16(src_mix_pixels, 8);
    const uint16x4_t opa_vec        = vmov_n_u16(opa);
    const uint16x4_t mix_vec        = vshr_n_u16(vmul_u16(opa_vec, mix_pixels), 8);
    return lv_color_8_16_mix_4_internal(src_pixels, dst_pixels, mix_vec);
}
static inline uint16x8_t lv_color_8_16_mix_8_with_mask(const uint16_t * src, const uint16_t * dst, const uint8_t * mask)
{
    const uint16x8_t src_mix_pixels = vld1q_u16(src);
    const uint16x8_t dst_pixels     = vld1q_u16(dst);
    const uint16x8_t src_pixels     = vandq_u16(src_mix_pixels, vdupq_n_u16(0xFF));
    const uint16x8_t mix_pixels     = vshrq_n_u16(src_mix_pixels, 8);
    const uint16x8_t mask_vec       = vmovl_u8(vld1_u8(mask));
    const uint16x8_t mix_vec        = vshrq_n_u16(vmulq_u16(mask_vec, mix_pixels), 8);
    return lv_color_8_16_mix_8_internal(src_pixels, dst_pixels, mix_vec);
}

static inline uint16x4_t lv_color_8_16_mix_4_with_mask(const uint16_t * src, const uint16_t * dst, const uint8_t * mask)
{
    const uint16x4_t src_mix_pixels = vld1_u16(src);
    const uint16x4_t dst_pixels     = vld1_u16(dst);
    const uint16x4_t src_pixels     = vand_u16(src_mix_pixels, vdup_n_u16(0xFF));
    const uint16x4_t mix_pixels     = vshr_n_u16(src_mix_pixels, 8);
    const uint16x4_t mask_vec       = vget_low_u16(vmovl_u8(vld1_u8(mask)));
    const uint16x4_t mix_vec        = vshr_n_u16(vmul_u16(mask_vec, mix_pixels), 8);
    return lv_color_8_16_mix_4_internal(src_pixels, dst_pixels, mix_vec);
}
static inline uint16x8_t lv_color_8_16_mix_8_with_opa_mask(const uint16_t * src, const uint16_t * dst, uint8_t opa,
                                                           const uint8_t * mask)
{
    const uint16x8_t src_mix_pixels = vld1q_u16(src);
    const uint16x8_t dst_pixels     = vld1q_u16(dst);
    const uint16x8_t src_pixels     = vandq_u16(src_mix_pixels, vdupq_n_u16(0xFF));
    const uint16x8_t mix_pixels     = vshrq_n_u16(src_mix_pixels, 8);
    const uint16x8_t mask_vec       = vmovl_u8(vld1_u8(mask));
    const uint16x8_t opa_vec        = vmovq_n_u16(opa);

    /* Use uint32 for intermediate multiplication results to avoid 16bit overflows */
    const uint32x4_t mix_pixels_low  = vmovl_u16(vget_low_u16(mix_pixels));
    const uint32x4_t mix_pixels_high = vmovl_u16(vget_high_u16(mix_pixels));
    const uint32x4_t opa_vec_low     = vmovl_u16(vget_low_u16(opa_vec));
    const uint32x4_t opa_vec_high    = vmovl_u16(vget_high_u16(opa_vec));
    const uint32x4_t mask_vec_low    = vmovl_u16(vget_low_u16(mask_vec));
    const uint32x4_t mask_vec_high   = vmovl_u16(vget_high_u16(mask_vec));

    const uint32x4_t mul_low  = vshrq_n_u32(vmulq_u32(vmulq_u32(mix_pixels_low, opa_vec_low), mask_vec_low), 16);
    const uint32x4_t mul_high = vshrq_n_u32(vmulq_u32(vmulq_u32(mix_pixels_high, opa_vec_high), mask_vec_high), 16);
    const uint16x8_t mix_vec  = vcombine_u16(vmovn_u32(mul_low), vmovn_u32(mul_high));
    return lv_color_8_16_mix_8_internal(src_pixels, dst_pixels, mix_vec);
}

static inline uint16x4_t lv_color_8_16_mix_4_with_opa_mask(const uint16_t * src, const uint16_t * dst, uint8_t opa,
                                                           const uint8_t * mask)
{

    const uint16x4_t src_mix_pixels = vld1_u16(src);
    const uint16x4_t dst_pixels     = vld1_u16(dst);
    const uint16x4_t src_pixels     = vand_u16(src_mix_pixels, vdup_n_u16(0xFF));
    const uint16x4_t mix_pixels     = vshr_n_u16(src_mix_pixels, 8);
    const uint16x4_t opa_vec        = vmov_n_u16(opa);
    const uint16x4_t mask_vec       = vget_low_u16(vmovl_u8(vld1_u8(mask)));
    const uint32x4_t a_pixels_low   = vmovl_u16(mix_pixels);
    const uint32x4_t opa_vec_low    = vmovl_u16(opa_vec);
    const uint32x4_t mask_vec_low   = vmovl_u16(mask_vec);

    const uint32x4_t mul_result = vshrq_n_u32(vmulq_u32(vmulq_u32(a_pixels_low, opa_vec_low), mask_vec_low), 16);
    const uint16x4_t mix_vec    = vmovn_u32(mul_result);
    return lv_color_8_16_mix_4_internal(src_pixels, dst_pixels, mix_vec);
}

static inline uint16x8_t lv_color_8_16_mix_8_internal(uint16x8_t src_pixels, uint16x8_t dst_pixels,
                                                      uint16x8_t mix_pixels)
{
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
static inline uint16x4_t lv_color_8_16_mix_4_internal(uint16x4_t src_pixels, uint16x4_t dst_pixels,
                                                      uint16x4_t mix_pixels)
{

    const uint16x4_t mix_zero_mask = vceq_u16(mix_pixels, vdup_n_u16(0));
    const uint16x4_t mix_full_mask = vceq_u16(mix_pixels, vdup_n_u16(255));
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

static inline uint16x4_t lv_color_8_16_mix_4(const uint16_t * src, const uint16_t * dst)
{
    const uint16x4_t src_mix_pixels = vld1_u16(src);
    const uint16x4_t dst_pixels     = vld1_u16(dst);
    const uint16x4_t src_pixels     = vand_u16(src_mix_pixels, vdup_n_u16(0xFF));
    const uint16x4_t mix_pixels     = vshr_n_u16(src_mix_pixels, 8);
    return lv_color_8_16_mix_4_internal(src_pixels, dst_pixels, mix_pixels);
}

static inline uint32_t lv_color_8_16_mix_2(const uint8_t * src, const uint16_t * dst)
{
    return ((uint32_t)lv_color_8_16_mix_1_internal(src[2], dst[1], src[3]) << 16) |
           lv_color_8_16_mix_1_internal(src[0], dst[0], src[1]);
}

static inline uint32_t lv_color_8_16_mix_2_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa)
{
    return ((uint32_t)lv_color_8_16_mix_1_internal(src[2], dst[1], LV_OPA_MIX2(src[3], opa)) << 16) |
           lv_color_8_16_mix_1_internal(src[0], dst[0], LV_OPA_MIX2(src[1], opa));
}

static inline uint32_t lv_color_8_16_mix_2_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask)
{

    return ((uint32_t)lv_color_8_16_mix_1_internal(src[2], dst[1], LV_OPA_MIX2(src[3], mask[1])) << 16) |
           lv_color_8_16_mix_1_internal(src[0], dst[0], LV_OPA_MIX2(src[1], mask[0]));
}
static inline uint32_t lv_color_8_16_mix_2_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                         const uint8_t * mask)
{

    return ((uint32_t)lv_color_8_16_mix_1_internal(src[2], dst[1], LV_OPA_MIX3(src[3], mask[1], opa)) << 16) |
           lv_color_8_16_mix_1_internal(src[0], dst[0], LV_OPA_MIX3(src[3], mask[0], opa));
}

static inline uint16_t lv_color_8_16_mix_1_internal(uint8_t src, uint16_t dst, uint8_t mix)
{
    const uint8_t c1  = src;
    const uint16_t c2 = dst;

    if(mix == 0) {
        return c2;
    }
    else if(mix == 255) {
        return ((c1 & 0xF8) << 8) + ((c1 & 0xFC) << 3) + ((c1 & 0xF8) >> 3);
    }
    else {
        uint8_t mix_inv = 255 - mix;
        return ((((c1 >> 3) * mix + ((c2 >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
               ((((c1 >> 2) * mix + ((c2 >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
               (((c1 >> 3) * mix + (c2 & 0x1F) * mix_inv) >> 8);
    }
}

static inline uint16x8_t lv_color_32_16_mix_8(const uint8_t * src, const uint16_t * dst)
{
    const uint16x8_t dst_pixels = vld1q_u16(dst);

    const uint8x8x4_t rgba    = vld4_u8(src);
    const uint16x8_t b_pixels = vmovl_u8(rgba.val[0]);
    const uint16x8_t g_pixels = vmovl_u8(rgba.val[1]);
    const uint16x8_t r_pixels = vmovl_u8(rgba.val[2]);
    const uint16x8_t a_pixels = vmovl_u8(rgba.val[3]);

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, a_pixels, dst_pixels);
}
static inline uint16x4_t lv_color_32_16_mix_4(const uint8_t * src, const uint16_t * dst)
{
    const uint16x4_t dst_pixels = vld1_u16(dst);
    uint8_t b_array[8]          = {src[0], src[4], src[8], src[12], 0, 0, 0, 0};
    uint8_t g_array[8]          = {src[1], src[5], src[9], src[13], 0, 0, 0, 0};
    uint8_t r_array[8]          = {src[2], src[6], src[10], src[14], 0, 0, 0, 0};
    uint8_t a_array[8]          = {src[3], src[7], src[11], src[15], 0, 0, 0, 0};
    const uint16x4_t b_pixels   = vget_low_u16(vmovl_u8(vld1_u8(b_array)));
    const uint16x4_t g_pixels   = vget_low_u16(vmovl_u8(vld1_u8(g_array)));
    const uint16x4_t r_pixels   = vget_low_u16(vmovl_u8(vld1_u8(r_array)));
    const uint16x4_t a_pixels   = vget_low_u16(vmovl_u8(vld1_u8(a_array)));
    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, a_pixels, dst_pixels);
}

static inline uint16x8_t lv_color_24_16_mix_8_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask,
                                                        uint8_t src_px_size)
{
    uint16x8_t b_pixels;
    uint16x8_t g_pixels;
    uint16x8_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    const uint16x8_t dst_pixels = vld1q_u16(dst);
    const uint16x8_t mask_vec   = vmovl_u8(vld1_u8(mask));

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, mask_vec, dst_pixels);
}
static inline uint16x4_t lv_color_24_16_mix_4_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask,
                                                        uint8_t src_px_size)
{
    uint16x4_t b_pixels;
    uint16x4_t g_pixels;
    uint16x4_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    const uint16x4_t dst_pixels = vld1_u16(dst);
    const uint16x4_t mask_vec   = vget_low_u16(vmovl_u8(vld1_u8(mask)));

    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, mask_vec, dst_pixels);
}

static inline uint16x8_t lv_color_24_16_mix_8_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask, uint8_t src_px_size)
{
    uint16x8_t b_pixels;
    uint16x8_t g_pixels;
    uint16x8_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    const uint16x8_t dst_pixels = vld1q_u16(dst);
    const uint16x8_t mask_vec   = vmovl_u8(vld1_u8(mask));
    const uint16x8_t opa_vec    = vmovq_n_u16(opa);
    const uint16x8_t mix_vec    = vshrq_n_u16(vmulq_u16(opa_vec, mask_vec), 8);

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, mix_vec, dst_pixels);
}
static inline uint16x4_t lv_color_24_16_mix_4_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask, uint8_t src_px_size)
{
    uint16x4_t b_pixels;
    uint16x4_t g_pixels;
    uint16x4_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    const uint16x4_t dst_pixels = vld1_u16(dst);
    const uint16x4_t mask_vec   = vget_low_u16(vmovl_u8(vld1_u8(mask)));
    const uint16x4_t opa_vec    = vmov_n_u16(opa);
    const uint16x4_t mix_vec    = vshr_n_u16(vmul_u16(opa_vec, mask_vec), 8);

    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, mix_vec, dst_pixels);
}
static inline uint32_t lv_color_24_16_mix_2_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                          const uint8_t * mask, uint8_t src_px_size)
{

    return (lv_color_24_16_mix_1_internal(src + src_px_size, dst + 1, LV_OPA_MIX2(mask[1], opa)) << 16) |
           lv_color_24_16_mix_1_internal(src, dst, LV_OPA_MIX2(mask[0], opa));
}
static inline uint16x8_t lv_color_24_16_mix_8_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                       uint8_t src_px_size)
{
    uint16x8_t b_pixels;
    uint16x8_t g_pixels;
    uint16x8_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    const uint16x8_t dst_pixels = vld1q_u16(dst);
    const uint16x8_t opa_vec    = vmovq_n_u16(opa);

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, opa_vec, dst_pixels);
}
static inline uint16x8_t lv_color_32_16_mix_8_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa)
{
    const uint16x8_t dst_pixels = vld1q_u16(dst);

    const uint8x8x4_t rgba     = vld4_u8(src);
    const uint16x8_t b_pixels  = vmovl_u8(rgba.val[0]);
    const uint16x8_t g_pixels  = vmovl_u8(rgba.val[1]);
    const uint16x8_t r_pixels  = vmovl_u8(rgba.val[2]);
    const uint16x8_t a_pixels  = vmovl_u8(rgba.val[3]);
    const uint16x8_t opa_vec   = vmovq_n_u16(opa);
    const uint16x8_t opa_a_mul = vshrq_n_u16(vmulq_u16(a_pixels, opa_vec), 8);

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, opa_a_mul, dst_pixels);
}

static inline uint16x4_t lv_color_24_16_mix_4_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t mix,
                                                       uint8_t src_px_size)
{

    uint16x4_t b_pixels;
    uint16x4_t g_pixels;
    uint16x4_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    const uint16x4_t dst_pixels = vld1_u16(dst);
    const uint16x4_t mix_vec    = vget_low_u16(vmovq_n_u16(mix));

    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, mix_vec, dst_pixels);
}
static inline uint16x4_t lv_color_32_16_mix_4_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa)
{
    const uint16x4_t dst_pixels = vld1_u16(dst);
    uint8_t b_array[8]          = {src[0], src[4], src[8], src[12], 0, 0, 0, 0};
    uint8_t g_array[8]          = {src[1], src[5], src[9], src[13], 0, 0, 0, 0};
    uint8_t r_array[8]          = {src[2], src[6], src[10], src[14], 0, 0, 0, 0};
    uint8_t a_array[8]          = {src[3], src[7], src[11], src[15], 0, 0, 0, 0};
    const uint16x4_t b_pixels   = vget_low_u16(vmovl_u8(vld1_u8(b_array)));
    const uint16x4_t g_pixels   = vget_low_u16(vmovl_u8(vld1_u8(g_array)));
    const uint16x4_t r_pixels   = vget_low_u16(vmovl_u8(vld1_u8(r_array)));
    const uint16x4_t a_pixels   = vget_low_u16(vmovl_u8(vld1_u8(a_array)));
    const uint16x4_t opa_vec    = vmov_n_u16(opa);
    const uint16x4_t opa_a_mul  = vshr_n_u16(vmul_u16(a_pixels, opa_vec), 8);

    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, opa_a_mul, dst_pixels);
}

static inline uint16x8_t lv_color_32_16_mix_8_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask)
{
    const uint16x8_t dst_pixels = vld1q_u16(dst);

    const uint8x8x4_t rgba      = vld4_u8(src);
    const uint16x8_t b_pixels   = vmovl_u8(rgba.val[0]);
    const uint16x8_t g_pixels   = vmovl_u8(rgba.val[1]);
    const uint16x8_t r_pixels   = vmovl_u8(rgba.val[2]);
    const uint16x8_t a_pixels   = vmovl_u8(rgba.val[3]);
    const uint16x8_t mask_vec   = vmovl_u8(vld1_u8(mask));
    const uint16x8_t mask_a_mul = vshrq_n_u16(vmulq_u16(a_pixels, mask_vec), 8);

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, mask_a_mul, dst_pixels);
}
static inline uint16x4_t lv_color_32_16_mix_4_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask)
{

    const uint16x4_t dst_pixels = vld1_u16(dst);
    uint8_t b_array[8]          = {src[0], src[4], src[8], src[12], 0, 0, 0, 0};
    uint8_t g_array[8]          = {src[1], src[5], src[9], src[13], 0, 0, 0, 0};
    uint8_t r_array[8]          = {src[2], src[6], src[10], src[14], 0, 0, 0, 0};
    uint8_t a_array[8]          = {src[3], src[7], src[11], src[15], 0, 0, 0, 0};
    uint8_t mask_array[8]       = {mask[0], mask[1], mask[2], mask[3], 0, 0, 0, 0};
    const uint16x4_t b_pixels   = vget_low_u16(vmovl_u8(vld1_u8(b_array)));
    const uint16x4_t g_pixels   = vget_low_u16(vmovl_u8(vld1_u8(g_array)));
    const uint16x4_t r_pixels   = vget_low_u16(vmovl_u8(vld1_u8(r_array)));
    const uint16x4_t a_pixels   = vget_low_u16(vmovl_u8(vld1_u8(a_array)));
    const uint16x4_t mask_vec   = vget_low_u16(vmovl_u8(vld1_u8(mask_array)));
    const uint16x4_t mask_a_mul = vshr_n_u16(vmul_u16(a_pixels, mask_vec), 8);
    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, mask_a_mul, dst_pixels);
}
static inline uint16x8_t lv_color_32_16_mix_8_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask)
{
    const uint16x8_t dst_pixels = vld1q_u16(dst);

    const uint8x8x4_t rgba    = vld4_u8(src);
    const uint16x8_t b_pixels = vmovl_u8(rgba.val[0]);
    const uint16x8_t g_pixels = vmovl_u8(rgba.val[1]);
    const uint16x8_t r_pixels = vmovl_u8(rgba.val[2]);
    const uint16x8_t a_pixels = vmovl_u8(rgba.val[3]);
    const uint16x8_t mask_vec = vmovl_u8(vld1_u8(mask));
    const uint16x8_t opa_vec  = vmovq_n_u16(opa);

    /* Use uint32 for intermediate multiplication results to avoid 16bit overflows */
    const uint32x4_t a_pixels_low  = vmovl_u16(vget_low_u16(a_pixels));
    const uint32x4_t a_pixels_high = vmovl_u16(vget_high_u16(a_pixels));
    const uint32x4_t opa_vec_low   = vmovl_u16(vget_low_u16(opa_vec));
    const uint32x4_t opa_vec_high  = vmovl_u16(vget_high_u16(opa_vec));
    const uint32x4_t mask_vec_low  = vmovl_u16(vget_low_u16(mask_vec));
    const uint32x4_t mask_vec_high = vmovl_u16(vget_high_u16(mask_vec));

    const uint32x4_t mul_low        = vshrq_n_u32(vmulq_u32(vmulq_u32(a_pixels_low, opa_vec_low), mask_vec_low), 16);
    const uint32x4_t mul_high       = vshrq_n_u32(vmulq_u32(vmulq_u32(a_pixels_high, opa_vec_high), mask_vec_high), 16);
    const uint16x8_t mask_opa_a_mul = vcombine_u16(vmovn_u32(mul_low), vmovn_u32(mul_high));

    return lv_color_32_16_mix_8_internal(r_pixels, g_pixels, b_pixels, mask_opa_a_mul, dst_pixels);
}
static inline uint16x4_t lv_color_32_16_mix_4_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                            const uint8_t * mask)
{

    const uint16x4_t dst_pixels = vld1_u16(dst);
    uint8_t b_array[8]          = {src[0], src[4], src[8], src[12], 0, 0, 0, 0};
    uint8_t g_array[8]          = {src[1], src[5], src[9], src[13], 0, 0, 0, 0};
    uint8_t r_array[8]          = {src[2], src[6], src[10], src[14], 0, 0, 0, 0};
    uint8_t a_array[8]          = {src[3], src[7], src[11], src[15], 0, 0, 0, 0};
    uint8_t mask_array[8]       = {mask[0], mask[1], mask[2], mask[3], 0, 0, 0, 0};
    const uint16x4_t b_pixels   = vget_low_u16(vmovl_u8(vld1_u8(b_array)));
    const uint16x4_t g_pixels   = vget_low_u16(vmovl_u8(vld1_u8(g_array)));
    const uint16x4_t r_pixels   = vget_low_u16(vmovl_u8(vld1_u8(r_array)));
    const uint16x4_t a_pixels   = vget_low_u16(vmovl_u8(vld1_u8(a_array)));
    const uint16x4_t mask_vec   = vget_low_u16(vmovl_u8(vld1_u8(mask_array)));
    const uint16x4_t opa_vec    = vmov_n_u16(opa);

    const uint32x4_t a_pixels_low = vmovl_u16(a_pixels);
    const uint32x4_t opa_vec_low  = vmovl_u16(opa_vec);
    const uint32x4_t mask_vec_low = vmovl_u16(mask_vec);

    const uint32x4_t mul_result     = vshrq_n_u32(vmulq_u32(vmulq_u32(a_pixels_low, opa_vec_low), mask_vec_low), 16);
    const uint16x4_t mask_opa_a_mul = vmovn_u32(mul_result);

    return lv_color_32_16_mix_4_internal(r_pixels, g_pixels, b_pixels, mask_opa_a_mul, dst_pixels);
}

static inline uint16x8_t lv_color_32_16_mix_8_internal(uint16x8_t r_pixels, uint16x8_t g_pixels, uint16x8_t b_pixels,
                                                       uint16x8_t a_pixels, uint16x8_t dst_pixels)
{
    const uint16x8_t mix_zero_mask = vceqq_u16(a_pixels, vdupq_n_u16(0));
    const uint16x8_t mix_full_mask = vceqq_u16(a_pixels, vdupq_n_u16(255));

    /* Prepare result in case alpha == 255 */
    const uint16x8_t src_r565   = vandq_u16(vshlq_n_u16(r_pixels, 8), vdupq_n_u16(0xF800));
    const uint16x8_t src_g565   = vandq_u16(vshlq_n_u16(g_pixels, 3), vdupq_n_u16(0x07E0));
    const uint16x8_t src_b565   = vshrq_n_u16(b_pixels, 3);
    const uint16x8_t src_rgb565 = vorrq_u16(vorrq_u16(src_r565, src_g565), src_b565);

    /* Do the actual blending */
    const uint16x8_t mix_inv_16 = vsubq_u16(vdupq_n_u16(255), a_pixels);

    /* Red: ((src_r >> 3) * mix + ((dst >> 11) & 0x1F) * mix_inv) << 3) & 0xF800 */
    const uint16x8_t src_r = vshrq_n_u16(r_pixels, 3);
    const uint16x8_t dst_r = vandq_u16(vshrq_n_u16(dst_pixels, 11), vdupq_n_u16(0x1F));
    uint16x8_t blended_r   = vmlaq_u16(vmulq_u16(src_r, a_pixels), dst_r, mix_inv_16);
    blended_r              = vandq_u16(vshlq_n_u16(blended_r, 3), vdupq_n_u16(0xF800));

    /* Green: ((src_g >> 2) * mix + ((dst >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0 */
    const uint16x8_t src_g = vshrq_n_u16(g_pixels, 2);
    const uint16x8_t dst_g = vandq_u16(vshrq_n_u16(dst_pixels, 5), vdupq_n_u16(0x3F));
    uint16x8_t blended_g   = vmlaq_u16(vmulq_u16(src_g, a_pixels), dst_g, mix_inv_16);
    blended_g              = vandq_u16(vshrq_n_u16(blended_g, 3), vdupq_n_u16(0x07E0));

    /* Blue: ((src_b >> 3) * mix + (dst & 0x1F) * mix_inv) >> 8 */
    const uint16x8_t src_b = vshrq_n_u16(b_pixels, 3);
    const uint16x8_t dst_b = vandq_u16(dst_pixels, vdupq_n_u16(0x1F));
    uint16x8_t blended_b   = vmlaq_u16(vmulq_u16(src_b, a_pixels), dst_b, mix_inv_16);
    blended_b              = vshrq_n_u16(blended_b, 8);

    const uint16x8_t blended_result = vorrq_u16(vorrq_u16(blended_r, blended_g), blended_b);
    const uint16x8_t result         = vbslq_u16(mix_zero_mask, dst_pixels, blended_result);
    return vbslq_u16(mix_full_mask, src_rgb565, result);
}
static inline uint16x4_t lv_color_32_16_mix_4_internal(uint16x4_t r_pixels, uint16x4_t g_pixels, uint16x4_t b_pixels,
                                                       uint16x4_t a_pixels, uint16x4_t dst_pixels)
{
    const uint16x4_t mix_zero_mask = vceq_u16(a_pixels, vdup_n_u16(0));
    const uint16x4_t mix_full_mask = vceq_u16(a_pixels, vdup_n_u16(255));

    /* Prepare result in case alpha == 255 */
    const uint16x4_t src_r565   = vand_u16(vshl_n_u16(r_pixels, 8), vdup_n_u16(0xF800));
    const uint16x4_t src_g565   = vand_u16(vshl_n_u16(g_pixels, 3), vdup_n_u16(0x07E0));
    const uint16x4_t src_b565   = vshr_n_u16(b_pixels, 3);
    const uint16x4_t src_rgb565 = vorr_u16(vorr_u16(src_r565, src_g565), src_b565);

    const uint16x4_t mix_inv_16 = vsub_u16(vdup_n_u16(255), a_pixels);

    /* Red: ((src_r >> 3) * mix + ((dst >> 11) & 0x1F) * mix_inv) << 3) & 0xF800 */
    const uint16x4_t src_r = vshr_n_u16(r_pixels, 3);
    const uint16x4_t dst_r = vand_u16(vshr_n_u16(dst_pixels, 11), vdup_n_u16(0x1F));
    uint16x4_t blended_r   = vmla_u16(vmul_u16(src_r, a_pixels), dst_r, mix_inv_16);
    blended_r              = vand_u16(vshl_n_u16(blended_r, 3), vdup_n_u16(0xF800));

    /* Green: ((src_g >> 2) * mix + ((dst >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0 */
    const uint16x4_t src_g = vshr_n_u16(g_pixels, 2);
    const uint16x4_t dst_g = vand_u16(vshr_n_u16(dst_pixels, 5), vdup_n_u16(0x3F));
    uint16x4_t blended_g   = vmla_u16(vmul_u16(src_g, a_pixels), dst_g, mix_inv_16);
    blended_g              = vand_u16(vshr_n_u16(blended_g, 3), vdup_n_u16(0x07E0));

    /* Blue: ((src_b >> 3) * mix + (dst & 0x1F) * mix_inv) >> 8 */
    const uint16x4_t src_b = vshr_n_u16(b_pixels, 3);
    const uint16x4_t dst_b = vand_u16(dst_pixels, vdup_n_u16(0x1F));
    uint16x4_t blended_b   = vmla_u16(vmul_u16(src_b, a_pixels), dst_b, mix_inv_16);
    blended_b              = vshr_n_u16(blended_b, 8);

    const uint16x4_t blended_result = vorr_u16(vorr_u16(blended_r, blended_g), blended_b);

    const uint16x4_t result = vbsl_u16(mix_zero_mask, dst_pixels, blended_result);
    return vbsl_u16(mix_full_mask, src_rgb565, result);
}

static inline uint32_t lv_color_24_16_mix_2_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask,
                                                      uint8_t src_px_size)
{

    return (lv_color_24_16_mix_1_internal(src + src_px_size, dst + 1, mask[1]) << 16) |
           lv_color_24_16_mix_1_internal(src, dst, *mask);
}

static inline uint32_t lv_color_24_16_mix_2_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t mix,
                                                     uint8_t src_px_size)
{
    return (lv_color_24_16_mix_1_internal(src + src_px_size, dst + 1, mix) << 16) |
           lv_color_24_16_mix_1_internal(src, dst, mix);
}
static inline uint32_t lv_color_32_16_mix_2_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa)
{
    return (lv_color_32_16_mix_1_with_opa(src + 4, dst + 1, opa) << 16) | lv_color_32_16_mix_1_with_opa(src, dst, opa);
}
static inline uint32_t lv_color_32_16_mix_2_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                          const uint8_t * mask)
{
    return (lv_color_32_16_mix_1_with_opa_mask(src + 4, dst + 1, opa, &mask[1]) << 16) |
           lv_color_32_16_mix_1_with_opa_mask(src, dst, opa, &mask[0]);
}

static inline uint32_t lv_color_32_16_mix_2_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask)
{
    return (lv_color_32_16_mix_1_with_mask(src + 4, dst + 1, mask + 1) << 16) |
           lv_color_32_16_mix_1_with_mask(src, dst, mask);
}
static inline uint32_t lv_color_32_16_mix_2(const uint8_t * src, const uint16_t * dst)
{
    return (lv_color_32_16_mix_1(src + 4, dst + 1) << 16) | lv_color_32_16_mix_1(src, dst);
}

static inline uint32_t lv_color_32_16_mix_1(const uint8_t * src, const uint16_t * dst)
{
    return lv_color_24_16_mix_1_internal(src, dst, src[3]);
}

static inline uint16_t lv_color_32_16_mix_1_with_opa_mask(const uint8_t * src, const uint16_t * dst, uint8_t opa,
                                                          const uint8_t * mask)
{
    return lv_color_24_16_mix_1_internal(src, dst, LV_OPA_MIX3(src[3], opa, *mask));
}

static inline uint16_t lv_color_32_16_mix_1_with_opa(const uint8_t * src, const uint16_t * dst, uint8_t opa)
{
    return lv_color_24_16_mix_1_internal(src, dst, LV_OPA_MIX2(src[3], opa));
}
static inline uint16_t lv_color_32_16_mix_1_with_mask(const uint8_t * src, const uint16_t * dst, const uint8_t * mask)
{

    return lv_color_24_16_mix_1_internal(src, dst, LV_OPA_MIX2(src[3], *mask));
}

static inline uint16_t lv_color_24_16_mix_1_internal(const uint8_t * src, const uint16_t * dst, uint8_t mix)
{
    if(mix == 0) {
        return *dst;
    }
    else if(mix == 255) {
        return ((src[2] & 0xF8) << 8) + ((src[1] & 0xFC) << 3) + ((src[0] & 0xF8) >> 3);
    }
    else {
        lv_opa_t mix_inv = 255 - mix;

        return ((((src[2] >> 3) * mix + ((*dst >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
               ((((src[1] >> 2) * mix + ((*dst >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
               (((src[0] >> 3) * mix + (*dst & 0x1F) * mix_inv) >> 8);
    }
}

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
static inline uint16x8_t lv_color_24_16_mix_premult_8(const uint8_t * src, const uint16_t * dst)
{
    const uint16x8_t dst_pixels = vld1q_u16(dst);

    const uint8x8x4_t rgba    = vld4_u8(src);
    const uint16x8_t b_pixels = vmovl_u8(rgba.val[0]);
    const uint16x8_t g_pixels = vmovl_u8(rgba.val[1]);
    const uint16x8_t r_pixels = vmovl_u8(rgba.val[2]);
    const uint16x8_t a_pixels = vmovl_u8(rgba.val[3]);

    const uint16x8_t mix_zero_mask = vceqq_u16(a_pixels, vdupq_n_u16(0));
    const uint16x8_t mix_full_mask = vceqq_u16(a_pixels, vdupq_n_u16(255));

    /* Prepare result in case alpha == 255 */
    const uint16x8_t src_r565   = vandq_u16(vshlq_n_u16(r_pixels, 8), vdupq_n_u16(0xF800));
    const uint16x8_t src_g565   = vandq_u16(vshlq_n_u16(g_pixels, 3), vdupq_n_u16(0x07E0));
    const uint16x8_t src_b565   = vshrq_n_u16(b_pixels, 3);
    const uint16x8_t src_rgb565 = vorrq_u16(vorrq_u16(src_r565, src_g565), src_b565);

    /* Do the actual blending */
    const uint16x8_t mix_inv_16 = vsubq_u16(vdupq_n_u16(255), a_pixels);

    const uint16x8_t src_r = vshrq_n_u16(r_pixels, 3);
    const uint16x8_t dst_r = vandq_u16(vshrq_n_u16(dst_pixels, 11), vdupq_n_u16(0x1F));
    uint16x8_t blended_r   = vaddq_u16(src_r, vshrq_n_u16(vmulq_u16(dst_r, mix_inv_16), 8));
    blended_r              = vshlq_n_u16(blended_r, 11);

    /* Green: ((src_g >> 2) * mix + ((dst >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0 */
    const uint16x8_t src_g = vshrq_n_u16(g_pixels, 2);
    const uint16x8_t dst_g = vandq_u16(vshrq_n_u16(dst_pixels, 5), vdupq_n_u16(0x3F));
    uint16x8_t blended_g   = vaddq_u16(src_g, vshrq_n_u16(vmulq_u16(dst_g, mix_inv_16), 8));
    blended_g              = vshlq_n_u16(blended_g, 5);

    /* Blue: ((src_b >> 3) * mix + (dst & 0x1F) * mix_inv) >> 8 */
    const uint16x8_t src_b = vshrq_n_u16(b_pixels, 3);
    const uint16x8_t dst_b = vandq_u16(dst_pixels, vdupq_n_u16(0x1F));
    uint16x8_t blended_b   = vaddq_u16(src_b, vshrq_n_u16(vmulq_u16(dst_b, mix_inv_16), 8));

    const uint16x8_t blended_result = vaddq_u16(vaddq_u16(blended_r, blended_g), blended_b);
    const uint16x8_t result         = vbslq_u16(mix_zero_mask, dst_pixels, blended_result);
    return vbslq_u16(mix_full_mask, src_rgb565, result);
}
static inline uint16x4_t lv_color_24_16_mix_premult_4(const uint8_t * src, const uint16_t * dst)
{
    const uint16x4_t dst_pixels = vld1_u16(dst);
    uint8_t b_array[8]          = {src[0], src[4], src[8], src[12], 0, 0, 0, 0};
    uint8_t g_array[8]          = {src[1], src[5], src[9], src[13], 0, 0, 0, 0};
    uint8_t r_array[8]          = {src[2], src[6], src[10], src[14], 0, 0, 0, 0};
    uint8_t a_array[8]          = {src[3], src[7], src[11], src[15], 0, 0, 0, 0};
    const uint16x4_t b_pixels   = vget_low_u16(vmovl_u8(vld1_u8(b_array)));
    const uint16x4_t g_pixels   = vget_low_u16(vmovl_u8(vld1_u8(g_array)));
    const uint16x4_t r_pixels   = vget_low_u16(vmovl_u8(vld1_u8(r_array)));
    const uint16x4_t a_pixels   = vget_low_u16(vmovl_u8(vld1_u8(a_array)));

    const uint16x4_t mix_zero_mask = vceq_u16(a_pixels, vdup_n_u16(0));
    const uint16x4_t mix_full_mask = vceq_u16(a_pixels, vdup_n_u16(255));

    /* Prepare result in case alpha == 255 */
    const uint16x4_t src_r565   = vand_u16(vshl_n_u16(r_pixels, 8), vdup_n_u16(0xF800));
    const uint16x4_t src_g565   = vand_u16(vshl_n_u16(g_pixels, 3), vdup_n_u16(0x07E0));
    const uint16x4_t src_b565   = vshr_n_u16(b_pixels, 3);
    const uint16x4_t src_rgb565 = vorr_u16(vorr_u16(src_r565, src_g565), src_b565);

    const uint16x4_t mix_inv_16 = vsub_u16(vdup_n_u16(255), a_pixels);

    const uint16x4_t src_r = vshr_n_u16(r_pixels, 3);
    const uint16x4_t dst_r = vand_u16(vshr_n_u16(dst_pixels, 11), vdup_n_u16(0x1F));
    uint16x4_t blended_r   = vadd_u16(src_r, vshr_n_u16(vmul_u16(dst_r, mix_inv_16), 8));
    blended_r              = vshl_n_u16(blended_r, 11);

    /* Green: ((src_g >> 2) * mix + ((dst >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0 */
    const uint16x4_t src_g = vshr_n_u16(g_pixels, 2);
    const uint16x4_t dst_g = vand_u16(vshr_n_u16(dst_pixels, 5), vdup_n_u16(0x3F));
    uint16x4_t blended_g   = vadd_u16(src_g, vshr_n_u16(vmul_u16(dst_g, mix_inv_16), 8));
    blended_g              = vshl_n_u16(blended_g, 5);

    /* Blue: ((src_b >> 3) * mix + (dst & 0x1F) * mix_inv) >> 8 */
    const uint16x4_t src_b = vshr_n_u16(b_pixels, 3);
    const uint16x4_t dst_b = vand_u16(dst_pixels, vdup_n_u16(0x1F));
    uint16x4_t blended_b   = vadd_u16(src_b, vshr_n_u16(vmul_u16(dst_b, mix_inv_16), 8));

    const uint16x4_t blended_result = vorr_u16(vorr_u16(blended_r, blended_g), blended_b);

    const uint16x4_t result = vbsl_u16(mix_zero_mask, dst_pixels, blended_result);
    return vbsl_u16(mix_full_mask, src_rgb565, result);
}

static inline uint32_t lv_color_24_16_mix_premult_2(const uint8_t * src, const uint16_t * dst)
{
    return ((uint32_t)lv_color_24_16_mix_premult_1(src + 4, dst + 1) << 16) | lv_color_24_16_mix_premult_1(src, dst);
}
static inline uint16_t lv_color_24_16_mix_premult_1(const uint8_t * src, const uint16_t * dst)
{
    const uint8_t mix = src[3];
    if(mix == 0) {
        return *dst;
    }
    else if(mix == 255) {
        return ((src[2] & 0xF8) << 8) + ((src[1] & 0xFC) << 3) + ((src[0] & 0xF8) >> 3);
    }
    else {
        lv_opa_t mix_inv = 255 - mix;

        return (((src[2] >> 3) + ((((*dst >> 11) & 0x1F) * mix_inv) >> 8)) << 11) +
               (((src[1] >> 2) + ((((*dst >> 5) & 0x3F) * mix_inv) >> 8)) << 5) +
               (((src[0] >> 3) + ((((*dst >> 0) & 0x1F) * mix_inv) >> 8)));
    }
}

#endif

static inline uint16x8_t rgb888_to_rgb565_8(const uint8_t * src, uint8_t src_px_size)
{
    uint16x8_t b_pixels;
    uint16x8_t g_pixels;
    uint16x8_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vmovl_u8(rgba.val[0]);
        g_pixels               = vmovl_u8(rgba.val[1]);
        r_pixels               = vmovl_u8(rgba.val[2]);
    }

    const uint16x8_t r = vshlq_n_u16(vandq_u16(r_pixels, vdupq_n_u16(0xF8)), 8);
    const uint16x8_t g = vshlq_n_u16(vandq_u16(g_pixels, vdupq_n_u16(0xFC)), 3);
    const uint16x8_t b = vshrq_n_u16(vandq_u16(b_pixels, vdupq_n_u16(0xF8)), 3);
    return vorrq_u16(vorrq_u16(r, g), b);
}

static inline uint16x4_t rgb888_to_rgb565_4(const uint8_t * src, uint8_t src_px_size)
{
    uint16x4_t b_pixels;
    uint16x4_t g_pixels;
    uint16x4_t r_pixels;
    LV_ASSERT_MSG(src_px_size == 3 || src_px_size == 4, "Invalid pixel size for rgb888. Expected either 3 or 4 bytes");

    if(src_px_size == 3) {
        const uint8x8x3_t rgba = vld3_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }
    else if(src_px_size == 4) {
        const uint8x8x4_t rgba = vld4_u8(src);
        b_pixels               = vget_low_u16(vmovl_u8(rgba.val[0]));
        g_pixels               = vget_low_u16(vmovl_u8(rgba.val[1]));
        r_pixels               = vget_low_u16(vmovl_u8(rgba.val[2]));
    }

    const uint16x4_t r = vshl_n_u16(vand_u16(r_pixels, vdup_n_u16(0xF8)), 8);
    const uint16x4_t g = vshl_n_u16(vand_u16(g_pixels, vdup_n_u16(0xFC)), 3);
    const uint16x4_t b = vshr_n_u16(vand_u16(b_pixels, vdup_n_u16(0xF8)), 3);
    return vorr_u16(vorr_u16(r, g), b);
}

static inline uint32_t rgb888_to_rgb565_2(const uint8_t * src, uint8_t src_px_size)
{
    return ((uint32_t)rgb888_to_rgb565_1(src + src_px_size) << 16) | rgb888_to_rgb565_1(src);
}
static inline uint16_t rgb888_to_rgb565_1(const uint8_t * src)
{
    return ((src[2] & 0xF8) << 8) + ((src[1] & 0xFC) << 3) + ((src[0] & 0xF8) >> 3);
}

static inline uint16x8_t lv_color_16_16_mix_8_with_opa(const uint16_t * c1, const uint16_t * c2, uint8_t opa)
{

    const uint16x8_t c1_vec  = vld1q_u16(c1);
    const uint16x8_t c2_vec  = vld1q_u16(c2);
    const uint16x8_t mix_vec = vmovq_n_u16(opa);
    return lv_color_16_16_mix_8_internal(c1_vec, c2_vec, mix_vec);
}
static inline uint16x8_t lv_color_16_16_mix_8_with_mask(const uint16_t * c1, const uint16_t * c2, const uint8_t * mask)
{
    const uint16x8_t c1_vec  = vld1q_u16(c1);
    const uint16x8_t c2_vec  = vld1q_u16(c2);
    const uint16x8_t mix_vec = vmovl_u8(vld1_u8(mask));
    return lv_color_16_16_mix_8_internal(c1_vec, c2_vec, mix_vec);
}
static inline uint16x8_t lv_color_16_16_mix_8_with_opa_mask(const uint16_t * c1, const uint16_t * c2, uint8_t opa,
                                                            const uint8_t * mask)
{

    const uint16x8_t c1_vec   = vld1q_u16(c1);
    const uint16x8_t c2_vec   = vld1q_u16(c2);
    const uint16x8_t opa_vec  = vmovq_n_u16(opa);
    const uint16x8_t mask_vec = vmovl_u8(vld1_u8(mask));
    const uint16x8_t mix_vec  = vshrq_n_u16(vmulq_u16(opa_vec, mask_vec), 8);
    return lv_color_16_16_mix_8_internal(c1_vec, c2_vec, mix_vec);
}

static inline uint16x8_t lv_color_16_16_mix_8_internal(uint16x8_t c1_vec, uint16x8_t c2_vec, uint16x8_t mix)
{
    const uint16x8_t mix_zero_mask = vceqq_u16(mix, vdupq_n_u16(0));
    const uint16x8_t mix_full_mask = vceqq_u16(mix, vdupq_n_u16(255));
    const uint16x8_t equal_mask    = vceqq_u16(c1_vec, c2_vec);

    mix = vshrq_n_u16(vaddq_u16(mix, vdupq_n_u16(4)), 3);
    /* Split into low and high parts for 32-bit operations */
    uint32x4_t c1_low  = vmovl_u16(vget_low_u16(c1_vec));
    uint32x4_t c1_high = vmovl_u16(vget_high_u16(c1_vec));
    uint32x4_t c2_low  = vmovl_u16(vget_low_u16(c2_vec));
    uint32x4_t c2_high = vmovl_u16(vget_high_u16(c2_vec));
    uint32x4_t fg_low  = vorrq_u32(c1_low, vshlq_n_u32(c1_low, 16));
    uint32x4_t fg_high = vorrq_u32(c1_high, vshlq_n_u32(c1_high, 16));
    uint32x4_t bg_low  = vorrq_u32(c2_low, vshlq_n_u32(c2_low, 16));
    uint32x4_t bg_high = vorrq_u32(c2_high, vshlq_n_u32(c2_high, 16));

    /* Apply mask 0x7E0F81F to extract RGB components */
    const uint32x4_t mask = vdupq_n_u32(0x7E0F81F);
    fg_low                = vandq_u32(fg_low, mask);
    fg_high               = vandq_u32(fg_high, mask);
    bg_low                = vandq_u32(bg_low, mask);
    bg_high               = vandq_u32(bg_high, mask);

    const uint32x4_t mix_low  = vmovl_u16(vget_low_u16(mix));
    const uint32x4_t mix_high = vmovl_u16(vget_high_u16(mix));

    /* Perform the blend: ((fg - bg) * mix) >> 5 + bg */
    const uint32x4_t diff_low     = vsubq_u32(fg_low, bg_low);
    const uint32x4_t diff_high    = vsubq_u32(fg_high, bg_high);
    const uint32x4_t scaled_low   = vmulq_u32(diff_low, mix_low);
    const uint32x4_t scaled_high  = vmulq_u32(diff_high, mix_high);
    const uint32x4_t shifted_low  = vshrq_n_u32(scaled_low, 5);
    const uint32x4_t shifted_high = vshrq_n_u32(scaled_high, 5);
    uint32x4_t result_low         = vaddq_u32(shifted_low, bg_low);
    uint32x4_t result_high        = vaddq_u32(shifted_high, bg_high);

    /* Apply final mask */
    result_low  = vandq_u32(result_low, mask);
    result_high = vandq_u32(result_high, mask);

    /* Convert back to 16-bit: (result >> 16) | result */
    const uint32x4_t final_low  = vorrq_u32(result_low, vshrq_n_u32(result_low, 16));
    const uint32x4_t final_high = vorrq_u32(result_high, vshrq_n_u32(result_high, 16));

    const uint16x4_t packed_low  = vmovn_u32(final_low);
    const uint16x4_t packed_high = vmovn_u32(final_high);
    uint16x8_t result            = vcombine_u16(packed_low, packed_high);

    result = vbslq_u16(mix_zero_mask, c2_vec, result);
    result = vbslq_u16(mix_full_mask, c1_vec, result);
    result = vbslq_u16(equal_mask, c1_vec, result);

    return result;
}

static inline uint16x4_t lv_color_16_16_mix_4_with_opa(const uint16_t * c1, const uint16_t * c2, uint8_t opa)
{
    const uint16x4_t c1_vec  = vld1_u16(c1);
    const uint16x4_t c2_vec  = vld1_u16(c2);
    const uint16x4_t mix_vec = vmov_n_u16(opa);
    return lv_color_16_16_mix_4_internal(c1_vec, c2_vec, mix_vec);
}
static inline uint16x4_t lv_color_16_16_mix_4_with_mask(const uint16_t * c1, const uint16_t * c2, const uint8_t * mask)
{
    const uint16x4_t c1_vec  = vld1_u16(c1);
    const uint16x4_t c2_vec  = vld1_u16(c2);
    const uint16x4_t mix_vec = vget_low_u16(vmovl_u8(vld1_u8(mask)));
    return lv_color_16_16_mix_4_internal(c1_vec, c2_vec, mix_vec);
}
static inline uint16x4_t lv_color_16_16_mix_4_with_opa_mask(const uint16_t * c1, const uint16_t * c2, uint8_t opa,
                                                            const uint8_t * mask)
{

    const uint16x4_t c1_vec   = vld1_u16(c1);
    const uint16x4_t c2_vec   = vld1_u16(c2);
    const uint16x4_t opa_vec  = vmov_n_u16(opa);
    const uint16x4_t mask_vec = vget_low_u16(vmovl_u8(vld1_u8(mask)));
    const uint16x4_t mix_vec  = vshr_n_u16(vmul_u16(opa_vec, mask_vec), 8);
    return lv_color_16_16_mix_4_internal(c1_vec, c2_vec, mix_vec);
}

static inline uint16x4_t lv_color_16_16_mix_4_internal(uint16x4_t c1_vec, uint16x4_t c2_vec, uint16x4_t mix)
{
    const uint16x4_t mix_zero_mask = vceq_u16(mix, vdup_n_u16(0));
    const uint16x4_t mix_full_mask = vceq_u16(mix, vdup_n_u16(255));
    const uint16x4_t equal_mask    = vceq_u16(c1_vec, c2_vec);

    mix = vshr_n_u16(vadd_u16(mix, vdup_n_u16(4)), 3);
    /* Split into low and high parts for 32-bit operations */
    uint32x4_t c1 = vmovl_u16(c1_vec);
    uint32x4_t c2 = vmovl_u16(c2_vec);
    uint32x4_t fg = vorrq_u32(c1, vshlq_n_u32(c1, 16));
    uint32x4_t bg = vorrq_u32(c2, vshlq_n_u32(c2, 16));

    /* Apply mask 0x7E0F81F to extract RGB components */
    const uint32x4_t mask = vdupq_n_u32(0x7E0F81F);
    fg                    = vandq_u32(fg, mask);
    bg                    = vandq_u32(bg, mask);

    const uint32x4_t mix32 = vmovl_u16(mix);

    /* Perform the blend: ((fg - bg) * mix) >> 5 + bg */
    const uint32x4_t diff    = vsubq_u32(fg, bg);
    const uint32x4_t scaled  = vmulq_u32(diff, mix32);
    const uint32x4_t shifted = vshrq_n_u32(scaled, 5);
    uint32x4_t result32      = vaddq_u32(shifted, bg);
    result32                 = vandq_u32(result32, mask);

    /* Convert back to 16-bit: (result >> 16) | result */
    const uint32x4_t final = vorrq_u32(result32, vshrq_n_u32(result32, 16));

    uint16x4_t result = vmovn_u32(final);
    result            = vbsl_u16(mix_zero_mask, c2_vec, result);
    result            = vbsl_u16(mix_full_mask, c1_vec, result);
    result            = vbsl_u16(equal_mask, c1_vec, result);

    return result;
}

static inline uint32_t lv_color_16_16_mix_2_with_opa(const uint16_t * c1, const uint16_t * c2, uint8_t opa)
{
    return lv_color_16_16_mix(c1[1], c2[1], opa) << 16 | lv_color_16_16_mix(*c1, *c2, opa);
}
static inline uint32_t lv_color_16_16_mix_2_with_mask(const uint16_t * c1, const uint16_t * c2, const uint8_t * mask)
{
    return lv_color_16_16_mix(c1[1], c2[1], mask[1]) << 16 | lv_color_16_16_mix(*c1, *c2, *mask);
}
static inline uint32_t lv_color_16_16_mix_2_with_opa_mask(const uint16_t * c1, const uint16_t * c2, uint8_t opa,
                                                          const uint8_t * mask)
{

    return lv_color_16_16_mix(c1[1], c2[1], LV_OPA_MIX2(opa, mask[1])) << 16 |
           lv_color_16_16_mix(*c1, *c2, LV_OPA_MIX2(*mask, opa));
}

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}
#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON*/
