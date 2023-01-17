/**
 * @file lv_draw_pxp.c
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

#include "lv_draw_pxp.h"

#if LV_USE_GPU_NXP_PXP
#include "lv_draw_pxp_blend.h"

#if LV_COLOR_DEPTH != 32
    #include "../../../core/lv_refr.h"
#endif

/*********************
 *      DEFINES
 *********************/

/* Minimum area (in pixels) for PXP blit/fill processing. */
#ifndef LV_GPU_NXP_PXP_SIZE_LIMIT
    #define LV_GPU_NXP_PXP_SIZE_LIMIT 5000
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_pxp_wait_for_finish(lv_draw_ctx_t * draw_ctx);

static void lv_draw_pxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t cf);

static void lv_draw_pxp_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_pxp_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_pxp_ctx_t * pxp_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;
    pxp_draw_ctx->base_draw.draw_img_decoded = lv_draw_pxp_img_decoded;
    pxp_draw_ctx->blend = lv_draw_pxp_blend;
    pxp_draw_ctx->base_draw.wait_for_finish = lv_draw_pxp_wait_for_finish;
}

void lv_draw_pxp_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
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
 * In this case, the PXP callbacks should fallback to SW rendering.
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

static void lv_draw_pxp_wait_for_finish(lv_draw_ctx_t * draw_ctx)
{
    lv_gpu_nxp_pxp_wait();

    lv_draw_sw_wait_for_finish(draw_ctx);
}

static void lv_draw_pxp_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    if(need_argb8565_support()) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to draw and the clip area*/
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
    if(dsc->mask_buf != NULL || dsc->blend_mode != LV_BLEND_MODE_NORMAL ||
       lv_area_get_size(&blend_area) < LV_GPU_NXP_PXP_SIZE_LIMIT) {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
        return;
    }

    /*Fill/Blend only non masked, normal blended*/
    lv_color_t * dest_buf = draw_ctx->buf;
    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
    const lv_color_t * src_buf = dsc->src_buf;

    if(src_buf == NULL) {
        lv_gpu_nxp_pxp_fill(dest_buf, &blend_area, dest_stride, dsc->color, dsc->opa);
    }
    else {
        lv_area_t src_area;
        src_area.x1 = blend_area.x1 - (dsc->blend_area->x1 - draw_ctx->buf_area->x1);
        src_area.y1 = blend_area.y1 - (dsc->blend_area->y1 - draw_ctx->buf_area->y1);
        src_area.x2 = src_area.x1 + lv_area_get_width(dsc->blend_area) - 1;
        src_area.y2 = src_area.y1 + lv_area_get_height(dsc->blend_area) - 1;
        lv_coord_t src_stride = lv_area_get_width(dsc->blend_area);

        lv_gpu_nxp_pxp_blit(dest_buf, &blend_area, dest_stride, src_buf, &src_area, src_stride,
                            dsc->opa, LV_DISP_ROT_NONE);
    }
}

static void lv_draw_pxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
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

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to draw and the clip area.*/
    if(!_lv_area_intersect(&blend_area, coords, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_coord_t src_width = lv_area_get_width(coords);
    lv_coord_t src_height = lv_area_get_height(coords);

    bool has_mask = lv_draw_mask_is_any(&blend_area);
    bool has_scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool has_rotation = (dsc->angle != 0);
    bool unsup_rotation = false;

    if(has_rotation) {
        /*
         * PXP can only rotate at 90x angles.
         */
        if(dsc->angle % 900) {
            PXP_LOG_TRACE("Rotation angle %d is not supported. PXP can rotate only 90x angle.", dsc->angle);
            unsup_rotation = true;
        }

        /*
         * PXP is set to process 16x16 blocks to optimize the system for memory
         * bandwidth and image processing time.
         * The output engine essentially truncates any output pixels after the
         * desired number of pixels has been written.
         * When rotating a source image and the output is not divisible by the block
         * size, the incorrect pixels could be truncated and the final output image
         * can look shifted.
         */
        if(src_width % 16 || src_height % 16) {
            PXP_LOG_TRACE("Rotation is not supported for image w/o alignment to block size 16x16.");
            unsup_rotation = true;
        }
    }

    if(has_mask || has_scale || unsup_rotation || lv_area_get_size(&blend_area) < LV_GPU_NXP_PXP_SIZE_LIMIT
#if LV_COLOR_DEPTH != 32
       || lv_img_cf_has_alpha(cf)
#endif
      ) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    lv_color_t * dest_buf = draw_ctx->buf;
    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - (coords->x1 - draw_ctx->buf_area->x1);
    src_area.y1 = blend_area.y1 - (coords->y1 - draw_ctx->buf_area->y1);
    src_area.x2 = src_area.x1 + src_width - 1;
    src_area.y2 = src_area.y1 + src_height - 1;
    lv_coord_t src_stride = lv_area_get_width(coords);

    lv_gpu_nxp_pxp_blit_transform(dest_buf, &blend_area, dest_stride, src_buf, &src_area, src_stride,
                                  dsc, cf);
}

#endif /*LV_USE_GPU_NXP_PXP*/
