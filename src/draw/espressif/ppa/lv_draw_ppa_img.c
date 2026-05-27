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
static bool ppa_rotation_to_srm_angle(int32_t rotation, ppa_srm_rotation_angle_t * angle_out);
static bool ppa_transform_requested(const lv_draw_image_dsc_t * draw_dsc);


void lv_draw_ppa_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                                           const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    lv_draw_image_normal_helper(t, dsc, coords, lv_draw_img_ppa_core, NULL);
}

void lv_draw_ppa_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                                             const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)dsc->src;
    if(layer_to_draw == NULL || layer_to_draw->draw_buf == NULL) return;

    lv_draw_image_dsc_t new_draw_dsc = *dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;
    new_draw_dsc.header = layer_to_draw->draw_buf->header;

    lv_draw_ppa_img(t, &new_draw_dsc, coords);
}

static void lv_draw_img_ppa_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                                       const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                                       const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    LV_UNUSED(sup);
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

    uint8_t src_bpp = lv_color_format_get_bpp(src_cf);
    uint8_t dest_bpp = lv_color_format_get_bpp(dest_cf);
    if(src_bpp == 0 || dest_bpp == 0) {
        LV_LOG_WARN("PPA draw_img invalid bpp src=%d dest=%d", src_bpp, dest_bpp);
        return;
    }

    uint32_t src_pic_w = (decoded->header.stride * 8U) / src_bpp;
    uint32_t dest_pic_w = (draw_buf->header.stride * 8U) / dest_bpp;
    uint32_t src_block_w = lv_area_get_width(&src_area);
    uint32_t src_block_h = lv_area_get_height(&src_area);
    uint32_t dest_block_w = lv_area_get_width(&dest_area);
    uint32_t dest_block_h = lv_area_get_height(&dest_area);

#if LV_USE_PPA_TRANSFORM
    if(ppa_transform_requested(draw_dsc)) {
        ppa_srm_rotation_angle_t srm_angle;
        if(!ppa_rotation_to_srm_angle(draw_dsc->rotation, &srm_angle)) {
            LV_LOG_WARN("PPA SRM rotation is not supported: %d", (int)draw_dsc->rotation);
            return;
        }

        lv_area_t transformed_area;
        int32_t scale_x_abs = draw_dsc->scale_x < 0 ? -draw_dsc->scale_x : draw_dsc->scale_x;
        int32_t scale_y_abs = draw_dsc->scale_y < 0 ? -draw_dsc->scale_y : draw_dsc->scale_y;
        if(scale_x_abs == 0 || scale_y_abs == 0) {
            LV_LOG_WARN("PPA SRM invalid scale: %d x %d", (int)draw_dsc->scale_x, (int)draw_dsc->scale_y);
            return;
        }

        lv_image_buf_get_transformed_area(&transformed_area, decoded->header.w, decoded->header.h,
                                          draw_dsc->rotation, scale_x_abs, scale_y_abs, &draw_dsc->pivot);
        lv_area_move(&transformed_area, img_coords->x1, img_coords->y1);
        if(!lv_area_is_equal(&transformed_area, clipped_img_area)) {
            LV_LOG_WARN("PPA SRM requires full transformed clip");
            return;
        }

        ppa_srm_oper_config_t srm_cfg = {0};
        srm_cfg.in.buffer = src_buf;
        srm_cfg.in.pic_w = src_pic_w;
        srm_cfg.in.pic_h = decoded->header.h;
        srm_cfg.in.block_w = decoded->header.w;
        srm_cfg.in.block_h = decoded->header.h;
        srm_cfg.in.block_offset_x = 0;
        srm_cfg.in.block_offset_y = 0;
        srm_cfg.in.srm_cm = lv_color_format_to_ppa_srm(src_cf);

        srm_cfg.out.buffer = dest_buf;
        srm_cfg.out.buffer_size = draw_buf->data_size;
        srm_cfg.out.pic_w = dest_pic_w;
        srm_cfg.out.pic_h = draw_buf->header.h;
        srm_cfg.out.block_offset_x = dest_area.x1;
        srm_cfg.out.block_offset_y = dest_area.y1;
        srm_cfg.out.srm_cm = lv_color_format_to_ppa_srm(dest_cf);

        srm_cfg.rotation_angle = srm_angle;
        srm_cfg.scale_x = (float)scale_x_abs / (float)LV_SCALE_NONE;
        srm_cfg.scale_y = (float)scale_y_abs / (float)LV_SCALE_NONE;
        srm_cfg.mirror_x = (draw_dsc->scale_x < 0);
        srm_cfg.mirror_y = (draw_dsc->scale_y < 0);

        srm_cfg.rgb_swap = false;
        srm_cfg.byte_swap = false;
        srm_cfg.alpha_update_mode = PPA_ALPHA_NO_CHANGE;
        srm_cfg.mode = PPA_TRANS_MODE_BLOCKING;
        srm_cfg.user_data = u;

        esp_err_t srm_ret = ppa_do_scale_rotate_mirror(u->srm_client, &srm_cfg);
        if(srm_ret != ESP_OK) {
            LV_LOG_WARN("PPA SRM failed: %d", srm_ret);
        }
        return;
    }
#endif

    ppa_blend_oper_config_t cfg = {0};
    cfg.in_bg.buffer = (void *)dest_buf;
    cfg.in_bg.pic_w = dest_pic_w;
    cfg.in_bg.pic_h = draw_buf->header.h;
    cfg.in_bg.block_w = dest_block_w;
    cfg.in_bg.block_h = dest_block_h;
    cfg.in_bg.block_offset_x = dest_area.x1;
    cfg.in_bg.block_offset_y = dest_area.y1;
    cfg.in_bg.blend_cm = lv_color_format_to_ppa_blend(dest_cf);

    cfg.bg_rgb_swap = false;
    cfg.bg_byte_swap = false;
    cfg.bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
    cfg.bg_ck_en = false;

    cfg.in_fg.buffer = (void *)src_buf;
    cfg.in_fg.pic_w = src_pic_w;
    cfg.in_fg.pic_h = decoded->header.h;
    cfg.in_fg.block_w = src_block_w;
    cfg.in_fg.block_h = src_block_h;
    cfg.in_fg.block_offset_x = src_area.x1;
    cfg.in_fg.block_offset_y = src_area.y1;
    cfg.in_fg.blend_cm = lv_color_format_to_ppa_blend(src_cf);
    cfg.fg_rgb_swap = false;
    cfg.fg_byte_swap = false;

    bool src_has_alpha = (src_cf == LV_COLOR_FORMAT_ARGB8888);
    if(src_has_alpha) {
        if(draw_dsc->opa >= (lv_opa_t)LV_OPA_MAX) {
            cfg.fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
        }
        else {
            cfg.fg_alpha_update_mode = PPA_ALPHA_SCALE;
            cfg.fg_alpha_scale_ratio = (float)draw_dsc->opa / 255.0f;
        }
    }
    else {
        cfg.fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
        cfg.fg_alpha_fix_val = draw_dsc->opa;
    }
    cfg.fg_ck_en = false;

    cfg.out.buffer = dest_buf;
    cfg.out.buffer_size = draw_buf->data_size;
    cfg.out.pic_w = dest_pic_w;
    cfg.out.pic_h = draw_buf->header.h;
    cfg.out.block_offset_x = dest_area.x1;
    cfg.out.block_offset_y = dest_area.y1;
    cfg.out.blend_cm = lv_color_format_to_ppa_blend(dest_cf);
    cfg.mode = PPA_TRANS_MODE_BLOCKING;
    cfg.user_data = u;

    esp_err_t ret = ppa_do_blend(u->blend_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA draw_img blend failed: %d", ret);
    }
}

static bool ppa_rotation_to_srm_angle(int32_t rotation, ppa_srm_rotation_angle_t * angle_out)
{
    int32_t r = rotation % 3600;
    if(r < 0) r += 3600;

    switch(r) {
        case 0:
            *angle_out = PPA_SRM_ROTATION_ANGLE_0;
            return true;
        case 900:
            *angle_out = PPA_SRM_ROTATION_ANGLE_90;
            return true;
        case 1800:
            *angle_out = PPA_SRM_ROTATION_ANGLE_180;
            return true;
        case 2700:
            *angle_out = PPA_SRM_ROTATION_ANGLE_270;
            return true;
        default:
            return false;
    }
}

static bool ppa_transform_requested(const lv_draw_image_dsc_t * draw_dsc)
{
    return (draw_dsc->rotation != 0 ||
            draw_dsc->scale_x != LV_SCALE_NONE ||
            draw_dsc->scale_y != LV_SCALE_NONE);
}

#endif /* LV_USE_PPA */
