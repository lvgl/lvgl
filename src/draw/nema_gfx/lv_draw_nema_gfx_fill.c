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
 * @file lv_draw_nema_gfx_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;

    lv_layer_t * layer = t->target_layer;
    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
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
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

    int32_t coords_bg_w = lv_area_get_width(&rel_coords);
    int32_t coords_bg_h = lv_area_get_height(&rel_coords);
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    int32_t radius = LV_MIN(dsc->radius, short_side >> 1);

    if((dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE)) {

        lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
        uint32_t bg_color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);

        if(col32.alpha < 255U) {
            nema_set_blend_fill(NEMA_BL_SRC_OVER);
            bg_color = nema_premultiply_rgba(bg_color);
        }
        else {
            nema_set_blend_fill(NEMA_BL_SRC);
        }

        if(radius > 0.f)
            nema_fill_rounded_rect_aa(rel_coords.x1, rel_coords.y1, coords_bg_w, coords_bg_h, radius, bg_color);
        else
            nema_fill_rect(rel_coords.x1, rel_coords.y1, coords_bg_w, coords_bg_h, bg_color);
    }
#if LV_USE_NEMA_VG
    else {

        nema_vg_paint_clear(draw_nema_gfx_unit->paint);

        nema_vg_set_blend(NEMA_BL_SRC_OVER | NEMA_BLOP_SRC_PREMULT);

        float stops[LV_GRADIENT_MAX_STOPS];
        color_var_t colors[LV_GRADIENT_MAX_STOPS];

        uint32_t cnt = LV_MIN(dsc->grad.stops_count, LV_GRADIENT_MAX_STOPS);

        for(uint8_t i = 0; i < cnt; i++) {
            stops[i] = (float)(dsc->grad.stops[i].frac) / 255.f;
            colors[i].a = LV_OPA_MIX2(dsc->grad.stops[i].opa, dsc->opa);
            colors[i].r = dsc->grad.stops[i].color.red;
            colors[i].g = dsc->grad.stops[i].color.green;
            colors[i].b = dsc->grad.stops[i].color.blue;
        }

        nema_vg_grad_set(draw_nema_gfx_unit->gradient, cnt, stops, colors);

        nema_tex_mode_t extend_type;
        switch(dsc->grad.extend) {
            case LV_GRAD_EXTEND_REPEAT:
                extend_type = NEMA_TEX_REPEAT;
                break;
            case LV_GRAD_EXTEND_REFLECT:
                extend_type = NEMA_TEX_MIRROR;
                break;
            case LV_GRAD_EXTEND_PAD:
            default:
                extend_type = NEMA_TEX_CLAMP;
                break;
        }

        if(dsc->grad.dir == LV_GRAD_DIR_VER || dsc->grad.dir == LV_GRAD_DIR_HOR
           || dsc->grad.dir == LV_GRAD_DIR_LINEAR) {
            nema_vg_paint_set_type(draw_nema_gfx_unit->paint, NEMA_VG_PAINT_GRAD_LINEAR);

            float x0, y0, x1, y1;

            if(dsc->grad.dir == LV_GRAD_DIR_HOR) {
                x0 = rel_coords.x1;
                x1 = rel_coords.x2;
                y0 = rel_coords.y1;
                y1 = rel_coords.y2;
            }
            else if(dsc->grad.dir == LV_GRAD_DIR_VER) {
                x0 = rel_coords.x1;
                x1 = rel_coords.x1;
                y0 = rel_coords.y1;
                y1 = rel_coords.y2;
            }
            else {
                x0 = rel_coords.x1 + lv_pct_to_px(dsc->grad.params.linear.start.x, lv_area_get_width(coords));
                x1 = rel_coords.x1 + lv_pct_to_px(dsc->grad.params.linear.end.x, lv_area_get_width(coords));
                y0 = rel_coords.y1 + lv_pct_to_px(dsc->grad.params.linear.start.y, lv_area_get_height(coords));
                y1 = rel_coords.y1 + lv_pct_to_px(dsc->grad.params.linear.end.y, lv_area_get_height(coords));
            }

            nema_vg_paint_set_grad_linear(draw_nema_gfx_unit->paint, draw_nema_gfx_unit->gradient, x0, y0, x1, y1,
                                          extend_type | NEMA_FILTER_BL);
        }
        else if(dsc->grad.dir == LV_GRAD_DIR_RADIAL) {
            nema_vg_paint_set_type(draw_nema_gfx_unit->paint, NEMA_VG_PAINT_GRAD_RADIAL);

            nema_vg_paint_set_grad_radial(draw_nema_gfx_unit->paint, draw_nema_gfx_unit->gradient,
                                          rel_coords.x1 + dsc->grad.params.radial.end.x,
                                          rel_coords.y1 + dsc->grad.params.radial.end.y,
                                          LV_ABS(dsc->grad.params.radial.end_extent.x - dsc->grad.params.radial.end.x),
                                          extend_type | NEMA_FILTER_BL);
        }

        if(radius > 0.f)
            nema_vg_draw_rounded_rect(rel_coords.x1, rel_coords.y1, coords_bg_w, coords_bg_h, radius, radius, NULL,
                                      draw_nema_gfx_unit->paint);
        else
            nema_vg_draw_rect(rel_coords.x1, rel_coords.y1, coords_bg_w, coords_bg_h, NULL, draw_nema_gfx_unit->paint);
    }
#endif

    nema_cl_submit(&(draw_nema_gfx_unit->cl));

}
#endif
