/**
 * @file lv_draw_ppa_img.c
 *
 */

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

#include "../../lv_draw_image_private.h"
#include "../../lv_image_decoder_private.h"

static void lv_draw_img_ppa_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                 const lv_area_t * img_coords, const lv_area_t * clipped_img_area);


void lv_draw_ppa_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    lv_draw_image_normal_helper(t, dsc, coords, lv_draw_img_ppa_core);
}

static void lv_draw_img_ppa_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                 const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)t->draw_unit;

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, clipped_img_area);
    lv_area_move(&rel_clip_area, -img_coords->x1, -img_coords->y1);

    lv_area_t rel_img_coords;
    lv_area_copy(&rel_img_coords, img_coords);
    lv_area_move(&rel_img_coords, -img_coords->x1, -img_coords->y1);

    lv_area_t src_area;
    if(!lv_area_intersect(&src_area, &rel_clip_area, &rel_img_coords))
        return;

    lv_area_t dest_area;
    lv_area_copy(&dest_area, clipped_img_area);
    lv_area_move(&dest_area, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    const uint8_t * src_buf = decoded->data;
    lv_color_format_t src_cf = draw_dsc->header.cf;
    lv_color_format_t dest_cf = draw_buf->header.cf;
    uint8_t * dest_buf = draw_buf->data;

    extern const lv_image_dsc_t img_benchmark_lvgl_logo_rgb;

    ppa_blend_oper_config_t cfg = {
        .in_bg = {
            .buffer          = (void *)src_buf,
            .pic_w           = draw_dsc->header.w,
            .pic_h           = draw_dsc->header.h,
            .block_w         = lv_area_get_width(clipped_img_area),
            .block_h         = lv_area_get_height(clipped_img_area),
            .block_offset_x  = src_area.x1,
            .block_offset_y  = src_area.y1,
            .blend_cm        = lv_color_format_to_ppa_blend(src_cf),
        },
        .bg_rgb_swap           = false,
        .bg_byte_swap          = false,
        .bg_alpha_update_mode  = PPA_ALPHA_FIX_VALUE,
        .bg_alpha_fix_val      = 0xFF,
        .bg_ck_en              = false,
        .in_fg = {
            .buffer          = (void *)dest_buf,
            .pic_w           = draw_dsc->header.w,
            .pic_h           = draw_dsc->header.h,
            .block_w         = lv_area_get_width(clipped_img_area),
            .block_h         = lv_area_get_height(clipped_img_area),
            .block_offset_x  = src_area.x1,
            .block_offset_y  = src_area.y1,
            .blend_cm        = PPA_BLEND_COLOR_MODE_A8,
        },
        .fg_fix_rgb_val = {
            .r = 0,
            .g = 0,
            .b = 0,
        },
        .fg_rgb_swap           = false,
        .fg_byte_swap          = false,
        .fg_alpha_update_mode  = PPA_ALPHA_FIX_VALUE,
        .fg_alpha_fix_val      = 0,
        .fg_ck_en              = false,
        .out = {
            .buffer          = dest_buf,
            .buffer_size     = draw_buf->data_size,
            .pic_w           = draw_buf->header.w,
            .pic_h           = draw_buf->header.h,
            .block_offset_x  = dest_area.x1,
            .block_offset_y  = dest_area.y1,
            .blend_cm        = lv_color_format_to_ppa_blend(dest_cf),
        },
        .mode            = PPA_TRANS_MODE_BLOCKING,
        .user_data       = u,
    };

    esp_err_t ret = ppa_do_blend(u->blend_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA draw_img blend failed: %d", ret);
    }
}

#endif /* LV_USE_PPA */
