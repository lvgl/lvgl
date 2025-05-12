/**
 * @file lv_draw_sw_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_utils.h"
#if LV_USE_DRAW_SW

/*********************
 *      DEFINES
 *********************/
#ifndef LV_DRAW_SW_RGB565_SWAP
    #define LV_DRAW_SW_RGB565_SWAP(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_ARGB8888
    #define LV_DRAW_SW_ROTATE90_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_ARGB8888
    #define LV_DRAW_SW_ROTATE180_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_ARGB8888
    #define LV_DRAW_SW_ROTATE270_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_RGB888
    #define LV_DRAW_SW_ROTATE90_RGB888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_RGB888
    #define LV_DRAW_SW_ROTATE180_RGB888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_RGB888
    #define LV_DRAW_SW_ROTATE270_RGB888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_RGB565
    #define LV_DRAW_SW_ROTATE90_RGB565(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_RGB565
    #define LV_DRAW_SW_ROTATE180_RGB565(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_RGB565
    #define LV_DRAW_SW_ROTATE270_RGB565(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE90_L8
    #define LV_DRAW_SW_ROTATE90_L8(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE180_L8
    #define LV_DRAW_SW_ROTATE180_L8(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ROTATE270_L8
    #define LV_DRAW_SW_ROTATE270_L8(...) LV_RESULT_INVALID
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
static void rotate90_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                              int32_t src_stride,
                              int32_t dst_stride);
static void rotate180_argb8888(const uint32_t * src, uint32_t * dst, int32_t width, int32_t height, int32_t src_stride,
                               int32_t dest_stride);
static void rotate270_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                               int32_t src_stride,
                               int32_t dst_stride);
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
static void rotate90_rgb888(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride);
static void rotate180_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride);
static void rotate270_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dst_stride);
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
static void rotate90_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride);
static void rotate180_rgb565(const uint16_t * src, uint16_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride);
static void rotate270_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                             int32_t src_stride,
                             int32_t dst_stride);
#endif

#if LV_DRAW_SW_SUPPORT_L8

static void rotate90_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                        int32_t src_stride,
                        int32_t dst_stride);
static void rotate180_l8(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                         int32_t dest_stride);
static void rotate270_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                         int32_t src_stride,
                         int32_t dst_stride);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_i1_to_argb8888(const void * buf_i1, void * buf_argb8888, uint32_t width, uint32_t height,
                               uint32_t buf_i1_stride, uint32_t buf_argb8888_stride, uint32_t index0_color, uint32_t index1_color)
{
    /*Extract the bits of I1 px_map and convert them to ARGB8888*/
    const uint8_t * src = buf_i1;
    uint32_t * dst = buf_argb8888;
    uint32_t i1_row_byte_count = width / 8;
    for(uint32_t row = 0; row < height; row++) {
        uint32_t * dst_p = dst;
        for(uint32_t i = 0; i < i1_row_byte_count; i++) {
            /*From MSB to LSB (pixel 0 to pixel 7 in a byte)*/
            for(int32_t bit = 7; bit >= 0; bit--) {
                *dst_p++ = ((src[i] >> bit) & 1) ? index1_color : index0_color;
            }
        }
        src += buf_i1_stride;
        dst += buf_argb8888_stride / 4;
    }
}

void lv_draw_sw_rgb565_swap(void * buf, uint32_t buf_size_px)
{
    if(LV_DRAW_SW_RGB565_SWAP(buf, buf_size_px) == LV_RESULT_OK) return;

    uint32_t u32_cnt = buf_size_px / 2;
    uint16_t * buf16 = buf;
    uint32_t * buf32 = buf;

    while(u32_cnt >= 8) {
        buf32[0] = ((buf32[0] & 0xff00ff00) >> 8) | ((buf32[0] & 0x00ff00ff) << 8);
        buf32[1] = ((buf32[1] & 0xff00ff00) >> 8) | ((buf32[1] & 0x00ff00ff) << 8);
        buf32[2] = ((buf32[2] & 0xff00ff00) >> 8) | ((buf32[2] & 0x00ff00ff) << 8);
        buf32[3] = ((buf32[3] & 0xff00ff00) >> 8) | ((buf32[3] & 0x00ff00ff) << 8);
        buf32[4] = ((buf32[4] & 0xff00ff00) >> 8) | ((buf32[4] & 0x00ff00ff) << 8);
        buf32[5] = ((buf32[5] & 0xff00ff00) >> 8) | ((buf32[5] & 0x00ff00ff) << 8);
        buf32[6] = ((buf32[6] & 0xff00ff00) >> 8) | ((buf32[6] & 0x00ff00ff) << 8);
        buf32[7] = ((buf32[7] & 0xff00ff00) >> 8) | ((buf32[7] & 0x00ff00ff) << 8);
        buf32 += 8;
        u32_cnt -= 8;
    }

    while(u32_cnt) {
        *buf32 = ((*buf32 & 0xff00ff00) >> 8) | ((*buf32 & 0x00ff00ff) << 8);
        buf32++;
        u32_cnt--;
    }

    if(buf_size_px & 0x1) {
        uint32_t e = buf_size_px - 1;
        buf16[e] = ((buf16[e] & 0xff00) >> 8) | ((buf16[e] & 0x00ff) << 8);
    }

}

void lv_draw_sw_i1_invert(void * buf, uint32_t buf_size)
{
    if(buf == NULL) return;

    uint8_t * byte_buf = (uint8_t *)buf;
    uint32_t i;

    /*Make the buffer aligned*/
    while(((uintptr_t)(byte_buf) & (sizeof(int) - 1)) && buf_size > 0) {
        *byte_buf = ~(*byte_buf);
        byte_buf++;
        buf_size--;
    }

    if(buf_size >= sizeof(uint32_t)) {
        uint32_t * aligned_addr = (uint32_t *)byte_buf;
        uint32_t word_count = buf_size / 4;

        for(i = 0; i < word_count; ++i) {
            aligned_addr[i] = ~aligned_addr[i];
        }

        byte_buf = (uint8_t *)(aligned_addr + word_count);
        buf_size = buf_size % sizeof(uint32_t);
    }

    for(i = 0; i < buf_size; ++i) {
        byte_buf[i] = ~byte_buf[i];
    }
}

void lv_draw_sw_i1_convert_to_vtiled(const void * buf, uint32_t buf_size, uint32_t width, uint32_t height,
                                     void * out_buf,
                                     uint32_t out_buf_size, bool bit_order_lsb)
{
    LV_ASSERT(buf && out_buf);
    LV_ASSERT(width % 8 == 0 && height % 8 == 0);
    LV_ASSERT(buf_size >= (width / 8) * height);
    LV_ASSERT(out_buf_size >= buf_size);

    lv_memset(out_buf, 0, out_buf_size);

    const uint8_t * src_buf = (uint8_t *)buf;
    uint8_t * dst_buf = (uint8_t *)out_buf;

    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            uint32_t src_index = y * width + x;
            uint32_t dst_index = x * height + y;
            uint8_t bit = (src_buf[src_index / 8] >> (7 - (src_index % 8))) & 0x01;
            if(bit_order_lsb) {
                dst_buf[dst_index / 8] |= (bit << (dst_index % 8));
            }
            else {
                dst_buf[dst_index / 8] |= (bit << (7 - (dst_index % 8)));
            }
        }
    }
}

void lv_draw_sw_rotate(const void * src, void * dest, int32_t src_width, int32_t src_height, int32_t src_stride,
                       int32_t dest_stride, lv_display_rotation_t rotation, lv_color_format_t color_format)
{
    if(rotation == LV_DISPLAY_ROTATION_90) {
        switch(color_format) {
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                rotate90_l8(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                rotate90_rgb565(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                rotate90_rgb888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
                rotate90_argb8888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
            default:
                break;
        }

        return;
    }

    if(rotation == LV_DISPLAY_ROTATION_180) {
        switch(color_format) {
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                rotate180_l8(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                rotate180_rgb565(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                rotate180_rgb888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
                rotate180_argb8888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
            default:
                break;
        }

        return;
    }

    if(rotation == LV_DISPLAY_ROTATION_270) {
        switch(color_format) {
#if LV_DRAW_SW_SUPPORT_L8
            case LV_COLOR_FORMAT_L8:
                rotate270_l8(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565
            case LV_COLOR_FORMAT_RGB565:
                rotate270_rgb565(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
            case LV_COLOR_FORMAT_RGB888:
                rotate270_rgb888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888
            case LV_COLOR_FORMAT_XRGB8888:
            case LV_COLOR_FORMAT_ARGB8888:
                rotate270_argb8888(src, dest, src_width, src_height, src_stride, dest_stride);
                break;
#endif
            default:
                break;
        }

        return;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DRAW_SW_SUPPORT_ARGB8888 || LV_DRAW_SW_SUPPORT_XRGB8888

static void rotate270_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                               int32_t src_stride,
                               int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_ARGB8888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint32_t);
    dst_stride /= sizeof(uint32_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = x * dst_stride;
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex + (src_height - y - 1)] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

static void rotate180_argb8888(const uint32_t * src, uint32_t * dst, int32_t width, int32_t height, int32_t src_stride,
                               int32_t dest_stride)
{
    LV_UNUSED(dest_stride);
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_ARGB8888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint32_t);
    dest_stride /= sizeof(uint32_t);

    for(int32_t y = 0; y < height; ++y) {
        int32_t dstIndex = (height - y - 1) * dest_stride;
        int32_t srcIndex = y * src_stride;
        for(int32_t x = 0; x < width; ++x) {
            dst[dstIndex + width - x - 1] = src[srcIndex + x];
        }
    }
}

static void rotate90_argb8888(const uint32_t * src, uint32_t * dst, int32_t src_width, int32_t src_height,
                              int32_t src_stride, int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_ARGB8888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint32_t);
    dst_stride /= sizeof(uint32_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = (src_width - x - 1);
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex * dst_stride + y] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB888

static void rotate90_rgb888(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_RGB888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < src_width; ++x) {
        for(int32_t y = 0; y < src_height; ++y) {
            int32_t srcIndex = y * src_stride + x * 3;
            int32_t dstIndex = (src_width - x - 1) * dst_stride + y * 3;
            dst[dstIndex] = src[srcIndex];       /*Red*/
            dst[dstIndex + 1] = src[srcIndex + 1]; /*Green*/
            dst[dstIndex + 2] = src[srcIndex + 2]; /*Blue*/
        }
    }
}

static void rotate180_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_RGB888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t y = 0; y < height; ++y) {
        for(int32_t x = 0; x < width; ++x) {
            int32_t srcIndex = y * src_stride + x * 3;
            int32_t dstIndex = (height - y - 1) * dest_stride + (width - x - 1) * 3;
            dst[dstIndex] = src[srcIndex];
            dst[dstIndex + 1] = src[srcIndex + 1];
            dst[dstIndex + 2] = src[srcIndex + 2];
        }
    }
}

static void rotate270_rgb888(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_RGB888(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < width; ++x) {
        for(int32_t y = 0; y < height; ++y) {
            int32_t srcIndex = y * src_stride + x * 3;
            int32_t dstIndex = x * dst_stride + (height - y - 1) * 3;
            dst[dstIndex] = src[srcIndex];       /*Red*/
            dst[dstIndex + 1] = src[srcIndex + 1]; /*Green*/
            dst[dstIndex + 2] = src[srcIndex + 2]; /*Blue*/
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB565

static void rotate270_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                             int32_t src_stride,
                             int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_RGB565(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint16_t);
    dst_stride /= sizeof(uint16_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = x * dst_stride;
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex + (src_height - y - 1)] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

static void rotate180_rgb565(const uint16_t * src, uint16_t * dst, int32_t width, int32_t height, int32_t src_stride,
                             int32_t dest_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_RGB565(src, dst, width, height, src_stride)) {
        return ;
    }

    src_stride /= sizeof(uint16_t);
    dest_stride /= sizeof(uint16_t);

    for(int32_t y = 0; y < height; ++y) {
        int32_t dstIndex = (height - y - 1) * dest_stride;
        int32_t srcIndex = y * src_stride;
        for(int32_t x = 0; x < width; ++x) {
            dst[dstIndex + width - x - 1] = src[srcIndex + x];
        }
    }
}

static void rotate90_rgb565(const uint16_t * src, uint16_t * dst, int32_t src_width, int32_t src_height,
                            int32_t src_stride,
                            int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_RGB565(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    src_stride /= sizeof(uint16_t);
    dst_stride /= sizeof(uint16_t);

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = (src_width - x - 1);
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex * dst_stride + y] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

#endif


#if LV_DRAW_SW_SUPPORT_L8

static void rotate90_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                        int32_t src_stride,
                        int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE90_L8(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = (src_width - x - 1);
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex * dst_stride + y] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

static void rotate180_l8(const uint8_t * src, uint8_t * dst, int32_t width, int32_t height, int32_t src_stride,
                         int32_t dest_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE180_L8(src, dst, width, height, src_stride)) {
        return ;
    }

    for(int32_t y = 0; y < height; ++y) {
        int32_t dstIndex = (height - y - 1) * dest_stride;
        int32_t srcIndex = y * src_stride;
        for(int32_t x = 0; x < width; ++x) {
            dst[dstIndex + width - x - 1] = src[srcIndex + x];
        }
    }
}

static void rotate270_l8(const uint8_t * src, uint8_t * dst, int32_t src_width, int32_t src_height,
                         int32_t src_stride,
                         int32_t dst_stride)
{
    if(LV_RESULT_OK == LV_DRAW_SW_ROTATE270_L8(src, dst, src_width, src_height, src_stride, dst_stride)) {
        return ;
    }

    for(int32_t x = 0; x < src_width; ++x) {
        int32_t dstIndex = x * dst_stride;
        int32_t srcIndex = x;
        for(int32_t y = 0; y < src_height; ++y) {
            dst[dstIndex + (src_height - y - 1)] = src[srcIndex];
            srcIndex += src_stride;
        }
    }
}

#endif

#endif /*LV_USE_DRAW_SW*/
