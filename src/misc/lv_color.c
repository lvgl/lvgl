/**
 * @file lv_color.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_color.h"
#include "lv_log.h"
#include "../misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_color_t lv_color_filter_shade_cb(const lv_color_filter_dsc_t * dsc, lv_color_t c, lv_opa_t opa);

/**********************
 *  GLOBAL VARIABLES
 **********************/

const lv_color_filter_dsc_t lv_color_filter_shade = {.filter_cb = lv_color_filter_shade_cb};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint8_t lv_color_format_get_bpp(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_A1:
            return 1;
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_A2:
            return 2;
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_A4:
            return 4;
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_I8:
            return 8;

        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB565:
            return 16;

        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_RGB888:
            return 24;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return 32;

        case LV_COLOR_FORMAT_UNKNOWN:
        default:
            return 0;
    }
}

bool lv_color_format_has_alpha(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A1:
        case LV_COLOR_FORMAT_A2:
        case LV_COLOR_FORMAT_A4:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_ARGB8888:
            return true;
        default:
            return false;
    }
}

lv_color32_t lv_color_to_32(lv_color_t color, lv_opa_t opa)
{
    lv_color32_t c32;
    c32.red = color.red;
    c32.green = color.green;
    c32.blue = color.blue;
    c32.alpha = opa;
    return c32;
}

uint16_t lv_color_to_u16(lv_color_t color)
{
    return ((color.red & 0xF8) << 8) + ((color.green & 0xFC) << 3) + ((color.blue & 0xF8) >> 3);
}

uint32_t lv_color_to_u32(lv_color_t color)
{
    return (uint32_t)((uint32_t)0xff << 24) + (color.red << 16) + (color.green << 8) + (color.blue);
}

lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl)
{

    return lv_color_mix(lv_color_white(), c, lvl);
}

lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl)
{
    return lv_color_mix(lv_color_black(), c, lvl);
}

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
lv_color_hsv_t lv_color_to_hsv(lv_color_t c)
{
    return lv_color_rgb_to_hsv(c.red, c.green, c.blue);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Helper function to easily create color filters
 * @param dsc       pointer to a color filter descriptor
 * @param c         the color to modify
 * @param opa       the intensity of the modification
 *                      - LV_OPA_50:    do nothing
 *                      - < LV_OPA_50:  darken
 *                      - LV_OPA_0:     fully black
 *                      - > LV_OPA_50:  lighten
 *                      - LV_OPA_100:   fully white
 * @return          the modified color
 */
static lv_color_t lv_color_filter_shade_cb(const lv_color_filter_dsc_t * dsc, lv_color_t c, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    if(opa == LV_OPA_50) return c;
    if(opa < LV_OPA_50) return lv_color_lighten(c, (LV_OPA_50 - opa) * 2);
    else return lv_color_darken(c, (opa - LV_OPA_50 * LV_OPA_50) * 2);
}
