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
 * @file lv_draw_nema_gfx_triangle.c
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
void lv_draw_nema_gfx_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;

    lv_layer_t * layer = t->target_layer;

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t coords;
    coords.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    coords.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    coords.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    coords.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_area_move(&coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &coords, &rel_clip_area))
        return; /* Fully clipped, nothing to do */

    nema_set_clip(rel_clip_area.x1, rel_clip_area.y1, lv_area_get_width(&rel_clip_area),
                  lv_area_get_height(&rel_clip_area));

    lv_color_format_t dst_cf = layer->draw_buf->header.cf;
    uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

    /* the stride should be computed internally for NEMA_TSC images and images missing a stride value */
    int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                     -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

    if(dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE) {

        lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);

        if(col32.alpha < 255U) {
            nema_set_blend_fill(NEMA_BL_SIMPLE);
        }
        else {
            nema_set_blend_fill(NEMA_BL_SRC);
        }

        uint32_t bg_color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);

        nema_enable_aa(1, 1, 1, 0);
        nema_fill_triangle(dsc->p[0].x - layer->buf_area.x1, dsc->p[0].y - layer->buf_area.y1,
                           dsc->p[1].x - layer->buf_area.x1, dsc->p[1].y - layer->buf_area.y1,
                           dsc->p[2].x - layer->buf_area.x1, dsc->p[2].y - layer->buf_area.y1, bg_color);
    }
#if LV_USE_NEMA_VG
    else {

        nema_vg_path_clear(draw_nema_gfx_unit->path);
        nema_vg_paint_clear(draw_nema_gfx_unit->paint);

        nema_vg_paint_set_type(draw_nema_gfx_unit->paint, NEMA_VG_PAINT_GRAD_LINEAR);
        nema_vg_set_blend(NEMA_BL_SRC_OVER | NEMA_BLOP_SRC_PREMULT);
        nema_vg_set_fill_rule(NEMA_VG_FILL_EVEN_ODD);


        float stops[LV_GRADIENT_MAX_STOPS];
        color_var_t colors[LV_GRADIENT_MAX_STOPS];

        uint32_t cnt = LV_MAX(dsc->grad.stops_count, LV_GRADIENT_MAX_STOPS);

        for(uint8_t i = 0; i < cnt; i++) {
            stops[i] = (float)(dsc->grad.stops[i].frac) / 255.f;
            colors[i].a = dsc->grad.stops[i].opa;
            colors[i].r = dsc->grad.stops[i].color.red;
            colors[i].g = dsc->grad.stops[i].color.green;
            colors[i].b = dsc->grad.stops[i].color.blue;
        }

        nema_vg_grad_set(draw_nema_gfx_unit->gradient, cnt, stops, colors);

        //Calculate Bounding Box
        float min_x = LV_MIN(dsc->p[0].x, dsc->p[1].x);
        min_x = LV_MIN(dsc->p[2].x, min_x);

        float min_y = LV_MIN(dsc->p[0].y, dsc->p[1].y);
        min_y = LV_MIN(dsc->p[2].y, min_y);

        float max_x = LV_MAX(dsc->p[0].x, dsc->p[1].x);
        max_x = LV_MAX(dsc->p[2].x, max_x);

        float max_y = LV_MAX(dsc->p[0].y, dsc->p[1].y);
        max_y = LV_MAX(dsc->p[2].y, max_y);

        float x0, x1, y0, y1;

        if(dsc->grad.dir == LV_GRAD_DIR_HOR) {
            x0 = min_x;
            x1 = max_x;
            y0 = min_y;
            y1 = min_y;
        }
        else {
            x0 = min_x;
            x1 = min_x;
            y0 = min_y;
            y1 = max_y;
        }

        y0 -= (float) layer->buf_area.y1;
        y1 -= (float) layer->buf_area.y1;
        x0 -= (float) layer->buf_area.x1;
        x1 -= (float) layer->buf_area.x1;

        uint8_t cmds_polygon[] = {NEMA_VG_PRIM_MOVE, NEMA_VG_PRIM_POLYGON};
        float triangle_coords[] = {dsc->p[0].x - layer->buf_area.x1, dsc->p[0].y - layer->buf_area.y1,
                                   4.0f,
                                   dsc->p[1].x - layer->buf_area.x1, dsc->p[1].y - layer->buf_area.y1,
                                   dsc->p[2].x - layer->buf_area.x1, dsc->p[2].y - layer->buf_area.y1
                                  };
        nema_enable_aa(0, 0, 0, 0);
        nema_vg_paint_set_grad_linear(draw_nema_gfx_unit->paint, draw_nema_gfx_unit->gradient, x0, y0, x1, y1,
                                      NEMA_TEX_CLAMP | NEMA_FILTER_BL);
        nema_vg_path_set_shape(draw_nema_gfx_unit->path, 2, cmds_polygon, 7, triangle_coords);
        nema_vg_draw_path(draw_nema_gfx_unit->path, draw_nema_gfx_unit->paint);
    }
#endif

    nema_cl_submit(&(draw_nema_gfx_unit->cl));

}
#endif
