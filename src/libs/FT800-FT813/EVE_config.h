#ifndef EVE_CONFIG_H
#define EVE_CONFIG_H

#include "../../draw/eve/lv_draw_eve_private.h"

#define EVE_HSIZE          (lv_draw_eve_unit_g->params.hor_res)
#define EVE_VSIZE          (lv_draw_eve_unit_g->params.ver_res)
#define EVE_VSYNC0         (lv_draw_eve_unit_g->params.vsync0)
#define EVE_VSYNC1         (lv_draw_eve_unit_g->params.vsync1)
#define EVE_VOFFSET        (lv_draw_eve_unit_g->params.voffset)
#define EVE_VCYCLE         (lv_draw_eve_unit_g->params.vcycle)
#define EVE_HSYNC0         (lv_draw_eve_unit_g->params.hsync0)
#define EVE_HSYNC1         (lv_draw_eve_unit_g->params.hsync1)
#define EVE_HOFFSET        (lv_draw_eve_unit_g->params.hoffset)
#define EVE_HCYCLE         (lv_draw_eve_unit_g->params.hcycle)
#define EVE_PCLK           (lv_draw_eve_unit_g->params.pclk)
#define EVE_PCLKPOL        (lv_draw_eve_unit_g->params.pclkpol)
#define EVE_SWIZZLE        (lv_draw_eve_unit_g->params.swizzle)
#define EVE_CSPREAD        (lv_draw_eve_unit_g->params.cspread)
#define EVE_HAS_CRYSTAL    (lv_draw_eve_unit_g->params.has_crystal)
#define EVE_HAS_GT911      (lv_draw_eve_unit_g->params.has_gt911)
#define EVE_GEN            LV_DRAW_EVE_EVE_GENERATION
#define EVE_BACKLIGHT_PWM  (lv_draw_eve_unit_g->params.backlight_pwm)
#define EVE_BACKLIGHT_FREQ (lv_draw_eve_unit_g->params.backlight_freq)

#endif /* EVE_CONFIG_H */
