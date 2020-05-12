/**
 * @file lv_color.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_color.h"
#include "lv_math.h"

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

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_color_fill(lv_color_t * buf, lv_color_t color, uint32_t px_num)
{
#if LV_COLOR_DEPTH == 16
    uintptr_t buf_int = (uintptr_t) buf;
    if(buf_int & 0x3) {
        *buf = color;
        buf++;
        px_num--;
    }

    uint32_t c32 = color.full + (color.full << 16);
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
        px_num --;
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
        px_num --;
    }
#endif
}


lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl)
{
    return lv_color_mix(LV_COLOR_WHITE, c, lvl);
}


lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl)
{
    return lv_color_mix(LV_COLOR_BLACK, c, lvl);
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
    if(LV_MATH_ABS(delta) < 3) {
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
    color32.full = lv_color_to32(color);
    return lv_color_rgb_to_hsv(color32.ch.red, color32.ch.green, color32.ch.blue);
}
