/**
 * @file lv_draw_nema_gfx.c
 *
 */

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

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_refr.h"

#if LV_USE_NEMA_GFX

#include "lv_draw_nema_gfx.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_res_t draw_nema_gfx_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);
static inline bool Isargb8565(lv_img_cf_t cf);
static uint32_t lv_cf_to_nema(lv_img_cf_t cf);
static uint32_t skip_pallete(lv_img_cf_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nema_gfx_init(void)
{
    nema_init();
    return;
}

static inline bool Isargb8565(lv_img_cf_t cf)
{
#if LV_COLOR_DEPTH != 32
    if(cf == LV_IMG_CF_TRUE_COLOR_ALPHA)
        return true;
#endif
    return false;
}

static uint32_t lv_cf_to_nema(lv_img_cf_t cf)
{
    switch(cf) {
        case LV_IMG_CF_ALPHA_1BIT:
            return NEMA_A1;
        case LV_IMG_CF_ALPHA_2BIT:
            return NEMA_A2;
        case LV_IMG_CF_ALPHA_4BIT:
            return NEMA_A4;
        case LV_IMG_CF_ALPHA_8BIT:
            return NEMA_A8;
        case LV_IMG_CF_INDEXED_1BIT:
            return NEMA_L1;
        case LV_IMG_CF_INDEXED_2BIT:
            return NEMA_L2;
        case LV_IMG_CF_INDEXED_4BIT:
            return NEMA_L4;
        case LV_IMG_CF_INDEXED_8BIT:
            return NEMA_L8;
        default:
            return LV_NEMA_GFX_COLOR_FORMAT;
    }
}

static uint32_t skip_pallete(lv_img_cf_t cf)
{
    switch(cf) {
        case LV_IMG_CF_INDEXED_1BIT:
            return 2;
        case LV_IMG_CF_INDEXED_2BIT:
            return 4;
        case LV_IMG_CF_INDEXED_4BIT:
            return 16;
        default:
            return 256;
    }
}

void lv_draw_nema_gfx_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc, const lv_area_t * coords,
                                  const uint8_t * map_p, lv_img_cf_t cf)
{

    /*Use the clip area as draw area*/
    lv_area_t draw_area;
    lv_area_copy(&draw_area, draw_ctx->clip_area);
    bool mask_any = lv_draw_mask_is_any(&draw_area);
    bool recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    bool transform = dsc->angle != 0 || dsc->zoom != LV_IMG_ZOOM_NONE ? true : false;

    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, coords, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_coord_t tex_w = lv_area_get_width(coords);
    lv_coord_t tex_h = lv_area_get_height(coords);

    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_ctx->clip_area);
    lv_area_move(&clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    const lv_color_t * src_buf = (const lv_color_t *)map_p;

    if(!src_buf || Isargb8565(cf)) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    lv_color_t * dest_buf = draw_ctx->buf;
    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);
    uint32_t blending_mode = NEMA_BL_SIMPLE;

    uint32_t color_format = lv_cf_to_nema(cf);

    nema_buffer_t Shuffle_pallete_bo;
    if(color_format == NEMA_L8) {
        Shuffle_pallete_bo = nema_buffer_create(256 * 4);
        uint32_t * Shuffle_pallete = Shuffle_pallete_bo.base_virt;
        for(int i = 0; i < 256; i++) {
            int idx = ((i >> 4) | ((i & 0xfU) << 4));
            lv_color32_t col32 = {.full = lv_color_to32(*(lv_color_t *)((uint32_t *)src_buf + idx))};
            Shuffle_pallete[i] = col32.full;
        }
    }

    if(!mask_any) {
        nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(draw_ctx->buf), lv_area_get_width(draw_ctx->buf_area),
                          lv_area_get_height(draw_ctx->buf_area), LV_NEMA_GFX_COLOR_FORMAT,
                          lv_area_get_width(draw_ctx->buf_area)*LV_NEMA_GFX_FORMAT_MULTIPLIER);
        nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));
        if(color_format == NEMA_L1 || color_format == NEMA_L2 || color_format == NEMA_L4) {
            blending_mode |= NEMA_BLOP_LUT;
            nema_bind_lut_tex((uintptr_t)((uint32_t *)src_buf + (skip_pallete(cf))), tex_w, tex_h, color_format, -1, 0,
                              (uintptr_t)(src_buf), NEMA_BGRA8888);
        }
        else if(color_format == NEMA_L8) {
            blending_mode |= NEMA_BLOP_LUT;
            nema_bind_lut_tex((uintptr_t)((uint32_t *)src_buf + (skip_pallete(cf))), tex_w, tex_h, color_format, -1, 0,
                              Shuffle_pallete_bo.base_phys, NEMA_BGRA8888);
        }
        else
            nema_bind_src_tex((uintptr_t)(src_buf), tex_w, tex_h, color_format, -1,
                              (dsc->antialias == true) ? NEMA_FILTER_BL : NEMA_FILTER_PS);

        if(recolor) {
            lv_color32_t col32 = {.full = lv_color_to32(dsc->recolor)};
            uint32_t color = nema_rgba(col32.ch.red, col32.ch.green, col32.ch.blue, dsc->recolor_opa);
            nema_set_recolor_color(color);
            blending_mode |= NEMA_BLOP_RECOLOR;
        }

        if(lv_img_cf_is_chroma_keyed(cf)) {
            blending_mode |= NEMA_BLOP_SRC_CKEY;
            lv_color_t ckey = LV_COLOR_CHROMA_KEY;
            lv_color32_t col32 = {.full = lv_color_to32(ckey)};
            uint32_t src_color_key = nema_rgba(col32.ch.red, col32.ch.green, col32.ch.blue, 0U);
            nema_set_src_color_key(src_color_key);
        }

        if(dsc->opa < 255) {
            uint32_t rgba = ((uint32_t)dsc->opa << 24U) | ((uint32_t)dsc->opa << 16U) | ((uint32_t)dsc->opa << 8U) | ((
                                                                                                                          uint32_t)dsc->opa);
            nema_set_const_color(rgba);
            blending_mode |= NEMA_BLOP_MODULATE_A;
        }

        nema_set_blend_blit(blending_mode);

        if(!transform) {
            nema_blit_rect((coords->x1 - draw_ctx->buf_area->x1),
                           (coords->y1 - draw_ctx->buf_area->y1), tex_w, tex_h);
        }
        else {

            /*Calculate the transformed points*/
            float x0 = (coords->x1 - draw_ctx->buf_area->x1);
            float y0 = (coords->y1 - draw_ctx->buf_area->y1);
            float x1 = x0 + tex_w  ;
            float y1 = y0;
            float x2 = x0 + tex_w  ;
            float y2 = y0 + tex_h;
            float x3 = x0          ;
            float y3 = y0 + tex_h;

            nema_matrix3x3_t m;
            nema_mat3x3_load_identity(m);
            nema_mat3x3_translate(m, -x0, -y0);
            nema_mat3x3_translate(m, -(float)dsc->pivot.x, -(float)dsc->pivot.y);
            nema_mat3x3_rotate(m, (dsc->angle / 10.0f)); /* angle is 1/10 degree */
            float scale = 1.f * dsc->zoom / LV_IMG_ZOOM_NONE;
            nema_mat3x3_scale(m, (float)scale, (float)scale);
            nema_mat3x3_translate(m, (float)dsc->pivot.x, (float)dsc->pivot.y);
            nema_mat3x3_translate(m, x0, y0);

            /*Apply Transformation Matrix to Vertices*/
            nema_mat3x3_mul_vec(m, &x0, &y0);
            nema_mat3x3_mul_vec(m, &x1, &y1);
            nema_mat3x3_mul_vec(m, &x2, &y2);
            nema_mat3x3_mul_vec(m, &x3, &y3);

            nema_blit_quad_fit(x0, y0,
                               x1, y1,
                               x2, y2,
                               x3, y3);
        }
        nema_cl_submit(&(nema_gfx_draw_ctx->cl));
        if(color_format == NEMA_L8) {
            nema_cl_wait(&(nema_gfx_draw_ctx->cl));
            nema_buffer_destroy(&Shuffle_pallete_bo);
        }
        return;
    }
    /*Wait for GPU to finish previous jobs in case of failure and call SW rendering*/
    nema_cl_wait(&(nema_gfx_draw_ctx->cl));
    lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
    return;
}


void lv_draw_nema_gfx_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{

    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;

    nema_gfx_draw_ctx->base_sw_ctx.blend                      = lv_draw_nema_gfx_blend;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_rect        = lv_draw_nema_gfx_rect;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_bg          = lv_draw_nema_gfx_bg;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_img_decoded = lv_draw_nema_gfx_img_decoded;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_img         = lv_draw_nema_gfx_img;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_letter      = lv_draw_nema_gfx_letter;

    //Overide this functions to wait for GPU
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_arc         = lv_draw_nema_gfx_arc;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_line        = lv_draw_nema_gfx_line;
    nema_gfx_draw_ctx->base_sw_ctx.base_draw.draw_polygon     = lv_draw_nema_gfx_polygon;

    /*Create GPU Command List*/
    nema_gfx_draw_ctx->cl = nema_cl_create();
    /*Bind Command List*/
    nema_cl_bind_circular(&(nema_gfx_draw_ctx->cl));

}

void lv_draw_nema_gfx_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *) draw_ctx;
    nema_cl_destroy(&(nema_gfx_draw_ctx->cl));
}


void lv_draw_nema_gfx_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    const lv_opa_t * mask;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;

    lv_area_t blend_area;

    bool render_on_gpu = false;

    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
        return; /*Fully clipped, nothing to do*/

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    if(mask == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL) {

        const lv_color_t * src_buf = dsc->src_buf;

        nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(draw_ctx->buf), lv_area_get_width(draw_ctx->buf_area),
                          lv_area_get_height(draw_ctx->buf_area), LV_NEMA_GFX_COLOR_FORMAT,
                          lv_area_get_width(draw_ctx->buf_area)*LV_NEMA_GFX_FORMAT_MULTIPLIER);

        //Set Clipping Area
        lv_area_t clip_area;
        lv_area_copy(&clip_area, draw_ctx->clip_area);
        lv_area_move(&clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

        nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

        lv_coord_t coords_bg_w = lv_area_get_width(&blend_area);
        lv_coord_t coords_bg_h = lv_area_get_height(&blend_area);

        if(src_buf == NULL) {
            //Simple Fill
            uint8_t opacity;
            lv_color32_t col32 = {.full = lv_color_to32(dsc->color)};
            if(dsc->opa < LV_OPA_MAX && dsc->opa > LV_OPA_MIN) {
                opacity = (uint8_t)(((uint16_t)col32.ch.alpha * dsc->opa) >> 8);
            }
            else if(dsc->opa >= LV_OPA_MAX) {
                opacity = col32.ch.alpha;
            }

            uint32_t color = nema_rgba(col32.ch.red, col32.ch.green, col32.ch.blue, opacity);

            if(opacity < 255U)
                nema_set_blend_fill(NEMA_BL_SRC_OVER | NEMA_BLOP_SRC_PREMULT);
            else
                nema_set_blend_fill(NEMA_BL_SRC_OVER);

            nema_fill_rect(blend_area.x1, blend_area.y1, coords_bg_w, coords_bg_h, color);
            render_on_gpu = true;
        }
        else { //Full Opaque mask
            nema_bind_src_tex((uintptr_t)(src_buf), coords_bg_w, coords_bg_h, LV_NEMA_GFX_COLOR_FORMAT,
                              coords_bg_w * LV_NEMA_GFX_FORMAT_MULTIPLIER, NEMA_FILTER_PS);
            if(dsc->opa < 255) {
                uint32_t rgba = ((uint32_t)dsc->opa << 24U) | ((uint32_t)dsc->opa << 16U) | ((uint32_t)dsc->opa << 8U) | ((
                                                                                                                              uint32_t)dsc->opa);
                nema_set_const_color(rgba);
                nema_set_blend_blit(NEMA_BL_SIMPLE | NEMA_BLOP_MODULATE_A);
            }
            else
                nema_set_blend_blit(NEMA_BL_SIMPLE);

            nema_blit_rect(blend_area.x1, blend_area.y1, coords_bg_w, coords_bg_h);
            render_on_gpu = true;
        }
    }

    if(render_on_gpu) {
        nema_cl_submit(&(nema_gfx_draw_ctx->cl));
    }
    else {
        nema_cl_wait(&(nema_gfx_draw_ctx->cl));
        lv_draw_sw_blend_basic(draw_ctx, dsc);
    }

    return;
}

void lv_draw_nema_gfx_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    bool done = false;
    lv_draw_rect_dsc_t nema_dsc;

    lv_memcpy(&nema_dsc, dsc, sizeof(nema_dsc));
#if LV_DRAW_COMPLEX
    /* Draw only the shadow */
    nema_dsc.bg_opa = 0;
    nema_dsc.bg_img_opa = 0;
    nema_dsc.border_opa = 0;
    nema_dsc.outline_opa = 0;
    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;
    nema_cl_wait(&(nema_gfx_draw_ctx->cl));

    lv_draw_sw_rect(draw_ctx, &nema_dsc, coords);

    /* Draw the background */
    nema_dsc.shadow_opa = 0;
    nema_dsc.bg_opa = dsc->bg_opa;
    if(draw_nema_gfx_bg(draw_ctx, &nema_dsc, coords) == LV_RES_OK) {
        nema_dsc.bg_opa = 0;
    }
#endif /*LV_DRAW_COMPLEX*/

    /* Draw the remaining parts */
    nema_dsc.bg_img_opa = dsc->bg_img_opa;
    nema_dsc.border_opa = dsc->border_opa;
    nema_dsc.outline_opa = dsc->outline_opa;

    lv_draw_sw_rect(draw_ctx, &nema_dsc, coords);

}

void lv_draw_nema_gfx_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{

    bool rendered_by_gpu = false;
    lv_draw_rect_dsc_t nema_dsc;
    lv_memcpy(&nema_dsc, dsc, sizeof(nema_dsc));

    // try to render with GPU
    rendered_by_gpu = (draw_nema_gfx_bg(draw_ctx, &nema_dsc, coords) == LV_RES_OK);

    if(rendered_by_gpu) {
        // rendered with gpu
        // continuew with the rest
        nema_dsc.bg_opa = 0;
        lv_draw_sw_bg(draw_ctx, &nema_dsc, coords);
    }
    else {
        // couldn't draw with gpu
        lv_draw_sw_bg(draw_ctx, dsc, coords);
    }
}

lv_res_t draw_nema_gfx_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{

#if LV_COLOR_SCREEN_TRANSP && LV_COLOR_DEPTH == 32
    lv_memset_00(draw_ctx->buf, lv_area_get_size(draw_ctx->buf_area) * sizeof(lv_color_t));
#endif

    if(dsc->bg_opa <= LV_OPA_MIN) return LV_RES_INV;

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

    /*Make the blend area relative to the buffer*/
    lv_area_move(&bg_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(draw_ctx->buf), lv_area_get_width(draw_ctx->buf_area),
                      lv_area_get_height(draw_ctx->buf_area), LV_NEMA_GFX_COLOR_FORMAT,
                      lv_area_get_width(draw_ctx->buf_area)*LV_NEMA_GFX_FORMAT_MULTIPLIER);
    //Set Clipping Area

    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_ctx->clip_area);
    lv_area_move(&clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    nema_set_clip(clip_area.x1, clip_area.y1, lv_area_get_width(&clip_area), lv_area_get_height(&clip_area));

    lv_coord_t coords_bg_w = lv_area_get_width(&bg_coords);
    lv_coord_t coords_bg_h = lv_area_get_height(&bg_coords);
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    int32_t radius = LV_MIN(dsc->radius, short_side >> 1);

    bool mask_any = lv_draw_mask_is_any(&bg_coords);

    if(!mask_any && (dsc->bg_grad.dir == LV_GRAD_DIR_NONE)) {

        uint8_t opacity;
        lv_color32_t bg_col32 = {.full = lv_color_to32(dsc->bg_color)};
        if(dsc->bg_opa < LV_OPA_MAX && dsc->bg_opa > LV_OPA_MIN) {
            opacity = (uint8_t)(((uint16_t)bg_col32.ch.alpha * dsc->bg_opa) >> 8);
        }
        else if(dsc->bg_opa >= LV_OPA_MAX) {
            opacity = bg_col32.ch.alpha;
        }

        if(opacity < 255U) {
            nema_set_blend_fill(NEMA_BL_SIMPLE);
        }
        else {
            nema_set_blend_fill(NEMA_BL_SRC);
        }

        uint32_t bg_color = nema_rgba(bg_col32.ch.red, bg_col32.ch.green, bg_col32.ch.blue, opacity);

        if(dsc->radius != 0)
            nema_fill_rounded_rect_aa(bg_coords.x1, bg_coords.y1, coords_bg_w, coords_bg_h, radius, bg_color);
        else
            nema_fill_rect(bg_coords.x1, bg_coords.y1, coords_bg_w, coords_bg_h, bg_color);

        nema_cl_submit(&(nema_gfx_draw_ctx->cl));
        nema_cl_wait(&(nema_gfx_draw_ctx->cl));

        return LV_RES_OK;
    }
    return LV_RES_INV;
}

LV_ATTRIBUTE_FAST_MEM void lv_draw_nema_gfx_line(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                 const lv_point_t * point1, const lv_point_t * point2)
{
    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;
    //Ensure that GPU has no unfinished workload
    nema_cl_wait(&(nema_gfx_draw_ctx->cl));
    lv_draw_sw_line(draw_ctx, dsc, point1, point2);
}

void lv_draw_nema_gfx_polygon(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc,
                              const lv_point_t * points, uint16_t point_cnt)
{
    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;
    //Ensure that GPU has no unfinished workload
    nema_cl_wait(&(nema_gfx_draw_ctx->cl));
    lv_draw_sw_polygon(draw_ctx, draw_dsc, points, point_cnt);
}

void lv_draw_nema_gfx_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius, uint16_t start_angle, uint16_t end_angle)
{
    lv_draw_nema_gfx_ctx_t * nema_gfx_draw_ctx = (lv_draw_nema_gfx_ctx_t *)draw_ctx;
    //Ensure that GPU has no unfinished workload
    nema_cl_wait(&(nema_gfx_draw_ctx->cl));
    lv_draw_sw_arc(draw_ctx, dsc, center, radius, start_angle, end_angle);
}

#endif
