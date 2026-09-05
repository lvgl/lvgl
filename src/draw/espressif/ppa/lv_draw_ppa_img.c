/**
 * @file lv_draw_ppa_img.c
 *
 */

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

#include "../../lv_draw_image_private.h"
#include "../../../image/lv_image_decoder_private.h"
#if LV_USE_DRAW_SW
    #include "../../sw/lv_draw_sw.h"
#endif

static void lv_draw_img_ppa_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                 const lv_area_t * img_coords, const lv_area_t * clipped_img_area);


void lv_draw_ppa_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    lv_draw_image_normal_helper(t, dsc, coords, lv_draw_img_ppa_core, NULL);
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
    rel_clip_area = *clipped_img_area;
    lv_area_move(&rel_clip_area, -img_coords->x1, -img_coords->y1);

    lv_area_t rel_img_coords;
    rel_img_coords = *img_coords;
    lv_area_move(&rel_img_coords, -img_coords->x1, -img_coords->y1);

    lv_area_t src_area;
    if(!lv_area_intersect(&src_area, &rel_clip_area, &rel_img_coords))
        return;

    lv_area_t dest_area;
    dest_area = *clipped_img_area;
    lv_area_move(&dest_area, -t->target_layer->buf_area.x1, -t->target_layer->buf_area.y1);

    const uint8_t * src_buf = decoded->data;
    lv_color_format_t src_cf = draw_dsc->header.cf;
    lv_color_format_t dest_cf = draw_buf->header.cf;
    uint8_t * dest_buf = draw_buf->data;

    /* Row pitch, not visible width: see lv_ppa_pic_w(). The task is already
     * assigned to this unit here, so a picture the PPA cannot describe goes to
     * the software unit rather than being dropped, which would finish the task
     * having drawn nothing and leave a hole on the screen. lv_draw_pxp.c falls
     * back the same way for fills. */
    const int32_t src_pic_w  = lv_ppa_pic_w(decoded->header.stride, draw_dsc->header.w, src_cf);
    const int32_t dest_pic_w = lv_ppa_pic_w(draw_buf->header.stride, draw_buf->header.w, dest_cf);
    if(src_pic_w == 0 || dest_pic_w == 0) {
        /* lv_draw_sw_image() redraws the whole task, and it runs its own decode
         * loop, so it must fire exactly once. A partial decoder calls this core
         * back per decoded chunk (see img_decode_and_draw()), which would
         * otherwise repeat the full software draw for every chunk. */
        if(u->img_sw_fallback) return;
        u->img_sw_fallback = true;

        LV_LOG_INFO("PPA draw_img: stride is not a whole number of pixels, drawing in software");
#if LV_USE_DRAW_SW
        lv_draw_sw_image(t, draw_dsc, &t->area);
#else
        LV_LOG_WARN("PPA draw_img: no software draw unit to fall back on, image skipped");
#endif
        return;
    }

    ppa_blend_oper_config_t cfg = {
        .in_bg = {
            .buffer          = (void *)src_buf,
            .pic_w           = src_pic_w,
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
        /* The transparent dummy foreground. Its buffer is the destination, so it
         * has to be described by the destination's geometry - it was carrying the
         * source's, which makes the PPA fetch a region that need not exist in
         * that buffer. It contributes no colour either way. */
        .in_fg = {
            .buffer          = (void *)dest_buf,
            .pic_w           = dest_pic_w,
            .pic_h           = draw_buf->header.h,
            .block_w         = lv_area_get_width(clipped_img_area),
            .block_h         = lv_area_get_height(clipped_img_area),
            .block_offset_x  = dest_area.x1,
            .block_offset_y  = dest_area.y1,
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
            .pic_w           = dest_pic_w,
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
