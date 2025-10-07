/**
 * @file lv_draw_buf_convert_neon.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON

#include <arm_neon.h>
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

    LV_ASSERT_NULL(buf);

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
    LV_ASSERT_NULL(buf);

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
            uint8x8x4_t rgba = vld4_u8(p);

            uint16x8_t r16 = vmovl_u8(rgba.val[0]);
            uint16x8_t g16 = vmovl_u8(rgba.val[1]);
            uint16x8_t b16 = vmovl_u8(rgba.val[2]);
            uint16x8_t a16 = vmovl_u8(rgba.val[3]);

            rgba.val[0] = vshrn_n_u16(vmulq_u16(r16, a16), 8);
            rgba.val[1] = vshrn_n_u16(vmulq_u16(g16, a16), 8);
            rgba.val[2] = vshrn_n_u16(vmulq_u16(b16, a16), 8);

            vst4_u8(p, rgba);

            p += 8 * 4;
            remaining_pixels -= 8;
        }

        if(remaining_pixels >= 4) {
            uint8x8x4_t rgba;
            rgba = vld4_lane_u8(p, rgba, 0);
            rgba = vld4_lane_u8(p + 4, rgba, 1);
            rgba = vld4_lane_u8(p + 8, rgba, 2);
            rgba = vld4_lane_u8(p + 12, rgba, 3);

            uint16x8_t r16 = vmovl_u8(rgba.val[0]);
            uint16x8_t g16 = vmovl_u8(rgba.val[1]);
            uint16x8_t b16 = vmovl_u8(rgba.val[2]);
            uint16x8_t a16 = vmovl_u8(rgba.val[3]);

            rgba.val[0] = vshrn_n_u16(vmulq_u16(r16, a16), 8);
            rgba.val[1] = vshrn_n_u16(vmulq_u16(g16, a16), 8);
            rgba.val[2] = vshrn_n_u16(vmulq_u16(b16, a16), 8);

            vst4_lane_u8(p, rgba, 0);
            vst4_lane_u8(p + 4, rgba, 1);
            vst4_lane_u8(p + 8, rgba, 2);
            vst4_lane_u8(p + 12, rgba, 3);

            p += 4 * 4;
            remaining_pixels -= 4;
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
#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON */
