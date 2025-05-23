/**
 * @file lv_draw_ppa_fill.c
 *
 */

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

void lv_draw_ppa_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN) {
        return;
    }

    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)t->draw_unit;
    lv_draw_buf_t * draw_buf = t->target_layer->draw_buf;
    int width  = lv_area_get_width(coords);
    int height = lv_area_get_height(coords);

    if(width <= 0 || height <= 0) {
        LV_LOG_WARN("Invalid draw area for filling!");
        return;
    }

    ppa_fill_oper_config_t cfg = {
        .fill_argb_color.val = lv_color_to_u32(dsc->color),
        .fill_block_w    = width,
        .fill_block_h    = height,
        .out = {
            .buffer         = PPA_PTR_ALIGN_UP(draw_buf->data, 64),
            .buffer_size    = PPA_ALIGN_UP(draw_buf->data_size, 64),
            .pic_w          = width,
            .pic_h          = height,
            .block_offset_x = coords->x1,
            .block_offset_y = coords->y1,
            .fill_cm        = lv_color_format_to_ppa_fill(dsc->base.layer->color_format),
        },
        .mode            = PPA_TRANS_MODE_BLOCKING,
        .user_data       = NULL,
    };

    esp_err_t ret = ppa_do_fill(u->fill_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_ERROR("PPA fill failed: %d", ret);
    }
}

#endif /* LV_USE_PPA */
