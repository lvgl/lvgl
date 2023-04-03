/**
 * @file lv_color_op.h
 *
 */

#ifndef LV_COLOR_OP_H
#define LV_COLOR_OP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_assert.h"
#include "lv_math.h"
#include "lv_color.h"
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_color_filter_dsc_t;

typedef lv_color_t (*lv_color_filter_cb_t)(const struct _lv_color_filter_dsc_t *, lv_color_t, lv_opa_t);

typedef struct _lv_color_filter_dsc_t {
    lv_color_filter_cb_t filter_cb;
    void * user_data;
} lv_color_filter_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

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

#if LV_COLOR_DEPTH == 16 && LV_COLOR_MIX_ROUND_OFS == 0
    /*Source: https://stackoverflow.com/a/50012418/1999969*/
    uint16_t c1_16 = lv_color_to_int(c1);
    uint16_t c2_16 = lv_color_to_int(c2);
    mix = (uint32_t)((uint32_t)mix + 4) >> 3;

    /*0x7E0F81F = 0b00000111111000001111100000011111*/
    uint32_t bg = (uint32_t)(c2_16 | ((uint32_t)c2_16 << 16)) & 0x7E0F81F;
    uint32_t fg = (uint32_t)(c1_16 | ((uint32_t)c1_16 << 16)) & 0x7E0F81F;
    uint32_t result = ((((fg - bg) * mix) >> 5) + bg) & 0x7E0F81F;
    lv_color_set_int(&ret, (uint16_t)((result >> 16) | result));
#elif LV_COLOR_DEPTH == 8
    LV_COLOR_SET_R(ret, LV_UDIV255((uint16_t)LV_COLOR_GET_R(c1) * mix + LV_COLOR_GET_R(c2) *
                                   (255 - mix) + LV_COLOR_MIX_ROUND_OFS));
#else
    /*LV_COLOR_DEPTH == 8, 16 or 32*/
    ret.red = LV_UDIV255((uint16_t)c1.red * mix + c2.red * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    ret.green = LV_UDIV255((uint16_t)c1.green * mix + c2.green * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    ret.blue = LV_UDIV255((uint16_t)c1.blue * mix + c2.blue * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
#if LV_COLOR_DEPTH == 32
    ret.alpha = 0xff;
#endif

#endif
    return ret;
}

LV_ATTRIBUTE_FAST_MEM static inline void lv_color_premult(lv_color_t c, uint8_t mix, uint16_t * out)
{
    out[0] = (uint16_t)c.red * mix;
    out[1] = (uint16_t)c.green * mix;
    out[2] = (uint16_t)c.blue * mix;
}

/**
 * Mix two colors with a given ratio. It runs faster than `lv_color_mix` but requires some pre computation.
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
    /*LV_COLOR_DEPTH == 8 or 32*/
    ret.red = LV_UDIV255(premult_c1[0] + c2.red * mix + LV_COLOR_MIX_ROUND_OFS);
    ret.green = LV_UDIV255(premult_c1[1] + c2.green * mix + LV_COLOR_MIX_ROUND_OFS);
    ret.blue = LV_UDIV255(premult_c1[2] + c2.blue * mix + LV_COLOR_MIX_ROUND_OFS);

#if LV_COLOR_DEPTH == 32
    ret.alpha = 0xff;
#endif
    return ret;
}


/**
 *
 * @param fg
 * @param bg
 * @return
 * @note Use bg.alpha in the return value
 * @note Use fg.alpha as mix ratio
 */
static inline lv_color32_t lv_color_mix32(lv_color32_t fg, lv_color32_t bg)
{
    bg.red = (uint32_t)((uint32_t)fg.red * fg.alpha + (uint32_t)bg.red * (255 - fg.alpha)) >> 8;
    bg.green = (uint32_t)((uint32_t)fg.green * fg.alpha + (uint32_t)bg.green * (255 - fg.alpha)) >> 8;
    bg.blue = (uint32_t)((uint32_t)fg.blue * fg.alpha + (uint32_t)bg.blue * (255 - fg.alpha)) >> 8;
    return bg;
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
LV_ATTRIBUTE_FAST_MEM static inline lv_color32_t lv_color_mix_with_alpha(lv_color32_t bg, lv_color32_t fg)
{
    /*Pick the foreground if it's fully opaque or the Background is fully transparent*/
    if(fg.alpha == 255 || bg.alpha <= LV_OPA_MIN) {
        return fg;
    }
    /*Opaque background: use simple mix*/
    else if(bg.alpha == 255) {
        return lv_color_mix32(fg, bg);
    }
    /*Transparent foreground: use the Background*/
    else if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    /*Both colors have alpha. Expensive calculation need to be applied*/
    else {
        /*Save the parameters and the result. If they will be asked again don't compute again*/
        static lv_color32_t fg_saved = {0, 0, 0, 0};
        static lv_color32_t bg_saved = {0, 0, 0, 0};
        static lv_color32_t res_saved = {0, 0, 0, 0};
        static lv_opa_t res_alpha_saved = 255;
        static lv_opa_t ratio_saved = 255;

        /*Update the ratio and the result alpha value if the input alpha values change*/
        if(bg.alpha != bg_saved.alpha || fg.alpha != fg_saved.alpha) {
            /*Info:
             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
            res_alpha_saved  = 255 - ((uint32_t)((uint32_t)(255 - fg.alpha) * (255 - bg.alpha)) >> 8);
            LV_ASSERT(ratio_saved != 0);
            ratio_saved = (uint32_t)((uint32_t)fg.alpha * 255) / res_alpha_saved;
        }

        if(!lv_color32_eq(bg, bg_saved) || !lv_color32_eq(fg, fg_saved)) {
            fg_saved = fg;
            bg_saved = bg;
            fg.alpha = ratio_saved;
            res_saved = lv_color_mix32(fg, bg);
            res_saved.alpha = res_alpha_saved;
        }

        return res_saved;
    }
}

static inline lv_color32_t lv_color_to_xrgb8888(lv_color_t c)
{
#if LV_COLOR_DEPTH == 16
    lv_color32_t ret;
    ret.red = (c.red * 263 + 7) >> 5;
    ret.green = (c.green * 259 + 3) >> 6;
    ret.blue = (c.blue * 263 + 7) >> 5;
    ret.alpha = 0xff;
    return ret;
#elif LV_COLOR_DEPTH == 24
    lv_color_t ret;
    ret.red = c.red;
    ret.green = c.green;
    ret.blue = c.blue;
    ret.alpha = 0xff;
    return ret;
#elif LV_COLOR_DEPTH == 32
    return *((lv_color32_t *)((void *)&c));
#endif
}

static inline lv_color_t lv_color_from_xrgb8888(lv_color32_t c)
{
#if LV_COLOR_DEPTH == 16
    lv_color_t ret;
    ret.red = c.red >> 3;
    ret.green = c.green >> 2;
    ret.blue = c.blue >> 3;
    return ret;
#elif LV_COLOR_DEPTH == 24
    lv_color_t ret;
    ret.red = c.red;
    ret.green = c.green;
    ret.blue = c.blue;
    ret.alpha = 0xff;
    return ret;
#elif LV_COLOR_DEPTH == 32
    return *((lv_color_t *)((void *)&c));
#endif
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
    c32 = lv_color_to_xrgb8888(color);
    uint16_t bright = (uint16_t)(3u * c32.red + c32.green + 4u * c32.blue);
    return (uint8_t)(bright >> 3);
}

static inline void lv_color_filter_dsc_init(lv_color_filter_dsc_t * dsc, lv_color_filter_cb_t cb)
{
    dsc->filter_cb = cb;
}

//! @cond Doxygen_Suppress
//!
LV_ATTRIBUTE_FAST_MEM void lv_color_buf_fill(lv_color_t * buf, lv_color_t color, uint32_t px_num);

//! @endcond


void lv_color_buf_to_native(const uint8_t * src_buf, lv_color_format_t src_cf, lv_color_t * c_out, lv_opa_t * a_out,
                            lv_color_t alpha_color, uint32_t px_cnt);

void lv_color_buf_from_native(const lv_color_t * src_buf, uint8_t * dest_buf, lv_color_format_t dest_cf,
                              uint32_t px_cnt);


void lv_color_buf_from_argb8888(const lv_color32_t * src_buf, uint8_t * dest_buf, lv_color_format_t dest_cf,
                                uint32_t px_cnt);

/**********************
 *  PREDEFINED COLORS
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_COLOR_H*/
