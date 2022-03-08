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

/**********************
 *  STATIC VARIABLES
 **********************/

/*
 * Detect if the image decoded is blend.
 */
static bool blend_img_decoded = false;
/*
 * Signal that image decoded is blend with CPU (software).
 */
static bool blend_with_cpu = false;
/*
 * Signal that image decoded is blend with PXP.
 */
static bool blend_with_pxp = false;
/*
 * Signal that image decoded is blend with VGLite.
 */
static bool blend_with_vglite = false;
/*
 * Save the blend status so that is visible from image decoded callback.
 */
static bool blend_done = false;


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

    nxp_draw_ctx->base_draw.draw_img_decoded = lv_draw_nxp_img_decoded;
    nxp_draw_ctx->blend = lv_draw_nxp_blend;
    //nxp_draw_ctx->base_draw.wait_for_finish = lv_draw_nxp_wait_cb;
}

void lv_draw_nxp_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_nxp_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    lv_area_t blend_area;

    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) {
        blend_done = true;

        return; /*Fully clipped, nothing to do*/
    }

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    if(blend_with_cpu) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        blend_done = true;

        return;
    }

    blend_done = false;

    /*Fill/Blend only non masked, normal blended*/
    if(dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        lv_color_t * dest_buf = draw_ctx->buf;
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
        lv_coord_t dest_width = lv_area_get_width(draw_ctx->buf_area);
        lv_coord_t dest_height = lv_area_get_height(draw_ctx->buf_area);

        const lv_color_t * src_buf = dsc->src_buf;

        if(src_buf == NULL) {
#if LV_USE_GPU_NXP_PXP
            if(!blend_with_vglite) {
                blend_done = (lv_gpu_nxp_pxp_fill(dest_buf, dest_stride, &blend_area,
                                                  dsc->color, dsc->opa) == LV_RES_OK);
                if(!blend_done)
                    PXP_LOG_TRACE("PXP fill failed. Fallback.");
            }
#endif
#if LV_USE_GPU_NXP_VG_LITE
            if(!blend_done && !blend_with_pxp) {
                blend_done = (lv_gpu_nxp_vglite_fill(dest_buf, dest_width, dest_height, &blend_area,
                                                     dsc->color, dsc->opa) == LV_RES_OK);
                if(!blend_done)
                    VG_LITE_LOG_TRACE("VG-Lite fill failed. Fallback.");
            }
#endif
        }
        else {
#if LV_USE_GPU_NXP_PXP
            if(!blend_with_vglite) {
                blend_done = (lv_gpu_nxp_pxp_blit(dest_buf, &blend_area, dest_stride, src_buf, dsc->blend_area,
                                                  dsc->opa) == LV_RES_OK);
                if(!blend_done)
                    PXP_LOG_TRACE("PXP blit failed. Fallback.");
            }
#endif
#if LV_USE_GPU_NXP_VG_LITE
            if(!blend_done && !blend_with_pxp) {
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

                blend_done = (lv_gpu_nxp_vglite_blit(&blit) == LV_RES_OK);

                if(!blend_done)
                    VG_LITE_LOG_TRACE("VG-Lite blit failed. Fallback.");
            }
#endif
        }
    }

    /*
     * Image decoded blend shall fallback to lv_draw_sw_img_decoded().
     */
    if(!blend_done && !blend_img_decoded) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        blend_done = true;
    }
}

static void lv_draw_nxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf)
{
    /*Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, draw_ctx->clip_area);
    bool mask_any = lv_draw_mask_is_any(&draw_area);

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.opa = dsc->opa;
    blend_dsc.blend_mode = dsc->blend_mode;
    blend_dsc.blend_area = coords;
    blend_dsc.src_buf = (const lv_color_t *)map_p;

    bool recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    bool scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool rotation = (dsc->angle != 0);

    blend_img_decoded = true;
    blend_done = false;

#if LV_USE_GPU_NXP_PXP
    if(!mask_any && !rotation && !scale
       && !lv_img_cf_has_alpha(cf)
      ) {
        if(lv_img_cf_is_chroma_keyed(cf))
            lv_gpu_nxp_pxp_enable_color_key();
        if(recolor)
            lv_gpu_nxp_pxp_enable_recolor(dsc->recolor, dsc->recolor_opa);

        blend_with_pxp = true;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
        blend_with_pxp = false;

        if(lv_img_cf_is_chroma_keyed(cf))
            lv_gpu_nxp_pxp_disable_color_key();
        if(recolor)
            lv_gpu_nxp_pxp_disable_recolor();
    }
#endif
#if LV_USE_GPU_NXP_VG_LITE
    if(!blend_done && !mask_any &&
       !lv_img_cf_is_chroma_keyed(cf) && !recolor
       && !rotation && !scale
       && !lv_img_cf_has_alpha(cf)
      ) {
        blend_with_vglite = true;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
        blend_with_vglite = false;
    }
#endif

    if(!blend_done) {
        blend_with_cpu = true;
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        blend_with_cpu = false;
    }

    blend_img_decoded = false;
}

#endif /*LV_USE_GPU_NXP_PXP || LV_USE_GPU_NXP_VG_LITE*/
