/**
 * @file color.h
 * 
 */

#ifndef COLOR_H
#define COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_COLOR != 0

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define COLOR_BLACK   COLOR_MAKE(0x00,0x00,0x00)
#define COLOR_WHITE   COLOR_MAKE(0xFF,0xFF,0xFF)
#define COLOR_RED     COLOR_MAKE(0xFF,0x00,0x00)
#define COLOR_LIME    COLOR_MAKE(0x00,0xFF,0x00)
#define COLOR_BLUE    COLOR_MAKE(0x00,0x00,0xFF)
#define COLOR_YELLOW  COLOR_MAKE(0xFF,0xFF,0x00)
#define COLOR_CYAN    COLOR_MAKE(0x00,0xFF,0xFF)
#define COLOR_AQUA    COLOR_CYAN
#define COLOR_MAGENTA COLOR_MAKE(0xFF,0x00,0xFF)
#define COLOR_SILVER  COLOR_MAKE(0xC0,0xC0,0xC0)
#define COLOR_GRAY    COLOR_MAKE(0x80,0x80,0x80)
#define COLOR_MARRON  COLOR_MAKE(0x80,0x00,0x00)
#define COLOR_OLIVE   COLOR_MAKE(0x80,0x80,0x00)
#define COLOR_GREEN   COLOR_MAKE(0x00,0x80,0x00)
#define COLOR_PURPLE  COLOR_MAKE(0x80,0x00,0x80)
#define COLOR_TEAL    COLOR_MAKE(0x00,0x80,0x80)
#define COLOR_NAVY    COLOR_MAKE(0x00,0x00,0x80)
#define COLOR_ORANGE  COLOR_MAKE(0xFF,0xA5,0x00)

#define OPA_TRANSP    0
#define OPA_0         0
#define OPA_10        25
#define OPA_20        51
#define OPA_30        76
#define OPA_40        102
#define OPA_50        127
#define OPA_60        153
#define OPA_70        178
#define OPA_80        204
#define OPA_90        229
#define OPA_100       256
#define OPA_COVER     256


/**********************
 *      TYPEDEFS
 **********************/

typedef union
{
    uint8_t blue  :1;
    uint8_t green :1;
    uint8_t red   :1;
    uint8_t full  :1;
}color1_t;

typedef union
{
    struct
    {
        uint8_t blue  :2;
        uint8_t green :3;
        uint8_t red   :3;
    };
    uint8_t full;
}color8_t;

typedef union
{
    struct
    {
        uint16_t blue  :5;
        uint16_t green :6;
        uint16_t red   :5;
    };
    uint16_t full;
}color16_t;

typedef union
{
    struct
    {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    };
    uint32_t full;
}color24_t;

#if COLOR_DEPTH == 1
typedef uint8_t color_int_t;
typedef color1_t color_t;
#elif COLOR_DEPTH == 8
typedef uint8_t color_int_t;
typedef color8_t color_t;
#elif COLOR_DEPTH == 16
typedef uint16_t color_int_t;
typedef color16_t color_t;
#elif COLOR_DEPTH == 24
typedef uint32_t color_int_t;
typedef color24_t color_t;
#else
#error "Invalid COLOR_DEPTH in misc_conf.h! Set it to 1, 8, 16 or 24!"
#endif

typedef uint16_t opa_t;     /* 0..256 (not 255 to cab be normalized with >> 8*/

typedef struct
{
    uint16_t h;
    uint8_t s;
    uint8_t v;
} color_hsv_t;

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

static inline uint8_t color_to1(color_t color)
{
#if COLOR_DEPTH == 1
	return color.full;
#elif COLOR_DEPTH == 8
    if((color.red   & 0b100) ||
       (color.green & 0b100) ||
	   (color.blue  & 0b10)) {
    	return 1;
    } else {
    	return 0;
    }
#elif COLOR_DEPTH == 16
    if((color.red   & 0b10000) ||
       (color.green & 0b100000) ||
	   (color.blue  & 0b10000)) {
    	return 1;
    } else {
    	return 0;
    }
#elif COLOR_DEPTH == 24
    if((color.red   & 0x80) ||
       (color.green & 0x80) ||
	   (color.blue  & 0x80)) {
    	return 1;
    } else {
    	return 0;
    }
#endif
}

static inline uint8_t color_to8(color_t color)
{
#if COLOR_DEPTH == 1
    if(color.full == 0) return 0;
    else return 0xFF;
#elif COLOR_DEPTH == 8
    return color.full;
#elif COLOR_DEPTH == 16
    color8_t ret;
    ret.red = color.red >> 2;       /* 5 - 3  = 2*/
    ret.green = color.green >> 3;   /* 6 - 3  = 3*/
    ret.blue = color.blue >> 3;     /* 5 - 2  = 3*/
    return ret.full;
#elif COLOR_DEPTH == 24
    color8_t ret;
    ret.red = color.red >> 5;       /* 8 - 3  = 5*/
    ret.green = color.green >> 5;   /* 8 - 3  = 5*/
    ret.blue = color.blue >> 6;     /* 8 - 2  = 6*/
    return ret.full;
#endif
}

static inline uint16_t color_to16(color_t color)
{
#if COLOR_DEPTH == 1
    if(color.full == 0) return 0;
    else return 0xFFFF;
#elif COLOR_DEPTH == 8
    color16_t ret;
    ret.red = color.red * 4;       /*(2^5 - 1)/(2^3 - 1) = 31/7 = 4*/
    ret.green = color.green * 9;   /*(2^6 - 1)/(2^3 - 1) = 63/7 = 9*/
    ret.blue = color.blue * 10;    /*(2^5 - 1)/(2^2 - 1) = 31/3 = 10*/
    return ret.full;
#elif COLOR_DEPTH == 16
    return color.full;
#elif COLOR_DEPTH == 24
    color16_t ret;
    ret.red = color.red >> 3;       /* 8 - 5  = 3*/
    ret.green = color.green >> 2;   /* 8 - 6  = 2*/
    ret.blue = color.blue >> 3;     /* 8 - 5  = 3*/
    return ret.full;
#endif
}

static inline uint32_t color_to24(color_t color)
{
#if COLOR_DEPTH == 1
    if(color.full == 0) return 0;
    else return 0xFFFFFF;
#elif COLOR_DEPTH == 8
    color24_t ret;
    ret.red = color.red * 36;        /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    ret.green = color.green * 36;    /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    ret.blue = color.blue * 85;     /*(2^8 - 1)/(2^2 - 1) = 255/3 = 85*/
    return ret.full;
#elif COLOR_DEPTH == 16
    color24_t ret;
    ret.red = color.red * 8;       /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    ret.green = color.green * 4;   /*(2^8 - 1)/(2^6 - 1) = 255/63 = 4*/
    ret.blue = color.blue * 8;     /*(2^8 - 1)/(2^5 - 1) = 255/31 = 8*/
    return ret.full;
#elif COLOR_DEPTH == 24
    return color.full;
#endif
}

static inline color_t color_mix(color_t c1, color_t c2, uint16_t mix)
{
    color_t ret;
    ret.red =   (uint16_t)((uint16_t) c1.red * mix + (c2.red * (256 - mix))) >> 8;  
    ret.green = (uint16_t)((uint16_t) c1.green * mix + (c2.green * (256 - mix))) >> 8;  
    ret.blue =  (uint16_t)((uint16_t) c1.blue * mix + (c2.blue * (256 - mix))) >> 8;  
    return ret;
}

/**
 * Get the brightness of a color
 * @param color a color
 * @return the brightness [0..255]
 */
static inline uint8_t color_brightness(color_t color) 
{
    color24_t c24;
    c24.full = color_to24(color);
    uint16_t bright = 3 * c24.red + c24.blue + 4 * c24.green;
    return (uint16_t) bright >> 3;
}

#if COLOR_DEPTH == 1
#define COLOR_MAKE(r8, g8, b8) ((color_t){b8 >> 7 | g8 >> 7 | r8 >> 7})
#elif COLOR_DEPTH == 8
#define COLOR_MAKE(r8, g8, b8) ((color_t){{b8 >> 6, g8 >> 5, r8 >> 5}})
#elif COLOR_DEPTH == 16
#define COLOR_MAKE(r8, g8, b8) ((color_t){{b8 >> 3, g8 >> 2, r8 >> 3}})
#elif COLOR_DEPTH == 24
#define COLOR_MAKE(r8, g8, b8) ((color_t){{b8, g8, r8}})
#endif

#define COLOR_HEX(c) COLOR_MAKE(((uint32_t)((uint32_t)c >> 16) & 0xFF), \
                                ((uint32_t)((uint32_t)c >> 8) & 0xFF), \
                                ((uint32_t) c & 0xFF))

/*Usage COLOR_HEX3(0x16C) which means COLOR_HEX(0x1166CC)*/
#define COLOR_HEX3(c) COLOR_MAKE((((c >> 4) & 0xF0) | ((c >> 8) & 0xF)),   \
                                ((uint32_t)(c & 0xF0)       | ((c & 0xF0) >> 4)), \
                                ((uint32_t)(c & 0xF)         | ((c & 0xF) << 4)))


/**
 * Convert a HSV color to RGB
 * @param h hue [0..359]
 * @param s saturation [0..100]
 * @param v value [0..100]
 * @return the given RGB color in RGB (with COLOR_DEPTH depth)
 */
color_t color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * Convert an RGB color to HSV
 * @param r red
 * @param g green
 * @param b blue
 * @return the given RGB color n HSV
 */
color_hsv_t color_rgb_to_hsv(uint8_t r, uint8_t g, uint8_t b);


/**********************
 *      MACROS
 **********************/

#endif /*USE_COLOR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
