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
#include <math.h>

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(dsc->side == LV_BORDER_SIDE_NONE)
        return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;

    lv_layer_t * layer = t->target_layer;
    lv_area_t inward_coords;
    int32_t width = dsc->width;

    /* Move border inwards to align with software rendered border */
    inward_coords.x1 = coords->x1 + ceil(width / 2.0f);
    inward_coords.x2 = coords->x2 - floor(width / 2.0f);
    inward_coords.y1 = coords->y1 + ceil(width / 2.0f);
    inward_coords.y2 = coords->y2 - floor(width / 2.0f);

    lv_area_move(&inward_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clip_area;
    lv_area_copy(&clip_area, &t->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &inward_coords, &clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_color_format_t dst_cf = layer->draw_buf->header.cf;
    uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

    /* the stride should be computed internally for NEMA_TSC images and images missing a stride value */
    int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                     -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

    /* Recalculate float Dimensions */
    float x1 = (float)coords->x1 + ((float)width / 2.0f) - (float)layer->buf_area.x1;
    float x2 = (float)coords->x2 - ((float)width / 2.0f) - (float)layer->buf_area.x1;
    float y1 = (float)coords->y1 + ((float)width / 2.0f) - (float)layer->buf_area.y1;
    float y2 = (float)coords->y2 - ((float)width / 2.0f) - (float)layer->buf_area.y1;
    float coords_bg_w = x2 - x1 + 1;
    float coords_bg_h = y2 - y1 + 1;
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    float radius = (float)LV_MIN(dsc->radius, short_side >> 1);

    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    uint32_t bg_color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);

    if(col32.alpha < 255U) {
        nema_set_blend_fill(NEMA_BL_SRC_OVER);
        bg_color = nema_premultiply_rgba(bg_color);
    }
    else {
        nema_set_blend_fill(NEMA_BL_SRC);
    }

    if(radius > 0.0f) {
        nema_draw_rounded_rect_aa(x1, y1, coords_bg_w, coords_bg_h, radius, width, bg_color);
    }
    else {
        lv_area_t rect_coords = *coords;
        lv_area_move(&rect_coords, -layer->buf_area.x1, -layer->buf_area.y1);
        int32_t border_width = lv_area_get_width(&rect_coords);
        int32_t border_height = lv_area_get_height(&rect_coords);

        if(dsc->side & LV_BORDER_SIDE_TOP) {
            float x = rect_coords.x1 + width;
            float y = rect_coords.y1;
            float w = border_width - 2 * width;
            float h = width;
            nema_enable_aa(1, 0, 1, 0);
            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        if(dsc->side & LV_BORDER_SIDE_BOTTOM) {
            float x = rect_coords.x1 + width;
            float y = rect_coords.y1 + border_height - width;
            float w = border_width - 2 * width;
            float h = width;
            nema_enable_aa(1, 0, 1, 0);
            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        if(dsc->side & LV_BORDER_SIDE_LEFT) {
            float x = rect_coords.x1;
            float y = rect_coords.y1 + width;
            float w = width;
            float h = border_height - 2 * width;
            nema_enable_aa(0, 1, 0, 1);
            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        if(dsc->side & LV_BORDER_SIDE_RIGHT) {
            float x = rect_coords.x1 + border_width - width;
            float y = rect_coords.y1 + width;
            float w = width;
            float h = border_height - 2 * width;
            nema_enable_aa(0, 1, 0, 1);
            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        /*Draw small corner rectangles
        Top Left*/
        if(dsc->side & LV_BORDER_SIDE_TOP || dsc->side & LV_BORDER_SIDE_LEFT) {
            float x = rect_coords.x1;
            float y = rect_coords.y1;
            float w = width;
            float h = width;

            if(!(dsc->side & LV_BORDER_SIDE_TOP))
                nema_enable_aa(1, 1, 0, 1);
            else if(!(dsc->side & LV_BORDER_SIDE_LEFT))
                nema_enable_aa(1, 0, 1, 1);
            else
                nema_enable_aa(1, 0, 0, 1);

            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        /*Top Right*/
        if(dsc->side & LV_BORDER_SIDE_TOP || dsc->side & LV_BORDER_SIDE_RIGHT) {
            float x = rect_coords.x1 + border_width - width;
            float y = rect_coords.y1;
            float w = width;
            float h = width;

            if(!(dsc->side & LV_BORDER_SIDE_TOP))
                nema_enable_aa(1, 1, 0, 1);
            else if(!(dsc->side & LV_BORDER_SIDE_RIGHT))
                nema_enable_aa(1, 1, 1, 0);
            else
                nema_enable_aa(1, 1, 0, 0);

            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        /*Bottom Right*/
        if(dsc->side & LV_BORDER_SIDE_BOTTOM || dsc->side & LV_BORDER_SIDE_RIGHT) {
            float x = rect_coords.x1 + border_width - width;
            float y = rect_coords.y1 + border_height - width;
            float w = width;
            float h = width;

            if(!(dsc->side & LV_BORDER_SIDE_BOTTOM))
                nema_enable_aa(0, 1, 1, 1);
            else if(!(dsc->side & LV_BORDER_SIDE_RIGHT))
                nema_enable_aa(1, 1, 1, 0);
            else
                nema_enable_aa(0, 1, 1, 0);

            nema_fill_rect_f(x, y, w, h, bg_color);
        }

        /*Bottom Left*/
        if(dsc->side & LV_BORDER_SIDE_BOTTOM || dsc->side & LV_BORDER_SIDE_LEFT) {
            float x = rect_coords.x1;
            float y = rect_coords.y1 + border_height - width;
            float w = width;
            float h = width;

            if(!(dsc->side & LV_BORDER_SIDE_BOTTOM))
                nema_enable_aa(0, 1, 1, 1);
            else if(!(dsc->side & LV_BORDER_SIDE_LEFT))
                nema_enable_aa(1, 0, 1, 1);
            else
                nema_enable_aa(0, 0, 1, 1);

            nema_fill_rect_f(x, y, w, h, bg_color);
        }

    }

    nema_cl_submit(&(draw_nema_gfx_unit->cl));

}
#endif
