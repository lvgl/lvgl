/**
 * @file lv_color.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_color_op_private.h"
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

lv_color_t LV_ATTRIBUTE_FAST_MEM lv_color_mix(lv_color_t c1, lv_color_t c2, uint8_t mix)
{
    lv_color_t ret;

    ret.red = LV_UDIV255((uint16_t)c1.red * mix + c2.red * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    ret.green = LV_UDIV255((uint16_t)c1.green * mix + c2.green * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    ret.blue = LV_UDIV255((uint16_t)c1.blue * mix + c2.blue * (255 - mix) + LV_COLOR_MIX_ROUND_OFS);
    return ret;
}

lv_color32_t lv_color_mix32(lv_color32_t fg, lv_color32_t bg)
{
    if(fg.alpha >= LV_OPA_MAX) {
        fg.alpha = bg.alpha;
        return fg;
    }
    if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    bg.red = LV_UDIV255((uint32_t)((uint32_t)fg.red * fg.alpha + (uint32_t)bg.red * (255 - fg.alpha)));
    bg.green = LV_UDIV255((uint32_t)((uint32_t)fg.green * fg.alpha + (uint32_t)bg.green * (255 - fg.alpha)));
    bg.blue = LV_UDIV255((uint32_t)((uint32_t)fg.blue * fg.alpha + (uint32_t)bg.blue * (255 - fg.alpha)));
    return bg;
}

uint8_t lv_color_brightness(lv_color_t c)
{
    uint16_t bright = (uint16_t)(3u * c.red + c.green + 4u * c.blue);
    return (uint8_t)(bright >> 3);
}

void lv_color_filter_dsc_init(lv_color_filter_dsc_t * dsc, lv_color_filter_cb_t cb)
{
    dsc->filter_cb = cb;
}

lv_color32_t lv_color_over32(lv_color32_t fg, lv_color32_t bg)
{
    if(fg.alpha >= LV_OPA_MAX || bg.alpha <= LV_OPA_MIN) {
        return fg;
    }
    else if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    else if(bg.alpha == 255) {
        return lv_color_mix32(fg, bg);
    }

    lv_opa_t res_alpha  = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha);
    lv_opa_t ratio = (uint32_t)((uint32_t)fg.alpha * 255) / res_alpha;
    fg.alpha = ratio;
    lv_color32_t res = lv_color_mix32(fg, bg);
    res.alpha = res_alpha;

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
