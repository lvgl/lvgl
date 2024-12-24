/**
 * MIT License
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file lv_draw_nema_gfx_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "../sw/blend/lv_draw_sw_blend_private.h"
#include "../lv_draw_private.h"
#include "../sw/lv_draw_sw.h"
#include "../../core/lv_global.h"

#if LV_USE_NEMA_GFX

#include "lv_draw_nema_gfx.h"

#if LV_DRAW_BLEND_CUSTOM == LV_DRAW_BLEND_CUSTOM_NEMA_GFX

#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_blend(lv_draw_unit_t * draw_unit, const lv_draw_sw_blend_dsc_t * blend_dsc)
{
    /*Do not draw transparent things*/
    if(blend_dsc->opa <= LV_OPA_MIN) return;
    if(blend_dsc->mask_buf && blend_dsc->mask_res == LV_DRAW_SW_MASK_RES_TRANSP) return;

    lv_area_t blend_area;
    if(!lv_area_intersect(&blend_area, blend_dsc->blend_area, draw_unit->clip_area)) return;

    LV_PROFILER_DRAW_BEGIN;
    lv_layer_t * layer = draw_unit->target_layer;
    uint32_t layer_stride_byte = layer->draw_buf->header.stride;

    if(blend_dsc->src_buf == NULL) {
        lv_draw_sw_blend_fill_dsc_t fill_dsc;
        fill_dsc.dest_w = lv_area_get_width(&blend_area);
        fill_dsc.dest_h = lv_area_get_height(&blend_area);
        fill_dsc.dest_stride = layer_stride_byte;
        fill_dsc.opa = blend_dsc->opa;
        fill_dsc.color = blend_dsc->color;

        if(blend_dsc->mask_buf == NULL) fill_dsc.mask_buf = NULL;
        else if(blend_dsc->mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) fill_dsc.mask_buf = NULL;
        else fill_dsc.mask_buf = blend_dsc->mask_buf;

        fill_dsc.relative_area  = blend_area;
        if(fill_dsc.mask_buf) {
            fill_dsc.mask_stride = blend_dsc->mask_stride == 0  ? lv_area_get_width(blend_dsc->mask_area) : blend_dsc->mask_stride;
            fill_dsc.mask_buf += fill_dsc.mask_stride * (blend_area.y1 - blend_dsc->mask_area->y1) +
                                 (blend_area.x1 - blend_dsc->mask_area->x1);
        }

        lv_layer_t * layer = draw_unit->target_layer;
        lv_area_t rel_coords;
        lv_area_copy(&rel_coords, &blend_area);
        lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

        lv_area_t rel_clip_area;
        lv_area_copy(&rel_clip_area, draw_unit->clip_area);
        lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

        nema_set_clip(rel_clip_area.x1, rel_clip_area.y1, lv_area_get_width(&rel_clip_area),
                      lv_area_get_height(&rel_clip_area));

        lv_area_t clipped_coords;
        if(!lv_area_intersect(&clipped_coords, &rel_coords, &rel_clip_area))
            return; /*Fully clipped, nothing to do*/

        lv_color_format_t dst_cf = layer->draw_buf->header.cf;
        uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

        /* the stride should be computed internally for NEMA_TSC images and images missing a stride value */
        int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                         -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

        nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                          lv_area_get_height(&(layer->buf_area)), lv_nemagfx_cf_to_nema(layer->color_format), stride);

        int32_t coords_bg_w = lv_area_get_width(&rel_coords);
        int32_t coords_bg_h = lv_area_get_height(&rel_coords);

        lv_color32_t col32 = lv_color_to_32(blend_dsc->color, blend_dsc->opa);
        uint32_t bg_color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);

        uint32_t blending_mode;

        if(fill_dsc.mask_buf) {
            nema_bind_src_tex((uintptr_t)(fill_dsc.mask_buf), lv_area_get_width(blend_dsc->mask_area),
                              lv_area_get_height(blend_dsc->mask_area), NEMA_A8, fill_dsc.mask_stride, NEMA_FILTER_BL);

            nema_set_tex_color(bg_color);

            if(col32.alpha < 255U) {
                nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
                nema_set_const_color(bg_color);
            }
            else {
                nema_set_blend_blit(NEMA_BL_SIMPLE);
            }

            nema_blit_rect(rel_coords.x1, rel_coords.y1, coords_bg_w, coords_bg_h);

        }
        else {
            if(col32.alpha < 255U) {
                nema_set_blend_fill(NEMA_BL_SRC_OVER);
                bg_color = nema_premultiply_rgba(bg_color);
            }
            else {
                nema_set_blend_fill(NEMA_BL_SRC);
            }

            nema_fill_rect(rel_coords.x1, rel_coords.y1, coords_bg_w, coords_bg_h, bg_color);
        }
    }
    else {
        if(!lv_area_intersect(&blend_area, &blend_area, blend_dsc->src_area)) {
            LV_PROFILER_DRAW_END;
            return;
        }

        if(blend_dsc->mask_area && !lv_area_intersect(&blend_area, &blend_area, blend_dsc->mask_area)) {
            LV_PROFILER_DRAW_END;
            return;
        }

        lv_draw_sw_blend_image_dsc_t image_dsc;
        image_dsc.dest_w = lv_area_get_width(&blend_area);
        image_dsc.dest_h = lv_area_get_height(&blend_area);
        image_dsc.dest_stride = layer_stride_byte;

        image_dsc.opa = blend_dsc->opa;
        image_dsc.blend_mode = blend_dsc->blend_mode;
        image_dsc.src_stride = blend_dsc->src_stride;
        image_dsc.src_color_format = blend_dsc->src_color_format;

        const uint8_t * src_buf = blend_dsc->src_buf;
        uint32_t src_px_size = lv_color_format_get_bpp(blend_dsc->src_color_format);
        src_buf += image_dsc.src_stride * (blend_area.y1 - blend_dsc->src_area->y1);
        src_buf += ((blend_area.x1 - blend_dsc->src_area->x1) * src_px_size) >> 3;
        image_dsc.src_buf = src_buf;


        if(blend_dsc->mask_buf == NULL) image_dsc.mask_buf = NULL;
        else if(blend_dsc->mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) image_dsc.mask_buf = NULL;
        else image_dsc.mask_buf = blend_dsc->mask_buf;

        if(image_dsc.mask_buf) {
            image_dsc.mask_buf = blend_dsc->mask_buf;
            image_dsc.mask_stride = blend_dsc->mask_stride ? blend_dsc->mask_stride : lv_area_get_width(blend_dsc->mask_area);
            image_dsc.mask_buf += image_dsc.mask_stride * (blend_area.y1 - blend_dsc->mask_area->y1) +
                                  (blend_area.x1 - blend_dsc->mask_area->x1);
        }

        image_dsc.relative_area  = blend_area;
        lv_area_move(&image_dsc.relative_area, -layer->buf_area.x1, -layer->buf_area.y1);

        image_dsc.src_area  = *blend_dsc->src_area;
        lv_area_move(&image_dsc.src_area, -layer->buf_area.x1, -layer->buf_area.y1);

        lv_area_t rel_clip_area;
        lv_area_copy(&rel_clip_area, draw_unit->clip_area);
        lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

        nema_set_clip(rel_clip_area.x1, rel_clip_area.y1, lv_area_get_width(&rel_clip_area),
                      lv_area_get_height(&rel_clip_area));

        lv_color_format_t dst_cf = layer->draw_buf->header.cf;
        uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

        uint32_t blending_mode = lv_nemagfx_blending_mode(blend_dsc->blend_mode);

        lv_color_format_t src_cf = blend_dsc->src_color_format;

        uint32_t src_nema_cf = lv_nemagfx_cf_to_nema(src_cf);

        /*Image contains Alpha*/
        if(src_cf == LV_COLOR_FORMAT_ARGB8888 || src_cf == LV_COLOR_FORMAT_XRGB8888) {
            blending_mode |= NEMA_BLOP_SRC_PREMULT;
        }

        uint32_t tex_w = lv_area_get_width(&blend_area);
        uint32_t tex_h = lv_area_get_height(&blend_area);

        /* the stride should be computed internally for NEMA_TSC images and images missing a stride value */
        int32_t src_stride = (src_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && src_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                             -1 : lv_area_get_width(&blend_area) * lv_color_format_get_size(src_cf);

        int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                         -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

        nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                          lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

        nema_bind_src_tex((uintptr_t)NEMA_VIRT2PHYS(src_buf), tex_w, tex_h, src_nema_cf, src_stride, NEMA_FILTER_PS);


        if(blend_dsc->opa < 255) {
            uint32_t rgba = ((uint32_t)blend_dsc->opa << 24U) | ((uint32_t)blend_dsc->opa << 16U) |
                            ((uint32_t)blend_dsc->opa << 8U) | ((uint32_t)blend_dsc->opa);
            nema_set_const_color(rgba);
            blending_mode |= NEMA_BLOP_MODULATE_A;
        }

        if(image_dsc.mask_buf) {
            blending_mode |= NEMA_BLOP_STENCIL_TXTY;
            nema_bind_tex(NEMA_TEX3, (uintptr_t)(image_dsc.mask_buf), lv_area_get_width(blend_dsc->mask_area),
                          lv_area_get_height(blend_dsc->mask_area), NEMA_A8, image_dsc.mask_stride, NEMA_FILTER_BL);
        }

        nema_set_blend_blit(blending_mode);

        nema_blit_rect(image_dsc.src_area.x1, image_dsc.src_area.y1, tex_w, tex_h);
    }

    /* Perform Submition */
    lv_draw_unit_t * draw_unit_u = _draw_info.unit_head;
    while(draw_unit_u) {
        if(lv_strcmp(draw_unit_u->name, "NEMA_GFX") == 0) {
            draw_unit_u->wait_for_finish_cb(draw_unit_u);
            break;
        }
        draw_unit_u = draw_unit_u->next;
    }
}

#endif
#endif
