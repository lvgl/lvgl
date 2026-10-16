/**
 * @file lv_draw_ipa_layer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_ipa_private.h"
#if LV_USE_DRAW_IPA

#include "../../../misc/lv_area_private.h"

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

bool lv_draw_ipa_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{
    LV_CHECK_ARG(t != NULL, return false);
    LV_CHECK_ARG(draw_dsc != NULL, return false);
    LV_CHECK_ARG(coords != NULL, return false);
    LV_CHECK_ARG(draw_dsc->src != NULL, return false);

    const lv_layer_t * source_layer = draw_dsc->src;
    lv_layer_t * target_layer = t->target_layer;
    const lv_draw_buf_t * source_buf = source_layer->draw_buf;
    if(source_buf == NULL || draw_dsc->opa <= LV_OPA_MIN) return false;

    lv_area_t clipped_area;
    if(!lv_area_intersect(&clipped_area, coords, &t->clip_area)) return false;
    if(!lv_area_intersect(&clipped_area, &clipped_area, &target_layer->buf_area)) return false;

    lv_area_t source_area = clipped_area;
    lv_area_move(&source_area, -coords->x1, -coords->y1);
    lv_color_format_t source_cf = source_buf->header.cf;
    lv_color_format_t target_cf = target_layer->draw_buf->header.cf;
    uint32_t source_size = lv_color_format_get_size(source_cf);
    uint32_t target_size = lv_color_format_get_size(target_cf);
    uint32_t source_stride = source_buf->header.stride;
    uint32_t target_stride = target_layer->draw_buf->header.stride;

    if(source_size == 0 || target_size == 0 || source_stride % source_size || target_stride % target_size ||
       source_area.x1 < 0 || source_area.y1 < 0 ||
       (uint32_t)source_area.x2 >= source_buf->header.w || (uint32_t)source_area.y2 >= source_buf->header.h ||
       source_stride / source_size < source_buf->header.w ||
       target_stride / target_size < target_layer->draw_buf->header.w) {
        LV_LOG_WARN("Invalid IPA layer buffer layout");
        t->state = LV_DRAW_TASK_STATE_FAILED;
        return false;
    }

    uint32_t width = lv_area_get_width(&clipped_area);
    void * destination = lv_draw_layer_go_to_xy(target_layer,
                                                clipped_area.x1 - target_layer->buf_area.x1,
                                                clipped_area.y1 - target_layer->buf_area.y1);
    const uint8_t * source = source_buf->data + (size_t)source_area.y1 * source_stride +
                             (size_t)source_area.x1 * source_size;
    lv_draw_ipa_output_cf_t output_cf = lv_draw_cf_to_ipa_output_cf(target_cf);
    lv_draw_ipa_configuration_t conf = {
        .mode = LV_DRAW_IPA_MODE_MEMORY_TO_MEMORY_WITH_BLENDING,
        .w = width,
        .h = lv_area_get_height(&clipped_area),
        .output_address = destination,
        .output_offset = target_stride / target_size - width,
        .output_cf = output_cf,
        .fg_address = source,
        .fg_offset = source_stride / source_size - width,
        .fg_cf = (lv_draw_ipa_fgbg_cf_t)lv_draw_cf_to_ipa_output_cf(source_cf),
        .fg_alpha_mode = LV_DRAW_IPA_ALPHA_MODE_MULTIPLY_IMAGE_ALPHA_CHANNEL,
        .fg_alpha = draw_dsc->opa,
        .bg_address = destination,
        .bg_offset = target_stride / target_size - width,
        .bg_cf = (lv_draw_ipa_fgbg_cf_t)output_cf,
    };

    if(!lv_color_format_has_alpha(source_cf)) {
        conf.fg_alpha_mode = LV_DRAW_IPA_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        if(draw_dsc->opa >= LV_OPA_MAX) conf.mode = LV_DRAW_IPA_MODE_MEMORY_TO_MEMORY_WITH_PFC;
    }
    if(target_cf == LV_COLOR_FORMAT_XRGB8888) {
        conf.bg_alpha_mode = LV_DRAW_IPA_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        conf.bg_alpha = LV_OPA_COVER;
    }

    lv_draw_buf_flush_cache(source_buf, &source_area);
    lv_draw_ipa_configure_and_start_transfer(&conf);
    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_IPA*/
