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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}
#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON64 */
