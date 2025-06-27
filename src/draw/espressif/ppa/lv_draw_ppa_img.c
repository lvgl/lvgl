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
    const lv_image_dsc_t * img_dsc = dsc->src;
    int width  = lv_area_get_width(coords);
    int height = lv_area_get_height(coords);

    ppa_blend_oper_config_t cfg = {
        .in_bg = {
            .buffer          = (void *)draw_buf->data,
            .pic_w           = width,
            .pic_h           = height,
            .block_w         = width,
            .block_h         = height,
            .block_offset_x  = 0,
            .block_offset_y  = 0,
            .blend_cm        = lv_color_format_to_ppa_blend(draw_buf->header.cf)
        },
        .in_fg = {
            .buffer          = (void *)img_dsc->data,
            .pic_w           = width,
            .pic_h           = height,
            .block_w         = width,
            .block_h         = height,
            .block_offset_x  = 0,
            .block_offset_y  = 0,
            .blend_cm        = lv_color_format_to_ppa_blend(dsc->header.cf)
        },
        .out = {
            .buffer          = draw_buf->data,
            .buffer_size     = draw_buf->data_size,
            .pic_w           = width,
            .pic_h           = height,
            .block_offset_x  = 0,
            .block_offset_y  = 0,
            .blend_cm        = lv_color_format_to_ppa_blend(draw_buf->header.cf)
        },
        .bg_rgb_swap           = false,
        .bg_byte_swap          = false,
        .fg_rgb_swap           = false,
        .fg_byte_swap          = false,
        .bg_alpha_update_mode = PPA_ALPHA_FIX_VALUE,
        .bg_alpha_fix_val = 0xff,

        .fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE,
        .fg_alpha_fix_val = dsc->opa,
        .bg_ck_en              = false,
        .fg_ck_en              = false,
#if LV_PPA_NONBLOCKING_OPS
        .mode            = PPA_TRANS_MODE_NON_BLOCKING,
        .user_data       = u,
#else
        .mode            = PPA_TRANS_MODE_BLOCKING,
#endif
    };

    esp_err_t ret = ppa_do_blend(u->blend_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA draw_img blend failed: %d", ret);
    }
}

#endif /* LV_USE_PPA */
