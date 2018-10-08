/**
 * @file lv_color.h
 *
 */

#ifndef LV_COLOR_H
#define LV_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

/*Error checking*/
#if LV_COLOR_DEPTH == 24
#error "LV_COLOR_DEPTH  24 is deprecated. Use LV_COLOR_DEPTH  32 instead (lv_conf.h)"
#endif

#if LV_COLOR_DEPTH != 32 && LV_COLOR_SCREEN_TRANSP != 0
#error "LV_COLOR_SCREEN_TRANSP requires LV_COLOR_DEPTH == 32. Set it in lv_conf.h"
#endif

#if LV_COLOR_DEPTH != 16 && LV_COLOR_16_SWAP != 0
#error "LV_COLOR_16_SWAP requires LV_COLOR_DEPTH == 16. Set it in lv_conf.h"
#endif


#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LV_COLOR_WHITE   LV_COLOR_MAKE(0xFF,0xFF,0xFF)
#define LV_COLOR_SILVER  LV_COLOR_MAKE(0xC0,0xC0,0xC0)
#define LV_COLOR_GRAY    LV_COLOR_MAKE(0x80,0x80,0x80)
#define LV_COLOR_BLACK   LV_COLOR_MAKE(0x00,0x00,0x00)
#define LV_COLOR_RED     LV_COLOR_MAKE(0xFF,0x00,0x00)
#define LV_COLOR_MAROON  LV_COLOR_MAKE(0x80,0x00,0x00)
#define LV_COLOR_YELLOW  LV_COLOR_MAKE(0xFF,0xFF,0x00)
#define LV_COLOR_OLIVE   LV_COLOR_MAKE(0x80,0x80,0x00)
#define LV_COLOR_LIME    LV_COLOR_MAKE(0x00,0xFF,0x00)
#define LV_COLOR_GREEN   LV_COLOR_MAKE(0x00,0x80,0x00)
#define LV_COLOR_CYAN    LV_COLOR_MAKE(0x00,0xFF,0xFF)
#define LV_COLOR_AQUA    LV_COLOR_CYAN
#define LV_COLOR_TEAL    LV_COLOR_MAKE(0x00,0x80,0x80)
#define LV_COLOR_BLUE    LV_COLOR_MAKE(0x00,0x00,0xFF)
#define LV_COLOR_NAVY    LV_COLOR_MAKE(0x00,0x00,0x80)
#define LV_COLOR_MAGENTA LV_COLOR_MAKE(0xFF,0x00,0xFF)
#define LV_COLOR_PURPLE  LV_COLOR_MAKE(0x80,0x00,0x80)
#define LV_COLOR_ORANGE  LV_COLOR_MAKE(0xFF,0xA5,0x00)

#define LV_OPA_TRANSP    0
#define LV_OPA_0         0
#define LV_OPA_10        25
#define LV_OPA_20        51
#define LV_OPA_30        76
#define LV_OPA_40        102
#define LV_OPA_50        127
#define LV_OPA_60        153
#define LV_OPA_70        178
#define LV_OPA_80        204
#define LV_OPA_90        229
#define LV_OPA_100       255
#define LV_OPA_COVER     255

#define LV_OPA_MIN      16      /*Opacities below this will be transparent*/
#define LV_OPA_MAX      251     /*Opacities above this will fully cover*/

#if LV_COLOR_DEPTH == 1
#define LV_COLOR_SIZE           8
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_SIZE           8
#elif LV_COLOR_DEPTH == 16
#define LV_COLOR_SIZE           16
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_SIZE           32
#else
#error "Invalid LV_COLOR_DEPTH in lv_conf.h! Set it to 1, 8, 16 or 32!"
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef union
{
    uint8_t blue  :1;
    uint8_t green :1;
    uint8_t red   :1;
    uint8_t full  :1;
} lv_color1_t;

typedef union
{
    struct
    {
        uint8_t blue  :2;
        uint8_t green :3;
        uint8_t red   :3;
    };
    uint8_t full;
} lv_color8_t;

typedef union
{
    struct
    {
#if LV_COLOR_16_SWAP == 0
        uint16_t blue  :5;
        uint16_t green :6;
        uint16_t red   :5;
#else
        uint16_t green_h :3;
        uint16_t red   :5;
        uint16_t blue  :5;
        uint16_t green_l :3;
#endif
    };
    uint16_t full;
} lv_color16_t;

typedef union
{
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };
    uint32_t full;
} lv_color32_t;

#if LV_COLOR_DEPTH == 1
typedef uint8_t lv_color_int_t;
typedef lv_color1_t lv_color_t;
#elif LV_COLOR_DEPTH == 8
typedef uint8_t lv_color_int_t;
typedef lv_color8_t lv_color_t;
#elif LV_COLOR_DEPTH == 16
typedef uint16_t lv_color_int_t;
typedef lv_color16_t lv_color_t;
#elif LV_COLOR_DEPTH == 32
typedef uint32_t lv_color_int_t;
typedef lv_color32_t lv_color_t;
#else
#error "Invalid LV_COLOR_DEPTH in lv_conf.h! Set it to 1, 8, 16 or 32!"
#endif

typedef uint8_t lv_opa_t;

typedef struct
{
    uint16_t h;
    uint8_t s;
    uint8_t v;
} lv_color_hsv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*In color conversations:
 * - When converting to bigger color type the LSB weight of 1 LSB is calculated
 *   E.g. 16 bit Red has 5 bits
 *         8 bit Red has 2 bits
 *        ----------------------
 *        8 bit red LSB = (2^5 - 1) / (2^2 - 1) = 31 / 3 = 10
 *
 * - When calculating to smaller color type simply shift out the LSBs
 *   E.g.  8 bit Red has 2 bits
 *        16 bit Red has 5 bits
 *        ----------------------
 *         Shift right with 5 - 3 = 2
 */

static inline uint8_t lv_color_to1(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    return color.full;
#elif LV_COLOR_DEPTH == 8
    if((color.red   & 0x4) ||
            (color.green & 0x4) ||
            (color.blue  & 0x2)) {
        return 1;
    } else {
        return 0;
    }
#elif LV_COLOR_DEPTH == 16
#  if LV_COLOR_16_SWAP == 0
    if((color.red   & 0x10) ||
            (color.green & 0x20) ||
            (color.blue  & 0x10)) {
        return 1;
#  else
    if((color.red   & 0x10) ||
            (color.green_h & 0x20) ||
            (color.blue  & 0x10)) {
        return 1;
#  endif
    } else {
        return 0;
    }
#elif LV_COLOR_DEPTH == 32
    if((color.red   & 0x80) ||
            (color.green & 0x80) ||
            (color.blue  & 0x80)) {
        return 1;
    } else {
        return 0;
    }
#endif
}

static inline uint8_t lv_color_to8(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    if(color.full == 0) return 0;
    else return 0xFF;
#elif LV_COLOR_DEPTH == 8
    return color.full;
#elif LV_COLOR_DEPTH == 16

#  if LV_COLOR_16_SWAP == 0
    lv_color8_t ret;
    ret.red = color.red >> 2;       /* 5 - 3  = 2*/
    ret.green = color.green >> 3;   /* 6 - 3  = 3*/
    ret.blue = color.blue >> 3;     /* 5 - 2  = 3*/
    return ret.full;
#  else
    lv_color8_t ret;
    ret.red = color.red >> 2;       /* 5 - 3  = 2*/
    ret.green = color.green_h;      /* 6 - 3  = 3*/
    ret.blue = color.blue >> 3;     /* 5 - 2  = 3*/
    return ret.full;
#  endif
#elif LV_COLOR_DEPTH == 32
    lv_color8_t ret;
    ret.red = color.red >> 5;       /* 8 - 3  = 5*/
    ret.green = color.green >> 5;   /* 8 - 3  = 5*/
    ret.blue = color.blue >> 6;     /* 8 - 2  = 6*/
    return ret.full;
#endif
}

static inline uint16_t lv_color_to16(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    if(color.full == 0) return 0;
    else return 0xFFFF;
#elif LV_COLOR_DEPTH == 8
    lv_color16_t ret;
#  if LV_COLOR_16_SWAP == 0
    ret.red = color.red * 4;       /*(2^5 - 1)/(2^3 - 1) = 31/7 = 4*/
    ret.green = color.green * 9;   /*(2^6 - 1)/(2^3 - 1) = 63/7 = 9*/
    ret.blue = color.blue * 10;    /*(2^5 - 1)/(2^2 - 1) = 31/3 = 10*/
#  else
    ret.red = color.red * 4;
    uint8_t g_tmp = color.green * 9;
    ret.green_h = (g_tmp & 0x1F) >> 3;
    ret.green_l = g_tmp & 0x07;
    ret.blue = color.blue * 10;
#  endif
    return ret.full;
#elif LV_COLOR_DEPTH == 16
    return color.full;
#elif LV_COLOR_DEPTH == 32
    lv_color16_t ret;
#  if LV_COLOR_16_SWAP == 0
    ret.red = color.red >> 3;       /* 8 - 5  = 3*/
    ret.green = color.green >> 2;   /* 8 - 6  = 2*/
    ret.blue = color.blue >> 3;     /* 8 - 5  = 3*/
#  else
    ret.red = color.red >> 3;
    ret.green_h = (color.green & 0xE0) >> 5;
    ret.green_l = (color.green & 0x1C) >> 2;
    ret.blue = color.blue >> 3;
#  endif
    return ret.full;
#endif
}

static inline uint32_t lv_color_to32(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    if(color.full == 0) return 0;
    else return 0xFFFFFFFF;
#elif LV_COLOR_DEPTH == 8
    lv_color32_t ret;
    ret.red = color.red * 36;        /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    ret.green = color.green * 36;    /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    ret.blue = color.blue * 85;      /*(2^8 - 1)/(2^2 - 1) = 255/3 = 85*/
    ret.alpha = 0xFF;
    return ret.full;
#elif LV_COLOR_DEPTH == 16
#  if LV_COLOR_16_SWAP == 0
    lv_color32_t ret;
    ret.red = color.red * 8;       /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.green = color.green * 4;   /*(2^8 - 1)/(2^6 - 1) = 255/63 = 4*/
    ret.blue = color.blue * 8;     /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.alpha = 0xFF;
    return ret.full;
#  else
    lv_color32_t ret;
    ret.red = color.red * 8;                                    /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.green = ((color.green_h << 3) + color.green_l) * 4;     /*(2^8 - 1)/(2^6 - 1) = 255/63 = 4*/
    ret.blue = color.blue * 8;                                   /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.alpha = 0xFF;
    return ret.full;
#  endif
#elif LV_COLOR_DEPTH == 32
    return color.full;
#endif
}

static inline lv_color_t lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix)
{
    lv_color_t ret;
#if LV_COLOR_DEPTH != 1
    /*LV_COLOR_DEPTH == 8, 16 or 32*/
    ret.red =   (uint16_t)((uint16_t) c1.red * mix + (c2.red * (255 - mix))) >> 8;
#  if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP
    /*If swapped Green is in 2 parts*/
    uint16_t g_1 = (c1.green_h << 3) + c1.green_l;
    uint16_t g_2 = (c2.green_h << 3) + c2.green_l;
    uint16_t g_out = (uint16_t)((uint16_t) g_1 * mix + (g_2 * (255 - mix))) >> 8;
    ret.green_h = g_out >> 3;
    ret.green_l = g_out & 0x7;
#  else
    ret.green = (uint16_t)((uint16_t) c1.green * mix + (c2.green * (255 - mix))) >> 8;
#  endif
    ret.blue =  (uint16_t)((uint16_t) c1.blue * mix + (c2.blue * (255 - mix))) >> 8;
# if LV_COLOR_DEPTH == 32
    ret.alpha = 0xFF;
# endif
#else
    /*LV_COLOR_DEPTH == 1*/
    ret.full = mix > LV_OPA_50 ? c1.full : c2.full;
#endif

    return ret;
}

/**
 * Get the brightness of a color
 * @param color a color
 * @return the brightness [0..255]
 */
static inline uint8_t lv_color_brightness(lv_color_t color)
{
    lv_color32_t c32;
    c32.full = lv_color_to32(color);
    uint16_t bright = 3 * c32.red + c32.blue + 4 * c32.green;
    return (uint16_t) bright >> 3;
}

/* The most simple macro to create a color from R,G and B values
 * The order of bit field is different on Big-endian and Little-endian machines*/
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#if LV_COLOR_DEPTH == 1
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){(b8 >> 7 | g8 >> 7 | r8 >> 7)})
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{b8 >> 6, g8 >> 5, r8 >> 5}})
#elif LV_COLOR_DEPTH == 16
#  if LV_COLOR_16_SWAP == 0
#    define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{b8 >> 3, g8 >> 2, r8 >> 3}})
#  else
#    define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{g8 >> 5, r8 >> 3, b8 >> 3, (g8 >> 2) & 0x7}})
#  endif
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{b8, g8, r8, 0xff}})            /*Fix 0xff alpha*/
#endif
#else
#if LV_COLOR_DEPTH == 1
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){(r8 >> 7 | g8 >> 7 | b8 >> 7)})
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{r8 >> 6, g8 >> 5, b8 >> 5}})
#elif LV_COLOR_DEPTH == 16
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{r8 >> 3, g8 >> 2, b8 >> 3}})
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_MAKE(r8, g8, b8) ((lv_color_t){{0xff, r8, g8, b8}})            /*Fix 0xff alpha*/
#endif
#endif

#define LV_COLOR_HEX(c) LV_COLOR_MAKE(((uint32_t)((uint32_t)c >> 16) & 0xFF), \
                                ((uint32_t)((uint32_t)c >> 8) & 0xFF), \
                                ((uint32_t) c & 0xFF))

/*Usage LV_COLOR_HEX3(0x16C) which means LV_COLOR_HEX(0x1166CC)*/
#define LV_COLOR_HEX3(c) LV_COLOR_MAKE((((c >> 4) & 0xF0) | ((c >> 8) & 0xF)),   \
                                ((uint32_t)(c & 0xF0)       | ((c & 0xF0) >> 4)), \
                                ((uint32_t)(c & 0xF)         | ((c & 0xF) << 4)))


/**
 * Convert a HSV color to RGB
 * @param h hue [0..359]
 * @param s saturation [0..100]
 * @param v value [0..100]
 * @return the given RGB color in RGB (with LV_COLOR_DEPTH depth)
 */
lv_color_t lv_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * Convert an RGB color to HSV
 * @param r red
 * @param g green
 * @param b blue
 * @return the given RGB color n HSV
 */
lv_color_hsv_t lv_color_rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_COLOR*/
