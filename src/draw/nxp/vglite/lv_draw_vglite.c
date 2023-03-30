/**
 * @file lv_draw_vglite.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2022, 2023 NXP
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

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite.h"

#if LV_USE_GPU_NXP_VG_LITE
#include <math.h>
#include "lv_draw_vglite_blend.h"
#include "lv_draw_vglite_line.h"
#include "lv_draw_vglite_rect.h"
#include "lv_draw_vglite_arc.h"
#include "lv_vglite_buf.h"

#if LV_COLOR_DEPTH != 32
    #include "../../../core/lv_refr.h"
#endif

/*********************
 *      DEFINES
 *********************/

/* Minimum area (in pixels) for VG-Lite blit/fill processing. */
#ifndef LV_GPU_NXP_VG_LITE_SIZE_LIMIT
    #define LV_GPU_NXP_VG_LITE_SIZE_LIMIT 5000
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_vglite_init_buf(lv_draw_ctx_t * draw_ctx);

static void lv_draw_vglite_wait_for_finish(lv_draw_ctx_t * draw_ctx);

static void lv_draw_vglite_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                       const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf);

static void lv_draw_vglite_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                       void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                       void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

static void lv_draw_vglite_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

static void lv_draw_vglite_line(lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1,
                                const lv_point_t * point2);

static void lv_draw_vglite_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

static lv_res_t lv_draw_vglite_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

static lv_res_t lv_draw_vglite_border(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc,
                                      const lv_area_t * coords);

static lv_res_t lv_draw_vglite_outline(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc,
                                       const lv_area_t * coords);

static void lv_draw_vglite_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                               uint16_t radius, uint16_t start_angle, uint16_t end_angle);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_vglite_ctx_t * vglite_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;
    vglite_draw_ctx->base_draw.init_buf = lv_draw_vglite_init_buf;
    vglite_draw_ctx->base_draw.draw_line = lv_draw_vglite_line;
    vglite_draw_ctx->base_draw.draw_arc = lv_draw_vglite_arc;
    vglite_draw_ctx->base_draw.draw_rect = lv_draw_vglite_rect;
    vglite_draw_ctx->base_draw.draw_img_decoded = lv_draw_vglite_img_decoded;
    vglite_draw_ctx->blend = lv_draw_vglite_blend;
    vglite_draw_ctx->base_draw.wait_for_finish = lv_draw_vglite_wait_for_finish;
    vglite_draw_ctx->base_draw.buffer_copy = lv_draw_vglite_buffer_copy;
}

void lv_draw_vglite_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_deinit_ctx(drv, draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * During rendering, LVGL might initializes new draw_ctxs and start drawing into
 * a separate buffer (called layer). If the content to be rendered has "holes",
 * e.g. rounded corner, LVGL temporarily sets the disp_drv.screen_transp flag.
 * It means the renderers should draw into an ARGB buffer.
 * With 32 bit color depth it's not a big problem but with 16 bit color depth
 * the target pixel format is ARGB8565 which is not supported by the GPU.
 * In this case, the VG-Lite callbacks should fallback to SW rendering.
 */
static inline bool need_argb8565_support()
{
#if LV_COLOR_DEPTH != 32
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(disp->driver->screen_transp == 1)
        return true;
#endif

    return false;
}

static void lv_draw_vglite_init_buf(lv_draw_ctx_t * draw_ctx)
{
    lv_gpu_nxp_vglite_init_buf(draw_ctx->buf, draw_ctx->buf_area, lv_area_get_width(draw_ctx->buf_area));
}

static void lv_draw_vglite_wait_for_finish(lv_draw_ctx_t * draw_ctx)
{
    vg_lite_finish();

    lv_draw_sw_wait_for_finish(draw_ctx);
}

static void lv_draw_vglite_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    if(need_argb8565_support()) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    bool done = false;
    /*Fill/Blend only non masked, normal blended*/
    if(dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL &&
       lv_area_get_size(&blend_area) >= LV_GPU_NXP_VG_LITE_SIZE_LIMIT) {
        const lv_color_t * src_buf = dsc->src_buf;

        if(src_buf == NULL) {
            done = (lv_gpu_nxp_vglite_fill(&blend_area, dsc->color, dsc->opa) == LV_RES_OK);
            if(!done)
                VG_LITE_LOG_TRACE("VG-Lite fill failed. Fallback.");
        }
        else {
            lv_area_t src_area;
            src_area.x1 = blend_area.x1 - (dsc->blend_area->x1 - draw_ctx->buf_area->x1);
            src_area.y1 = blend_area.y1 - (dsc->blend_area->y1 - draw_ctx->buf_area->y1);
            src_area.x2 = src_area.x1 + lv_area_get_width(dsc->blend_area) - 1;
            src_area.y2 = src_area.y1 + lv_area_get_height(dsc->blend_area) - 1;
            lv_coord_t src_stride = lv_area_get_width(dsc->blend_area);

#if VG_LITE_BLIT_SPLIT_ENABLED
            lv_color_t * dest_buf = draw_ctx->buf;
            lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

            done = (lv_gpu_nxp_vglite_blit_split(dest_buf, &blend_area, dest_stride,
                                                 src_buf, &src_area, src_stride, dsc->opa) == LV_RES_OK);
#else
            done = (lv_gpu_nxp_vglite_blit(&blend_area, src_buf, &src_area, src_stride, dsc->opa) == LV_RES_OK);
#endif

            if(!done)
                VG_LITE_LOG_TRACE("VG-Lite blit failed. Fallback.");
        }
    }

    if(!done)
        lv_draw_sw_blend_basic(draw_ctx, dsc);
}

static void lv_draw_vglite_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                       const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    if(need_argb8565_support()) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    const lv_color_t * src_buf = (const lv_color_t *)map_p;
    if(!src_buf) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t blend_area;
    bool has_transform = dsc->angle != 0 || dsc->zoom != LV_IMG_ZOOM_NONE;

    if(has_transform)
        lv_area_copy(&blend_area, &rel_coords);
    else if(!_lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    bool has_mask = lv_draw_mask_is_any(&blend_area);
    bool has_recolor = (dsc->recolor_opa != LV_OPA_TRANSP);

    bool done = false;
    if(!has_mask && !has_recolor && !lv_img_cf_is_chroma_keyed(cf) &&
       lv_area_get_size(&blend_area) >= LV_GPU_NXP_VG_LITE_SIZE_LIMIT
#if LV_COLOR_DEPTH != 32
       && !lv_img_cf_has_alpha(cf)
#endif
      ) {
        lv_area_t src_area;
        src_area.x1 = blend_area.x1 - (coords->x1 - draw_ctx->buf_area->x1);
        src_area.y1 = blend_area.y1 - (coords->y1 - draw_ctx->buf_area->y1);
        src_area.x2 = src_area.x1 + lv_area_get_width(coords) - 1;
        src_area.y2 = src_area.y1 + lv_area_get_height(coords) - 1;
        lv_coord_t src_stride = lv_area_get_width(coords);

#if VG_LITE_BLIT_SPLIT_ENABLED
        lv_color_t * dest_buf = draw_ctx->buf;
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

        if(has_transform)
            /* VG-Lite blit split with transformation is not supported! */
            done = false;
        else
            done = (lv_gpu_nxp_vglite_blit_split(dest_buf, &blend_area, dest_stride,
                                                 src_buf, &src_area, src_stride, dsc->opa) == LV_RES_OK);
#else
        if(has_transform)
            done = (lv_gpu_nxp_vglite_blit_transform(&blend_area, &rel_clip_area,
                                                     src_buf, &src_area, src_stride, dsc) == LV_RES_OK);
        else
            done = (lv_gpu_nxp_vglite_blit(&blend_area, src_buf, &src_area, src_stride, dsc->opa) == LV_RES_OK);
#endif

        if(!done)
            VG_LITE_LOG_TRACE("VG-Lite blit %sfailed. Fallback.", has_transform ? "transform " : "");
    }

    if(!done)
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
}

static void lv_draw_vglite_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                       void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                       void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area)
{
    bool done = false;

    if(lv_area_get_size(dest_area) >= LV_GPU_NXP_VG_LITE_SIZE_LIMIT) {
        done = lv_gpu_nxp_vglite_buffer_copy(dest_buf, dest_area, dest_stride, src_buf, src_area, src_stride);
        if(!done)
            VG_LITE_LOG_TRACE("VG-Lite buffer copy failed. Fallback.");
    }

    if(!done)
        lv_draw_sw_buffer_copy(draw_ctx, dest_buf, dest_stride, dest_area, src_buf, src_stride, src_area);
}

static void lv_draw_vglite_line(lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc, const lv_point_t * point1,
                                const lv_point_t * point2)
{
    if(dsc->width == 0)
        return;
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    if(point1->x == point2->x && point1->y == point2->y)
        return;

    if(need_argb8565_support()) {
        lv_draw_sw_line(draw_ctx, dsc, point1, point2);
        return;
    }

    lv_area_t rel_clip_area;
    rel_clip_area.x1 = LV_MIN(point1->x, point2->x) - dsc->width / 2;
    rel_clip_area.x2 = LV_MAX(point1->x, point2->x) + dsc->width / 2;
    rel_clip_area.y1 = LV_MIN(point1->y, point2->y) - dsc->width / 2;
    rel_clip_area.y2 = LV_MAX(point1->y, point2->y) + dsc->width / 2;

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &rel_clip_area, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_point_t rel_point1 = { point1->x - draw_ctx->buf_area->x1, point1->y - draw_ctx->buf_area->y1 };
    lv_point_t rel_point2 = { point2->x - draw_ctx->buf_area->x1, point2->y - draw_ctx->buf_area->y1 };

    bool done = false;
    bool has_mask = lv_draw_mask_is_any(&rel_clip_area);

    if(!has_mask) {
        done = (lv_gpu_nxp_vglite_draw_line(&rel_point1, &rel_point2, &rel_clip_area, dsc) == LV_RES_OK);
        if(!done)
            VG_LITE_LOG_TRACE("VG-Lite draw line failed. Fallback.");
    }

    if(!done)
        lv_draw_sw_line(draw_ctx, dsc, point1, point2);
}

static void lv_draw_vglite_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    if(need_argb8565_support()) {
        lv_draw_sw_rect(draw_ctx, dsc, coords);
        return;
    }

    lv_draw_rect_dsc_t vglite_dsc;

    lv_memcpy(&vglite_dsc, dsc, sizeof(vglite_dsc));
    vglite_dsc.bg_opa = 0;
    vglite_dsc.bg_img_opa = 0;
    vglite_dsc.border_opa = 0;
    vglite_dsc.outline_opa = 0;
#if LV_DRAW_COMPLEX
    /* Draw the shadow with CPU */
    lv_draw_sw_rect(draw_ctx, &vglite_dsc, coords);
    vglite_dsc.shadow_opa = 0;
#endif /*LV_DRAW_COMPLEX*/

    /* Draw the background */
    vglite_dsc.bg_opa = dsc->bg_opa;
    if(lv_draw_vglite_bg(draw_ctx, &vglite_dsc, coords) != LV_RES_OK)
        lv_draw_sw_rect(draw_ctx, &vglite_dsc, coords);
    vglite_dsc.bg_opa = 0;

    /* Draw the background image
     * It will be done once draw_ctx->draw_img_decoded()
     * callback gets called from lv_draw_sw_rect().
     */
    vglite_dsc.bg_img_opa = dsc->bg_img_opa;
    lv_draw_sw_rect(draw_ctx, &vglite_dsc, coords);
    vglite_dsc.bg_img_opa = 0;

    /* Draw the border */
    vglite_dsc.border_opa = dsc->border_opa;
    if(lv_draw_vglite_border(draw_ctx, &vglite_dsc, coords) != LV_RES_OK)
        lv_draw_sw_rect(draw_ctx, &vglite_dsc, coords);
    vglite_dsc.border_opa = 0;

    /* Draw the outline */
    vglite_dsc.outline_opa = dsc->outline_opa;
    if(lv_draw_vglite_outline(draw_ctx, &vglite_dsc, coords) != LV_RES_OK)
        lv_draw_sw_rect(draw_ctx, &vglite_dsc, coords);
}

static lv_res_t lv_draw_vglite_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->bg_opa <= (lv_opa_t)LV_OPA_MIN)
        return LV_RES_INV;

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if(dsc->border_width > 1 && dsc->border_opa >= (lv_opa_t)LV_OPA_MAX && dsc->radius != 0) {
        rel_coords.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
        rel_coords.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
        rel_coords.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
        rel_coords.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
    }
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &rel_coords, &rel_clip_area))
        return LV_RES_OK; /*Fully clipped, nothing to do*/

    bool has_mask = lv_draw_mask_is_any(&rel_coords);
    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color = (grad_dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE) ?
                          dsc->bg_color : dsc->bg_grad.stops[0].color;
    if(bg_color.full == dsc->bg_grad.stops[1].color.full)
        grad_dir = LV_GRAD_DIR_NONE;

    /*
     * Most simple case: just a plain rectangle (no mask, no radius, no gradient)
     * shall be handled by draw_ctx->blend().
     *
     * Complex case: gradient or radius but no mask.
     */
    if(!has_mask && ((dsc->radius != 0) || (grad_dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))) {
        lv_res_t res = lv_gpu_nxp_vglite_draw_bg(&rel_coords, &rel_clip_area, dsc);
        if(res != LV_RES_OK)
            VG_LITE_LOG_TRACE("VG-Lite draw bg failed. Fallback.");

        return res;
    }

    return LV_RES_INV;
}

static lv_res_t lv_draw_vglite_border(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc,
                                      const lv_area_t * coords)
{
    if(dsc->border_opa <= (lv_opa_t)LV_OPA_MIN)
        return LV_RES_INV;
    if(dsc->border_width == 0)
        return LV_RES_INV;
    if(dsc->border_post)
        return LV_RES_INV;
    if(dsc->border_side != (lv_border_side_t)LV_BORDER_SIDE_FULL)
        return LV_RES_INV;

    lv_area_t rel_coords;
    lv_coord_t border_width = dsc->border_width;

    /* Move border inwards to align with software rendered border */
    rel_coords.x1 = coords->x1 + ceil(border_width / 2.0f);
    rel_coords.x2 = coords->x2 - floor(border_width / 2.0f);
    rel_coords.y1 = coords->y1 + ceil(border_width / 2.0f);
    rel_coords.y2 = coords->y2 - floor(border_width / 2.0f);

    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &rel_coords, &rel_clip_area))
        return LV_RES_OK; /*Fully clipped, nothing to do*/

    lv_res_t res = lv_gpu_nxp_vglite_draw_border_generic(&rel_coords, &rel_clip_area, dsc, true);
    if(res != LV_RES_OK)
        VG_LITE_LOG_TRACE("VG-Lite draw border failed. Fallback.");

    return res;
}

static lv_res_t lv_draw_vglite_outline(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc,
                                       const lv_area_t * coords)
{
    if(dsc->outline_opa <= (lv_opa_t)LV_OPA_MIN)
        return LV_RES_INV;
    if(dsc->outline_width == 0)
        return LV_RES_INV;

    /* Move outline outwards to align with software rendered outline */
    lv_coord_t outline_pad = dsc->outline_pad - 1;
    lv_area_t rel_coords;
    rel_coords.x1 = coords->x1 - outline_pad - floor(dsc->outline_width / 2.0f);
    rel_coords.x2 = coords->x2 + outline_pad + ceil(dsc->outline_width / 2.0f);
    rel_coords.y1 = coords->y1 - outline_pad - floor(dsc->outline_width / 2.0f);
    rel_coords.y2 = coords->y2 + outline_pad + ceil(dsc->outline_width / 2.0f);

    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &rel_coords, &rel_clip_area))
        return LV_RES_OK; /*Fully clipped, nothing to do*/

    lv_res_t res = lv_gpu_nxp_vglite_draw_border_generic(&rel_coords, &rel_clip_area, dsc, false);
    if(res != LV_RES_OK)
        VG_LITE_LOG_TRACE("VG-Lite draw outline failed. Fallback.");

    return res;
}

static void lv_draw_vglite_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                               uint16_t radius, uint16_t start_angle, uint16_t end_angle)
{
    bool done = false;

#if LV_DRAW_COMPLEX
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(start_angle == end_angle)
        return;

    if(need_argb8565_support()) {
        lv_draw_sw_arc(draw_ctx, dsc, center, radius, start_angle, end_angle);
        return;
    }

    lv_point_t rel_center = {center->x - draw_ctx->buf_area->x1, center->y - draw_ctx->buf_area->y1};

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    bool has_mask = lv_draw_mask_is_any(&rel_clip_area);

    if(!has_mask) {
        done = (lv_gpu_nxp_vglite_draw_arc(&rel_center, (int32_t)radius, (int32_t)start_angle, (int32_t)end_angle,
                                           &rel_clip_area, dsc) == LV_RES_OK);
        if(!done)
            VG_LITE_LOG_TRACE("VG-Lite draw arc failed. Fallback.");
    }

#endif/*LV_DRAW_COMPLEX*/

    if(!done)
        lv_draw_sw_arc(draw_ctx, dsc, center, radius, start_angle, end_angle);
}

#endif /*LV_USE_GPU_NXP_VG_LITE*/
