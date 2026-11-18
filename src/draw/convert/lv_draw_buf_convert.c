/**
 * @file lv_draw_buf_convert.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"
#include "lv_draw_buf_convert.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON
    #include "neon/lv_draw_buf_convert_neon.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "helium/lv_draw_buf_convert_helium.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool buf_size_is_valid(const lv_draw_buf_t * buf);
static void convert_rgb565_to_rgb888(lv_draw_buf_t * dest, const lv_draw_buf_t * src);

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool buf_size_is_valid(const lv_draw_buf_t * buf)
{
    const lv_image_header_t * header = &buf->header;
    const uint32_t row_size = ((uint32_t)header->w * lv_color_format_get_bpp(header->cf) + 7) >> 3;
    const uint64_t palette_size = (uint64_t)LV_COLOR_INDEXED_PALETTE_SIZE(header->cf) * sizeof(lv_color32_t);

    return header->stride >= row_size &&
           palette_size + (uint64_t)header->stride * header->h <= buf->data_size;
}

static void convert_rgb565_to_rgb888(lv_draw_buf_t * dest, const lv_draw_buf_t * src)
{
    const uint32_t w = src->header.w;
    const uint32_t h = src->header.h;

    for(uint32_t y = 0; y < h; y++) {
        const uint8_t * src_px = (const uint8_t *)src->data + (size_t)y * src->header.stride;
        uint8_t * dest_px = (uint8_t *)dest->data + (size_t)y * dest->header.stride;

        for(uint32_t x = 0; x < w; x++) {
            /*Read through a copy, the row is not guaranteed to be aligned for 16 bit reads*/
            lv_color16_t c;
            lv_memcpy(&c, src_px, sizeof(c));
            src_px += sizeof(c);

            /*Replicate the top bits into the low bits so that the full range maps to 0..255*/
            dest_px[0] = (uint8_t)((c.blue << 3) | (c.blue >> 2));
            dest_px[1] = (uint8_t)((c.green << 2) | (c.green >> 4));
            dest_px[2] = (uint8_t)((c.red << 3) | (c.red >> 2));
            dest_px += 3;
        }
    }
}

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
    LV_PROFILER_DRAW_BEGIN;
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
    else {
        LV_LOG_WARN("color format: %d not supported for premultiply", cf);
        LV_PROFILER_DRAW_END;
        return LV_RESULT_INVALID;
    }

    LV_PROFILER_DRAW_END;
    return LV_RESULT_OK;
}

lv_result_t lv_draw_buf_convert_format(lv_draw_buf_t * dest, const lv_draw_buf_t * src)
{
    LV_CHECK_ARG(dest != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(src != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(dest->data != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(src->data != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(dest->header.w == src->header.w && dest->header.h == src->header.h, return LV_RESULT_INVALID);

    if(!buf_size_is_valid(dest) || !buf_size_is_valid(src)) {
        LV_LOG_WARN("the draw buffer is too small for its width, height and stride");
        return LV_RESULT_INVALID;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_result_t res = LV_RESULT_INVALID;
    if(src->header.cf == LV_COLOR_FORMAT_RGB565 && dest->header.cf == LV_COLOR_FORMAT_RGB888) {
        convert_rgb565_to_rgb888(dest, src);
        res = LV_RESULT_OK;
    }
    else {
        LV_LOG_WARN("converting color format %d to %d is not supported", src->header.cf, dest->header.cf);
    }

    if(res == LV_RESULT_OK) lv_draw_buf_flush_cache(dest, NULL);

    LV_PROFILER_DRAW_END;
    return res;
}
