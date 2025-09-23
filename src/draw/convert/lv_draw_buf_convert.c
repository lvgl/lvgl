/**
 * @file lv_draw_buf_convert.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_buf_convert.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON
    #include "neon/lv_draw_buf_convert_neon.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "helium/lv_draw_buf_convert_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

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

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_INDEXED
    #define LV_DRAW_CONVERT_PREMULTIPLY_INDEXED(...)                         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888
    #define LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888(...)                         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_RGB565A8
    #define LV_DRAW_CONVERT_PREMULTIPLY_RGB565A8(...)                         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_CONVERT_PREMULTIPLY_ARGB8565
    #define LV_DRAW_CONVERT_PREMULTIPLY_ARGB8565(...)                         LV_RESULT_INVALID
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_draw_buf_convert_premultiply(lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(draw_buf);

    /*Premultiply color with alpha, do case by case by judging color format*/
    lv_color_format_t cf = draw_buf->header.cf;
    if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
        if(LV_RESULT_INVALID == LV_DRAW_CONVERT_PREMULTIPLY_INDEXED(draw_buf)) {
            int size = LV_COLOR_INDEXED_PALETTE_SIZE(cf);
            lv_color32_t * palette = (lv_color32_t *)draw_buf->data;
            for(int i = 0; i < size; i++) {
                lv_color_premultiply(&palette[i]);
            }
        }
    }
    else if(cf == LV_COLOR_FORMAT_ARGB8888) {
        if(LV_RESULT_INVALID == LV_DRAW_CONVERT_PREMULTIPLY_ARGB8888(draw_buf)) {
            uint32_t h = draw_buf->header.h;
            uint32_t w = draw_buf->header.w;
            uint32_t stride = draw_buf->header.stride;
            uint8_t * line = (uint8_t *)draw_buf->data;
            for(uint32_t y = 0; y < h; y++) {
                lv_color32_t * pixel = (lv_color32_t *)line;
                for(uint32_t x = 0; x < w; x++) {
                    lv_color_premultiply(pixel);
                    pixel++;
                }
                line += stride;
            }
        }
    }
    else if(cf == LV_COLOR_FORMAT_RGB565A8) {
        if(LV_RESULT_INVALID == LV_DRAW_CONVERT_PREMULTIPLY_RGB565A8(draw_buf)) {
            uint32_t h = draw_buf->header.h;
            uint32_t w = draw_buf->header.w;
            uint32_t stride = draw_buf->header.stride;
            uint32_t alpha_stride = stride / 2;
            uint8_t * line = (uint8_t *)draw_buf->data;
            lv_opa_t * alpha = (lv_opa_t *)(line + stride * h);
            for(uint32_t y = 0; y < h; y++) {
                lv_color16_t * pixel = (lv_color16_t *)line;
                for(uint32_t x = 0; x < w; x++) {
                    lv_color16_premultiply(pixel, alpha[x]);
                    pixel++;
                }
                line += stride;
                alpha += alpha_stride;
            }
        }
    }
    else if(cf == LV_COLOR_FORMAT_ARGB8565) {
        if(LV_RESULT_INVALID == LV_DRAW_CONVERT_PREMULTIPLY_ARGB8565(draw_buf)) {
            uint32_t h = draw_buf->header.h;
            uint32_t w = draw_buf->header.w;
            uint32_t stride = draw_buf->header.stride;
            uint8_t * line = (uint8_t *)draw_buf->data;
            for(uint32_t y = 0; y < h; y++) {
                uint8_t * pixel = line;
                for(uint32_t x = 0; x < w; x++) {
                    uint8_t alpha = pixel[2];
                    lv_color16_premultiply((lv_color16_t *)pixel, alpha);
                    pixel += 3;
                }
                line += stride;
            }
        }
    }
    else if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf)) {
        /*Pass*/
    }
    else {
        LV_LOG_WARN("draw buf has no alpha, cf: %d", cf);
    }

    return LV_RESULT_OK;
}