/**
 * @file lv_draw_g2d_img.c
 *
 */

/**
 * Copyright 2020-2023，2024 G2D
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"

#if LV_USE_DRAW_G2D

#include <math.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/* Blit w/ transformation for images w/o opa and alpha channel */
static void _g2d_blit_transform(uint8_t * dest_buf, const lv_area_t * dest_area, uint32_t dest_stride,
                                lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                                uint32_t src_stride, lv_color_format_t src_cf, const lv_draw_image_dsc_t * dsc, const lv_area_t * clip_area,
                                const lv_area_t * buf_area);

/* Blit simple w/ opa and alpha channel */
static void _g2d_blit(uint8_t * dest_buf, const lv_area_t * dest_area, uint32_t dest_stride,
                      lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                      uint32_t src_stride, lv_color_format_t src_cf, lv_opa_t opa);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                     const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;

    lv_area_t blend_area;
    lv_area_t transformed_area;
    lv_area_copy(&transformed_area, coords);
    bool has_transform = dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE;
    if(has_transform) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_image_buf_get_transformed_area(&transformed_area, w, h, dsc->rotation, dsc->scale_x, dsc->scale_y, &dsc->pivot);
        transformed_area.x1 += coords->x1;
        transformed_area.y1 += coords->y1;
        transformed_area.x2 += coords->x1;
        transformed_area.y2 += coords->y1;

    }
    if(has_transform)
        lv_area_copy(&blend_area, &transformed_area);
    else if(!_lv_area_intersect(&blend_area, draw_unit->clip_area, &transformed_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - coords->x1;
    src_area.y1 = blend_area.y1 - coords->y1;
    if(!has_transform) {
        src_area.x2 = src_area.x1 + lv_area_get_width(&blend_area) - 1;
        src_area.y2 = src_area.y1 + lv_area_get_height(&blend_area) - 1;
    }
    else {
        src_area.x2 = src_area.x1 + lv_area_get_width(coords) - 1;
        src_area.y2 = src_area.y1 + lv_area_get_height(coords) - 1;

    }

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }
    uint32_t src_stride = decoder_dsc.decoded->header.stride;
    lv_color_format_t src_cf = decoder_dsc.decoded->header.cf;

    const uint8_t * src_buf = decoder_dsc.decoded->data;
    uint8_t * dest_buf = layer->draw_buf->data;
    uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), layer->color_format);
    lv_color_format_t dest_cf = layer->color_format;

    if(has_transform)
        _g2d_blit_transform(dest_buf, &blend_area, dest_stride, dest_cf,
                            src_buf, &src_area, src_stride, src_cf, dsc, draw_unit->clip_area, &layer->buf_area);
    else {
        lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);
        _g2d_blit(dest_buf, &blend_area, dest_stride, dest_cf,
                  src_buf, &src_area, src_stride, src_cf, dsc->opa);
    }
    lv_image_decoder_close(&decoder_dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _g2d_blit_transform(uint8_t * dest_buf, const lv_area_t * dest_area, uint32_t dest_stride,
                                lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                                uint32_t src_stride, lv_color_format_t src_cf, const lv_draw_image_dsc_t * dsc, const lv_area_t * clip_area,
                                const lv_area_t * buf_area)
{
    int32_t src_w = lv_area_get_width(src_area);
    int32_t src_h = lv_area_get_height(src_area);

    bool has_scale = (dsc->scale_x != LV_SCALE_NONE) || (dsc->scale_y != LV_SCALE_NONE);
    uint8_t src_px_size = lv_color_format_get_size(src_cf);
    if(has_scale) {

        int32_t zoom_w = lv_area_get_width(dest_area);
        int32_t zoom_h = lv_area_get_height(dest_area);
        lv_color_t * scale_buf = lv_malloc(zoom_w * zoom_h * src_px_size);
        uint8_t * scale_buf_aligned = lv_draw_buf_align(scale_buf, dest_cf);
        if(NULL == scale_buf)
            return ;
        g2d_blt_h info;
        memset(&info, 0, sizeof(g2d_blt_h));
        info.flag_h = G2D_BLT_NONE_H;
        info.src_image_h.format = g2d_get_px_format(src_cf);
        info.src_image_h.clip_rect.x = 0;
        info.src_image_h.clip_rect.y = 0;
        info.src_image_h.clip_rect.w = src_w;
        info.src_image_h.clip_rect.h = src_h;
        info.src_image_h.width = src_stride / src_px_size;
        info.src_image_h.height = src_h;
        info.src_image_h.mode = G2D_PIXEL_ALPHA;
        info.src_image_h.alpha = 255;
        info.src_image_h.color = 0xee8899;
        info.src_image_h.align[0] = 0;
        info.src_image_h.align[1] = info.src_image_h.align[0];
        info.src_image_h.align[2] = info.src_image_h.align[0];
        info.src_image_h.laddr[0] = (uintptr_t)(src_buf);
        info.src_image_h.laddr[1] = (uintptr_t) 0;
        info.src_image_h.laddr[2] = (uintptr_t) 0;
        info.src_image_h.use_phy_addr = 1;

        info.dst_image_h.format = g2d_get_px_format(src_cf);
        info.dst_image_h.clip_rect.x = 0;
        info.dst_image_h.clip_rect.y = 0;
        info.dst_image_h.clip_rect.w = zoom_w;
        info.dst_image_h.clip_rect.h = zoom_h;
        info.dst_image_h.width = zoom_w;
        info.dst_image_h.height = zoom_h;
        info.dst_image_h.mode = G2D_PIXEL_ALPHA;
        info.dst_image_h.alpha = 255;
        info.dst_image_h.color = 0xee8899;
        info.dst_image_h.align[0] = 0;
        info.dst_image_h.align[1] = info.dst_image_h.align[0];
        info.dst_image_h.align[2] = info.dst_image_h.align[0];
        info.dst_image_h.laddr[0] = (uintptr_t)(scale_buf_aligned);
        info.dst_image_h.laddr[1] = (uintptr_t) 0;
        info.dst_image_h.laddr[2] = (uintptr_t) 0;
        info.dst_image_h.use_phy_addr = 1;

        unsigned long src_vaddr_start = (unsigned long)src_buf;
        unsigned long dest_vaddr_start = (unsigned long)scale_buf_aligned;

        hal_dcache_clean_invalidate(src_vaddr_start, src_stride * src_h);
        hal_dcache_clean_invalidate(dest_vaddr_start, zoom_w * zoom_h * src_px_size);
        if(sunxi_g2d_control(G2D_CMD_BITBLT_H, &info) < 0) {
            lv_free(scale_buf);
            return;
        }
        lv_area_t src_area_clip, dest_area_clip;
        lv_area_copy(&src_area_clip, clip_area);
        lv_area_move(&src_area_clip, -dest_area->x1, -dest_area->y1);
        lv_area_copy(&dest_area_clip, clip_area);
        lv_area_move(&dest_area_clip, -buf_area->x1, -buf_area->y1);
        _g2d_blit(dest_buf, &dest_area_clip, dest_stride, dest_cf,
                  scale_buf_aligned, &src_area_clip, zoom_w * src_px_size, src_cf, dsc->opa);
        lv_free(scale_buf);
    }

}

static void _g2d_blit(uint8_t * dest_buf, const lv_area_t * dest_area, uint32_t dest_stride,
                      lv_color_format_t dest_cf, const uint8_t * src_buf, const lv_area_t * src_area,
                      uint32_t src_stride, lv_color_format_t src_cf, lv_opa_t opa)
{
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);
    int32_t src_w = lv_area_get_width(src_area);
    int32_t src_h = lv_area_get_height(src_area);

    bool src_has_alpha = (src_cf == LV_COLOR_FORMAT_ARGB8888);
    uint8_t src_px_size = lv_color_format_get_size(src_cf);
    uint8_t dest_px_size = lv_color_format_get_size(dest_cf);

    lv_draw_buf_invalidate_cache((uint8_t *)src_buf, src_w, src_cf, (const lv_area_t *)src_area);

    unsigned long src_vaddr_start = (unsigned long)src_buf + src_stride * src_area->y1 + src_px_size * src_area->x1;
    unsigned long dest_vaddr_start = (unsigned long)dest_buf + dest_stride * dest_area->y1 + dest_px_size * dest_area->x1;

    hal_dcache_clean_invalidate(src_vaddr_start, src_stride * src_h);
    hal_dcache_clean_invalidate(dest_vaddr_start, dest_stride * dest_h);
    if(opa >= (lv_opa_t)LV_OPA_MAX && !src_has_alpha) {
        /*Simple blit, no effect - Disable PS buffer*/
        //G2D_SetProcessSurfacePosition(G2D_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);
        g2d_blt_h info;
        memset(&info, 0, sizeof(g2d_blt_h));
        info.src_image_h.mode = G2D_PIXEL_ALPHA;
        info.src_image_h.alpha = 255;
        info.flag_h = G2D_ROT_0;
        info.src_image_h.format = g2d_get_px_format(src_cf);
        info.src_image_h.clip_rect.x = 0;
        info.src_image_h.clip_rect.y = 0;
        info.src_image_h.clip_rect.w = src_w;
        info.src_image_h.clip_rect.h = src_h;
        info.src_image_h.width = src_stride / src_px_size;
        info.src_image_h.height = src_h;
        info.src_image_h.color = 0xee8899;
        info.src_image_h.align[0] = 0;
        info.src_image_h.align[1] = info.src_image_h.align[0];
        info.src_image_h.align[2] = info.src_image_h.align[0];
        info.src_image_h.laddr[0] = (uintptr_t)(src_buf + src_stride * src_area->y1 + src_px_size * src_area->x1);
        info.src_image_h.laddr[1] = (uintptr_t) 0;
        info.src_image_h.laddr[2] = (uintptr_t) 0;
        info.src_image_h.use_phy_addr = 1;

        info.dst_image_h.format = g2d_get_px_format(dest_cf);
        info.dst_image_h.clip_rect.x = 0;
        info.dst_image_h.clip_rect.y = 0;
        info.dst_image_h.clip_rect.w = dest_w;
        info.dst_image_h.clip_rect.h = dest_h;
        info.dst_image_h.width = dest_stride / dest_px_size;
        info.dst_image_h.height = dest_h;
        info.dst_image_h.mode = G2D_GLOBAL_ALPHA;
        info.dst_image_h.alpha = 255;
        info.dst_image_h.color = 0xee8899;
        info.dst_image_h.align[0] = 0;
        info.dst_image_h.align[1] = info.dst_image_h.align[0];
        info.dst_image_h.align[2] = info.dst_image_h.align[0];
        info.dst_image_h.laddr[0] = (uintptr_t)(dest_buf + dest_stride * dest_area->y1 + dest_px_size * dest_area->x1);
        info.dst_image_h.laddr[1] = (uintptr_t) 0;
        info.dst_image_h.laddr[2] = (uintptr_t) 0;
        info.dst_image_h.use_phy_addr = 1;

        if(sunxi_g2d_control(G2D_CMD_BITBLT_H, &info) < 0) {
            LV_LOG_WARN("Error: sunxifb_g2d_blit G2D_CMD_BITBLT_H failed/n");
            LV_LOG_WARN(
                "sunxifb_g2d_blit_to_fb src[phy=%p format=%d alpha=%d wh=[%ld %ld] clip=[%ld %ld %ld %ld]] "
                "dst=[phy=%p format=%d wh=[%ld %ld] clip=[%ld %ld %ld %ld]]\n",
                (void *) info.src_image_h.laddr[0], info.src_image_h.format,
                info.src_image_h.alpha, info.src_image_h.width,
                info.src_image_h.height, info.src_image_h.clip_rect.x,
                info.src_image_h.clip_rect.y, info.src_image_h.clip_rect.w,
                info.src_image_h.clip_rect.h, (void *) info.dst_image_h.laddr[0],
                info.dst_image_h.format, info.dst_image_h.width,
                info.dst_image_h.height, info.dst_image_h.clip_rect.x,
                info.dst_image_h.clip_rect.y, info.dst_image_h.clip_rect.w,
                info.dst_image_h.clip_rect.h);
            return ;
        }
    }
    else {
        g2d_bld info;
        lv_memset(&info, 0, sizeof(g2d_bld));
        info.bld_cmd = G2D_BLD_SRCOVER;
        if(opa > LV_OPA_MAX && src_has_alpha) {
            info.src_image[1].mode = G2D_PIXEL_ALPHA;
            info.src_image[1].alpha = 255;
        }
        else if(opa <= LV_OPA_MAX && !src_has_alpha) {
            info.src_image[1].mode = G2D_GLOBAL_ALPHA;
            info.src_image[1].alpha = opa;
        }
        else {
            info.src_image[1].mode = G2D_MIXER_ALPHA;
            info.src_image[1].alpha = opa;
        }
        info.src_image[1].format = g2d_get_px_format(src_cf);
        info.src_image[1].clip_rect.x = src_area->x1;
        info.src_image[1].clip_rect.y = src_area->y1;
        info.src_image[1].clip_rect.w = src_w;
        info.src_image[1].clip_rect.h = src_h;
        info.src_image[1].width = src_stride / src_px_size;
        info.src_image[1].height = src_area->y2 + 1;
        info.src_image[1].color = 0xee8899;
        info.src_image[1].align[0] = 0;
        info.src_image[1].align[1] = info.src_image[0].align[0];
        info.src_image[1].align[2] = info.src_image[0].align[0];
        info.src_image[1].laddr[0] = (uintptr_t)(src_buf);
        info.src_image[1].laddr[1] = (uintptr_t) 0;
        info.src_image[1].laddr[2] = (uintptr_t) 0;
        info.src_image[1].use_phy_addr = 1;

        info.dst_image.format = g2d_get_px_format(dest_cf);
        info.dst_image.clip_rect.x = dest_area->x1;
        info.dst_image.clip_rect.y = dest_area->y1;
        info.dst_image.clip_rect.w = dest_w;
        info.dst_image.clip_rect.h = dest_h;
        info.dst_image.width = dest_stride / dest_px_size;
        info.dst_image.height = dest_area->y2 + 1;
        info.dst_image.mode = G2D_PIXEL_ALPHA;
        info.dst_image.alpha = 255;
        info.dst_image.color = 0xee8899;
        info.dst_image.align[0] = 0;
        info.dst_image.align[1] = info.dst_image.align[0];
        info.dst_image.align[2] = info.dst_image.align[0];
        info.dst_image.laddr[0] = (uintptr_t)(dest_buf);
        info.dst_image.laddr[1] = (uintptr_t) 0;
        info.dst_image.laddr[2] = (uintptr_t) 0;
        info.dst_image.use_phy_addr = 1;
        /* src_image[1] is the top, src_image[0] is the bottom */
        /* src_image[0] is used as dst_image, no need to malloc a buffer */
        info.src_image[0] = info.dst_image;

        if(sunxi_g2d_control(G2D_CMD_BLD_H, &info) < 0) {
            LV_LOG_WARN("ERROR: sunxifb_g2d_blend G2D_CMD_BLD_H failed\n");
            LV_LOG_WARN(
                "sunxifb_g2d_blend "
                "src=[vir=%p phy=%p cmd=%x format=%d alpha=%d wh=[%ld %ld] clip=[%ld %ld %ld %ld]] "
                "dst=[vir=%p phy=%p format=%d wh=[%ld %ld] clip=[%ld %ld %ld %ld]]\n",
                (void *)info.src_image[1].laddr[0], (void *) info.src_image[1].laddr[0], info.bld_cmd,
                info.src_image[1].format, info.src_image[1].alpha,
                info.src_image[1].width, info.src_image[1].height,
                info.src_image[1].clip_rect.x, info.src_image[1].clip_rect.y,
                info.src_image[1].clip_rect.w, info.src_image[1].clip_rect.h,
                (void *)info.dst_image.laddr[0], (void *) info.dst_image.laddr[0], info.dst_image.format,
                info.dst_image.width, info.dst_image.height,
                info.dst_image.clip_rect.x, info.dst_image.clip_rect.y,
                info.dst_image.clip_rect.w, info.dst_image.clip_rect.h);
            return ;
        }
    }
}

#endif /*LV_USE_DRAW_G2D*/
