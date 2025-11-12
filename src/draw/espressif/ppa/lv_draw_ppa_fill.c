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
    ppa_fill_oper_config_t fill_cfg = {0};
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)t->draw_unit;
    lv_draw_buf_t * draw_buf = t->target_layer->draw_buf;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
    lv_area_move(&rel_clip_area, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    fill_cfg.fill_argb_color.val = lv_color_to_u32(dsc->color);
    fill_cfg.out.block_offset_x = blend_area.x1;
    fill_cfg.out.block_offset_y = blend_area.y1;
    fill_cfg.out.fill_cm        = lv_color_format_to_ppa_fill(draw_buf->header.cf);
    fill_cfg.fill_block_w       = lv_area_get_width(&blend_area);
    fill_cfg.fill_block_h       = lv_area_get_height(&blend_area);
    fill_cfg.out.buffer         = draw_buf->data;
    fill_cfg.out.buffer_size    = draw_buf->data_size;
    fill_cfg.out.pic_w          = draw_buf->header.w;
    fill_cfg.out.pic_h          = draw_buf->header.h;;
    fill_cfg.mode            = PPA_TRANS_MODE_BLOCKING;

    esp_err_t ret = ppa_do_fill(u->fill_client, &fill_cfg);
    if(ret != ESP_OK) {
        LV_LOG_ERROR("PPA fill failed: %d", ret);
    }
}

#endif /* LV_USE_PPA */
