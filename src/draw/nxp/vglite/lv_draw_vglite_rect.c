/**
 * @file lv_draw_vglite_rect.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2021, 2022 NXP
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

#include "lv_draw_vglite_rect.h"

#if LV_USE_GPU_NXP_VG_LITE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_gpu_nxp_vglite_draw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    vg_lite_buffer_t vgbuf;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_coord_t dest_width = lv_area_get_width(draw_ctx->buf_area);
    lv_coord_t dest_height = lv_area_get_height(draw_ctx->buf_area);
    vg_lite_path_t path;
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    vg_lite_matrix_t matrix;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    vg_lite_linear_gradient_t gradient;
    vg_lite_matrix_t * grad_matrix;
    lv_opa_t bg_opa = dsc->bg_opa;

    if(dsc->radius < 0)
        return LV_RES_INV;

    /* Make areas relative to draw buffer */
    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip;
    lv_area_copy(&rel_clip, draw_ctx->clip_area);
    lv_area_move(&rel_clip, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    /*** Init destination buffer ***/
    if(lv_vglite_init_buf(&vgbuf, (uint32_t)dest_width, (uint32_t)dest_height, (uint32_t)dest_width * sizeof(lv_color_t),
                          (const lv_color_t *)draw_ctx->buf, false) != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");

    /*Get the real radius. Can't be larger than the half of the shortest side */
    int32_t short_side = LV_MIN(width, height);
    int32_t rad = LV_MIN(dsc->radius, short_side >> 1);

    /*** Init path ***/
    if((dsc->radius == (lv_coord_t)LV_RADIUS_CIRCLE) && (width == height)) {
        float tang = ((float)rad * BEZIER_OPTIM_CIRCLE);
        int32_t cpoff = (int32_t)tang;
        int32_t circle_path[] = { /*VG circle path*/
            VLC_OP_MOVE, rel_coords.x1 + rad,  rel_coords.y1,
            VLC_OP_CUBIC_REL, cpoff, 0, rad, rad - cpoff, rad, rad, /* top-right */
            VLC_OP_CUBIC_REL, 0, cpoff, cpoff - rad, rad, 0 - rad, rad, /* bottom-right */
            VLC_OP_CUBIC_REL, 0 - cpoff, 0, 0 - rad, cpoff - rad, 0 - rad, 0 - rad, /* bottom-left */
            VLC_OP_CUBIC_REL, 0, 0 - cpoff, rad - cpoff, 0 - rad, rad, 0 - rad, /* top-left */
            VLC_OP_END
        };
        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(circle_path), circle_path,
                                (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                                ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);
    }
    else if(dsc->radius > 0) {
        float tang = ((float)rad * BEZIER_OPTIM_CIRCLE);
        int32_t cpoff = (int32_t)tang;
        int32_t rounded_path[] = { /*VG rounded rectangular path*/
            VLC_OP_MOVE, rel_coords.x1 + rad,  rel_coords.y1,
            VLC_OP_LINE, rel_coords.x2 - rad + 1,  rel_coords.y1, /* top */
            VLC_OP_CUBIC_REL, cpoff, 0, rad, rad - cpoff, rad, rad, /* top-right */
            VLC_OP_LINE, rel_coords.x2 + 1,  rel_coords.y2 - rad + 1, /* right */
            VLC_OP_CUBIC_REL, 0, cpoff, cpoff - rad, rad, 0 - rad, rad, /* bottom-right */
            VLC_OP_LINE, rel_coords.x1 + rad,  rel_coords.y2 + 1, /* bottom */
            VLC_OP_CUBIC_REL, 0 - cpoff, 0, 0 - rad, cpoff - rad, 0 - rad, 0 - rad, /* bottom-left */
            VLC_OP_LINE, rel_coords.x1,  rel_coords.y1 + rad, /* left */
            VLC_OP_CUBIC_REL, 0, 0 - cpoff, rad - cpoff, 0 - rad, rad, 0 - rad, /* top-left */
            VLC_OP_END
        };
        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(rounded_path), rounded_path,
                                (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                                ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);
    }
    else {
        int32_t rect_path[] = { /*VG rectangular path*/
            VLC_OP_MOVE, rel_coords.x1,  rel_coords.y1,
            VLC_OP_LINE, rel_coords.x2 + 1,  rel_coords.y1,
            VLC_OP_LINE, rel_coords.x2 + 1,  rel_coords.y2 + 1,
            VLC_OP_LINE, rel_coords.x1,  rel_coords.y2 + 1,
            VLC_OP_LINE, rel_coords.x1,  rel_coords.y1,
            VLC_OP_END
        };
        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_LOW, sizeof(rect_path), rect_path,
                                (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                                ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);
    }

    VG_LITE_ERR_RETURN_INV(err, "Init path failed.");
    vg_lite_identity(&matrix);

    /*** Init Color/Gradient ***/
    if(dsc->bg_grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE) {
        uint32_t colors[2];
        uint32_t stops[2];
        lv_color32_t col32[2];

        /* Gradient setup */
        uint8_t cnt = LV_MAX(dsc->bg_grad.stops_count, 2);
        for(uint8_t i = 0; i < cnt; i++) {
            col32[i].full = lv_color_to32(dsc->bg_grad.stops[i].color); /*Convert color to RGBA8888*/

            if(bg_opa <= (lv_opa_t)LV_OPA_MAX) {
                /* Only pre-multiply color if hardware pre-multiplication is not present */
                if(!vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
                    col32[i].ch.red = (uint8_t)(((uint16_t)col32[i].ch.red * bg_opa) >> 8);
                    col32[i].ch.green = (uint8_t)(((uint16_t)col32[i].ch.green * bg_opa) >> 8);
                    col32[i].ch.blue = (uint8_t)(((uint16_t)col32[i].ch.blue * bg_opa) >> 8);
                }
                col32[i].ch.alpha = bg_opa;
            }

            stops[i] = dsc->bg_grad.stops[i].frac;
#if LV_COLOR_DEPTH==16
            colors[i] = ((uint32_t)col32[i].ch.alpha << 24) | ((uint32_t)col32[i].ch.blue << 16) |
                        ((uint32_t)col32[i].ch.green << 8) | (uint32_t)col32[i].ch.red;
#else /*LV_COLOR_DEPTH==32*/
            /* watchout: red and blue color components are inverted versus vg_lite_color_t order */
            colors[i] = ((uint32_t)col32[i].ch.alpha << 24) | ((uint32_t)col32[i].ch.red << 16) |
                        ((uint32_t)col32[i].ch.green << 8) | (uint32_t)col32[i].ch.blue;
#endif
        }

        lv_memset_00(&gradient, sizeof(vg_lite_linear_gradient_t));

        err = vg_lite_init_grad(&gradient);
        VG_LITE_ERR_RETURN_INV(err, "Init gradient failed");

        err = vg_lite_set_grad(&gradient, cnt, colors, stops);
        VG_LITE_ERR_RETURN_INV(err, "Set gradient failed.");

        err = vg_lite_update_grad(&gradient);
        VG_LITE_ERR_RETURN_INV(err, "Update gradient failed.");

        grad_matrix = vg_lite_get_grad_matrix(&gradient);
        vg_lite_identity(grad_matrix);
        vg_lite_translate((float)rel_coords.x1, (float)rel_coords.y1, grad_matrix);

        if(dsc->bg_grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_VER) {
            vg_lite_scale(1.0f, (float)height / 256.0f, grad_matrix);
            vg_lite_rotate(90.0f, grad_matrix);
        }
        else {   /*LV_GRAD_DIR_HOR*/
            vg_lite_scale((float)width / 256.0f, 1.0f, grad_matrix);
        }
    }

    lv_color32_t bg_col32 = {.full = lv_color_to32(dsc->bg_color)}; /*Convert color to RGBA8888*/
    if(bg_opa <= (lv_opa_t)LV_OPA_MAX) {
        /* Only pre-multiply color if hardware pre-multiplication is not present */
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
            bg_col32.ch.red = (uint8_t)(((uint16_t)bg_col32.ch.red * bg_opa) >> 8);
            bg_col32.ch.green = (uint8_t)(((uint16_t)bg_col32.ch.green * bg_opa) >> 8);
            bg_col32.ch.blue = (uint8_t)(((uint16_t)bg_col32.ch.blue * bg_opa) >> 8);
        }
        bg_col32.ch.alpha = bg_opa;
    }

#if LV_COLOR_DEPTH==16
    vgcol = bg_col32.full;
#else /*LV_COLOR_DEPTH==32*/
    vgcol = ((uint32_t)bg_col32.ch.alpha << 24) | ((uint32_t)bg_col32.ch.blue << 16) |
            ((uint32_t)bg_col32.ch.green << 8) | (uint32_t)bg_col32.ch.red;
#endif

    /*** Draw rectangle ***/
    if(dsc->bg_grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE) {
        err = vg_lite_draw(&vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
    }
    else {
        err = vg_lite_draw_gradient(&vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, &gradient, VG_LITE_BLEND_SRC_OVER);
    }
    VG_LITE_ERR_RETURN_INV(err, "Draw gradient failed.");

    if(lv_vglite_run() != LV_RES_OK)
        VG_LITE_RETURN_INV("Run failed.");

    err = vg_lite_clear_path(&path);
    VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");

    if(dsc->bg_grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE) {
        err = vg_lite_clear_grad(&gradient);
        VG_LITE_ERR_RETURN_INV(err, "Clear gradient failed.");
    }

    return LV_RES_OK;
}

lv_res_t lv_gpu_nxp_vglite_draw_border_generic(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc,
                                               const lv_area_t * coords, bool border)
{
    vg_lite_buffer_t vgbuf;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_coord_t dest_width = lv_area_get_width(draw_ctx->buf_area);
    lv_coord_t dest_height = lv_area_get_height(draw_ctx->buf_area);
    vg_lite_path_t path;
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    vg_lite_matrix_t matrix;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t radius = dsc->radius;

    if(radius < 0)
        return LV_RES_INV;

    if(radius > 0 && radius < (lv_coord_t)LV_RADIUS_CIRCLE) {
        if(border)
            radius = (radius > dsc->border_width / 2) ? radius - dsc->border_width / 2 : 0;
        else
            radius = (radius < (lv_coord_t)LV_RADIUS_CIRCLE - dsc->outline_width / 2) ?
                     radius + dsc->outline_width / 2 : LV_RADIUS_CIRCLE;
    }

    vg_lite_cap_style_t cap_style = (radius) ? VG_LITE_CAP_ROUND : VG_LITE_CAP_BUTT;
    vg_lite_join_style_t join_style = (radius) ? VG_LITE_JOIN_ROUND : VG_LITE_JOIN_MITER;

    /* Make areas relative to draw buffer */
    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip;
    lv_area_copy(&rel_clip, draw_ctx->clip_area);
    lv_area_move(&rel_clip, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    /* Choose vglite blend mode based on given lvgl blend mode */
    lv_blend_mode_t blend_mode = dsc->blend_mode;
    vg_lite_blend_t vglite_blend_mode = VG_LITE_BLEND_NONE;

    switch(blend_mode) {
        case LV_BLEND_MODE_NORMAL:
            vglite_blend_mode = VG_LITE_BLEND_SRC_OVER;
            break;
        case LV_BLEND_MODE_ADDITIVE:
            vglite_blend_mode = VG_LITE_BLEND_ADDITIVE;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            vglite_blend_mode = VG_LITE_BLEND_SUBTRACT;
            break;
        case LV_BLEND_MODE_MULTIPLY:
            vglite_blend_mode = VG_LITE_BLEND_MULTIPLY;
            break;
        case LV_BLEND_MODE_REPLACE:
            vglite_blend_mode = VG_LITE_BLEND_NONE;
            break;
    }

    /*** Init destination buffer ***/
    if(lv_vglite_init_buf(&vgbuf, (uint32_t)dest_width, (uint32_t)dest_height, (uint32_t)dest_width * sizeof(lv_color_t),
                          (const lv_color_t *)draw_ctx->buf, false) != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");

    /*Get the real radius. Can't be larger than the half of the shortest side */
    int32_t short_side = LV_MIN(width, height);
    int32_t rad = LV_MIN(radius, short_side >> 1);

    if((radius == (lv_coord_t)LV_RADIUS_CIRCLE) && (width == height)) {
        float tang = ((float)rad * BEZIER_OPTIM_CIRCLE);
        int32_t cpoff = (int32_t)tang;
        int32_t circle_path[] = { /*VG circle path*/
            VLC_OP_MOVE, rel_coords.x1 + rad,  rel_coords.y1,
            VLC_OP_CUBIC_REL, cpoff, 0, rad, rad - cpoff, rad, rad, /* top-right */
            VLC_OP_CUBIC_REL, 0, cpoff, cpoff - rad, rad, 0 - rad, rad, /* bottom-right */
            VLC_OP_CUBIC_REL, 0 - cpoff, 0, 0 - rad, cpoff - rad, 0 - rad, 0 - rad, /* bottom-left */
            VLC_OP_CUBIC_REL, 0, 0 - cpoff, rad - cpoff, 0 - rad, rad, 0 - rad, /* top-left */
            VLC_OP_END
        };
        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(circle_path), circle_path,
                                (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                                ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);
    }
    else if(radius > 0) {
        float tang = ((float)rad * BEZIER_OPTIM_CIRCLE);
        int32_t cpoff = (int32_t)tang;
        int32_t rounded_border_path[] = { /*VG rounded rectangular path*/
            VLC_OP_MOVE, rel_coords.x1 + rad,  rel_coords.y1,
            VLC_OP_LINE, rel_coords.x2 - rad + 1,  rel_coords.y1, /* top */
            VLC_OP_CUBIC_REL, cpoff, 0, rad, rad - cpoff, rad, rad, /* top-right */
            VLC_OP_LINE, rel_coords.x2 + 1,  rel_coords.y2 - rad + 1, /* right */
            VLC_OP_CUBIC_REL, 0, cpoff, cpoff - rad, rad, 0 - rad, rad, /* bottom-right */
            VLC_OP_LINE, rel_coords.x1 + rad,  rel_coords.y2 + 1, /* bottom */
            VLC_OP_CUBIC_REL, 0 - cpoff, 0, 0 - rad, cpoff - rad, 0 - rad, 0 - rad, /* bottom-left */
            VLC_OP_LINE, rel_coords.x1,  rel_coords.y1 + rad, /* left */
            VLC_OP_CUBIC_REL, 0, 0 - cpoff, rad - cpoff, 0 - rad, rad, 0 - rad, /* top-left */
            VLC_OP_END
        };
        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(rounded_border_path), rounded_border_path,
                                (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                                ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);
    }
    else {
        int32_t border_path[] = { /*VG rectangular path*/
            VLC_OP_MOVE, rel_coords.x1,  rel_coords.y1,
            VLC_OP_LINE, rel_coords.x2 + 1,  rel_coords.y1,
            VLC_OP_LINE, rel_coords.x2 + 1,  rel_coords.y2 + 1,
            VLC_OP_LINE, rel_coords.x1,  rel_coords.y2 + 1,
            VLC_OP_LINE, rel_coords.x1,  rel_coords.y1,
            VLC_OP_END
        };
        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_LOW, sizeof(border_path), border_path,
                                (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                                ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);
    }

    VG_LITE_ERR_RETURN_INV(err, "Init path failed.");
    vg_lite_identity(&matrix);

    lv_opa_t opa;
    lv_color32_t col32;
    lv_coord_t line_width;

    if(border) {
        opa = dsc->border_opa;
        col32.full = lv_color_to32(dsc->border_color); /*Convert color to RGBA8888*/
        line_width = dsc->border_width;
    }
    else {
        opa = dsc->outline_opa;
        col32.full = lv_color_to32(dsc->outline_color); /*Convert color to RGBA8888*/
        line_width = dsc->outline_width;
    }

    if(opa <= (lv_opa_t)LV_OPA_MAX) {
        /* Only pre-multiply color if hardware pre-multiplication is not present */
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
            col32.ch.red = (uint8_t)(((uint16_t)col32.ch.red * opa) >> 8);
            col32.ch.green = (uint8_t)(((uint16_t)col32.ch.green * opa) >> 8);
            col32.ch.blue = (uint8_t)(((uint16_t)col32.ch.blue * opa) >> 8);
        }
        col32.ch.alpha = opa;
    }

#if LV_COLOR_DEPTH==16
    vgcol = col32.full;
#else /*LV_COLOR_DEPTH==32*/
    vgcol = ((uint32_t)col32.ch.alpha << 24) | ((uint32_t)col32.ch.blue << 16) |
            ((uint32_t)col32.ch.green << 8) | (uint32_t)col32.ch.red;
#endif

    /*** Draw border ***/
    err = vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH);
    VG_LITE_ERR_RETURN_INV(err, "Set draw path type failed.");

    err = vg_lite_set_stroke(&path, cap_style, join_style, line_width, 8, NULL, 0, 0, vgcol);
    VG_LITE_ERR_RETURN_INV(err, "Set stroke failed.");

    err = vg_lite_update_stroke(&path);
    VG_LITE_ERR_RETURN_INV(err, "Update stroke failed.");

    err = vg_lite_draw(&vgbuf, &path, VG_LITE_FILL_NON_ZERO, &matrix, vglite_blend_mode, vgcol);
    VG_LITE_ERR_RETURN_INV(err, "Draw border failed.");

    if(lv_vglite_run() != LV_RES_OK)
        VG_LITE_RETURN_INV("Run failed.");

    err = vg_lite_clear_path(&path);
    VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");

    return LV_RES_OK;

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_NXP_VG_LITE*/
