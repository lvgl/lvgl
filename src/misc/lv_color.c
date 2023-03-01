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

LV_ATTRIBUTE_FAST_MEM void lv_color_fill(lv_color_t * buf, lv_color_t color, uint32_t px_num)
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

uint8_t lv_color_format_get_size(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_NATIVE_REVERSED:
            return LV_COLOR_DEPTH / 8;
        case LV_COLOR_FORMAT_NATIVE_ALPHA_REVERSED:
            return LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_ARGB2222:
            return 1;
        case LV_COLOR_FORMAT_A8L8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
            return 2;

        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_ARGB8565:
            return 3;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return 4;

        case LV_COLOR_FORMAT_UNKNOWN:
        default:
            return 0;
    }
}

bool lv_color_format_has_alpha(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_NATIVE_ALPHA_REVERSED:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_ARGB2222:
        case LV_COLOR_FORMAT_ARGB4444:
        case LV_COLOR_FORMAT_A8L8:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB8888:
            return true;
        default:
            return false;
    }
}

static uint8_t bit_2_to_8[4] = {0, 85, 170, 255};
static uint8_t bit_4_to_8[16] = {0,   17,  34,  51,  68,  85,  102, 119,
                                 136, 153, 170, 187, 204, 221, 238, 255
                                };
#if LV_COLOR_DEPTH != 16
static uint8_t bit_6_to_8[64] = {0,   4,   8,   12,  16,  20,  24,  28,
                                 32,  36,  40,  45,  49,  53,  57,  61,
                                 65,  69,  73,  77,  81,  85,  89,  93,
                                 97,  101, 105, 109, 113, 117, 121, 125,
                                 130, 134, 138, 142, 146, 150, 154, 158,
                                 162, 166, 170, 174, 178, 182, 186, 190,
                                 194, 198, 202, 206, 210, 215, 219, 223,
                                 227, 231, 235, 239, 243, 247, 251, 255
                                };
#endif

static uint8_t bit_5_to_8[32] = {0,   8,   16,  25,  33,  41,  49,  58,
                                 66,  74,  82,  90,  99,  107, 115, 123,
                                 132, 140, 148, 156, 165, 173, 181, 189,
                                 197, 206, 214, 222, 230, 239, 247, 255
                                };

void lv_color_to_native(const uint8_t * src_buf, lv_color_format_t src_cf, lv_color_t * c_out, lv_opa_t * a_out,
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
            lv_color_fill(c_out, alpha_color, px_cnt);
            lv_memcpy(a_out, src_buf, px_cnt);
            break;
        case LV_COLOR_FORMAT_A8L8:
            c = lv_color_black();
            for(i = 0; i < px_cnt; i++) {
                c_out[i] = lv_color_mix(c, alpha_color, src_buf[0]);
                a_out[i] = src_buf[1];
                src_buf += 2;
            }
            break;
        case LV_COLOR_FORMAT_ARGB2222:
            for(i = 0; i < px_cnt; i++) {
                c_out[i] = lv_color_make(bit_2_to_8[(src_buf[i]) & 0x30], bit_2_to_8[(src_buf[i]) & 0x0C],
                                         bit_2_to_8[(src_buf[i]) & 0x03]);
                a_out[i] = bit_2_to_8[(src_buf[i]) & 0xC0];
            }
            break;
        case LV_COLOR_FORMAT_RGB565:
#if LV_COLOR_DEPTH == 16
            lv_memcpy(c_out, src_buf, px_cnt * 2);
            lv_memset(a_out, 0xff, px_cnt);
#else
            for(i = 0; i < px_cnt; i++) {
                tmp = src_buf[0] + (src_buf[1] << 8);
                *c_out = lv_color_make(bit_5_to_8[tmp >> 11], bit_6_to_8[(tmp & 0x07E0) >> 5], bit_5_to_8[tmp & 0x001F]);
                src_buf += 2;
            }
#endif
            break;
        case LV_COLOR_FORMAT_ARGB1555:
            for(i = 0; i < px_cnt; i++) {
                tmp = src_buf[0] + (src_buf[1] << 8);
                c_out[i] = lv_color_make(bit_5_to_8[(tmp & 0x7C00) >> 10], bit_5_to_8[(tmp & 0x03E0) >> 5], bit_5_to_8[tmp & 0x001F]);
                a_out[i] = tmp & 0x8000 ? 0xFF : 0x00;
                src_buf += 2;
            }
            break;
        case LV_COLOR_FORMAT_ARGB4444:
            for(i = 0; i < px_cnt; i++) {
                tmp = src_buf[0] + (src_buf[1] << 8);
                c_out[i] = lv_color_make(bit_4_to_8[(tmp & 0x0F00) >> 8], bit_4_to_8[(tmp & 0x00F0) >> 4], bit_4_to_8[tmp & 0x000F]);
                a_out[i] = bit_4_to_8[(tmp & 0xF000) >> 12];
                src_buf += 2;
            }
            break;
        case LV_COLOR_FORMAT_ARGB8565:
            for(i = 0; i < px_cnt; i++) {
#if LV_COLOR_DEPTH == 16
                lv_color_set_int(&c_out[i], src_buf[0] + (src_buf[1] << 8));
#else
                tmp = src_buf[0] + (src_buf[1] << 8);
                c_out[i] = lv_color_make(bit_5_to_8[tmp >> 11], bit_6_to_8[(tmp & 0x07E0) >> 5], bit_5_to_8[tmp & 0x001F]);
#endif
                a_out[i] = src_buf[2];
                src_buf += 3;
            }
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
            lv_color_fill(c_out, lv_color_hex(0x000000), px_cnt);
            lv_memset(a_out, 0xFF, px_cnt);
            break;
    }
}


void lv_color_from_native(const lv_color_t * src_buf, uint8_t * dest_buf, lv_color_format_t dest_cf, uint32_t px_cnt)
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
        case LV_COLOR_FORMAT_A8L8:
            for(i = 0; i < px_cnt; i++) {
                dest_buf[0] = lv_color_brightness(src_buf[i]);
                dest_buf[1] = 0xff;
            }
            break;
        case LV_COLOR_FORMAT_ARGB2222:
            for(i = 0; i < px_cnt; i++) {
                lv_color16_t c16 = lv_color_to16(src_buf[i]);
                dest_buf[i] = 0xC0 + ((c16.red >> 3) << 4) + ((c16.green >> 4) << 2) + (c16.blue >> 3);
            }
            break;
        case LV_COLOR_FORMAT_ARGB4444:
            for(i = 0; i < px_cnt; i++) {
                lv_color16_t c16 = lv_color_to16(src_buf[i]);
                dest_buf[0] = ((c16.green >> 2) << 4) + (c16.blue >> 1);
                dest_buf[1] = 0xF0 + (c16.red >> 1);
                dest_buf += 2;
            }
            break;
        case LV_COLOR_FORMAT_RGB565:
#if LV_COLOR_DEPTH == 16
            lv_memcpy(dest_buf, src_buf, px_cnt * 2);
#else
            for(i = 0; i < px_cnt; i++) {
                *((lv_color16_t *)dest_buf) = lv_color_to16(src_buf[i]);
                dest_buf += 2;
            }
#endif
            break;
        case LV_COLOR_FORMAT_ARGB1555:
            for(i = 0; i < px_cnt; i++) {
                lv_color16_t c16 = lv_color_to16(src_buf[i]);
                dest_buf[0] = (((c16.green >> 1) & 0x07) << 5) + c16.blue;
                dest_buf[1] = 0x80 + (c16.red << 2) + (c16.green >> 4);
                dest_buf += 2;
            }
            break;
        case LV_COLOR_FORMAT_ARGB8565:
            for(i = 0; i < px_cnt; i++) {
                uint16_t tmp = lv_color16_to_int(lv_color_to16(src_buf[i]));
                dest_buf[0] = tmp & 0xff;
                dest_buf[1] = tmp >> 8;
                dest_buf[2] = 0xff;
                dest_buf += 3;
            }
            break;
        case LV_COLOR_FORMAT_RGB888:
#if LV_COLOR_DEPTH == 24
            lv_memcpy(dest_buf, src_buf, px_cnt * 3);
#else
            for(i = 0; i < px_cnt; i++) {
                lv_color24_t c24 = lv_color_to24(src_buf[i]);
                dest_buf[0] = c24.blue;
                dest_buf[1] = c24.green;
                dest_buf[2] = c24.red;
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
                lv_color24_t c24 = lv_color_to24(src_buf[i]);
                dest_buf[0] = c24.blue;
                dest_buf[1] = c24.green;
                dest_buf[2] = c24.red;
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

void lv_color_from_native_alpha(const uint8_t * src_buf, uint8_t * dest_buf, lv_color_format_t dest_cf, uint32_t px_cnt)
{
    uint32_t i;
    switch(dest_cf) {
        case LV_COLOR_FORMAT_L8:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                dest_buf[0] = lv_color_brightness(color);
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_A8:
            for(i = 0; i < px_cnt; i++) {
                dest_buf[i] = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_A8L8:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                dest_buf[0] = lv_color_brightness(color);
                dest_buf[1] = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_ARGB2222:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color16_t c16 = lv_color_to16(color);
                lv_opa_t opa = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];
                dest_buf[i] = (opa & 0xC0) + ((c16.red >> 3) << 4) + ((c16.green >> 4) << 2) + (c16.blue >> 3);
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_ARGB4444:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color16_t c16 = lv_color_to16(color);
                lv_opa_t opa = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];
                dest_buf[0] = ((c16.green >> 2) << 4) + (c16.blue >> 1);
                dest_buf[1] = (opa & 0xF0) + (c16.red >> 1);
                dest_buf += 2;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_RGB565:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color16_t c16 = lv_color_to16(color);
                *((uint16_t *) dest_buf) = lv_color16_to_int(c16);
                dest_buf += 2;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_ARGB1555:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color16_t c16 = lv_color_to16(color);
                lv_opa_t opa = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];
                dest_buf[0] = (((c16.green >> 1) & 0x07) << 5) + c16.blue;
                dest_buf[1] = (opa & 0x80) + (c16.red << 2) + (c16.green >> 4);
                dest_buf += 2;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_ARGB8565:
#if LV_COLOR_DEPTH == 16
            lv_memcpy(dest_buf, src_buf, px_cnt * 3);

#else
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color16_t c16 = lv_color_to16(color);
                *((uint16_t *) dest_buf) = lv_color16_to_int(c16);
                dest_buf[2] = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];;
                dest_buf += 3;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
#endif
            break;
        case LV_COLOR_FORMAT_RGB888:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color24_t c24 = lv_color_to24(color);
                dest_buf[0] = c24.blue;
                dest_buf[1] = c24.green;
                dest_buf[2] = c24.red;
                dest_buf += 3;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color24_t c24 = lv_color_to24(color);
                dest_buf[0] = c24.blue;
                dest_buf[1] = c24.green;
                dest_buf[2] = c24.red;
                dest_buf[3] = 0xff;
                dest_buf += 4;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
            }
            break;
        case LV_COLOR_FORMAT_ARGB8888:
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
            lv_memcpy(dest_buf, src_buf, px_cnt * 4);
#else
            for(i = 0; i < px_cnt; i++) {
                lv_color_t color = lv_color_from_buf(src_buf);
                lv_color24_t c24 = lv_color_to24(color);
                dest_buf[0] = c24.blue;
                dest_buf[1] = c24.green;
                dest_buf[2] = c24.red;
                dest_buf[3] = src_buf[LV_COLOR_FORMAT_NATIVE_ALPHA_OFS];
                dest_buf += 4;
                src_buf += LV_COLOR_FORMAT_NATIVE_ALPHA_SIZE;
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



lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl)
{
    return LV_COLOR_MIX(lv_color_white(), c, lvl);
}

lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl)
{
    return LV_COLOR_MIX(lv_color_black(), c, lvl);
}

lv_color_t lv_color_change_lightness(lv_color_t c, lv_opa_t lvl)
{
    /*It'd be better to convert the color to HSL, change L and convert back to RGB.*/
    if(lvl == LV_OPA_50) return c;
    else if(lvl < LV_OPA_50) return lv_color_darken(c, (LV_OPA_50 - lvl) * 2);
    else return lv_color_lighten(c, (lvl - LV_OPA_50) * 2);
}

/**
 * Convert a HSV color to RGB
 * @param h hue [0..359]
 * @param s saturation [0..100]
 * @param v value [0..100]
 * @return the given RGB color in RGB (with LV_COLOR_DEPTH depth)
 */
lv_color_t lv_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v)
{
    h = (uint32_t)((uint32_t)h * 255) / 360;
    s = (uint16_t)((uint16_t)s * 255) / 100;
    v = (uint16_t)((uint16_t)v * 255) / 100;

    uint8_t r, g, b;

    uint8_t region, remainder, p, q, t;

    if(s == 0) {
        return lv_color_make(v, v, v);
    }

    region    = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch(region) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        default:
            r = v;
            g = p;
            b = q;
            break;
    }

    lv_color_t result = lv_color_make(r, g, b);
    return result;
}

/**
 * Convert a 32-bit RGB color to HSV
 * @param r8 8-bit red
 * @param g8 8-bit green
 * @param b8 8-bit blue
 * @return the given RGB color in HSV
 */
lv_color_hsv_t lv_color_rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8)
{
    uint16_t r = ((uint32_t)r8 << 10) / 255;
    uint16_t g = ((uint32_t)g8 << 10) / 255;
    uint16_t b = ((uint32_t)b8 << 10) / 255;

    uint16_t rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    uint16_t rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    lv_color_hsv_t hsv;

    // https://en.wikipedia.org/wiki/HSL_and_HSV#Lightness
    hsv.v = (100 * rgbMax) >> 10;

    int32_t delta = rgbMax - rgbMin;
    if(delta < 3) {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    // https://en.wikipedia.org/wiki/HSL_and_HSV#Saturation
    hsv.s = 100 * delta / rgbMax;
    if(hsv.s < 3) {
        hsv.h = 0;
        return hsv;
    }

    // https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma
    int32_t h;
    if(rgbMax == r)
        h = (((g - b) << 10) / delta) + (g < b ? (6 << 10) : 0); // between yellow & magenta
    else if(rgbMax == g)
        h = (((b - r) << 10) / delta) + (2 << 10); // between cyan & yellow
    else if(rgbMax == b)
        h = (((r - g) << 10) / delta) + (4 << 10); // between magenta & cyan
    else
        h = 0;
    h *= 60;
    h >>= 10;
    if(h < 0) h += 360;

    hsv.h = h;
    return hsv;
}

/**
 * Convert a color to HSV
 * @param color color
 * @return the given color in HSV
 */
lv_color_hsv_t lv_color_to_hsv(lv_color_t color)
{
    lv_color32_t color32;
    color32 = lv_color_to32(color);
    return lv_color_rgb_to_hsv(color32.red, color32.green, color32.blue);
}


lv_color_t lv_palette_main(lv_palette_t p)
{
    static const lv_color_t colors[] = {
        LV_COLOR_MAKE(0xF4, 0x43, 0x36), LV_COLOR_MAKE(0xE9, 0x1E, 0x63), LV_COLOR_MAKE(0x9C, 0x27, 0xB0), LV_COLOR_MAKE(0x67, 0x3A, 0xB7),
        LV_COLOR_MAKE(0x3F, 0x51, 0xB5), LV_COLOR_MAKE(0x21, 0x96, 0xF3), LV_COLOR_MAKE(0x03, 0xA9, 0xF4), LV_COLOR_MAKE(0x00, 0xBC, 0xD4),
        LV_COLOR_MAKE(0x00, 0x96, 0x88), LV_COLOR_MAKE(0x4C, 0xAF, 0x50), LV_COLOR_MAKE(0x8B, 0xC3, 0x4A), LV_COLOR_MAKE(0xCD, 0xDC, 0x39),
        LV_COLOR_MAKE(0xFF, 0xEB, 0x3B), LV_COLOR_MAKE(0xFF, 0xC1, 0x07), LV_COLOR_MAKE(0xFF, 0x98, 0x00), LV_COLOR_MAKE(0xFF, 0x57, 0x22),
        LV_COLOR_MAKE(0x79, 0x55, 0x48), LV_COLOR_MAKE(0x60, 0x7D, 0x8B), LV_COLOR_MAKE(0x9E, 0x9E, 0x9E)
    };

    if(p >= _LV_PALETTE_LAST) {
        LV_LOG_WARN("Invalid palette: %d", p);
        return lv_color_black();
    }

    return colors[p];

}

lv_color_t lv_palette_lighten(lv_palette_t p, uint8_t lvl)
{
    static const lv_color_t colors[][5] = {
        {LV_COLOR_MAKE(0xEF, 0x53, 0x50), LV_COLOR_MAKE(0xE5, 0x73, 0x73), LV_COLOR_MAKE(0xEF, 0x9A, 0x9A), LV_COLOR_MAKE(0xFF, 0xCD, 0xD2), LV_COLOR_MAKE(0xFF, 0xEB, 0xEE)},
        {LV_COLOR_MAKE(0xEC, 0x40, 0x7A), LV_COLOR_MAKE(0xF0, 0x62, 0x92), LV_COLOR_MAKE(0xF4, 0x8F, 0xB1), LV_COLOR_MAKE(0xF8, 0xBB, 0xD0), LV_COLOR_MAKE(0xFC, 0xE4, 0xEC)},
        {LV_COLOR_MAKE(0xAB, 0x47, 0xBC), LV_COLOR_MAKE(0xBA, 0x68, 0xC8), LV_COLOR_MAKE(0xCE, 0x93, 0xD8), LV_COLOR_MAKE(0xE1, 0xBE, 0xE7), LV_COLOR_MAKE(0xF3, 0xE5, 0xF5)},
        {LV_COLOR_MAKE(0x7E, 0x57, 0xC2), LV_COLOR_MAKE(0x95, 0x75, 0xCD), LV_COLOR_MAKE(0xB3, 0x9D, 0xDB), LV_COLOR_MAKE(0xD1, 0xC4, 0xE9), LV_COLOR_MAKE(0xED, 0xE7, 0xF6)},
        {LV_COLOR_MAKE(0x5C, 0x6B, 0xC0), LV_COLOR_MAKE(0x79, 0x86, 0xCB), LV_COLOR_MAKE(0x9F, 0xA8, 0xDA), LV_COLOR_MAKE(0xC5, 0xCA, 0xE9), LV_COLOR_MAKE(0xE8, 0xEA, 0xF6)},
        {LV_COLOR_MAKE(0x42, 0xA5, 0xF5), LV_COLOR_MAKE(0x64, 0xB5, 0xF6), LV_COLOR_MAKE(0x90, 0xCA, 0xF9), LV_COLOR_MAKE(0xBB, 0xDE, 0xFB), LV_COLOR_MAKE(0xE3, 0xF2, 0xFD)},
        {LV_COLOR_MAKE(0x29, 0xB6, 0xF6), LV_COLOR_MAKE(0x4F, 0xC3, 0xF7), LV_COLOR_MAKE(0x81, 0xD4, 0xFA), LV_COLOR_MAKE(0xB3, 0xE5, 0xFC), LV_COLOR_MAKE(0xE1, 0xF5, 0xFE)},
        {LV_COLOR_MAKE(0x26, 0xC6, 0xDA), LV_COLOR_MAKE(0x4D, 0xD0, 0xE1), LV_COLOR_MAKE(0x80, 0xDE, 0xEA), LV_COLOR_MAKE(0xB2, 0xEB, 0xF2), LV_COLOR_MAKE(0xE0, 0xF7, 0xFA)},
        {LV_COLOR_MAKE(0x26, 0xA6, 0x9A), LV_COLOR_MAKE(0x4D, 0xB6, 0xAC), LV_COLOR_MAKE(0x80, 0xCB, 0xC4), LV_COLOR_MAKE(0xB2, 0xDF, 0xDB), LV_COLOR_MAKE(0xE0, 0xF2, 0xF1)},
        {LV_COLOR_MAKE(0x66, 0xBB, 0x6A), LV_COLOR_MAKE(0x81, 0xC7, 0x84), LV_COLOR_MAKE(0xA5, 0xD6, 0xA7), LV_COLOR_MAKE(0xC8, 0xE6, 0xC9), LV_COLOR_MAKE(0xE8, 0xF5, 0xE9)},
        {LV_COLOR_MAKE(0x9C, 0xCC, 0x65), LV_COLOR_MAKE(0xAE, 0xD5, 0x81), LV_COLOR_MAKE(0xC5, 0xE1, 0xA5), LV_COLOR_MAKE(0xDC, 0xED, 0xC8), LV_COLOR_MAKE(0xF1, 0xF8, 0xE9)},
        {LV_COLOR_MAKE(0xD4, 0xE1, 0x57), LV_COLOR_MAKE(0xDC, 0xE7, 0x75), LV_COLOR_MAKE(0xE6, 0xEE, 0x9C), LV_COLOR_MAKE(0xF0, 0xF4, 0xC3), LV_COLOR_MAKE(0xF9, 0xFB, 0xE7)},
        {LV_COLOR_MAKE(0xFF, 0xEE, 0x58), LV_COLOR_MAKE(0xFF, 0xF1, 0x76), LV_COLOR_MAKE(0xFF, 0xF5, 0x9D), LV_COLOR_MAKE(0xFF, 0xF9, 0xC4), LV_COLOR_MAKE(0xFF, 0xFD, 0xE7)},
        {LV_COLOR_MAKE(0xFF, 0xCA, 0x28), LV_COLOR_MAKE(0xFF, 0xD5, 0x4F), LV_COLOR_MAKE(0xFF, 0xE0, 0x82), LV_COLOR_MAKE(0xFF, 0xEC, 0xB3), LV_COLOR_MAKE(0xFF, 0xF8, 0xE1)},
        {LV_COLOR_MAKE(0xFF, 0xA7, 0x26), LV_COLOR_MAKE(0xFF, 0xB7, 0x4D), LV_COLOR_MAKE(0xFF, 0xCC, 0x80), LV_COLOR_MAKE(0xFF, 0xE0, 0xB2), LV_COLOR_MAKE(0xFF, 0xF3, 0xE0)},
        {LV_COLOR_MAKE(0xFF, 0x70, 0x43), LV_COLOR_MAKE(0xFF, 0x8A, 0x65), LV_COLOR_MAKE(0xFF, 0xAB, 0x91), LV_COLOR_MAKE(0xFF, 0xCC, 0xBC), LV_COLOR_MAKE(0xFB, 0xE9, 0xE7)},
        {LV_COLOR_MAKE(0x8D, 0x6E, 0x63), LV_COLOR_MAKE(0xA1, 0x88, 0x7F), LV_COLOR_MAKE(0xBC, 0xAA, 0xA4), LV_COLOR_MAKE(0xD7, 0xCC, 0xC8), LV_COLOR_MAKE(0xEF, 0xEB, 0xE9)},
        {LV_COLOR_MAKE(0x78, 0x90, 0x9C), LV_COLOR_MAKE(0x90, 0xA4, 0xAE), LV_COLOR_MAKE(0xB0, 0xBE, 0xC5), LV_COLOR_MAKE(0xCF, 0xD8, 0xDC), LV_COLOR_MAKE(0xEC, 0xEF, 0xF1)},
        {LV_COLOR_MAKE(0xBD, 0xBD, 0xBD), LV_COLOR_MAKE(0xE0, 0xE0, 0xE0), LV_COLOR_MAKE(0xEE, 0xEE, 0xEE), LV_COLOR_MAKE(0xF5, 0xF5, 0xF5), LV_COLOR_MAKE(0xFA, 0xFA, 0xFA)},
    };

    if(p >= _LV_PALETTE_LAST) {
        LV_LOG_WARN("Invalid palette: %d", p);
        return lv_color_black();
    }

    if(lvl == 0 || lvl > 5) {
        LV_LOG_WARN("Invalid level: %d. Must be 1..5", lvl);
        return lv_color_black();
    }

    lvl--;

    return colors[p][lvl];
}

lv_color_t lv_palette_darken(lv_palette_t p, uint8_t lvl)
{
    static const lv_color_t colors[][4] = {
        {LV_COLOR_MAKE(0xE5, 0x39, 0x35), LV_COLOR_MAKE(0xD3, 0x2F, 0x2F), LV_COLOR_MAKE(0xC6, 0x28, 0x28), LV_COLOR_MAKE(0xB7, 0x1C, 0x1C)},
        {LV_COLOR_MAKE(0xD8, 0x1B, 0x60), LV_COLOR_MAKE(0xC2, 0x18, 0x5B), LV_COLOR_MAKE(0xAD, 0x14, 0x57), LV_COLOR_MAKE(0x88, 0x0E, 0x4F)},
        {LV_COLOR_MAKE(0x8E, 0x24, 0xAA), LV_COLOR_MAKE(0x7B, 0x1F, 0xA2), LV_COLOR_MAKE(0x6A, 0x1B, 0x9A), LV_COLOR_MAKE(0x4A, 0x14, 0x8C)},
        {LV_COLOR_MAKE(0x5E, 0x35, 0xB1), LV_COLOR_MAKE(0x51, 0x2D, 0xA8), LV_COLOR_MAKE(0x45, 0x27, 0xA0), LV_COLOR_MAKE(0x31, 0x1B, 0x92)},
        {LV_COLOR_MAKE(0x39, 0x49, 0xAB), LV_COLOR_MAKE(0x30, 0x3F, 0x9F), LV_COLOR_MAKE(0x28, 0x35, 0x93), LV_COLOR_MAKE(0x1A, 0x23, 0x7E)},
        {LV_COLOR_MAKE(0x1E, 0x88, 0xE5), LV_COLOR_MAKE(0x19, 0x76, 0xD2), LV_COLOR_MAKE(0x15, 0x65, 0xC0), LV_COLOR_MAKE(0x0D, 0x47, 0xA1)},
        {LV_COLOR_MAKE(0x03, 0x9B, 0xE5), LV_COLOR_MAKE(0x02, 0x88, 0xD1), LV_COLOR_MAKE(0x02, 0x77, 0xBD), LV_COLOR_MAKE(0x01, 0x57, 0x9B)},
        {LV_COLOR_MAKE(0x00, 0xAC, 0xC1), LV_COLOR_MAKE(0x00, 0x97, 0xA7), LV_COLOR_MAKE(0x00, 0x83, 0x8F), LV_COLOR_MAKE(0x00, 0x60, 0x64)},
        {LV_COLOR_MAKE(0x00, 0x89, 0x7B), LV_COLOR_MAKE(0x00, 0x79, 0x6B), LV_COLOR_MAKE(0x00, 0x69, 0x5C), LV_COLOR_MAKE(0x00, 0x4D, 0x40)},
        {LV_COLOR_MAKE(0x43, 0xA0, 0x47), LV_COLOR_MAKE(0x38, 0x8E, 0x3C), LV_COLOR_MAKE(0x2E, 0x7D, 0x32), LV_COLOR_MAKE(0x1B, 0x5E, 0x20)},
        {LV_COLOR_MAKE(0x7C, 0xB3, 0x42), LV_COLOR_MAKE(0x68, 0x9F, 0x38), LV_COLOR_MAKE(0x55, 0x8B, 0x2F), LV_COLOR_MAKE(0x33, 0x69, 0x1E)},
        {LV_COLOR_MAKE(0xC0, 0xCA, 0x33), LV_COLOR_MAKE(0xAF, 0xB4, 0x2B), LV_COLOR_MAKE(0x9E, 0x9D, 0x24), LV_COLOR_MAKE(0x82, 0x77, 0x17)},
        {LV_COLOR_MAKE(0xFD, 0xD8, 0x35), LV_COLOR_MAKE(0xFB, 0xC0, 0x2D), LV_COLOR_MAKE(0xF9, 0xA8, 0x25), LV_COLOR_MAKE(0xF5, 0x7F, 0x17)},
        {LV_COLOR_MAKE(0xFF, 0xB3, 0x00), LV_COLOR_MAKE(0xFF, 0xA0, 0x00), LV_COLOR_MAKE(0xFF, 0x8F, 0x00), LV_COLOR_MAKE(0xFF, 0x6F, 0x00)},
        {LV_COLOR_MAKE(0xFB, 0x8C, 0x00), LV_COLOR_MAKE(0xF5, 0x7C, 0x00), LV_COLOR_MAKE(0xEF, 0x6C, 0x00), LV_COLOR_MAKE(0xE6, 0x51, 0x00)},
        {LV_COLOR_MAKE(0xF4, 0x51, 0x1E), LV_COLOR_MAKE(0xE6, 0x4A, 0x19), LV_COLOR_MAKE(0xD8, 0x43, 0x15), LV_COLOR_MAKE(0xBF, 0x36, 0x0C)},
        {LV_COLOR_MAKE(0x6D, 0x4C, 0x41), LV_COLOR_MAKE(0x5D, 0x40, 0x37), LV_COLOR_MAKE(0x4E, 0x34, 0x2E), LV_COLOR_MAKE(0x3E, 0x27, 0x23)},
        {LV_COLOR_MAKE(0x54, 0x6E, 0x7A), LV_COLOR_MAKE(0x45, 0x5A, 0x64), LV_COLOR_MAKE(0x37, 0x47, 0x4F), LV_COLOR_MAKE(0x26, 0x32, 0x38)},
        {LV_COLOR_MAKE(0x75, 0x75, 0x75), LV_COLOR_MAKE(0x61, 0x61, 0x61), LV_COLOR_MAKE(0x42, 0x42, 0x42), LV_COLOR_MAKE(0x21, 0x21, 0x21)},
    };

    if(p >= _LV_PALETTE_LAST) {
        LV_LOG_WARN("Invalid palette: %d", p);
        return lv_color_black();
    }

    if(lvl == 0 || lvl > 4) {
        LV_LOG_WARN("Invalid level: %d. Must be 1..4", lvl);
        return lv_color_black();
    }

    lvl--;

    return colors[p][lvl];
}
