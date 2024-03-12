/**
 * @file lv_draw_g2d_fill.c
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
#include <g2d_driver.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _g2d_fill(uint8_t * dest_buf, const lv_area_t * dest_area, uint32_t dest_stride,
                      lv_area_t * disp_area, lv_color_format_t dest_cf, const lv_draw_fill_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                      const lv_area_t * coords)
{

    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    lv_layer_t * layer = draw_unit->target_layer;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    uint8_t * dest_buf = layer->draw_buf->data;
    uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), layer->color_format);
    lv_color_format_t dest_cf = layer->color_format;
    lv_area_t buf_area = layer->buf_area;
    _g2d_fill(dest_buf, &blend_area, dest_stride, &buf_area, dest_cf, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _g2d_fill(uint8_t * dest_buf, const lv_area_t * dest_area, uint32_t dest_stride,
                      lv_area_t * disp_area, lv_color_format_t dest_cf, const lv_draw_fill_dsc_t * dsc)
{
    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);

    uint8_t px_size = lv_color_format_get_size(dest_cf);

    unsigned long vaddr_start = (unsigned long)dest_buf + dest_stride * dest_area->y1 + px_size * dest_area->x1;
    hal_dcache_clean_invalidate(vaddr_start, dest_stride * dest_h);

    g2d_fillrect_h info;
    lv_memset(&info, 0, sizeof(g2d_fillrect_h));

    if(dsc->opa >= (lv_opa_t)LV_OPA_MAX) {
        info.dst_image_h.alpha = 255;
    }
    else {
        info.dst_image_h.alpha = dsc->opa;
    }

    info.dst_image_h.mode = G2D_PIXEL_ALPHA;
    info.dst_image_h.color = (lv_color_to_u32(dsc->color) & (~(0xff << 24))) | (dsc->opa << 24);
    info.dst_image_h.format = 0;
    info.dst_image_h.clip_rect.x = 0;
    info.dst_image_h.clip_rect.y = 0;
    info.dst_image_h.clip_rect.w = dest_w;
    info.dst_image_h.clip_rect.h = dest_h;
    info.dst_image_h.width = dest_stride / px_size;
    info.dst_image_h.height = dest_h;
    info.dst_image_h.align[0] = 0;
    info.dst_image_h.align[1] = info.dst_image_h.align[0];
    info.dst_image_h.align[2] = info.dst_image_h.align[0];
    info.dst_image_h.laddr[0] = (uintptr_t)(dest_buf + dest_stride * dest_area->y1 + px_size * dest_area->x1);
    info.dst_image_h.laddr[1] = (uintptr_t) 0;
    info.dst_image_h.laddr[2] = (uintptr_t) 0;
    info.dst_image_h.use_phy_addr = 1;

    if(sunxi_g2d_control(G2D_CMD_FILLRECT_H, &info) < 0) {
        LV_LOG_WARN("ERROR: sunxifb_g2d_fill G2D_CMD_FILLRECT_H failed\n");
        LV_LOG_WARN(
            "sunxifb_g2d_fill dst=[vir=%p phy=%p color=%lx alpha=%d format=%d wh=[%ld %ld] clip=[%ld %ld %ld %ld]]\n",
            dest_buf, (void *) info.dst_image_h.laddr[0], info.dst_image_h.color,
            info.dst_image_h.alpha, info.dst_image_h.format,
            info.dst_image_h.width, info.dst_image_h.height,
            info.dst_image_h.clip_rect.x, info.dst_image_h.clip_rect.y,
            info.dst_image_h.clip_rect.w, info.dst_image_h.clip_rect.h);
        return ;
    }
    return ;

}

#endif /*LV_USE_DRAW_G2D*/
