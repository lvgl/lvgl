/**
 * @file lv_draw_buf_convert_neon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_BUF_CONVERT_ASM == LV_DRAW_BUF_CONVERT_ASM_NEON

#include "lv_draw_buf_convert_neon.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t _lv_draw_buf_convert_premultiply_indexed_neon(lv_draw_buf_t * buf)
{
    lv_draw_buf_t palette_draw_buf;

    if(!LV_COLOR_FORMAT_IS_INDEXED(buf->header.cf)) {
        LV_LOG_WARN("Unsupported color format : %d", buf->header.cf);
        return LV_RESULT_INVALID;
    }

    lv_memcpy(&palette_draw_buf, buf, sizeof(lv_draw_buf_t));

    palette_draw_buf.header.w = LV_COLOR_INDEXED_PALETTE_SIZE(buf->header.cf);
    palette_draw_buf.header.h = 1;
    palette_draw_buf.header.cf = LV_COLOR_FORMAT_ARGB8888;
    palette_draw_buf.header.stride = 4 * palette_draw_buf.header.w;

    return _lv_draw_buf_convert_premultiply_argb8888_neon(&palette_draw_buf);

}

lv_result_t _lv_draw_buf_convert_premultiply_argb8888_neon(lv_draw_buf_t * buf)
{
    uint32_t h = buf->header.h;
    uint32_t w = buf->header.w;
    uint32_t stride = buf->header.stride;
    uint8_t * data = (uint8_t *)buf->data;

    if(buf->header.cf != LV_COLOR_FORMAT_ARGB8888) {
        LV_LOG_WARN("Unsupported color format : %d", buf->header.cf);
        return LV_RESULT_INVALID;
    }

    for(uint32_t y = 0; y < h; y++) {
        uint8_t * p = (uint8_t *)data;
        uint32_t remaining_pixels = w;

        while(remaining_pixels >= 8) {
            __asm volatile(
                "vld4.8 {d0, d1, d2, d3}, [%[p]]  \n"

                "vmovl.u8 q8, d0                  \n"
                "vmovl.u8 q9, d1                  \n"
                "vmovl.u8 q10, d2                 \n"
                "vmovl.u8 q11, d3                 \n"

                "vmul.u16 q8, q8, q11             \n"
                "vmul.u16 q9, q9, q11             \n"
                "vmul.u16 q10, q10, q11           \n"

                "vshrn.u16 d0, q8, #8             \n"
                "vshrn.u16 d1, q9, #8             \n"
                "vshrn.u16 d2, q10, #8            \n"

                "vst4.8 {d0, d1, d2, d3}, [%[p]]! \n"

                : [p] "+r"(p)
                :
                : "q0", "q1", "q8", "q9", "q10", "q11", "memory"
            );
            remaining_pixels -= 8;
        }

        while(remaining_pixels--) {
            uint8_t a = p[3];
            p[0] = ((uint16_t)(p[0]) * a) >> 8;
            p[1] = ((uint16_t)(p[1]) * a) >> 8;
            p[2] = ((uint16_t)(p[2]) * a) >> 8;
            p += 4;
        }

        data += stride;
    }

    return LV_RESULT_OK;
}
#endif /* LV_USE_DRAW_BUF_CONVERT_ASM == LV_DRAW_BUF_CONVERT_ASM_NEON */
