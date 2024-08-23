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
#include "../lv_conf_internal.h"
#include "lv_assert.h"
#include "lv_math.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
LV_EXPORT_CONST_INT(LV_COLOR_DEPTH);

#if LV_COLOR_DEPTH == 8
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 2
#elif LV_COLOR_DEPTH == 16
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 3
#elif LV_COLOR_DEPTH == 24
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 4
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_NATIVE_WITH_ALPHA_SIZE 4
#endif

/**
 * Opacity percentages.
 */

enum {
    LV_OPA_TRANSP = 0,
    LV_OPA_0      = 0,
    LV_OPA_10     = 25,
    LV_OPA_20     = 51,
    LV_OPA_30     = 76,
    LV_OPA_40     = 102,
    LV_OPA_50     = 127,
    LV_OPA_60     = 153,
    LV_OPA_70     = 178,
    LV_OPA_80     = 204,
    LV_OPA_90     = 229,
    LV_OPA_100    = 255,
    LV_OPA_COVER  = 255,
};

#define LV_OPA_MIN 2    /**< Opacities below this will be transparent */
#define LV_OPA_MAX 253  /**< Opacities above this will fully cover */

/**
 * Get the pixel size of a color format in bits, bpp
 * @param cf        a color format (`LV_COLOR_FORMAT_...`)
 * @return          the pixel size in bits
 * @sa              lv_color_format_get_bpp
 */
#define LV_COLOR_FORMAT_GET_BPP(cf) (       \
                                            (cf) == LV_COLOR_FORMAT_I1 ? 1 :        \
                                            (cf) == LV_COLOR_FORMAT_A1 ? 1 :        \
                                            (cf) == LV_COLOR_FORMAT_I2 ? 2 :        \
                                            (cf) == LV_COLOR_FORMAT_A2 ? 2 :        \
                                            (cf) == LV_COLOR_FORMAT_I4 ? 4 :        \
                                            (cf) == LV_COLOR_FORMAT_A4 ? 4 :        \
                                            (cf) == LV_COLOR_FORMAT_L8 ? 8 :        \
                                            (cf) == LV_COLOR_FORMAT_A8 ? 8 :        \
                                            (cf) == LV_COLOR_FORMAT_I8 ? 8 :        \
                                            (cf) == LV_COLOR_FORMAT_AL88 ? 16 :     \
                                            (cf) == LV_COLOR_FORMAT_RGB565 ? 16 :   \
                                            (cf) == LV_COLOR_FORMAT_RGB565A8 ? 16 : \
                                            (cf) == LV_COLOR_FORMAT_ARGB8565 ? 24 : \
                                            (cf) == LV_COLOR_FORMAT_RGB888 ? 24 :   \
                                            (cf) == LV_COLOR_FORMAT_ARGB8888 ? 32 : \
                                            (cf) == LV_COLOR_FORMAT_XRGB8888 ? 32 : \
                                            0                                       \
                                    )

/**
 * Get the pixel size of a color format in bytes
 * @param cf        a color format (`LV_COLOR_FORMAT_...`)
 * @return          the pixel size in bytes
 * @sa              lv_color_format_get_size
 */
#define LV_COLOR_FORMAT_GET_SIZE(cf) ((LV_COLOR_FORMAT_GET_BPP(cf) + 7) >> 3)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} lv_color_t;

typedef struct {
    uint16_t blue : 5;
    uint16_t green : 6;
    uint16_t red : 5;
} lv_color16_t;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} lv_color32_t;

typedef struct {
    uint16_t h;
    uint8_t s;
    uint8_t v;
} lv_color_hsv_t;

typedef struct {
    uint8_t lumi;
    uint8_t alpha;
} lv_color16a_t;

typedef enum {
    LV_COLOR_FORMAT_UNKNOWN           = 0,

    LV_COLOR_FORMAT_RAW               = 0x01,
    LV_COLOR_FORMAT_RAW_ALPHA         = 0x02,

    /*<=1 byte (+alpha) formats*/
    LV_COLOR_FORMAT_L8                = 0x06,
    LV_COLOR_FORMAT_I1                = 0x07,
    LV_COLOR_FORMAT_I2                = 0x08,
    LV_COLOR_FORMAT_I4                = 0x09,
    LV_COLOR_FORMAT_I8                = 0x0A,
    LV_COLOR_FORMAT_A8                = 0x0E,

    /*2 byte (+alpha) formats*/
    LV_COLOR_FORMAT_RGB565            = 0x12,
    LV_COLOR_FORMAT_ARGB8565          = 0x13,   /**< Not supported by sw renderer yet. */
    LV_COLOR_FORMAT_RGB565A8          = 0x14,   /**< Color array followed by Alpha array*/
    LV_COLOR_FORMAT_AL88              = 0x15,   /**< L8 with alpha >*/

    /*3 byte (+alpha) formats*/
    LV_COLOR_FORMAT_RGB888            = 0x0F,
    LV_COLOR_FORMAT_ARGB8888          = 0x10,
    LV_COLOR_FORMAT_XRGB8888          = 0x11,

    /*Formats not supported by software renderer but kept here so GPU can use it*/
    LV_COLOR_FORMAT_A1                = 0x0B,
    LV_COLOR_FORMAT_A2                = 0x0C,
    LV_COLOR_FORMAT_A4                = 0x0D,

    /* reference to https://wiki.videolan.org/YUV/ */
    /*YUV planar formats*/
    LV_COLOR_FORMAT_YUV_START         = 0x20,
    LV_COLOR_FORMAT_I420              = LV_COLOR_FORMAT_YUV_START,  /*YUV420 planar(3 plane)*/
    LV_COLOR_FORMAT_I422              = 0x21,  /*YUV422 planar(3 plane)*/
    LV_COLOR_FORMAT_I444              = 0x22,  /*YUV444 planar(3 plane)*/
    LV_COLOR_FORMAT_I400              = 0x23,  /*YUV400 no chroma channel*/
    LV_COLOR_FORMAT_NV21              = 0x24,  /*YUV420 planar(2 plane), UV plane in 'V, U, V, U'*/
    LV_COLOR_FORMAT_NV12              = 0x25,  /*YUV420 planar(2 plane), UV plane in 'U, V, U, V'*/

    /*YUV packed formats*/
    LV_COLOR_FORMAT_YUY2              = 0x26,  /*YUV422 packed like 'Y U Y V'*/
    LV_COLOR_FORMAT_UYVY              = 0x27,  /*YUV422 packed like 'U Y V Y'*/

    LV_COLOR_FORMAT_YUV_END           = LV_COLOR_FORMAT_UYVY,

    /*Color formats in which LVGL can render*/
#if LV_COLOR_DEPTH == 1
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_I1,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_I1,
#elif LV_COLOR_DEPTH == 8
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_L8,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_AL88,
#elif LV_COLOR_DEPTH == 16
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_RGB565,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_RGB565A8,
#elif LV_COLOR_DEPTH == 24
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_RGB888,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_ARGB8888,
#elif LV_COLOR_DEPTH == 32
    LV_COLOR_FORMAT_NATIVE            = LV_COLOR_FORMAT_XRGB8888,
    LV_COLOR_FORMAT_NATIVE_WITH_ALPHA = LV_COLOR_FORMAT_ARGB8888,
#else
#error "LV_COLOR_DEPTH should be 1, 8, 16, 24 or 32"
#endif

} lv_color_format_t;

#define LV_COLOR_FORMAT_IS_ALPHA_ONLY(cf) ((cf) >= LV_COLOR_FORMAT_A1 && (cf) <= LV_COLOR_FORMAT_A8)
#define LV_COLOR_FORMAT_IS_INDEXED(cf) ((cf) >= LV_COLOR_FORMAT_I1 && (cf) <= LV_COLOR_FORMAT_I8)
#define LV_COLOR_FORMAT_IS_YUV(cf)  ((cf) >= LV_COLOR_FORMAT_YUV_START && (cf) <= LV_COLOR_FORMAT_YUV_END)
#define LV_COLOR_INDEXED_PALETTE_SIZE(cf) ((cf) == LV_COLOR_FORMAT_I1 ? 2 :\
                                           (cf) == LV_COLOR_FORMAT_I2 ? 4 :\
                                           (cf) == LV_COLOR_FORMAT_I4 ? 16 :\
                                           (cf) == LV_COLOR_FORMAT_I8 ? 256 : 0)

/**********************
 * MACROS
 **********************/

#define LV_COLOR_MAKE(r8, g8, b8) {b8, g8, r8}

#define LV_OPA_MIX2(a1, a2) (((int32_t)(a1) * (a2)) >> 8)
#define LV_OPA_MIX3(a1, a2, a3) (((int32_t)(a1) * (a2) * (a3)) >> 16)

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the pixel size of a color format in bits, bpp
 * @param cf        a color format (`LV_COLOR_FORMAT_...`)
 * @return          the pixel size in bits
 * @sa              LV_COLOR_FORMAT_GET_BPP
 */
uint8_t lv_color_format_get_bpp(lv_color_format_t cf);

/**
 * Get the pixel size of a color format in bytes
 * @param cf        a color format (`LV_COLOR_FORMAT_...`)
 * @return          the pixel size in bytes
 * @sa              LV_COLOR_FORMAT_GET_SIZE
 */
uint8_t lv_color_format_get_size(lv_color_format_t cf);

/**
 * Check if a color format has alpha channel or not
 * @param src_cf    a color format (`LV_COLOR_FORMAT_...`)
 * @return          true: has alpha channel; false: doesn't have alpha channel
 */
bool lv_color_format_has_alpha(lv_color_format_t src_cf);

/**
 * Create an ARGB8888 color from RGB888 + alpha
 * @param color     an RGB888 color
 * @param opa       the alpha value
 * @return          the ARGB8888 color
 */
lv_color32_t lv_color_to_32(lv_color_t color, lv_opa_t opa);

/**
 * Convert an RGB888 color to an integer
 * @param c     an RGB888 color
 * @return      `c` as an integer
 */
uint32_t lv_color_to_int(lv_color_t c);

/**
 * Check if two RGB888 color are equal
 * @param c1    the first color
 * @param c2    the second color
 * @return      true: equal
 */
bool lv_color_eq(lv_color_t c1, lv_color_t c2);

/**
 * Check if two ARGB8888 color are equal
 * @param c1    the first color
 * @param c2    the second color
 * @return      true: equal
 */
bool lv_color32_eq(lv_color32_t c1, lv_color32_t c2);

/**
 * Create a color from 0x000000..0xffffff input
 * @param c     the hex input
 * @return      the color
 */
lv_color_t lv_color_hex(uint32_t c);

/**
 * Create an RGB888 color
 * @param r     the red channel (0..255)
 * @param g     the green channel (0..255)
 * @param b     the blue channel (0..255)
 * @return      the color
 */
lv_color_t lv_color_make(uint8_t r, uint8_t g, uint8_t b);

/**
 * Create an ARGB8888 color
 * @param r     the red channel (0..255)
 * @param g     the green channel (0..255)
 * @param b     the blue channel (0..255)
 * @param a     the alpha channel (0..255)
 * @return      the color
 */
lv_color32_t lv_color32_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * Create a color from 0x000..0xfff input
 * @param c     the hex input (e.g. 0x123 will be 0x112233)
 * @return      the color
 */
lv_color_t lv_color_hex3(uint32_t c);

/**
 * Convert am RGB888 color to RGB565 stored in `uint16_t`
 * @param color     and RGB888 color
 * @return          `color` as RGB565 on `uin16_t`
 */
uint16_t lv_color_to_u16(lv_color_t color);

/**
 * Convert am RGB888 color to XRGB8888 stored in `uint32_t`
 * @param color     and RGB888 color
 * @return          `color` as XRGB8888 on `uin32_t` (the alpha channel is always set to 0xFF)
 */
uint32_t lv_color_to_u32(lv_color_t color);

/**
 * Mix two RGB565 colors
 * @param c1        the first color (typically the foreground color)
 * @param c2        the second color  (typically the background color)
 * @param mix       0..255, or LV_OPA_0/10/20...
 * @return          mix == 0: c2
 *                  mix == 255: c1
 *                  mix == 128: 0.5 x c1 + 0.5 x c2
 */
uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_16_16_mix(uint16_t c1, uint16_t c2, uint8_t mix);

/**
 * Mix white to a color
 * @param c     the base color
 * @param lvl   the intensity of white (0: no change, 255: fully white)
 * @return      the mixed color
 */
lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl);

/**
 * Mix black to a color
 * @param c     the base color
 * @param lvl   the intensity of black (0: no change, 255: fully black)
 * @return      the mixed color
 */
lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl);

/**
 * Convert a HSV color to RGB
 * @param h hue [0..359]
 * @param s saturation [0..100]
 * @param v value [0..100]
 * @return the given RGB color in RGB (with LV_COLOR_DEPTH depth)
 */
lv_color_t lv_color_hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * Convert a 32-bit RGB color to HSV
 * @param r8 8-bit red
 * @param g8 8-bit green
 * @param b8 8-bit blue
 * @return the given RGB color in HSV
 */
lv_color_hsv_t lv_color_rgb_to_hsv(uint8_t r8, uint8_t g8, uint8_t b8);

/**
 * Convert a color to HSV
 * @param color color
 * @return the given color in HSV
 */
lv_color_hsv_t lv_color_to_hsv(lv_color_t color);

/*Source: https://vuetifyjs.com/en/styles/colors/#material-colors*/

/**
 * A helper for white color
 * @return      a white color
 */
lv_color_t lv_color_white(void);

/**
 * A helper for black color
 * @return      a black color
 */
lv_color_t lv_color_black(void);

void lv_color_premultiply(lv_color32_t * c);

void lv_color16_premultiply(lv_color16_t * c, lv_opa_t a);

/**
 * Get the luminance of a color: luminance = 0.3 R + 0.59 G + 0.11 B
 * @param c a color
 * @return the brightness [0..255]
 */
uint8_t lv_color_luminance(lv_color_t c);

/**
 * Get the luminance of a color16: luminance = 0.3 R + 0.59 G + 0.11 B
 * @param c a color
 * @return the brightness [0..255]
 */
uint8_t lv_color16_luminance(const lv_color16_t c);

/**
 * Get the luminance of a color24: luminance = 0.3 R + 0.59 G + 0.11 B
 * @param c a color
 * @return the brightness [0..255]
 */
uint8_t lv_color24_luminance(const uint8_t * c);

/**
 * Get the luminance of a color32: luminance = 0.3 R + 0.59 G + 0.11 B
 * @param c a color
 * @return the brightness [0..255]
 */
uint8_t lv_color32_luminance(lv_color32_t c);

/**********************
 *      MACROS
 **********************/

#include "lv_palette.h"
#include "lv_color_op.h"

LV_ATTRIBUTE_EXTERN_DATA extern const lv_color_filter_dsc_t lv_color_filter_shade;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_COLOR_H*/
