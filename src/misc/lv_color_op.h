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

    ret.red = LV_UDIV255((uint16_t)c1.red * mix + c2.red * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    ret.green = LV_UDIV255((uint16_t)c1.green * mix + c2.green * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    ret.blue = LV_UDIV255((uint16_t)c1.blue * mix + c2.blue * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
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
    if(fg.alpha >= LV_OPA_MAX) {
        fg.alpha = bg.alpha;
        return fg;
    }
    if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    bg.red = (uint32_t)((uint32_t)fg.red * fg.alpha + (uint32_t)bg.red * (255 - fg.alpha)) >> 8;
    bg.green = (uint32_t)((uint32_t)fg.green * fg.alpha + (uint32_t)bg.green * (255 - fg.alpha)) >> 8;
    bg.blue = (uint32_t)((uint32_t)fg.blue * fg.alpha + (uint32_t)bg.blue * (255 - fg.alpha)) >> 8;
    return bg;
}

//! @endcond

/**
 * Get the brightness of a color
 * @param color a color
 * @return the brightness [0..255]
 */
static inline uint8_t lv_color_brightness(lv_color_t c)
{
    uint16_t bright = (uint16_t)(3u * c.red + c.green + 4u * c.blue);
    return (uint8_t)(bright >> 3);
}

static inline void lv_color_filter_dsc_init(lv_color_filter_dsc_t * dsc, lv_color_filter_cb_t cb)
{
    dsc->filter_cb = cb;
}

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
