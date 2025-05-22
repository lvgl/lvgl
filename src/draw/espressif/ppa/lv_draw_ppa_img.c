/**
 * @file lv_draw_ppa_img.c
 *
 */

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

void lv_draw_ppa_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN) {
        return;
    }

    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)t->draw_unit;
    lv_draw_buf_t * draw_buf = t->target_layer->draw_buf;
    int width  = lv_area_get_width(coords);
    int height = lv_area_get_height(coords);

    ppa_blend_oper_config_t cfg = {
        .in_bg = {
            .buffer          = (void *)dsc->src,
            .pic_w           = width,
            .pic_h           = height,
            .block_offset_x  = coords->x1,
            .block_offset_y  = coords->y1,
            .blend_cm        = lv_color_format_to_ppa_blend(dsc->header.cf)
        },
        .in_fg = {
            .buffer          = (void *)dsc->src,
            .pic_w           = width,
            .pic_h           = height,
            .block_offset_x  = coords->x1,
            .block_offset_y  = coords->y1,
            .blend_cm        = lv_color_format_to_ppa_blend(dsc->header.cf)
        },
        .out = {
            .buffer          = PPA_PTR_ALIGN_UP(draw_buf->data, 64),
            .buffer_size     = PPA_ALIGN_UP(draw_buf->data_size, 64),
            .pic_w           = width,
            .pic_h           = height,
            .block_offset_x  = coords->x1,
            .block_offset_y  = coords->y1,
            .blend_cm        = lv_color_format_to_ppa_blend(dsc->base.layer->color_format)
        },
        .bg_rgb_swap           = false,
        .bg_byte_swap          = false,
        .bg_alpha_update_mode  = PPA_ALPHA_NO_CHANGE,
        .fg_rgb_swap           = false,
        .fg_byte_swap          = false,
        .fg_alpha_update_mode  = PPA_ALPHA_FIX_VALUE,
        .fg_alpha_fix_val      = dsc->opa,
        .fg_fix_rgb_val        = { .r = 0, .g = 0, .b = 0 },
        .bg_ck_en              = false,
        .fg_ck_en              = false,
        .mode                  = PPA_TRANS_MODE_BLOCKING,
        .user_data             = NULL
    };

    esp_err_t ret = ppa_do_blend(u->blend_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA draw_img blend failed: %d", ret);
    }
}

#endif /* LV_USE_PPA */
