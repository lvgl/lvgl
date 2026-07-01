/**
 * @file lv_draw_ppa_img.c
 *
 */

#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"
#include "lv_draw_ppa_srm.h"

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

#if LV_USE_PPA_IMG

/* Round a byte count up to the cache line, as required by PPA DMA and esp_cache_msync(). */
static inline uint32_t lv_draw_ppa_align_size(uint32_t size)
{
    return (uint32_t)PPA_ALIGN_UP(size, CONFIG_CACHE_L2_CACHE_LINE_SIZE);
}

void lv_draw_ppa_img_srm(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                         const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN) return;

    lv_draw_ppa_unit_t * u   = (lv_draw_ppa_unit_t *)t->draw_unit;
    lv_layer_t * layer        = t->target_layer;
    lv_draw_buf_t * dest_buf  = layer->draw_buf;

    /* coords = image rect at 1:1 scale (may extend off-screen).
     * Skip the decode entirely if nothing intersects the render tile. */
    lv_area_t visible_area;
    if(!lv_area_intersect(&visible_area, coords, &layer->buf_area)) return;
    LV_UNUSED(visible_area);

    lv_image_decoder_dsc_t decoder_dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.flush_cache = true;

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, dsc->src, &dec_args);
    if(res != LV_RESULT_OK) return;

    const lv_draw_buf_t * decoded = decoder_dsc.decoded;
    if(!decoded || !decoded->data) {
        lv_image_decoder_close(&decoder_dsc);
        return;
    }

    lv_color_format_t src_cf  = decoded->header.cf;
    lv_color_format_t dest_cf = dest_buf->header.cf;
    if(!ppa_src_cf_supported(src_cf) || !ppa_dest_cf_supported(dest_cf)) {
        lv_image_decoder_close(&decoder_dsc);
        return;
    }

    uint32_t src_w = decoded->header.w;
    uint32_t src_h = decoded->header.h;

    /* Map the visible render tile back onto a PPA source block.
     * Pure geometry, shared with the host unit test (lv_draw_ppa_srm.h). */
    lv_draw_ppa_srm_block_t blk = lv_draw_ppa_srm_calc_block(
                                      coords, &layer->buf_area,
                                      (int32_t)dest_buf->header.w, (int32_t)dest_buf->header.h,
                                      (int32_t)src_w, (int32_t)src_h,
                                      dsc->scale_x, dsc->scale_y, dsc->pivot.x, dsc->pivot.y);
    if(!blk.draw) {
        lv_image_decoder_close(&decoder_dsc);
        return;
    }

    if(decoded->data_size > 0) {
        esp_cache_msync((void *)decoded->data,
                        lv_draw_ppa_align_size(decoded->data_size),
                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }

    uint32_t out_bpp = (dest_cf == LV_COLOR_FORMAT_RGB565) ? 2u :
                       (dest_cf == LV_COLOR_FORMAT_RGB888)  ? 3u : 4u;
    uint32_t raw_bytes    = (uint32_t)dest_buf->header.w * dest_buf->header.h * out_bpp;
    uint32_t aligned_size = lv_draw_ppa_align_size(raw_bytes);

    ppa_srm_oper_config_t cfg;
    lv_memzero(&cfg, sizeof(cfg));

    cfg.in.buffer         = (void *)decoded->data;
    cfg.in.pic_w          = src_w;
    cfg.in.pic_h          = src_h;
    cfg.in.block_w        = (uint32_t)blk.block_w;
    cfg.in.block_h        = (uint32_t)blk.block_h;
    cfg.in.block_offset_x = (uint32_t)blk.block_x;
    cfg.in.block_offset_y = (uint32_t)blk.block_y;
    cfg.in.srm_cm         = lv_color_format_to_ppa_srm(src_cf);

    cfg.out.buffer         = dest_buf->data;
    cfg.out.buffer_size    = aligned_size;
    cfg.out.pic_w          = dest_buf->header.w;
    cfg.out.pic_h          = dest_buf->header.h;
    cfg.out.block_offset_x = (uint32_t)blk.dest_area.x1;
    cfg.out.block_offset_y = (uint32_t)blk.dest_area.y1;
    cfg.out.srm_cm         = lv_color_format_to_ppa_srm(dest_cf);

    cfg.rotation_angle    = PPA_SRM_ROTATION_ANGLE_0;
    cfg.scale_x           = blk.scale_x;
    cfg.scale_y           = blk.scale_y;
    cfg.mirror_x          = false;
    cfg.mirror_y          = false;
    cfg.rgb_swap          = false;
    cfg.byte_swap         = false;
    cfg.alpha_update_mode = PPA_ALPHA_NO_CHANGE;
    cfg.mode              = PPA_TRANS_MODE_BLOCKING;
    cfg.user_data         = u;

    esp_err_t ret = ppa_do_scale_rotate_mirror(u->srm_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA SRM scale failed: %d (src %ux%u scale %.2f/%.2f)",
                    (int)ret, src_w, src_h, (double)blk.scale_x, (double)blk.scale_y);
    }

    /* PPA floorf rounding leaves a 1-pixel gap at right/bottom edges.
     * Fill it by duplicating the last rendered column/row.
     * Must invalidate CPU cache first: PPA wrote via DMA, CPU cache is stale. */
    if(ret == ESP_OK && (blk.gap_right || blk.gap_bottom)) {
        esp_cache_msync(dest_buf->data, aligned_size,
                        ESP_CACHE_MSYNC_FLAG_DIR_M2C | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

        uint8_t * base = dest_buf->data;
        uint32_t stride = dest_buf->header.w * out_bpp;

        if(blk.gap_right && blk.clip_w >= 2) {
            uint32_t col = blk.dest_area.x1 + (uint32_t)blk.clip_w - 1;
            uint32_t col_prev = col - 1;
            for(int32_t y = 0; y < blk.clip_h; y++) {
                uint32_t row_off = (blk.dest_area.y1 + (uint32_t)y) * stride;
                lv_memcpy(base + row_off + col * out_bpp,
                          base + row_off + col_prev * out_bpp, out_bpp);
            }
        }
        if(blk.gap_bottom && blk.clip_h >= 2) {
            uint32_t row = blk.dest_area.y1 + (uint32_t)blk.clip_h - 1;
            uint32_t row_prev = row - 1;
            lv_memcpy(base + row * stride + blk.dest_area.x1 * out_bpp,
                      base + row_prev * stride + blk.dest_area.x1 * out_bpp,
                      (uint32_t)blk.clip_w * out_bpp);
        }

        esp_cache_msync(dest_buf->data, aligned_size,
                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }

    lv_image_decoder_close(&decoder_dsc);
}

/**
 * PPA SRM hardware-accelerated image rotation (0/90/180/270 degrees)
 * Uses the ESP32-P4 PPA Scale-Rotate-Mirror engine for zero-CPU-cost rotation.
 */
void lv_draw_ppa_img_rotate(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc,
                            const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_draw_ppa_unit_t * u = (lv_draw_ppa_unit_t *)t->draw_unit;
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * dest_buf = layer->draw_buf;

    /* Decode the source image */
    lv_image_decoder_dsc_t decoder_dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.stride_align = false;
    dec_args.premultiply = false;
    dec_args.no_cache = false;
    dec_args.use_indexed = false;
    dec_args.flush_cache = true;  /* Ensure cache coherency for PPA DMA */

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, dsc->src, &dec_args);
    if(res != LV_RESULT_OK) {
        LV_LOG_WARN("PPA SRM: failed to decode image");
        return;
    }

    const lv_draw_buf_t * decoded = decoder_dsc.decoded;
    if(!decoded || !decoded->data) {
        lv_image_decoder_close(&decoder_dsc);
        return;
    }

    lv_color_format_t src_cf = decoded->header.cf;
    lv_color_format_t dest_cf = dest_buf->header.cf;

    /* Verify PPA format support for both source and destination */
    if(!ppa_src_cf_supported(src_cf) || !ppa_dest_cf_supported(dest_cf)) {
        LV_LOG_WARN("PPA SRM: unsupported color format src=%d dest=%d", src_cf, dest_cf);
        lv_image_decoder_close(&decoder_dsc);
        return;
    }

    /* Map LVGL rotation (clockwise, 0.1 deg units) to PPA rotation (counter-clockwise) */
    int32_t angle = dsc->rotation % 3600;
    if(angle < 0) angle += 3600;

    ppa_srm_rotation_angle_t ppa_rot;
    switch(angle) {
        case 0:
            ppa_rot = PPA_SRM_ROTATION_ANGLE_0;
            break;
        case 900:
            ppa_rot = PPA_SRM_ROTATION_ANGLE_270;
            break;  /* 90° CW = 270° CCW */
        case 1800:
            ppa_rot = PPA_SRM_ROTATION_ANGLE_180;
            break;
        case 2700:
            ppa_rot = PPA_SRM_ROTATION_ANGLE_90;
            break;  /* 270° CW = 90° CCW */
        default:
            lv_image_decoder_close(&decoder_dsc);
            return;
    }

    uint32_t src_w = decoded->header.w;
    uint32_t src_h = decoded->header.h;

    /* Map the visible render tile back onto a PPA source block, clipping to the
     * layer buffer. Pure geometry, shared with the host unit test
     * (lv_draw_ppa_rot.h); also guarantees the destination stays inside the
     * buffer so the PPA cannot write out of bounds. */
    lv_draw_ppa_rot_block_t blk = lv_draw_ppa_rot_calc_block(
                                      coords, &layer->buf_area,
                                      (int32_t)dest_buf->header.w, (int32_t)dest_buf->header.h,
                                      (int32_t)src_w, (int32_t)src_h, angle);
    if(!blk.draw) {
        lv_image_decoder_close(&decoder_dsc);
        return;
    }

    /* Flush decoded source buffer for PPA DMA access. Align size to cache
     * line; _UNALIGNED flag is only a safety net for the address. */
    if(decoded->data_size > 0) {
        esp_cache_msync((void *)decoded->data,
                        lv_draw_ppa_align_size(decoded->data_size),
                        ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);
    }

    /* Configure PPA SRM operation */
    ppa_srm_oper_config_t cfg;
    lv_memzero(&cfg, sizeof(cfg));

    /* Input: the source sub-block that maps onto the visible (clipped) tile. */
    cfg.in.buffer         = (void *)decoded->data;
    cfg.in.pic_w          = src_w;
    cfg.in.pic_h          = src_h;
    cfg.in.block_w        = (uint32_t)blk.block_w;
    cfg.in.block_h        = (uint32_t)blk.block_h;
    cfg.in.block_offset_x = (uint32_t)blk.block_x;
    cfg.in.block_offset_y = (uint32_t)blk.block_y;
    cfg.in.srm_cm         = lv_color_format_to_ppa_srm(src_cf);

    uint32_t out_bpp_r    = (dest_cf == LV_COLOR_FORMAT_RGB565) ? 2u :
                            (dest_cf == LV_COLOR_FORMAT_RGB888)  ? 3u : 4u;
    uint32_t aligned_size_r = lv_draw_ppa_align_size(
                                  (uint32_t)dest_buf->header.w * dest_buf->header.h * out_bpp_r);

    /* Draw buffers are cache-aligned (lv_draw_buf_ppa_init_handlers). */
    cfg.out.buffer         = dest_buf->data;
    cfg.out.buffer_size    = aligned_size_r;
    cfg.out.pic_w          = dest_buf->header.w;
    cfg.out.pic_h          = dest_buf->header.h;
    cfg.out.block_offset_x = (uint32_t)blk.dest_area.x1;
    cfg.out.block_offset_y = (uint32_t)blk.dest_area.y1;
    cfg.out.srm_cm         = lv_color_format_to_ppa_srm(dest_cf);

    cfg.rotation_angle     = ppa_rot;
    /* ppa_evaluate() rejects rotation combined with scaling, so this path is
     * always 1:1 and the source-block geometry above assumes it. */
    cfg.scale_x            = 1.0f;
    cfg.scale_y            = 1.0f;
    cfg.mirror_x           = false;
    cfg.mirror_y           = false;
    cfg.rgb_swap           = false;
    cfg.byte_swap          = false;
    cfg.alpha_update_mode  = PPA_ALPHA_NO_CHANGE;
    cfg.mode               = PPA_TRANS_MODE_BLOCKING;
    cfg.user_data          = u;

    esp_err_t ret = ppa_do_scale_rotate_mirror(u->srm_client, &cfg);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA SRM rotation failed: %d  (src %ux%u, angle %d)", (int)ret, src_w, src_h, angle);
    }

    lv_image_decoder_close(&decoder_dsc);
}

#endif /* LV_USE_PPA_IMG */

#endif /* LV_USE_PPA */
