/**
 * @file lv_color.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_color.h"
#include "lv_log.h"

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
    uintptr_t buf_int = (uintptr_t) buf;
    if(buf_int & 0x3) {
        *buf = color;
        buf++;
        px_num--;
    }

    uint32_t c32 = (uint32_t)color.full + ((uint32_t)color.full << 16);
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
    return lv_color_mix(lv_color_white(), c, lvl);
}

lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl)
{
    return lv_color_mix(lv_color_black(), c, lvl);
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
    color32.full = lv_color_to32(color);
    return lv_color_rgb_to_hsv(color32.ch.red, color32.ch.green, color32.ch.blue);
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


