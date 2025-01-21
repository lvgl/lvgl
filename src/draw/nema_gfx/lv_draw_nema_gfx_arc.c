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
 * @file lv_draw_nema_gfx_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX && LV_USE_NEMA_VG

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{

    LV_UNUSED(coords);

    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(dsc->start_angle == dsc->end_angle)
        return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;

    lv_layer_t * layer = t->target_layer;
    lv_point_t center = {dsc->center.x - layer->buf_area.x1, dsc->center.y - layer->buf_area.y1};

    lv_area_t clip_area;
    lv_area_copy(&clip_area, &t->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

    lv_value_precise_t start_angle = dsc->start_angle;
    lv_value_precise_t end_angle = dsc->end_angle;

    if(start_angle >= end_angle) {
        end_angle += 360.0f;
    }

    if(end_angle - start_angle > 360.0f) {
        start_angle = 0.0f;
        end_angle = 360.0f;
    }
    else {
        while(end_angle > 360.0f) {
            start_angle -= 360.0f;
            end_angle -= 360.0f;
        }
    }

    nema_vg_paint_clear(draw_nema_gfx_unit->paint);
    nema_vg_paint_set_type(draw_nema_gfx_unit->paint, NEMA_VG_PAINT_COLOR);
    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    uint32_t bg_color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);
    nema_vg_paint_set_paint_color(draw_nema_gfx_unit->paint, bg_color); // green
    nema_vg_paint_set_stroke_width(draw_nema_gfx_unit->paint, dsc->width);
    nema_vg_set_blend(NEMA_BL_SRC_OVER | NEMA_BLOP_SRC_PREMULT);

    if(dsc->rounded == 1) {
        nema_vg_draw_ring(center.x, center.y, (float)dsc->radius - (float)dsc->width * 0.5f, start_angle, end_angle,
                          draw_nema_gfx_unit->paint);
    }
    else {
        nema_vg_draw_ring_generic(center.x, center.y, (float)dsc->radius - (float)dsc->width * 0.5f, start_angle,
                                  end_angle, draw_nema_gfx_unit->paint, 0U);
    }

    nema_cl_submit(&(draw_nema_gfx_unit->cl));

}

#endif

