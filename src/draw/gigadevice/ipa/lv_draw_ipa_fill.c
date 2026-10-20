/**
 * @file lv_draw_ipa_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_ipa_private.h"
#if LV_USE_DRAW_IPA

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

void lv_draw_ipa_fill(lv_draw_task_t * t, void * first_pixel, int32_t w, int32_t h, int32_t stride)
{
    lv_draw_fill_dsc_t * dsc = t->draw_dsc;
    lv_color_format_t cf = dsc->base.layer->color_format;
    lv_opa_t opa = dsc->opa;

    lv_draw_ipa_output_cf_t output_cf = lv_draw_cf_to_ipa_output_cf(cf);
    uint32_t cf_size = LV_COLOR_FORMAT_GET_SIZE(cf);

    uint32_t output_offset = (stride / cf_size) - w;

    lv_draw_ipa_configuration_t conf = {
        .mode = LV_DRAW_IPA_MODE_REGISTER_TO_MEMORY,
        .w = w,
        .h = h,

        .output_address = first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf,
    };

    if(dsc->opa >= LV_OPA_MAX) {
        conf.reg_to_mem_mode_color = lv_draw_ipa_color_to_ipa_color(output_cf, dsc->color);
    }
    else {
        conf.mode = LV_DRAW_IPA_MODE_MEMORY_TO_MEMORY_WITH_BLENDING;
        conf.fg_color = lv_color_to_u32(dsc->color);
        conf.fg_address = first_pixel;
        conf.fg_offset = output_offset;
        conf.fg_alpha_mode = LV_DRAW_IPA_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        conf.fg_alpha = opa;
        conf.fg_cf = LV_DRAW_IPA_FGBG_CF_A8;

        conf.bg_address = first_pixel;
        conf.bg_offset = output_offset;
        conf.bg_alpha_mode = LV_DRAW_IPA_ALPHA_MODE_NO_MODIFY_IMAGE_ALPHA_CHANNEL;
        conf.bg_alpha = opa;
        conf.bg_cf = (lv_draw_ipa_fgbg_cf_t) output_cf;

        /* Background alpha channel should be treated as 0xFF if the cf is XRGB */
        if(cf == LV_COLOR_FORMAT_XRGB8888) {
            conf.bg_alpha_mode = LV_DRAW_IPA_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
            conf.bg_alpha = 0xff;
        }
    }
    lv_draw_ipa_configure_and_start_transfer(&conf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_IPA*/
