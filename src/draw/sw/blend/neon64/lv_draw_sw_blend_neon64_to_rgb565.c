/**
 * @file lv_draw_sw_blend_neon64_to_rgb565.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_neon64_to_rgb565.h"
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
    LV_LOG_USER("lv_draw_sw_blend_neon64_l8_to_rgb565");
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

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}
#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON64 */
