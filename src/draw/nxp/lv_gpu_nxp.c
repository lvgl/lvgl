/**
 * @file lv_gpu_nxp.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2022 NXP
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

#include "lv_gpu_nxp.h"

#if LV_USE_GPU_NXP_PXP || LV_USE_GPU_NXP_VG_LITE

/*
 * allow to use both PXP and VGLITE

 * both 2D accelerators can be used at the same time:
 * thus VGLITE can be used to accelerate widget drawing
 * while PXP accelerates Blit & Fill operations.
 */
#if LV_USE_GPU_NXP_PXP
    #include "pxp/lv_draw_pxp_blend.h"
#endif
#if LV_USE_GPU_NXP_VG_LITE
    #include "vglite/lv_draw_vglite_blend.h"
    #include "vglite/lv_draw_vglite_rect.h"
    #include "vglite/lv_draw_vglite_arc.h"
#endif

#if LV_COLOR_DEPTH != 32
    #include "../../core/lv_refr.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_nxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf);

static void lv_draw_nxp_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

static void lv_draw_nxp_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

static lv_res_t draw_nxp_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

static void lv_draw_nxp_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
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

void lv_draw_nxp_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_nxp_ctx_t * nxp_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    nxp_draw_ctx->base_draw.draw_arc = lv_draw_nxp_arc;
    nxp_draw_ctx->base_draw.draw_rect = lv_draw_nxp_rect;
    nxp_draw_ctx->base_draw.draw_img_decoded = lv_draw_nxp_img_decoded;
    nxp_draw_ctx->blend = lv_draw_nxp_blend;
    //nxp_draw_ctx->base_draw.wait_for_finish = lv_draw_nxp_wait_cb;
}

void lv_draw_nxp_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
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
 * In this case, the NXP callbacks should fallback to SW rendering.
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

static void lv_draw_nxp_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    lv_area_t blend_area;

    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    bool done = false;

    /*Fill/Blend only non masked, normal blended*/
    if(dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL && !need_argb8565_support()) {
        lv_color_t * dest_buf = draw_ctx->buf;
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
#if LV_USE_GPU_NXP_VG_LITE
        lv_coord_t dest_width = lv_area_get_width(draw_ctx->buf_area);
        lv_coord_t dest_height = lv_area_get_height(draw_ctx->buf_area);
#endif

        const lv_color_t * src_buf = dsc->src_buf;

        if(src_buf == NULL) {
#if LV_USE_GPU_NXP_PXP
            done = (lv_gpu_nxp_pxp_fill(dest_buf, dest_stride, &blend_area,
                                        dsc->color, dsc->opa) == LV_RES_OK);
            if(!done)
                PXP_LOG_TRACE("PXP fill failed. Fallback.");

#endif
#if LV_USE_GPU_NXP_VG_LITE
            if(!done) {
                done = (lv_gpu_nxp_vglite_fill(dest_buf, dest_width, dest_height, &blend_area,
                                               dsc->color, dsc->opa) == LV_RES_OK);
                if(!done)
                    VG_LITE_LOG_TRACE("VG-Lite fill failed. Fallback.");
            }
#endif
        }
        else {
#if LV_USE_GPU_NXP_PXP
            done = (lv_gpu_nxp_pxp_blit(dest_buf, &blend_area, dest_stride, src_buf, dsc->blend_area,
                                        dsc->opa, LV_DISP_ROT_NONE) == LV_RES_OK);
            if(!done)
                PXP_LOG_TRACE("PXP blit failed. Fallback.");
#endif
#if LV_USE_GPU_NXP_VG_LITE
            if(!done) {
                lv_gpu_nxp_vglite_blit_info_t blit;
                lv_coord_t src_stride = lv_area_get_width(dsc->blend_area);

                blit.src = src_buf;
                blit.src_width = lv_area_get_width(dsc->blend_area);
                blit.src_height = lv_area_get_height(dsc->blend_area);
                blit.src_stride = src_stride * (int32_t)sizeof(lv_color_t);
                blit.src_area.x1 = (blend_area.x1 - (dsc->blend_area->x1 - draw_ctx->buf_area->x1));
                blit.src_area.y1 = (blend_area.y1 - (dsc->blend_area->y1 - draw_ctx->buf_area->y1));
                blit.src_area.x2 = blit.src_area.x1 + blit.src_width - 1;
                blit.src_area.y2 = blit.src_area.y1 + blit.src_height - 1;

                blit.dst = dest_buf;
                blit.dst_width = dest_width;
                blit.dst_height = dest_height;
                blit.dst_stride = dest_stride * (int32_t)sizeof(lv_color_t);
                blit.dst_area.x1 = blend_area.x1;
                blit.dst_area.y1 = blend_area.y1;
                blit.dst_area.x2 = blend_area.x2;
                blit.dst_area.y2 = blend_area.y2;

                blit.opa = dsc->opa;
                blit.zoom = LV_IMG_ZOOM_NONE;
                blit.angle = 0;

                done = (lv_gpu_nxp_vglite_blit(&blit) == LV_RES_OK);

                if(!done)
                    VG_LITE_LOG_TRACE("VG-Lite blit failed. Fallback.");
            }
#endif
        }
    }

    if(!done)
        lv_draw_sw_blend_basic(draw_ctx, dsc);
}

static void lv_draw_nxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf)
{
    /*Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, draw_ctx->clip_area);
    bool mask_any = lv_draw_mask_is_any(&draw_area);
#if LV_USE_GPU_NXP_VG_LITE
    bool recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
#endif
#if LV_USE_GPU_NXP_PXP
    bool scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
#endif
    bool done = false;

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, coords, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    const lv_color_t * src_buf = (const lv_color_t *)map_p;
    if(!src_buf) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    lv_color_t * dest_buf = draw_ctx->buf;
    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

#if LV_USE_GPU_NXP_PXP
    if(!mask_any && !scale && !need_argb8565_support()
#if LV_COLOR_DEPTH!=32
       && !lv_img_cf_has_alpha(cf)
#endif
      ) {
        done = (lv_gpu_nxp_pxp_blit_transform(dest_buf, &blend_area, dest_stride, src_buf, coords,
                                              dsc, cf) == LV_RES_OK);
        if(!done)
            PXP_LOG_TRACE("PXP blit transform failed. Fallback.");
    }
#endif

#if LV_USE_GPU_NXP_VG_LITE
    if(!done && !mask_any && !need_argb8565_support() &&
       !lv_img_cf_is_chroma_keyed(cf) && !recolor
#if LV_COLOR_DEPTH!=32
       && !lv_img_cf_has_alpha(cf)
#endif
      ) {
        lv_gpu_nxp_vglite_blit_info_t blit;
        lv_coord_t src_stride = lv_area_get_width(coords);

        blit.src = src_buf;
        blit.src_width = lv_area_get_width(coords);
        blit.src_height = lv_area_get_height(coords);
        blit.src_stride = src_stride * (int32_t)sizeof(lv_color_t);
        blit.src_area.x1 = (blend_area.x1 - (coords->x1 - draw_ctx->buf_area->x1));
        blit.src_area.y1 = (blend_area.y1 - (coords->y1 - draw_ctx->buf_area->y1));
        blit.src_area.x2 = blit.src_area.x1 + blit.src_width - 1;
        blit.src_area.y2 = blit.src_area.y1 + blit.src_height - 1;

        blit.dst = dest_buf;
        blit.dst_width = lv_area_get_width(draw_ctx->buf_area);
        blit.dst_height = lv_area_get_height(draw_ctx->buf_area);
        blit.dst_stride = dest_stride * (int32_t)sizeof(lv_color_t);
        blit.dst_area.x1 = blend_area.x1;
        blit.dst_area.y1 = blend_area.y1;
        blit.dst_area.x2 = blend_area.x2;
        blit.dst_area.y2 = blend_area.y2;

        blit.opa = dsc->opa;
        blit.angle = dsc->angle;
        blit.pivot = dsc->pivot;
        blit.zoom = dsc->zoom;

        done = (lv_gpu_nxp_vglite_blit_transform(&blit) == LV_RES_OK);

        if(!done)
            VG_LITE_LOG_TRACE("VG-Lite blit transform failed. Fallback.");
    }
#endif

    if(!done)
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
}

static void lv_draw_nxp_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    bool done = false;
    lv_draw_rect_dsc_t nxp_dsc;

    lv_memcpy(&nxp_dsc, dsc, sizeof(nxp_dsc));
#if LV_DRAW_COMPLEX
    /* Draw only the shadow */
    nxp_dsc.bg_opa = 0;
    nxp_dsc.bg_img_opa = 0;
    nxp_dsc.border_opa = 0;
    nxp_dsc.outline_opa = 0;

    lv_draw_sw_rect(draw_ctx, &nxp_dsc, coords);

    /* Draw the background */
    nxp_dsc.shadow_opa = 0;
    nxp_dsc.bg_opa = dsc->bg_opa;
    done = (draw_nxp_bg(draw_ctx, &nxp_dsc, coords) == LV_RES_OK);
#endif /*LV_DRAW_COMPLEX*/

    /* Draw the remaining parts */
    nxp_dsc.shadow_opa = 0;
    if(done)
        nxp_dsc.bg_opa = 0;
    nxp_dsc.bg_img_opa = dsc->bg_img_opa;
    nxp_dsc.border_opa = dsc->border_opa;
    nxp_dsc.outline_opa = dsc->outline_opa;

    lv_draw_sw_rect(draw_ctx, &nxp_dsc, coords);
}

static lv_res_t draw_nxp_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->bg_opa <= LV_OPA_MIN)
        return LV_RES_INV;

    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if(dsc->border_width > 1 && dsc->border_opa >= (lv_opa_t)LV_OPA_MAX && dsc->radius != 0) {
        bg_coords.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
        bg_coords.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
        bg_coords.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
        bg_coords.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
    }

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &bg_coords, draw_ctx->clip_area))
        return LV_RES_INV;

    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color    = grad_dir == LV_GRAD_DIR_NONE ? dsc->bg_color : dsc->bg_grad.stops[0].color;
    if(bg_color.full == dsc->bg_grad.stops[1].color.full) grad_dir = LV_GRAD_DIR_NONE;

    bool mask_any = lv_draw_mask_is_any(&bg_coords);

    /*
     * Most simple case: just a plain rectangle (no mask, no radius, no gradient)
     * shall fallback to lv_draw_sw_blend().
     *
     * Complex case: gradient or radius but no mask.
     */
    if(!mask_any && ((dsc->radius != 0) || (grad_dir != LV_GRAD_DIR_NONE)) && !need_argb8565_support()) {
#if LV_USE_GPU_NXP_VG_LITE
        lv_res_t res = lv_gpu_nxp_vglite_draw_bg(draw_ctx, dsc, &bg_coords);
        if(res != LV_RES_OK)
            VG_LITE_LOG_TRACE("VG-Lite draw bg failed. Fallback.");

        return res;
#endif
    }

    return LV_RES_INV;
}

static void lv_draw_nxp_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                            uint16_t radius, uint16_t start_angle, uint16_t end_angle)
{
    bool done = false;

#if LV_DRAW_COMPLEX
    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(start_angle == end_angle)
        return;

#if LV_USE_GPU_NXP_VG_LITE
    if(!need_argb8565_support()) {
        done = (lv_gpu_nxp_vglite_draw_arc(draw_ctx, dsc, center, (int32_t)radius,
                                           (int32_t)start_angle, (int32_t)end_angle) == LV_RES_OK);
        if(!done)
            VG_LITE_LOG_TRACE("VG-Lite draw arc failed. Fallback.");
    }
#endif
#endif/*LV_DRAW_COMPLEX*/

    if(!done)
        lv_draw_sw_arc(draw_ctx, dsc, center, radius, start_angle, end_angle);
}

#endif /*LV_USE_GPU_NXP_PXP || LV_USE_GPU_NXP_VG_LITE*/
