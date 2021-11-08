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

/*Error checking*/
#if LV_COLOR_DEPTH == 24
#error "LV_COLOR_DEPTH 24 is deprecated. Use LV_COLOR_DEPTH 32 instead (lv_conf.h)"
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
LV_EXPORT_CONST_INT(LV_COLOR_DEPTH);
LV_EXPORT_CONST_INT(LV_COLOR_16_SWAP);

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

#define LV_OPA_MIN 2    /*Opacities below this will be transparent*/
#define LV_OPA_MAX 253  /*Opacities above this will fully cover*/

#if LV_COLOR_DEPTH == 1
#define LV_COLOR_SIZE 8
#elif LV_COLOR_DEPTH == 8
#define LV_COLOR_SIZE 8
#elif LV_COLOR_DEPTH == 16
#define LV_COLOR_SIZE 16
#elif LV_COLOR_DEPTH == 32
#define LV_COLOR_SIZE 32
#else
#error "Invalid LV_COLOR_DEPTH in lv_conf.h! Set it to 1, 8, 16 or 32!"
#endif

#if defined(__cplusplus) && !defined(_LV_COLOR_HAS_MODERN_CPP)
/**
* MSVC compiler's definition of the __cplusplus indicating 199711L regardless to C++ standard version
* see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-cplusplus
* so we use _MSC_VER macro instead of __cplusplus
*/
#ifdef _MSC_VER
#if _MSC_VER >= 1900 /*Visual Studio 2015*/
#define _LV_COLOR_HAS_MODERN_CPP 1
#endif
#else
#if __cplusplus >= 201103L
#define _LV_COLOR_HAS_MODERN_CPP 1
#endif
#endif
#endif /*__cplusplus*/

#ifndef _LV_COLOR_HAS_MODERN_CPP
#define _LV_COLOR_HAS_MODERN_CPP 0
#endif

#if _LV_COLOR_HAS_MODERN_CPP
/*Fix msvc compiler error C4576 inside C++ code*/
#define _LV_COLOR_MAKE_TYPE_HELPER lv_color_t
#else
#define _LV_COLOR_MAKE_TYPE_HELPER (lv_color_t)
#endif

/*---------------------------------------
 * Macros for all existing color depths
 * to set/get values of the color channels
 *------------------------------------------*/
# define LV_COLOR_SET_R1(c, v) (c).ch.red = (uint8_t)((v) & 0x1)
# define LV_COLOR_SET_G1(c, v) (c).ch.green = (uint8_t)((v) & 0x1)
# define LV_COLOR_SET_B1(c, v) (c).ch.blue = (uint8_t)((v) & 0x1)
# define LV_COLOR_SET_A1(c, v) do {} while(0)

# define LV_COLOR_GET_R1(c) (c).ch.red
# define LV_COLOR_GET_G1(c) (c).ch.green
# define LV_COLOR_GET_B1(c) (c).ch.blue
# define LV_COLOR_GET_A1(c) 0xFF

# define _LV_COLOR_ZERO_INITIALIZER1 {0x00}
# define LV_COLOR_MAKE1(r8, g8, b8)  {(uint8_t)((b8 >> 7) | (g8 >> 7) | (r8 >> 7))}

# define LV_COLOR_SET_R8(c, v) (c).ch.red = (uint8_t)((v) & 0x7U)
# define LV_COLOR_SET_G8(c, v) (c).ch.green = (uint8_t)((v) & 0x7U)
# define LV_COLOR_SET_B8(c, v) (c).ch.blue = (uint8_t)((v) & 0x3U)
# define LV_COLOR_SET_A8(c, v) do {} while(0)

# define LV_COLOR_GET_R8(c) (c).ch.red
# define LV_COLOR_GET_G8(c) (c).ch.green
# define LV_COLOR_GET_B8(c) (c).ch.blue
# define LV_COLOR_GET_A8(c) 0xFF

# define _LV_COLOR_ZERO_INITIALIZER8 {{0x00, 0x00, 0x00}}
# define LV_COLOR_MAKE8(r8, g8, b8) {{(uint8_t)((b8 >> 6) & 0x3U), (uint8_t)((g8 >> 5) & 0x7U), (uint8_t)((r8 >> 5) & 0x7U)}}

# define LV_COLOR_SET_R16(c, v) (c).ch.red = (uint8_t)((v) & 0x1FU)
#if LV_COLOR_16_SWAP == 0
# define LV_COLOR_SET_G16(c, v) (c).ch.green = (uint8_t)((v) & 0x3FU)
#else
# define LV_COLOR_SET_G16(c, v) {(c).ch.green_h = (uint8_t)(((v) >> 3) & 0x7); (c).ch.green_l = (uint8_t)((v) & 0x7);}
#endif
# define LV_COLOR_SET_B16(c, v) (c).ch.blue = (uint8_t)((v) & 0x1FU)
# define LV_COLOR_SET_A16(c, v) do {} while(0)

# define LV_COLOR_GET_R16(c) (c).ch.red
#if LV_COLOR_16_SWAP == 0
# define LV_COLOR_GET_G16(c) (c).ch.green
#else
# define LV_COLOR_GET_G16(c) (((c).ch.green_h << 3) + (c).ch.green_l)
#endif
# define LV_COLOR_GET_B16(c) (c).ch.blue
# define LV_COLOR_GET_A16(c) 0xFF

#if LV_COLOR_16_SWAP == 0
# define _LV_COLOR_ZERO_INITIALIZER16  {{0x00, 0x00, 0x00}}
# define LV_COLOR_MAKE16(r8, g8, b8) {{(uint8_t)((b8 >> 3) & 0x1FU), (uint8_t)((g8 >> 2) & 0x3FU), (uint8_t)((r8 >> 3) & 0x1FU)}}
#else
# define _LV_COLOR_ZERO_INITIALIZER16 {{0x00, 0x00, 0x00, 0x00}}
# define LV_COLOR_MAKE16(r8, g8, b8) {{(uint8_t)((g8 >> 5) & 0x7U), (uint8_t)((r8 >> 3) & 0x1FU), (uint8_t)((b8 >> 3) & 0x1FU), (uint8_t)((g8 >> 2) & 0x7U)}}
#endif

# define LV_COLOR_SET_R32(c, v) (c).ch.red = (uint8_t)((v) & 0xFF)
# define LV_COLOR_SET_G32(c, v) (c).ch.green = (uint8_t)((v) & 0xFF)
# define LV_COLOR_SET_B32(c, v) (c).ch.blue = (uint8_t)((v) & 0xFF)
# define LV_COLOR_SET_A32(c, v) (c).ch.alpha = (uint8_t)((v) & 0xFF)

# define LV_COLOR_GET_R32(c) (c).ch.red
# define LV_COLOR_GET_G32(c) (c).ch.green
# define LV_COLOR_GET_B32(c) (c).ch.blue
# define LV_COLOR_GET_A32(c) (c).ch.alpha

# define _LV_COLOR_ZERO_INITIALIZER32  {{0x00, 0x00, 0x00, 0x00}}
# define LV_COLOR_MAKE32(r8, g8, b8) {{b8, g8, r8, 0xff}} /*Fix 0xff alpha*/

/*---------------------------------------
 * Macros for the current color depth
 * to set/get values of the color channels
 *------------------------------------------*/
#define LV_COLOR_SET_R(c, v) LV_CONCAT(LV_COLOR_SET_R, LV_COLOR_DEPTH)(c, v)
#define LV_COLOR_SET_G(c, v) LV_CONCAT(LV_COLOR_SET_G, LV_COLOR_DEPTH)(c, v)
#define LV_COLOR_SET_B(c, v) LV_CONCAT(LV_COLOR_SET_B, LV_COLOR_DEPTH)(c, v)
#define LV_COLOR_SET_A(c, v) LV_CONCAT(LV_COLOR_SET_A, LV_COLOR_DEPTH)(c, v)

#define LV_COLOR_GET_R(c) LV_CONCAT(LV_COLOR_GET_R, LV_COLOR_DEPTH)(c)
#define LV_COLOR_GET_G(c) LV_CONCAT(LV_COLOR_GET_G, LV_COLOR_DEPTH)(c)
#define LV_COLOR_GET_B(c) LV_CONCAT(LV_COLOR_GET_B, LV_COLOR_DEPTH)(c)
#define LV_COLOR_GET_A(c) LV_CONCAT(LV_COLOR_GET_A, LV_COLOR_DEPTH)(c)

#define _LV_COLOR_ZERO_INITIALIZER LV_CONCAT(_LV_COLOR_ZERO_INITIALIZER, LV_COLOR_DEPTH)
#define LV_COLOR_MAKE(r8, g8, b8) LV_CONCAT(LV_COLOR_MAKE, LV_COLOR_DEPTH)(r8, g8, b8)

/**********************
 *      TYPEDEFS
 **********************/

typedef union {
    uint8_t full; /*must be declared first to set all bits of byte via initializer list*/
    union {
        uint8_t blue : 1;
        uint8_t green : 1;
        uint8_t red : 1;
    } ch;
} lv_color1_t;

typedef union {
    struct {
        uint8_t blue : 2;
        uint8_t green : 3;
        uint8_t red : 3;
    } ch;
    uint8_t full;
} lv_color8_t;

typedef union {
    struct {
#if LV_COLOR_16_SWAP == 0
        uint16_t blue : 5;
        uint16_t green : 6;
        uint16_t red : 5;
#else
        uint16_t green_h : 3;
        uint16_t red : 5;
        uint16_t blue : 5;
        uint16_t green_l : 3;
#endif
    } ch;
    uint16_t full;
} lv_color16_t;

typedef union {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t full;
} lv_color32_t;

typedef LV_CONCAT3(uint, LV_COLOR_SIZE, _t) lv_color_int_t;
typedef LV_CONCAT3(lv_color, LV_COLOR_DEPTH, _t) lv_color_t;

typedef struct {
    uint16_t h;
    uint8_t s;
    uint8_t v;
} lv_color_hsv_t;

//! @cond Doxygen_Suppress
/*No idea where the guard is required but else throws warnings in the docs*/
typedef uint8_t lv_opa_t;
//! @endcond

struct _lv_color_filter_dsc_t;

typedef lv_color_t (*lv_color_filter_cb_t)(const struct _lv_color_filter_dsc_t *, lv_color_t, lv_opa_t);

typedef struct _lv_color_filter_dsc_t {
    lv_color_filter_cb_t filter_cb;
    void * user_data;
} lv_color_filter_dsc_t;


typedef enum {
    LV_PALETTE_RED,
    LV_PALETTE_PINK,
    LV_PALETTE_PURPLE,
    LV_PALETTE_DEEP_PURPLE,
    LV_PALETTE_INDIGO,
    LV_PALETTE_BLUE,
    LV_PALETTE_LIGHT_BLUE,
    LV_PALETTE_CYAN,
    LV_PALETTE_TEAL,
    LV_PALETTE_GREEN,
    LV_PALETTE_LIGHT_GREEN,
    LV_PALETTE_LIME,
    LV_PALETTE_YELLOW,
    LV_PALETTE_AMBER,
    LV_PALETTE_ORANGE,
    LV_PALETTE_DEEP_ORANGE,
    LV_PALETTE_BROWN,
    LV_PALETTE_BLUE_GREY,
    LV_PALETTE_GREY,
    _LV_PALETTE_LAST,
    LV_PALETTE_NONE = 0xff,
} lv_palette_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*In color conversations:
 * - When converting to bigger color type the LSB weight of 1 LSB is calculated
 *   E.g. 16 bit Red has 5 bits
 *         8 bit Red has 3 bits
 *        ----------------------
 *        8 bit red LSB = (2^5 - 1) / (2^3 - 1) = 31 / 7 = 4
 *
 * - When calculating to smaller color type simply shift out the LSBs
 *   E.g.  8 bit Red has 3 bits
 *        16 bit Red has 5 bits
 *        ----------------------
 *         Shift right with 5 - 3 = 2
 */
static inline uint8_t lv_color_to1(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    return color.full;
#elif LV_COLOR_DEPTH == 8
    if((LV_COLOR_GET_R(color) & 0x4) || (LV_COLOR_GET_G(color) & 0x4) || (LV_COLOR_GET_B(color) & 0x2)) {
        return 1;
    }
    else {
        return 0;
    }
#elif LV_COLOR_DEPTH == 16
    if((LV_COLOR_GET_R(color) & 0x10) || (LV_COLOR_GET_G(color) & 0x20) || (LV_COLOR_GET_B(color) & 0x10)) {
        return 1;
    }
    else {
        return 0;
    }
#elif LV_COLOR_DEPTH == 32
    if((LV_COLOR_GET_R(color) & 0x80) || (LV_COLOR_GET_G(color) & 0x80) || (LV_COLOR_GET_B(color) & 0x80)) {
        return 1;
    }
    else {
        return 0;
    }
#endif
}

static inline uint8_t lv_color_to8(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    if(color.full == 0)
        return 0;
    else
        return 0xFF;
#elif LV_COLOR_DEPTH == 8
    return color.full;
#elif LV_COLOR_DEPTH == 16
    lv_color8_t ret;
    LV_COLOR_SET_R8(ret, LV_COLOR_GET_R(color) >> 2); /*5 - 3  = 2*/
    LV_COLOR_SET_G8(ret, LV_COLOR_GET_G(color) >> 3); /*6 - 3  = 3*/
    LV_COLOR_SET_B8(ret, LV_COLOR_GET_B(color) >> 3); /*5 - 2  = 3*/
    return ret.full;
#elif LV_COLOR_DEPTH == 32
    lv_color8_t ret;
    LV_COLOR_SET_R8(ret, LV_COLOR_GET_R(color) >> 5); /*8 - 3  = 5*/
    LV_COLOR_SET_G8(ret, LV_COLOR_GET_G(color) >> 5); /*8 - 3  = 5*/
    LV_COLOR_SET_B8(ret, LV_COLOR_GET_B(color) >> 6); /*8 - 2  = 6*/
    return ret.full;
#endif
}

static inline uint16_t lv_color_to16(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    if(color.full == 0)
        return 0;
    else
        return 0xFFFF;
#elif LV_COLOR_DEPTH == 8
    lv_color16_t ret;
    LV_COLOR_SET_R16(ret, LV_COLOR_GET_R(color) * 4);  /*(2^5 - 1)/(2^3 - 1) = 31/7 = 4*/
    LV_COLOR_SET_G16(ret, LV_COLOR_GET_G(color) * 9);  /*(2^6 - 1)/(2^3 - 1) = 63/7 = 9*/
    LV_COLOR_SET_B16(ret, LV_COLOR_GET_B(color) * 10); /*(2^5 - 1)/(2^2 - 1) = 31/3 = 10*/
    return ret.full;
#elif LV_COLOR_DEPTH == 16
    return color.full;
#elif LV_COLOR_DEPTH == 32
    lv_color16_t ret;
    LV_COLOR_SET_R16(ret, LV_COLOR_GET_R(color) >> 3); /*8 - 5  = 3*/
    LV_COLOR_SET_G16(ret, LV_COLOR_GET_G(color) >> 2); /*8 - 6  = 2*/
    LV_COLOR_SET_B16(ret, LV_COLOR_GET_B(color) >> 3); /*8 - 5  = 3*/
    return ret.full;
#endif
}

static inline uint32_t lv_color_to32(lv_color_t color)
{
#if LV_COLOR_DEPTH == 1
    if(color.full == 0)
        return 0xFF000000;
    else
        return 0xFFFFFFFF;
#elif LV_COLOR_DEPTH == 8
    lv_color32_t ret;
    LV_COLOR_SET_R32(ret, LV_COLOR_GET_R(color) * 36); /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    LV_COLOR_SET_G32(ret, LV_COLOR_GET_G(color) * 36); /*(2^8 - 1)/(2^3 - 1) = 255/7 = 36*/
    LV_COLOR_SET_B32(ret, LV_COLOR_GET_B(color) * 85); /*(2^8 - 1)/(2^2 - 1) = 255/3 = 85*/
    LV_COLOR_SET_A32(ret, 0xFF);
    return ret.full;
#elif LV_COLOR_DEPTH == 16
    /**
     * The floating point math for conversion is:
     *  valueto = valuefrom * ( (2^bitsto - 1) / (float)(2^bitsfrom - 1) )
     * The faster integer math for conversion is:
     *  valueto = ( valuefrom * multiplier + adder ) >> divisor
     *   multiplier = FLOOR( ( (2^bitsto - 1) << divisor ) / (float)(2^bitsfrom - 1) )
     *
     * Find the first divisor where ( adder >> divisor ) <= 0
     *
     * 5-bit to 8-bit: ( 31 * multiplier + adder ) >> divisor = 255
     * divisor  multiplier  adder  min (0)  max (31)
     *       0           8      7        7       255
     *       1          16     14        7       255
     *       2          32     28        7       255
     *       3          65     25        3       255
     *       4         131     19        1       255
     *       5         263      7        0       255
     *
     * 6-bit to 8-bit: 255 = ( 63 * multiplier + adder ) >> divisor
     * divisor  multiplier  adder  min (0)  max (63)
     *       0           4      3        3       255
     *       1           8      6        3       255
     *       2          16     12        3       255
     *       3          32     24        3       255
     *       4          64     48        3       255
     *       5         129     33        1       255
     *       6         259      3        0       255
     */

    lv_color32_t ret;
    LV_COLOR_SET_R32(ret, (LV_COLOR_GET_R(color) * 263 + 7) >> 5);
    LV_COLOR_SET_G32(ret, (LV_COLOR_GET_G(color) * 259 + 3) >> 6);
    LV_COLOR_SET_B32(ret, (LV_COLOR_GET_B(color) * 263 + 7) >> 5);
    LV_COLOR_SET_A32(ret, 0xFF);
    return ret.full;
#elif LV_COLOR_DEPTH == 32
    return color.full;
#endif
}

//! @cond Doxygen_Suppress

/**
 * Mix two colors with a given ratio.
 * @param c1 the first color to mix (usually the foreground)
 * @param c2 the second color to mix (usually the background)
 * @param mix The ratio of the colors. 0: full `c2`, 255: full `c1`, 127: half `c1` and half`c2`
 * @return the mixed color
 */
LV_ATTRIBUTE_FAST_MEM static inline lv_color_t lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix)
{
    lv_color_t ret;

#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
    /*Source: https://stackoverflow.com/a/50012418/1999969*/
    mix = (mix + 4) >> 3;
    uint32_t bg = (uint32_t)((uint32_t)c2.full | ((uint32_t)c2.full << 16)) &
                  0x7E0F81F; /*0b00000111111000001111100000011111*/
    uint32_t fg = (uint32_t)((uint32_t)c1.full | ((uint32_t)c1.full << 16)) & 0x7E0F81F;
    uint32_t result = ((((fg - bg) * mix) >> 5) + bg) & 0x7E0F81F;
    ret.full = (uint16_t)((result >> 16) | result);
#elif LV_COLOR_DEPTH != 1
    /*LV_COLOR_DEPTH == 8, 16 or 32*/
    LV_COLOR_SET_R(ret, LV_UDIV255((uint16_t)LV_COLOR_GET_R(c1) * mix + LV_COLOR_GET_R(c2) *
                                   (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_G(ret, LV_UDIV255((uint16_t)LV_COLOR_GET_G(c1) * mix + LV_COLOR_GET_G(c2) *
                                   (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_B(ret, LV_UDIV255((uint16_t)LV_COLOR_GET_B(c1) * mix + LV_COLOR_GET_B(c2) *
                                   (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_A(ret, 0xFF);
#else
    /*LV_COLOR_DEPTH == 1*/
    ret.full = mix > LV_OPA_50 ? c1.full : c2.full;
#endif

    return ret;
}

LV_ATTRIBUTE_FAST_MEM static inline void lv_color_premult(lv_color_t c, uint8_t mix, uint16_t * out)
{
#if LV_COLOR_DEPTH != 1
    out[0] = (uint16_t)LV_COLOR_GET_R(c) * mix;
    out[1] = (uint16_t)LV_COLOR_GET_G(c) * mix;
    out[2] = (uint16_t)LV_COLOR_GET_B(c) * mix;
#else
    (void) mix;
    /*Pre-multiplication can't be used with 1 bpp*/
    out[0] = LV_COLOR_GET_R(c);
    out[1] = LV_COLOR_GET_G(c);
    out[2] = LV_COLOR_GET_B(c);
#endif

}

/**
 * Mix two colors with a given ratio. It runs faster then `lv_color_mix` but requires some pre computation.
 * @param premult_c1 The first color. Should be preprocessed with `lv_color_premult(c1)`
 * @param c2 The second color. As it is no pre computation required on it
 * @param mix The ratio of the colors. 0: full `c1`, 255: full `c2`, 127: half `c1` and half `c2`.
 *            Should be modified like mix = `255 - mix`
 * @return the mixed color
 * @note 255 won't give clearly `c1`.
 */
LV_ATTRIBUTE_FAST_MEM static inline lv_color_t lv_color_mix_premult(uint16_t * premult_c1, lv_color_t c2, uint8_t mix)
{
    lv_color_t ret;
#if LV_COLOR_DEPTH != 1
    /*LV_COLOR_DEPTH == 8 or 32*/
    LV_COLOR_SET_R(ret, LV_UDIV255(premult_c1[0] + LV_COLOR_GET_R(c2) * mix + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_G(ret, LV_UDIV255(premult_c1[1] + LV_COLOR_GET_G(c2) * mix + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_B(ret, LV_UDIV255(premult_c1[2] + LV_COLOR_GET_B(c2) * mix + LV_COLOR_MIX_ROUND_OFS));
    LV_COLOR_SET_A(ret, 0xFF);
#else
    /*LV_COLOR_DEPTH == 1*/
    /*Restore color1*/
    lv_color_t c1;
    LV_COLOR_SET_R(c1, premult_c1[0]);
    LV_COLOR_SET_G(c1, premult_c1[1]);
    LV_COLOR_SET_B(c1, premult_c1[2]);
    ret.full = mix > LV_OPA_50 ? c2.full : c1.full;
#endif

    return ret;
}

/**
 * Mix two colors. Both color can have alpha value.
 * @param bg_color background color
 * @param bg_opa alpha of the background color
 * @param fg_color foreground color
 * @param fg_opa alpha of the foreground color
 * @param res_color the result color
 * @param res_opa the result opacity
 */
LV_ATTRIBUTE_FAST_MEM static inline void lv_color_mix_with_alpha(lv_color_t bg_color, lv_opa_t bg_opa,
                                                                 lv_color_t fg_color, lv_opa_t fg_opa,
                                                                 lv_color_t * res_color, lv_opa_t * res_opa)
{
    /*Pick the foreground if it's fully opaque or the Background is fully transparent*/
    if(fg_opa >= LV_OPA_MAX || bg_opa <= LV_OPA_MIN) {
        res_color->full = fg_color.full;
        *res_opa = fg_opa;
    }
    /*Transparent foreground: use the Background*/
    else if(fg_opa <= LV_OPA_MIN) {
        res_color->full = bg_color.full;
        *res_opa = bg_opa;
    }
    /*Opaque background: use simple mix*/
    else if(bg_opa >= LV_OPA_MAX) {
        *res_color = lv_color_mix(fg_color, bg_color, fg_opa);
        *res_opa = LV_OPA_COVER;
    }
    /*Both colors have alpha. Expensive calculation need to be applied*/
    else {
        /*Save the parameters and the result. If they will be asked again don't compute again*/
        static lv_opa_t fg_opa_save     = 0;
        static lv_opa_t bg_opa_save     = 0;
        static lv_color_t fg_color_save = _LV_COLOR_ZERO_INITIALIZER;
        static lv_color_t bg_color_save = _LV_COLOR_ZERO_INITIALIZER;
        static lv_color_t res_color_saved = _LV_COLOR_ZERO_INITIALIZER;
        static lv_opa_t res_opa_saved = 0;

        if(fg_opa != fg_opa_save || bg_opa != bg_opa_save || fg_color.full != fg_color_save.full ||
           bg_color.full != bg_color_save.full) {
            fg_opa_save        = fg_opa;
            bg_opa_save        = bg_opa;
            fg_color_save.full = fg_color.full;
            bg_color_save.full = bg_color.full;
            /*Info:
             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
            res_opa_saved = 255 - ((uint16_t)((uint16_t)(255 - fg_opa) * (255 - bg_opa)) >> 8);
            LV_ASSERT(res_opa_saved != 0);
            lv_opa_t ratio = (uint16_t)((uint16_t)fg_opa * 255) / res_opa_saved;
            res_color_saved = lv_color_mix(fg_color, bg_color, ratio);

        }

        res_color->full = res_color_saved.full;
        *res_opa = res_opa_saved;
    }
}

//! @endcond

/**
 * Get the brightness of a color
 * @param color a color
 * @return the brightness [0..255]
 */
static inline uint8_t lv_color_brightness(lv_color_t color)
{
    lv_color32_t c32;
    c32.full        = lv_color_to32(color);
    uint16_t bright = (uint16_t)(3u * LV_COLOR_GET_R32(c32) + LV_COLOR_GET_B32(c32) + 4u * LV_COLOR_GET_G32(c32));
    return (uint8_t)(bright >> 3);
}

static inline lv_color_t lv_color_make(uint8_t r, uint8_t g, uint8_t b)
{
    return _LV_COLOR_MAKE_TYPE_HELPER LV_COLOR_MAKE(r, g, b);
}

static inline lv_color_t lv_color_hex(uint32_t c)
{
    return lv_color_make((uint8_t)((c >> 16) & 0xFF), (uint8_t)((c >> 8) & 0xFF), (uint8_t)(c & 0xFF));
}

static inline lv_color_t lv_color_hex3(uint32_t c)
{
    return lv_color_make((uint8_t)(((c >> 4) & 0xF0) | ((c >> 8) & 0xF)), (uint8_t)((c & 0xF0) | ((c & 0xF0) >> 4)),
                         (uint8_t)((c & 0xF) | ((c & 0xF) << 4)));
}

static inline void lv_color_filter_dsc_init(lv_color_filter_dsc_t * dsc, lv_color_filter_cb_t cb)
{
    dsc->filter_cb = cb;
}

//! @cond Doxygen_Suppress
//!
LV_ATTRIBUTE_FAST_MEM void lv_color_fill(lv_color_t * buf, lv_color_t color, uint32_t px_num);

//! @endcond
lv_color_t lv_color_lighten(lv_color_t c, lv_opa_t lvl);

lv_color_t lv_color_darken(lv_color_t c, lv_opa_t lvl);

lv_color_t lv_color_change_lightness(lv_color_t c, lv_opa_t lvl);

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

/**
 * Just a wrapper around LV_COLOR_CHROMA_KEY because it might be more convenient to use a function is some cases
 * @return LV_COLOR_CHROMA_KEY
 */
static inline lv_color_t lv_color_chroma_key(void)
{
    return LV_COLOR_CHROMA_KEY;
}

/**********************
 *  PREDEFINED COLORS
 **********************/
/*Source: https://vuetifyjs.com/en/styles/colors/#material-colors*/

lv_color_t lv_palette_main(lv_palette_t p);
static inline lv_color_t lv_color_white(void)
{
    return lv_color_make(0xff, 0xff, 0xff);
}
static inline lv_color_t lv_color_black(void)
{
    return lv_color_make(0x00, 0x0, 0x00);
}
lv_color_t lv_palette_lighten(lv_palette_t p, uint8_t lvl);
lv_color_t lv_palette_darken(lv_palette_t p, uint8_t lvl);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_COLOR_H*/
