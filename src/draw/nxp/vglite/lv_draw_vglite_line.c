/**
 * @file lv_draw_vglite_line.c
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

#include "lv_draw_vglite_line.h"
#include "math.h"
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
lv_res_t lv_gpu_nxp_vglite_draw_line(lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                     const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip_line)
{
    vg_lite_buffer_t vgbuf;
    vg_lite_error_t err = VG_LITE_SUCCESS;

    int32_t dest_width = lv_area_get_width(draw_ctx->buf_area);
    int32_t dest_height = lv_area_get_height(draw_ctx->buf_area);
    vg_lite_path_t path;
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    vg_lite_matrix_t matrix;
    lv_opa_t opa = dsc->opa;

    vg_lite_cap_style_t cap_style = (dsc->round_start || dsc->round_end) ? VG_LITE_CAP_ROUND : VG_LITE_CAP_BUTT;
    vg_lite_join_style_t join_style = (dsc->round_start || dsc->round_end) ? VG_LITE_JOIN_ROUND : VG_LITE_JOIN_MITER;

    bool is_dashed = (dsc->dash_width && dsc->dash_gap);

    vg_lite_float_t stroke_dash_pattern[2] = {0, 0};
    uint32_t stroke_dash_count = 0;
    vg_lite_float_t stroke_dash_phase = 0;
    if(is_dashed) {
        stroke_dash_pattern[0] = (vg_lite_float_t)dsc->dash_width;
        stroke_dash_pattern[1] = (vg_lite_float_t)dsc->dash_gap;
        stroke_dash_count = sizeof(stroke_dash_pattern) / sizeof(vg_lite_float_t);
        stroke_dash_phase = (vg_lite_float_t)dsc->dash_width / 2;
    }

    /* Make points relative to the draw buffer */
    lv_point_t rel_point1 = { point1->x - draw_ctx->buf_area->x1, point1->y - draw_ctx->buf_area->y1 };
    lv_point_t rel_point2 = { point2->x - draw_ctx->buf_area->x1, point2->y - draw_ctx->buf_area->y1 };

    lv_area_t rel_clip;
    lv_area_copy(&rel_clip, clip_line);
    lv_area_move(&rel_clip, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    /* Choose vglite blend mode based on given lvgl blend mode */
    lv_blend_mode_t blend_mode = dsc->blend_mode;
    vg_lite_blend_t vglite_blend_mode = lv_vglite_get_blend_mode(blend_mode);

    /*** Init destination buffer ***/
    if(lv_vglite_init_buf(&vgbuf, (uint32_t)dest_width, (uint32_t)dest_height, (uint32_t)dest_width * sizeof(lv_color_t),
                          (const lv_color_t *)draw_ctx->buf, false)
       != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");


    /*** Init path ***/
    lv_coord_t width = dsc->width;

    int32_t line_path[] = { /*VG line path*/
        VLC_OP_MOVE, rel_point1.x, rel_point1.y,
        VLC_OP_LINE, rel_point2.x, rel_point2.y,
        VLC_OP_END
    };

    err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(line_path), line_path,
                            (vg_lite_float_t)rel_clip.x1, (vg_lite_float_t)rel_clip.y1,
                            ((vg_lite_float_t)rel_clip.x2) + 1.0f, ((vg_lite_float_t)rel_clip.y2) + 1.0f);
    VG_LITE_ERR_RETURN_INV(err, "Init path failed.");

    /* Set rotation angle */
    vg_lite_identity(&matrix);

    lv_color32_t col32 = { .full = lv_color_to32(dsc->color) }; /*Convert color to RGBA8888*/
    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if(lv_vglite_premult_and_swizzle(&vgcol, col32, opa, color_format) != LV_RES_OK)
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");

    /*** Draw line ***/
    err = vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH);
    VG_LITE_ERR_RETURN_INV(err, "Set draw path type failed.");

    err = vg_lite_set_stroke(&path, cap_style, join_style, width, 8, stroke_dash_pattern, stroke_dash_count,
                             stroke_dash_phase, vgcol);
    VG_LITE_ERR_RETURN_INV(err, "Set stroke failed.");

    err = vg_lite_update_stroke(&path);
    VG_LITE_ERR_RETURN_INV(err, "Update stroke failed.");

    err = vg_lite_draw(&vgbuf, &path, VG_LITE_FILL_NON_ZERO, &matrix, vglite_blend_mode, vgcol);
    VG_LITE_ERR_RETURN_INV(err, "Draw line failed.");

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
