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
    return lv_color_mix(lv_color_white(), c, lvl);
}

lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl)
{
    return lv_color_mix(lv_color_black(), c, lvl);
}

lv_color_t lv_color_change_lightness(lv_color_t c, lv_opa_t lvl)
{
    /* It'd be better to convert the color to HSL, change L and convert back to RGB.*/
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

 lv_color_t lv_color_get_palette_main(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_lighten_1(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_lighten_1();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_lighten_1();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_lighten_1();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_lighten_1();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_lighten_1();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_lighten_1();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_lighten_1();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_lighten_1();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_lighten_1();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_lighten_1();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_lighten_1();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_lighten_1();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_lighten_1();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_lighten_1();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_lighten_1();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_lighten_1();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_lighten_1();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_lighten_1();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_lighten_1();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_lighten_2(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_lighten_2();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_lighten_2();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_lighten_2();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_lighten_2();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_lighten_2();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_lighten_2();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_lighten_2();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_lighten_2();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_lighten_2();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_lighten_2();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_lighten_2();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_lighten_2();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_lighten_2();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_lighten_2();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_lighten_2();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_lighten_2();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_lighten_2();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_lighten_2();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_lighten_2();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_lighten_3(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_lighten_3();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_lighten_3();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_lighten_3();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_lighten_3();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_lighten_3();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_lighten_3();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_lighten_3();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_lighten_3();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_lighten_3();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_lighten_3();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_lighten_3();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_lighten_3();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_lighten_3();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_lighten_3();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_lighten_3();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_lighten_3();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_lighten_3();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_lighten_3();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_lighten_3();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_lighten_4(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_lighten_4();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_lighten_4();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_lighten_4();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_lighten_4();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_lighten_4();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_lighten_4();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_lighten_4();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_lighten_4();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_lighten_4();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_lighten_4();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_lighten_4();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_lighten_4();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_lighten_4();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_lighten_4();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_lighten_4();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_lighten_4();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_lighten_4();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_lighten_4();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_lighten_4();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_lighten_5(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_lighten_5();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_lighten_5();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_lighten_5();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_lighten_5();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_lighten_5();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_lighten_5();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_lighten_5();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_lighten_5();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_lighten_5();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_lighten_5();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_lighten_5();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_lighten_5();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_lighten_5();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_lighten_5();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_lighten_5();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_lighten_5();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_lighten_5();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_lighten_5();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_lighten_5();
    }

    return lv_color_black();
}

 lv_color_t lv_color_get_palette_darken_1(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_darken_1();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_darken_1();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_darken_1();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_darken_1();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_darken_1();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_darken_1();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_darken_1();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_darken_1();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_darken_1();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_darken_1();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_darken_1();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_darken_1();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_darken_1();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_darken_1();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_darken_1();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_darken_1();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_darken_1();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_darken_1();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_darken_1();
    }

    return lv_color_black();
}

 lv_color_t lv_color_get_palette_darken_2(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_darken_2();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_darken_2();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_darken_2();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_darken_2();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_darken_2();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_darken_2();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_darken_2();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_darken_2();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_darken_2();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_darken_2();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_darken_2();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_darken_2();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_darken_2();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_darken_2();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_darken_2();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_darken_2();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_darken_2();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_darken_2();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_darken_2();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_darken_3(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_darken_3();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_darken_3();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_darken_3();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_darken_3();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_darken_3();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_darken_3();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_darken_3();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_darken_3();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_darken_3();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_darken_3();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_darken_3();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_darken_3();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_darken_3();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_darken_3();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_darken_3();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_darken_3();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_darken_3();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_darken_3();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_darken_3();
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_darken_4(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_darken_4();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_darken_4();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_darken_4();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_darken_4();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_darken_4();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_darken_4();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_darken_4();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_darken_4();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_darken_4();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_darken_4();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_darken_4();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_darken_4();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_darken_4();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_darken_4();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_darken_4();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_darken_4();
        case LV_COLOR_PALETTE_BROWN:        return lv_color_brown_darken_4();
        case LV_COLOR_PALETTE_BLUE_GREY:    return lv_color_blue_grey_darken_4();
        case LV_COLOR_PALETTE_GREY:         return lv_color_grey_darken_4();
    }
    return lv_color_black();
}

lv_color_t lv_color_get_palette_accent_1(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_accent_1();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_accent_1();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_accent_1();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_accent_1();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_accent_1();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_accent_1();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_accent_1();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_accent_1();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_accent_1();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_accent_1();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_accent_1();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_accent_1();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_accent_1();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_accent_1();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_accent_1();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_accent_1();
        default:
            LV_LOG_WARN("color palette %d has no accent colors, use main color instead", palette);
            return lv_color_get_palette_main(palette);
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_accent_2(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_accent_2();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_accent_2();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_accent_2();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_accent_2();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_accent_2();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_accent_2();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_accent_2();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_accent_2();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_accent_2();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_accent_2();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_accent_2();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_accent_2();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_accent_2();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_accent_2();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_accent_2();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_accent_2();
        default:
            LV_LOG_WARN("color palette %d has no accent colors, use main color instead", palette);
            return lv_color_get_palette_main(palette);
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_accent_3(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_accent_3();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_accent_3();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_accent_3();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_accent_3();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_accent_3();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_accent_3();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_accent_3();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_accent_3();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_accent_3();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_accent_3();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_accent_3();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_accent_3();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_accent_3();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_accent_3();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_accent_3();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_accent_3();
        default:
            LV_LOG_WARN("color palette %d has no accent colors, use main color instead", palette);
            return lv_color_get_palette_main(palette);
    }
    return lv_color_black();
}

 lv_color_t lv_color_get_palette_accent_4(lv_color_palette_t palette)
{
    switch(palette) {
        case LV_COLOR_PALETTE_RED:          return lv_color_red_accent_4();
        case LV_COLOR_PALETTE_PINK:         return lv_color_pink_accent_4();
        case LV_COLOR_PALETTE_PURPLE:       return lv_color_purple_accent_4();
        case LV_COLOR_PALETTE_DEEP_PURPLE:  return lv_color_deep_purple_accent_4();
        case LV_COLOR_PALETTE_INDIGO:       return lv_color_indigo_accent_4();
        case LV_COLOR_PALETTE_BLUE:         return lv_color_blue_accent_4();
        case LV_COLOR_PALETTE_LIGHT_BLUE:   return lv_color_light_blue_accent_4();
        case LV_COLOR_PALETTE_CYAN:         return lv_color_cyan_accent_4();
        case LV_COLOR_PALETTE_TEAL:         return lv_color_teal_accent_4();
        case LV_COLOR_PALETTE_GREEN:        return lv_color_green_accent_4();
        case LV_COLOR_PALETTE_LIGHT_GREEN:  return lv_color_light_green_accent_4();
        case LV_COLOR_PALETTE_LIME:         return lv_color_lime_accent_4();
        case LV_COLOR_PALETTE_YELLOW:       return lv_color_yellow_accent_4();
        case LV_COLOR_PALETTE_AMBER:        return lv_color_amber_accent_4();
        case LV_COLOR_PALETTE_ORANGE:       return lv_color_orange_accent_4();
        case LV_COLOR_PALETTE_DEEP_ORANGE:  return lv_color_deep_orange_accent_4();
        default:
            LV_LOG_WARN("color palette %d has no accent colors, use main color instead", palette);
            return lv_color_get_palette_main(palette);
    }
    return lv_color_black();
}
