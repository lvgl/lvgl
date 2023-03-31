/**
 * @file lv_color.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_color.h"
#include "lv_log.h"
#include LV_COLOR_EXTERN_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_color_buf_fill(lv_color_t * buf, lv_color_t color, uint32_t px_num)
{
#if LV_COLOR_DEPTH == 16
    uintptr_t buf_int = (uintptr_t)buf;
    if(buf_int & 0x3) {
        *buf = color;
        buf++;
        px_num--;
    }

    uint32_t cint = lv_color_to_int(color);
    uint32_t c32 = (uint32_t)cint + ((uint32_t)cint << 16);
    uint32_t * buf32 = (uint32_t *)buf;

    while(px_num > 16) {
        *buf32 = c32;
        buf32++;
        *buf32 = c32;
        buf32++;
        *buf32 = c32;
        buf32++;
        *buf32 = c32;
        buf32++;

        *buf32 = c32;
        buf32++;
        *buf32 = c32;
        buf32++;
        *buf32 = c32;
        buf32++;
        *buf32 = c32;
        buf32++;

        px_num -= 16;
    }

    buf = (lv_color_t *)buf32;

    while(px_num) {
        *buf = color;
        buf++;
        px_num--;
    }
#else
    while(px_num > 16) {
        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;

        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;

        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;

        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;
        *buf = color;
        buf++;

        px_num -= 16;
    }
    while(px_num) {
        *buf = color;
        buf++;
        px_num--;
    }
#endif
}

void lv_color_buf_to_native(const uint8_t * src_buf, lv_color_format_t src_cf, lv_color_t * c_out, lv_opa_t * a_out,
                            lv_color_t alpha_color, uint32_t px_cnt)
{
    uint32_t i;
    uint32_t tmp;
    lv_color_t c;
    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
#if LV_COLOR_DEPTH == 8
            lv_memcpy(c_out, src_buf, px_cnt);
            lv_memset(a_out, 0xff, px_cnt);
#else
            lv_memset(a_out, 0xff, px_cnt);
            c = lv_color_black();
            for(i = 0; i < px_cnt; i++) {
                c_out[i] = lv_color_mix(alpha_color, c, src_buf[i]);
            }
#endif
            break;
        case LV_COLOR_FORMAT_A8:
            lv_color_buf_fill(c_out, alpha_color, px_cnt);
            lv_memcpy(a_out, src_buf, px_cnt);
            break;
        case LV_COLOR_FORMAT_RGB565:
            lv_memset(a_out, 0xff, px_cnt);
#if LV_COLOR_DEPTH == 16
            lv_memcpy(c_out, src_buf, px_cnt * 2);
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
            for(i = 0; i < px_cnt; i++) {
                c_out[i].red = src_buf[0] >> 3;
                c_out[i].green = (src_buf[0] & 0x7) << 3 | (src_buf[1] >> 5);
                c_out[i].blue = src_buf[1] & 0x1F;
#if LV_COLOR_DEPTH == 32
                c_out[i].alpha = 0xff;
#endif
                src_buf += 2;
            }
#endif
            break;
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_RGB888:
            lv_memset(a_out, 0xFF, px_cnt);
            tmp = LV_COLOR_FORMAT_RGB888 ? 3 : 4;
            for(i = 0; i < px_cnt; i++) {
                c_out[i] = lv_color_make(src_buf[2], src_buf[1], src_buf[0]);
                src_buf += tmp;
            }
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            for(i = 0; i < px_cnt; i++) {
                c_out[i] = lv_color_make(src_buf[2], src_buf[1], src_buf[0]);
                a_out[i] = src_buf[3];
                src_buf += 4;
            }
            break;
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_RGB565A8:
        default:
            LV_LOG_WARN("Can't convert to %d format", src_cf);
            lv_color_buf_fill(c_out, lv_color_hex(0x000000), px_cnt);
            lv_memset(a_out, 0xFF, px_cnt);
            break;
    }
}

void lv_color_buf_from_native(const lv_color_t * src_buf, uint8_t * dest_buf, lv_color_format_t dest_cf,
                              uint32_t px_cnt)
{
    uint32_t i;
    switch(dest_cf) {
        case LV_COLOR_FORMAT_L8:
            for(i = 0; i < px_cnt; i++) {
                dest_buf[i] = lv_color_brightness(src_buf[i]);
            }
            break;
        case LV_COLOR_FORMAT_A8:
            lv_memset(dest_buf, 0xff, px_cnt);
            break;
        case LV_COLOR_FORMAT_RGB565: {
#if LV_COLOR_DEPTH == 16
                lv_memcpy(dest_buf, src_buf, px_cnt * 2);
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
                uint16_t * dest_buf16 = (uint16_t *) dest_buf;
                for(i = 0; i < px_cnt; i++) {
                    dest_buf16 = ((src_buf[i].red >> 3) << 11) | (src_buf[i].green >> 2) << 6 | (src_buf[i].green >> 3);
                    dest_buf += 2;
                }
#endif
                break;
            }
        case LV_COLOR_FORMAT_RGB888:
#if LV_COLOR_DEPTH == 24
            lv_memcpy(dest_buf, src_buf, px_cnt * 3);
#else
            for(i = 0; i < px_cnt; i++) {
                lv_color32_t c32 = lv_color_to_xrgb8888(src_buf[i]);
                dest_buf[0] = c32.blue;
                dest_buf[1] = c32.green;
                dest_buf[2] = c32.red;
                dest_buf += 3;
            }
#endif
            break;
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888:
#if LV_COLOR_DEPTH == 32
            lv_memcpy(dest_buf, src_buf, px_cnt * 4);
#else
            for(i = 0; i < px_cnt; i++) {
                lv_color32_t c32 = lv_color_to_xrgb8888(src_buf[i]);
                dest_buf[0] = c32.blue;
                dest_buf[1] = c32.green;
                dest_buf[2] = c32.red;
                dest_buf[3] = 0xff;
                dest_buf += 4;
            }

#endif
            break;
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_RGB565A8:
        default:
            LV_LOG_WARN("Can't convert from %d format", dest_cf);
            return;
    }
}

void lv_color_buf_from_argb8888(const lv_color32_t * src_buf, uint8_t * dest_buf, lv_color_format_t dest_cf,
                                uint32_t px_cnt)
{
    uint32_t i;
    switch(dest_cf) {
        case LV_COLOR_FORMAT_L8:
            for(i = 0; i < px_cnt; i++) {
                uint16_t bright = (uint16_t)(3u * src_buf[i].red + src_buf[i].green + 4u * src_buf[i].blue);
                dest_buf[i] = bright >> 3;
            }
            break;
        case LV_COLOR_FORMAT_A8:
            for(i = 0; i < px_cnt; i++) {
                dest_buf[i] = src_buf[i].alpha;
            }
            break;
        case LV_COLOR_FORMAT_RGB565:
            for(i = 0; i < px_cnt; i++) {

                *((uint16_t *) dest_buf) = ((src_buf[i].red >> 3) << 11) +
                                           ((src_buf[i].green >> 2) << 6) +
                                           (src_buf[i].blue >> 3);
                dest_buf += 2;
            }
            break;
        case LV_COLOR_FORMAT_RGB888:
            for(i = 0; i < px_cnt; i++) {
                dest_buf[0] = src_buf[i].blue;
                dest_buf[1] = src_buf[i].green;
                dest_buf[2] = src_buf[i].red;
                dest_buf += 3;
            }
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            for(i = 0; i < px_cnt; i++) {
                dest_buf[0] = src_buf[i].blue;
                dest_buf[1] = src_buf[i].green;
                dest_buf[2] = src_buf[i].red;
                dest_buf[3] = 0xFF;
                dest_buf += 4;
            }
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            lv_memcpy(dest_buf, src_buf, px_cnt * 4);
            break;

        default:
            LV_LOG_WARN("Can't convert from %d format", dest_cf);
            return;
    }
}
