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
        case LV_COLOR_FORMAT_NEMA_TSC4:
            return 4;
        case LV_COLOR_FORMAT_NEMA_TSC6:
        case LV_COLOR_FORMAT_NEMA_TSC6A:
        case LV_COLOR_FORMAT_NEMA_TSC6AP:
            return 6;
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_ARGB2222:
            return 8;
        case LV_COLOR_FORMAT_NEMA_TSC12:
        case LV_COLOR_FORMAT_NEMA_TSC12A:
            return 12;
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
        case LV_COLOR_FORMAT_YUY2:
        case LV_COLOR_FORMAT_AL88:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
            return 16;

        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_RGB888:
            return 24;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
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
        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
        case LV_COLOR_FORMAT_AL88:
        case LV_COLOR_FORMAT_ARGB2222:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
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

uint8_t lv_color_format_get_size(lv_color_format_t cf)
{
    return (lv_color_format_get_bpp(cf) + 7) >> 3;
}

uint32_t lv_color_to_int(lv_color_t c)
{
    uint8_t * tmp = (uint8_t *) &c;
    return tmp[0] + (tmp[1] << 8) + (tmp[2] << 16);
}

bool lv_color_eq(lv_color_t c1, lv_color_t c2)
{
    return lv_color_to_int(c1) == lv_color_to_int(c2);
}

bool lv_color32_eq(lv_color32_t c1, lv_color32_t c2)
{
    return *((uint32_t *)&c1) == *((uint32_t *)&c2);
}

lv_color_t lv_color_hex(uint32_t c)
{
    lv_color_t ret;
    ret.red = (c >> 16) & 0xff;
    ret.green = (c >> 8) & 0xff;
    ret.blue = (c >> 0) & 0xff;
    return ret;
}

lv_color_t lv_color_make(uint8_t r, uint8_t g, uint8_t b)
{
    lv_color_t ret;
    ret.red = r;
    ret.green = g;
    ret.blue = b;
    return ret;
}

lv_color32_t lv_color32_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    lv_color32_t ret;
    ret.red = r;
    ret.green = g;
    ret.blue = b;
    ret.alpha = a;
    return ret;
}

lv_color_t lv_color_hex3(uint32_t c)
{
    return lv_color_make((uint8_t)(((c >> 4) & 0xF0) | ((c >> 8) & 0xF)), (uint8_t)((c & 0xF0) | ((c & 0xF0) >> 4)),
                         (uint8_t)((c & 0xF) | ((c & 0xF) << 4)));
}

uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_16_16_mix(uint16_t c1, uint16_t c2, uint8_t mix)
{
    if(mix == 255) return c1;
    if(mix == 0) return c2;
    if(c1 == c2) return c1;

    uint16_t ret;

    /* Source: https://stackoverflow.com/a/50012418/1999969*/
    mix = (uint32_t)((uint32_t)mix + 4) >> 3;

    /*0x7E0F81F = 0b00000111111000001111100000011111*/
    uint32_t bg = (uint32_t)(c2 | ((uint32_t)c2 << 16)) & 0x7E0F81F;
    uint32_t fg = (uint32_t)(c1 | ((uint32_t)c1 << 16)) & 0x7E0F81F;
    uint32_t result = ((((fg - bg) * mix) >> 5) + bg) & 0x7E0F81F;
    ret = (uint16_t)(result >> 16) | result;

    return ret;
}

lv_color_t lv_color_white(void)
{
    return lv_color_make(0xff, 0xff, 0xff);
}

lv_color_t lv_color_black(void)
{
    return lv_color_make(0x00, 0x00, 0x00);
}

void lv_color_premultiply(lv_color32_t * c)
{
    if(c->alpha == LV_OPA_COVER) {
        return;
    }

    if(c->alpha == LV_OPA_TRANSP) {
        lv_memzero(c, sizeof(lv_color32_t));
        return;
    }

    c->red = LV_OPA_MIX2(c->red, c->alpha);
    c->green = LV_OPA_MIX2(c->green, c->alpha);
    c->blue = LV_OPA_MIX2(c->blue, c->alpha);
}

void lv_color16_premultiply(lv_color16_t * c, lv_opa_t a)
{
    if(a == LV_OPA_COVER) {
        return;
    }

    if(a == LV_OPA_TRANSP) {
        lv_memzero(c, sizeof(lv_color16_t));
        return;
    }

    c->red = LV_OPA_MIX2(c->red, a);
    c->green = LV_OPA_MIX2(c->green, a);
    c->blue = LV_OPA_MIX2(c->blue, a);
}

uint8_t lv_color_luminance(lv_color_t c)
{
    return (uint8_t)((uint16_t)(77u * c.red + 151u * c.green + 28u * c.blue) >> 8);
}

uint8_t lv_color16_luminance(const lv_color16_t c)
{
    return (uint8_t)((uint16_t)(635u * c.red + 613u * c.green + 231u * c.blue) >> 8);
}

uint8_t lv_color24_luminance(const uint8_t * c)
{
    return (uint8_t)((uint16_t)(77u * c[2] + 151u * c[1] + 28u * c[0]) >> 8);
}

uint8_t lv_color32_luminance(lv_color32_t c)
{
    return (uint8_t)((uint16_t)(77u * c.red + 151u * c.green + 28u * c.blue) >> 8);
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
