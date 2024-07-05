/* TSI 2023.gen */
// -----------------------------------------------------------------------------
// Copyright (c) 2008-23 Think Silicon Single Member PC
// Think Silicon Single Member PC Confidential Proprietary
// -----------------------------------------------------------------------------
//     All Rights reserved - Unpublished -rights reserved under
//         the Copyright laws of the European Union
//
//  This file includes the Confidential information of Think Silicon Single
//  Member PC The receiver of this Confidential Information shall not disclose
//  it to any third party and shall protect its confidentiality by using the
//  same degree of care, but not less then a reasonable degree of care, as the
//  receiver uses to protect receiver's own Confidential Information. The entire
//  notice must be reproduced on all authorized copies and copies may only be
//  made to the extent permitted by a licensing agreement from Think Silicon
//  Single Member PC.
//
//  The software/data is provided 'as is', without warranty of any kind,
//  expressed or implied, including but not limited to the warranties of
//  merchantability, fitness for a particular purpose and noninfringement. In no
//  event shall Think Silicon Single Member PC be liable for any claim, damages
//  or other liability, whether in an action of contract, tort or otherwise,
//  arising from, out of or in connection with the software.
//
//  For further information please contact:
//
//                    Think Silicon Single Member PC
//                    http://www.think-silicon.com
//                    Patras Science Park
//                    Rion Achaias 26504
//                    Greece
// -----------------------------------------------------------------------------

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
 *  STATIC PROTOTYPES
 **********************/

static uint32_t lv_cf_to_nema(lv_color_format_t cf);

static void _draw_nema_gfx_tile(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

static void _draw_nema_gfx_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);

static uint32_t skip_pallete(lv_color_format_t cf);

/**********************
 *  STATIC FUNCTIONS
 **********************/

static uint32_t skip_pallete(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_I1:
            return 2;
        case LV_COLOR_FORMAT_I2:
            return 4;
        case LV_COLOR_FORMAT_I4:
            return 16;
        case LV_COLOR_FORMAT_I8:
            return 256;
    }
}

static uint32_t lv_cf_to_nema(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A1:
            return NEMA_A1;
        case LV_COLOR_FORMAT_A2:
            return NEMA_A2;
        case LV_COLOR_FORMAT_A4:
            return NEMA_A4;
        case LV_COLOR_FORMAT_A8:
            return NEMA_A8;
        case LV_COLOR_FORMAT_I1:
            return NEMA_L1;
        case LV_COLOR_FORMAT_I2:
            return NEMA_L2;
        case LV_COLOR_FORMAT_I4:
            return NEMA_L4;
        case LV_COLOR_FORMAT_I8:
            return NEMA_L8;
        default:
            return LV_NEMA_GFX_COLOR_FORMAT;
    }
}

static void _draw_nema_gfx_tile(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    int32_t img_w = dsc->header.w;
    int32_t img_h = dsc->header.h;

    lv_area_t tile_area = *coords;
    lv_area_set_width(&tile_area, img_w);
    lv_area_set_height(&tile_area, img_h);

    int32_t tile_x_start = tile_area.x1;

    while(tile_area.y1 <= draw_unit->clip_area->y2) {
        while(tile_area.x1 <= draw_unit->clip_area->x2) {

            lv_area_t clipped_img_area;
            if(_lv_area_intersect(&clipped_img_area, &tile_area, draw_unit->clip_area)) {
                _draw_nema_gfx_img(draw_unit, dsc, &tile_area);
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

static void _draw_nema_gfx_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)draw_unit;

    lv_layer_t * layer = draw_unit->target_layer;
    const lv_image_dsc_t * img_dsc = dsc->src;

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    if(!_lv_area_intersect(&blend_area, coords, draw_unit->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    bool has_transform = (dsc->rotation != 0 || dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE);
    bool recolor = (dsc->recolor_opa > LV_OPA_MIN);

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -layer->buf_area.x1, -layer->buf_area.y1);

    uint32_t tex_w = lv_area_get_width(coords);
    uint32_t tex_h = lv_area_get_height(coords);

    nema_set_clip(rel_clip_area.x1, rel_clip_area.y1, lv_area_get_width(&rel_clip_area),
                  lv_area_get_height(&rel_clip_area));

    const void * src_buf = img_dsc->data;

    uint32_t blending_mode = nemagfx_blending_mode(dsc->blend_mode);

    lv_color_format_t src_cf = img_dsc->header.cf;

    //Image contains Alpha
    if(src_cf == LV_COLOR_FORMAT_ARGB8888 || src_cf == LV_COLOR_FORMAT_XRGB8888) {
        blending_mode |= NEMA_BLOP_SRC_PREMULT;
    }

    uint32_t color_format = lv_cf_to_nema(src_cf);
    uint32_t src_stride = img_dsc->header.stride;

    nema_bind_dst_tex((uintptr_t)NEMA_VIRT2PHYS(layer->draw_buf->data), lv_area_get_width(&(layer->buf_area)),
                      lv_area_get_height(&(layer->buf_area)), LV_NEMA_GFX_COLOR_FORMAT,
                      lv_area_get_width(&(layer->buf_area))*LV_NEMA_GFX_FORMAT_MULTIPLIER);

    nema_bind_src_tex((uintptr_t)(src_buf), tex_w, tex_h, color_format, -1,
                      (dsc->antialias == true) ? NEMA_FILTER_BL : NEMA_FILTER_PS);

    if(recolor) {
        lv_color32_t col32 = lv_color_to_32(dsc->recolor, LV_OPA_MIX2(dsc->recolor_opa, dsc->opa));
        uint32_t color = nema_rgba(col32.red, col32.green, col32.blue, col32.alpha);
        nema_set_recolor_color(color);
        blending_mode |= NEMA_BLOP_RECOLOR;
    }

    if(dsc->opa < 255) {
        uint32_t rgba = ((uint32_t)dsc->opa << 24U) | ((uint32_t)dsc->opa << 16U) | ((uint32_t)dsc->opa << 8U) | ((
                                                                                                                      uint32_t)dsc->opa);
        nema_set_const_color(rgba);
        blending_mode |= NEMA_BLOP_MODULATE_A;
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
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_nema_gfx_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{

    if(!dsc->tile) {
        _draw_nema_gfx_img(draw_unit, dsc, coords);
    }
    else {
        _draw_nema_gfx_tile(draw_unit, dsc, coords);
    }

}
#endif