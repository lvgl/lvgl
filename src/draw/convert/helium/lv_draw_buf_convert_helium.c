/**
 * @file lv_draw_buf_convert_helium.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
#include "lv_draw_buf_convert_helium.h"

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

lv_result_t _lv_draw_buf_convert_premultiply_indexed_helium(lv_draw_buf_t * buf)
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

    return _lv_draw_buf_convert_premultiply_argb8888_helium(&palette_draw_buf);
}

lv_result_t _lv_draw_buf_convert_premultiply_argb8888_helium(lv_draw_buf_t * buf)
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

    __asm volatile(
        "   .p2align 2                                                  \n"
        "   1:                                                          \n"
        "   mov                     r0, %[pSource]                      \n"
        "   mov                     r1, %[pTarget]                      \n"
        "   wlstp.8                 lr, %[w], 3f                        \n"
        "   2:                                                          \n"
        "   vld40.u8                {q0, q1, q2, q3}, [r0]              \n"
        "   vld41.u8                {q0, q1, q2, q3}, [r0]              \n"
        "   vld42.u8                {q0, q1, q2, q3}, [r0]              \n"
        "   vld43.u8                {q0, q1, q2, q3}, [r0]!             \n"
        "   vrmulh.u8               q0, q0, q3                          \n"
        "   vrmulh.u8               q1, q1, q3                          \n"
        "   vrmulh.u8               q2, q2, q3                          \n"
        "   vst40.u8                {q0, q1, q2, q3}, [r1]              \n"
        "   vst41.u8                {q0, q1, q2, q3}, [r1]              \n"
        "   vst42.u8                {q0, q1, q2, q3}, [r1]              \n"
        "   vst43.u8                {q0, q1, q2, q3}, [r1]!             \n"
        "   letp                    lr, 2b                              \n"
        "   3:                                                          \n"
        "   adds                    %[pSource], %[src_stride]           \n"
        "   adds                    %[pTarget], %[dst_stride]           \n"
        "   subs                    %[h], #1                            \n"
        "   bne                     1b                                  \n"
        : [pSource] "+r"(data), [pTarget] "+r"(data), [h] "+r"(h)
        : [w] "r"(w), [src_stride] "r"(stride), [dst_stride] "r"(stride)
        : "q0", "q1", "q2", "q3", "r0", "r1", "lr", "memory");

    return LV_RESULT_OK;
}

#endif  /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM */
