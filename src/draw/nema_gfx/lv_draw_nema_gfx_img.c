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
 * @file lv_draw_nema_gfx_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_nema_gfx.h"

#if LV_USE_NEMA_GFX


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _draw_nema_gfx_tile(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

static void _draw_nema_gfx_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

static uint32_t lv_nemagfx_mask_cf_to_nema(lv_color_format_t cf);

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void _draw_nema_gfx_tile(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    int32_t img_w = dsc->header.w;
    int32_t img_h = dsc->header.h;

    lv_area_t tile_area;
    if(lv_area_get_width(&dsc->image_area) >= 0) {
        tile_area = dsc->image_area;
    }
    else {
        tile_area = *coords;
    }
    lv_area_set_width(&tile_area, img_w);
    lv_area_set_height(&tile_area, img_h);

    int32_t tile_x_start = tile_area.x1;

    while(tile_area.y1 <= t->clip_area.y2) {
        while(tile_area.x1 <= t->clip_area.x2) {

            lv_area_t clipped_img_area;
            if(lv_area_intersect(&clipped_img_area, &tile_area, &t->clip_area)) {
                _draw_nema_gfx_img(t, dsc, &tile_area);
            }

            tile_area.x1 += img_w;
            tile_area.x2 += img_w;
        }

        tile_area.y1 += img_h;
        tile_area.y2 += img_h;
        tile_area.x1 = tile_x_start;
        tile_area.x2 = tile_x_start + img_w - 1;
    }

    lv_image_decoder_close(&decoder_dsc);
}

static void _draw_nema_gfx_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)t->draw_unit;

    lv_layer_t * layer = t->target_layer;
    const lv_image_dsc_t * img_dsc = dsc->src;

    bool masked = dsc->bitmap_mask_src != NULL;

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!lv_area_intersect(&blend_area, coords, &t->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, &t->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE);
    bool recolor = (dsc->recolor_opa > LV_OPA_MIN);

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);

    uint32_t tex_w = lv_area_get_width(coords);
    uint32_t tex_h = lv_area_get_height(coords);

    nema_set_clip(rel_clip_area.x1, rel_clip_area.y1, lv_area_get_width(&rel_clip_area),
                  lv_area_get_height(&rel_clip_area));

    lv_color_format_t dst_cf = layer->draw_buf->header.cf;
    uint32_t dst_nema_cf = lv_nemagfx_cf_to_nema(dst_cf);

    const void * src_buf = img_dsc->data;

    uint32_t blending_mode = lv_nemagfx_blending_mode(dsc->blend_mode);

    lv_color_format_t src_cf = img_dsc->header.cf;

    /*Image contains Alpha*/
    if(src_cf == LV_COLOR_FORMAT_ARGB8888 || src_cf == LV_COLOR_FORMAT_XRGB8888) {
        blending_mode |= NEMA_BLOP_SRC_PREMULT;
    }

    uint32_t src_nema_cf = lv_nemagfx_cf_to_nema(src_cf);
    /* the stride should be computed internally for NEMA_TSC images and images missing a stride value */
    int32_t src_stride = (src_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && src_cf <= LV_COLOR_FORMAT_NEMA_TSC_END)
                         || img_dsc->header.stride == 0 ? -1 : (int32_t)img_dsc->header.stride;

    int32_t stride = (dst_cf >= LV_COLOR_FORMAT_NEMA_TSC_START && dst_cf <= LV_COLOR_FORMAT_NEMA_TSC_END) ?
                     -1 : lv_area_get_width(&(layer->buf_area)) * lv_color_format_get_size(dst_cf);

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), dst_nema_cf, stride);

    if(!LV_COLOR_FORMAT_IS_INDEXED(src_cf)) {
        nema_bind_src_tex((uintptr_t)(src_buf), tex_w, tex_h, src_nema_cf, src_stride,
                          dsc->antialias ? NEMA_FILTER_BL : NEMA_FILTER_PS);
    }
    else {
        nema_bind_lut_tex((uintptr_t)((uint8_t *)src_buf + LV_COLOR_INDEXED_PALETTE_SIZE(src_cf) * 4), tex_w, tex_h,
                          src_nema_cf, src_stride, NEMA_FILTER_PS, (uintptr_t)(src_buf), NEMA_BGRA8888);
        blending_mode |= NEMA_BLOP_LUT;
    }

    /*Guard for previous NemaGFX Version*/
#ifdef NEMA_BLOP_RECOLOR
    if(recolor) {
        lv_color32_t col32 = lv_color_to_32(dsc->recolor, LV_OPA_MIX2(dsc->recolor_opa, dsc->opa));
        uint32_t color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);
        nema_set_recolor_color(color);
        blending_mode |= NEMA_BLOP_RECOLOR;
    }
#endif

    if(dsc->opa < 255) {
        uint32_t rgba = ((uint32_t)dsc->opa << 24U) | ((uint32_t)dsc->opa << 16U) | ((uint32_t)dsc->opa << 8U) | ((
                                                                                                                      uint32_t)dsc->opa);
        nema_set_const_color(rgba);
        blending_mode |= NEMA_BLOP_MODULATE_A;
    }

    if(!has_transform && masked && !recolor) {
        if(dsc->bitmap_mask_src->header.cf == LV_COLOR_FORMAT_A8 || dsc->bitmap_mask_src->header.cf == LV_COLOR_FORMAT_L8) {
            blending_mode |= NEMA_BLOP_STENCIL_TXTY;
            const lv_image_dsc_t * mask = dsc->bitmap_mask_src;
            const void * mask_buf = mask->data;

            const lv_area_t * image_area;
            lv_area_t mask_area;
            if(lv_area_get_width(&dsc->image_area) < 0) image_area = coords;
            else image_area = &dsc->image_area;

            lv_area_set(&mask_area, 0, 0, dsc->bitmap_mask_src->header.w - 1, dsc->bitmap_mask_src->header.h - 1);
            lv_area_align(image_area, &mask_area, LV_ALIGN_CENTER, 0, 0);

            mask_buf += dsc->bitmap_mask_src->header.w * (coords->y1 - mask_area.y1) + (coords->x1 - mask_area.x1);

            nema_bind_tex(NEMA_TEX3, (uintptr_t)NEMA_VIRT2PHYS(mask_buf), mask->header.w, mask->header.h,
                          lv_nemagfx_mask_cf_to_nema(mask->header.cf),
                          mask->header.stride, NEMA_FILTER_BL);
        }
    }

    nema_set_blend_blit(blending_mode);

    if(!has_transform) {
        nema_blit_rect((coords->x1 - layer->buf_area.x1),
                       (coords->y1 - layer->buf_area.y1), tex_w, tex_h);
    }
    else {
        /*Calculate the transformed points*/
        float x0 = (coords->x1 - layer->buf_area.x1);
        float y0 = (coords->y1 - layer->buf_area.y1);
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
        nema_mat3x3_rotate(m, (dsc->rotation / 10.0f)); /* angle is 1/10 degree */
        float scale_x = 1.f * dsc->scale_x / LV_SCALE_NONE;
        float scale_y = 1.f * dsc->scale_y / LV_SCALE_NONE;
        nema_mat3x3_scale(m, (float)scale_x, (float)scale_y);
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

    nema_cl_submit(&(draw_nema_gfx_unit->cl));

}

/*NemaGFX does mask operations with A8,A4,A2 and A1 formats*/
static uint32_t lv_nemagfx_mask_cf_to_nema(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A1:
            return NEMA_A1;
        case LV_COLOR_FORMAT_A2:
            return NEMA_A2;
        case LV_COLOR_FORMAT_A4:
            return NEMA_A4;
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
        default:
            break;
    }

    return NEMA_A8;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_img(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{

    if(!dsc->tile) {
        _draw_nema_gfx_img(t, dsc, coords);
    }
    else {
        _draw_nema_gfx_tile(t, dsc, coords);
    }

}
#endif
