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

    const int32_t block_w = lv_area_get_width(clipped_img_area);
    const int32_t block_h = lv_area_get_height(clipped_img_area);

    /* Does this draw need real compositing, or is a plain copy enough?
     * - a source alpha channel has to be honoured against what the destination
     *   already holds;
     * - a global opacity has to fade the image into it.
     * LVGL treats opa >= LV_OPA_MAX as fully covering, so only a value below
     * that is worth a compositing pass. Everything else keeps the copy. */
    const lv_opa_t opa = draw_dsc->opa;
    const bool src_has_alpha = lv_ppa_cf_has_alpha(src_cf);
    const bool opa_is_partial = opa < (lv_opa_t)LV_OPA_MAX;
    const bool needs_compositing = src_has_alpha || opa_is_partial;

    ppa_blend_oper_config_t cfg;
    lv_memzero(&cfg, sizeof(cfg));

    if(needs_compositing) {
        /* Composite: the background is what the destination already holds and
         * the foreground is the image drawn over it. The copy path below uses
         * the opposite assignment, which is why it cannot honour a source
         * alpha channel - it overwrites that alpha with 0xFF. */
        cfg.in_bg.buffer         = (void *)dest_buf;
        cfg.in_bg.pic_w          = draw_buf->header.w;
        cfg.in_bg.pic_h          = draw_buf->header.h;
        cfg.in_bg.block_w        = block_w;
        cfg.in_bg.block_h        = block_h;
        cfg.in_bg.block_offset_x = dest_area.x1;
        cfg.in_bg.block_offset_y = dest_area.y1;
        cfg.in_bg.blend_cm       = lv_color_format_to_ppa_blend(dest_cf);
        /* The backdrop is opaque whatever its format claims. */
        cfg.bg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
        cfg.bg_alpha_fix_val     = 0xFF;

        cfg.in_fg.buffer         = (void *)src_buf;
        cfg.in_fg.pic_w          = draw_dsc->header.w;
        cfg.in_fg.pic_h          = draw_dsc->header.h;
        cfg.in_fg.block_w        = block_w;
        cfg.in_fg.block_h        = block_h;
        cfg.in_fg.block_offset_x = src_area.x1;
        cfg.in_fg.block_offset_y = src_area.y1;
        cfg.in_fg.blend_cm       = lv_color_format_to_ppa_blend(src_cf);

        if(src_has_alpha && !opa_is_partial) {
            /* The image's own per-pixel alpha, unchanged. */
            cfg.fg_alpha_update_mode = PPA_ALPHA_NO_CHANGE;
        }
        else if(src_has_alpha) {
            /* Per-pixel alpha scaled by the global opacity. alpha_scale_ratio is
             * a float in the OPEN range (0, 1): opa is above LV_OPA_MIN (the
             * caller returns early at or below it) and below LV_OPA_MAX here, so
             * the quotient stays strictly inside it. */
            cfg.fg_alpha_update_mode = PPA_ALPHA_SCALE;
            cfg.fg_alpha_scale_ratio = (float)opa / 255.0f;
        }
        else {
            /* No alpha channel, or an undefined X byte: the global opacity is
             * the alpha of every pixel. */
            cfg.fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
            cfg.fg_alpha_fix_val     = opa;
        }
    }
    else {
        /* Opaque copy: the source is the background and the foreground is a
         * fully transparent dummy, so the output is the converted source. */
        cfg.in_bg.buffer         = (void *)src_buf;
        cfg.in_bg.pic_w          = draw_dsc->header.w;
        cfg.in_bg.pic_h          = draw_dsc->header.h;
        cfg.in_bg.block_w        = block_w;
        cfg.in_bg.block_h        = block_h;
        cfg.in_bg.block_offset_x = src_area.x1;
        cfg.in_bg.block_offset_y = src_area.y1;
        cfg.in_bg.blend_cm       = lv_color_format_to_ppa_blend(src_cf);
        cfg.bg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
        cfg.bg_alpha_fix_val     = 0xFF;

        /* Dummy A8 foreground. It must describe a region that really exists in
         * the destination buffer: the PPA still fetches it even though
         * fg_alpha_fix_val = 0 makes it contribute nothing. */
        cfg.in_fg.buffer         = (void *)dest_buf;
        cfg.in_fg.pic_w          = draw_buf->header.w;
        cfg.in_fg.pic_h          = draw_buf->header.h;
        cfg.in_fg.block_w        = block_w;
        cfg.in_fg.block_h        = block_h;
        cfg.in_fg.block_offset_x = dest_area.x1;
        cfg.in_fg.block_offset_y = dest_area.y1;
        cfg.in_fg.blend_cm       = PPA_BLEND_COLOR_MODE_A8;
        cfg.fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE;
        cfg.fg_alpha_fix_val     = 0;
    }

    cfg.out.buffer          = dest_buf;
    cfg.out.buffer_size     = draw_buf->data_size;
    cfg.out.pic_w           = draw_buf->header.w;
    cfg.out.pic_h           = draw_buf->header.h;
    cfg.out.block_offset_x  = dest_area.x1;
    cfg.out.block_offset_y  = dest_area.y1;
    cfg.out.blend_cm        = lv_color_format_to_ppa_blend(dest_cf);
    cfg.mode                = PPA_TRANS_MODE_BLOCKING;
    cfg.user_data           = u;

    esp_err_t ret = ppa_do_blend(u->blend_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA draw_img blend failed: %d", ret);
    }
}

#endif /* LV_USE_PPA */
