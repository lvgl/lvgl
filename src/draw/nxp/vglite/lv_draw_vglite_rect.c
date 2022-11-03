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

#include <math.h>
/*********************
 *      DEFINES
 *********************/
/*********************
 *      DEFINES
 *********************/
/* Path data sizes for different elements */
#define CUBIC_PATH_DATA_SIZE 7 /* 1 opcode, 6 arguments */
#define LINE_PATH_DATA_SIZE 3  /* 1 opcode, 2 arguments */
#define MOVE_PATH_DATA_SIZE 3  /* 1 opcode, 2 arguments */
#define END_PATH_DATA_SIZE 1   /* 1 opcode, 0 arguments */
/* Maximum possible rectangle path size
 * is in the rounded rectangle case:
 * - 1 move for the path start
 * - 4 cubics for the corners
 * - 4 lines for the sides
 * - 1 end for the path end */
#define RECT_PATH_DATA_MAX_SIZE 1 * MOVE_PATH_DATA_SIZE + 4 * CUBIC_PATH_DATA_SIZE + 4 * LINE_PATH_DATA_SIZE + 1 * END_PATH_DATA_SIZE

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/**
 * Generates path data for rectangle drawing.
 *
 * @param[in/out] path The path data to initialize
 * @param[in/out] path_size The resulting size of the created path data
 * @param[in] dsc The style descriptor for the rectangle to be drawn
 * @param[in] coords The coordinates of the rectangle to be drawn
 */
static void lv_vglite_create_rect_path_data(int32_t * path_data, uint32_t * path_data_size,
                                            lv_coord_t radius,
                                            const lv_area_t * coords);
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
    vg_lite_matrix_t matrix;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    vg_lite_linear_gradient_t gradient;
    vg_lite_matrix_t * grad_matrix;
    vg_lite_color_t vgcol;
    lv_coord_t radius = dsc->radius;

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

    /*** Init path ***/
    int32_t path_data[RECT_PATH_DATA_MAX_SIZE];
    uint32_t path_data_size;
    lv_vglite_create_rect_path_data(path_data, &path_data_size, radius, &rel_coords);
    vg_lite_quality_t path_quality = dsc->radius > 0 ? VG_LITE_HIGH : VG_LITE_LOW;

    vg_lite_path_t path;
    err = vg_lite_init_path(&path, VG_LITE_S32, path_quality, path_data_size, path_data,
                            (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                            ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);

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
            stops[i] = dsc->bg_grad.stops[i].frac;

            vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_ABGR8888 : VG_LITE_ARGB8888;
            if(lv_vglite_premult_and_swizzle(&colors[i], col32[i], dsc->bg_opa, color_format) != LV_RES_OK)
                VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");
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
    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if(lv_vglite_premult_and_swizzle(&vgcol, bg_col32, dsc->bg_opa, color_format) != LV_RES_OK)
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");

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
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    vg_lite_matrix_t matrix;
    lv_coord_t radius = dsc->radius;

    if(radius < 0)
        return LV_RES_INV;

    if(border) {
        /* Draw border - only has radius if object has radius*/
        float border_half = floor(dsc->border_width / 2.0f);
        if(radius > border_half)
            radius = radius - border_half;
    }
    else {
        /* Draw outline - always has radius */
        radius = radius + ceil(dsc->outline_width / 2.0f);

        if(radius > (lv_coord_t)LV_RADIUS_CIRCLE)
            radius = (lv_coord_t)LV_RADIUS_CIRCLE;
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
    vg_lite_blend_t vglite_blend_mode = lv_vglite_get_blend_mode(blend_mode);

    /*** Init destination buffer ***/
    if(lv_vglite_init_buf(&vgbuf, (uint32_t)dest_width, (uint32_t)dest_height, (uint32_t)dest_width * sizeof(lv_color_t),
                          (const lv_color_t *)draw_ctx->buf, false) != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");

    /*** Init path ***/
    int32_t path_data[RECT_PATH_DATA_MAX_SIZE];
    uint32_t path_data_size;
    lv_vglite_create_rect_path_data(path_data, &path_data_size, radius, &rel_coords);
    vg_lite_quality_t path_quality = dsc->radius > 0 ? VG_LITE_HIGH : VG_LITE_LOW;

    vg_lite_path_t path;
    err = vg_lite_init_path(&path, VG_LITE_S32, path_quality, path_data_size, path_data,
                            (vg_lite_float_t) rel_clip.x1, (vg_lite_float_t) rel_clip.y1,
                            ((vg_lite_float_t) rel_clip.x2) + 1.0f, ((vg_lite_float_t) rel_clip.y2) + 1.0f);

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

    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if(lv_vglite_premult_and_swizzle(&vgcol, col32, opa, color_format) != LV_RES_OK)
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");

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

static void lv_vglite_create_rect_path_data(int32_t * path_data, uint32_t * path_data_size,
                                            lv_coord_t radius,
                                            const lv_area_t * coords)
{
    lv_coord_t rect_width = lv_area_get_width(coords);
    lv_coord_t rect_height = lv_area_get_height(coords);

    /* Get the final radius. Can't be larger than the half of the shortest side */
    int32_t shortest_side = LV_MIN(rect_width, rect_height);
    int32_t final_radius = LV_MIN(radius, shortest_side / 2);

    if((radius == (lv_coord_t)LV_RADIUS_CIRCLE) && (rect_width == rect_height)) {
        float cpoff_fp = ((float)final_radius * BEZIER_OPTIM_CIRCLE);
        int32_t cpoff = (int32_t)cpoff_fp; /* Control point offset */
        int32_t circle_path_data[] = { /* Path data for circle */
            VLC_OP_MOVE, coords->x1 + final_radius,  coords->y1, /* Starting point */
            VLC_OP_CUBIC_REL, cpoff, 0, final_radius, final_radius - cpoff, final_radius, final_radius, /* Top-right arc */
            VLC_OP_CUBIC_REL, 0, cpoff, cpoff - final_radius, final_radius, 0 - final_radius, final_radius, /* Bottom-right arc*/
            VLC_OP_CUBIC_REL, 0 - cpoff, 0, 0 - final_radius, cpoff - final_radius, 0 - final_radius, 0 - final_radius, /* Bottom-left arc */
            VLC_OP_CUBIC_REL, 0, 0 - cpoff, final_radius - cpoff, 0 - final_radius, final_radius, 0 - final_radius, /* Top-left arc*/
            VLC_OP_END
        };

        *path_data_size = sizeof(circle_path_data);
        memcpy(path_data, circle_path_data, *path_data_size);
    }
    else if(radius > 0) {
        float cpoff_fp = ((float)final_radius * BEZIER_OPTIM_CIRCLE);
        int32_t cpoff = (int32_t)cpoff_fp; /* Control point offset */
        /* Draw is extended by 1 for vglite */
        int32_t rounded_path_data[] = { /* Path data for rounded rectangle */
            VLC_OP_MOVE, coords->x1 + final_radius,  coords->y1, /* Starting point */
            VLC_OP_LINE, coords->x2 + 1 - final_radius,  coords->y1, /* Top side */
            VLC_OP_CUBIC_REL, cpoff, 0, final_radius, final_radius - cpoff, final_radius, final_radius, /* Top-right corner */
            VLC_OP_LINE, coords->x2 + 1,  coords->y2 + 1 - final_radius, /* Right side */
            VLC_OP_CUBIC_REL, 0, cpoff, cpoff - final_radius, final_radius, 0 - final_radius, final_radius, /* Bottom-right corner*/
            VLC_OP_LINE, coords->x1 + final_radius,  coords->y2 + 1, /* Bottom side */
            VLC_OP_CUBIC_REL, 0 - cpoff, 0, 0 - final_radius, cpoff - final_radius, 0 - final_radius, 0 - final_radius, /* Bottom-left corner */
            VLC_OP_LINE, coords->x1,  coords->y1 + final_radius, /* Left side*/
            VLC_OP_CUBIC_REL, 0, 0 - cpoff, final_radius - cpoff, 0 - final_radius, final_radius, 0 - final_radius, /* Top-left corner */
            VLC_OP_END
        };
        *path_data_size = sizeof(rounded_path_data);
        memcpy(path_data, rounded_path_data, *path_data_size);
    }
    else {
        /* Draw is extended by 1 for vglite */
        int32_t rect_path_data[] = { /* Path data for simple rectangle */
            VLC_OP_MOVE, coords->x1,  coords->y1, /* Starting point */
            VLC_OP_LINE, coords->x2 + 1,  coords->y1, /* Top side */
            VLC_OP_LINE, coords->x2 + 1,  coords->y2 + 1, /* Right side */
            VLC_OP_LINE, coords->x1,  coords->y2 + 1, /* Bottom side */
            VLC_OP_LINE, coords->x1,  coords->y1, /* Left side*/
            VLC_OP_END
        };
        *path_data_size = sizeof(rect_path_data);
        memcpy(path_data, rect_path_data, *path_data_size);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_NXP_VG_LITE*/
