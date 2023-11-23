/**
 * @file lv_draw_vglite_triangle.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_path.h"
#include "lv_vglite_utils.h"

#include "../../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Draw triangle shape with effects (opacity, gradient)
 *
 * @param[in] coords Coordinates of the triangle (relative to dest buff)
 * @param[in] clip_area Clipping area with relative coordinates to dest buff
 * @param[in] dsc Description of the triangle
 *
 */
static void _vglite_draw_triangle(const lv_area_t * coords, const lv_area_t * clip_area,
                                  const lv_draw_triangle_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc)
{
    if(dsc->bg_opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_unit->clip_area);
    lv_area_move(&rel_clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t rel_coords;
    rel_coords.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    rel_coords.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    rel_coords.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    rel_coords.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_area_move(&rel_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &rel_coords, &rel_clip_area))
        return; /* Fully clipped, nothing to do */

    _vglite_draw_triangle(&rel_coords, &rel_clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_draw_triangle(const lv_area_t * coords, const lv_area_t * clip_area,
                                  const lv_draw_triangle_dsc_t * dsc)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t * vgbuf = vglite_get_dest_buf();

    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    uint32_t width = tri_area.x2 - tri_area.x1;
    uint32_t height = tri_area.y2 - tri_area.y1;

    /* Init path */
    int32_t triangle_path[] = { /*VG line path*/
        VLC_OP_MOVE, dsc->p[0].x, dsc->p[0].y,
        VLC_OP_LINE, dsc->p[1].x, dsc->p[1].y,
        VLC_OP_LINE, dsc->p[2].x, dsc->p[2].y,
        VLC_OP_LINE, dsc->p[0].x, dsc->p[0].y,
        VLC_OP_END
    };

    vg_lite_path_t path;
    err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(triangle_path), triangle_path,
                            (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                            ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Init path failed.");

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    /* Init Color */
    lv_color32_t col32 = lv_color_to_32(dsc->bg_color, dsc->bg_opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vglite_set_scissor(clip_area);

    vg_lite_linear_gradient_t gradient;
    bool has_gradient = (dsc->bg_grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE);

    /* Init Gradient*/
    if(has_gradient) {
        vg_lite_matrix_t * grad_matrix;

        uint32_t colors[LV_GRADIENT_MAX_STOPS];
        uint32_t stops[LV_GRADIENT_MAX_STOPS];
        lv_color32_t col32[LV_GRADIENT_MAX_STOPS];

        /* Gradient Setup */
        uint8_t cnt = LV_MAX(dsc->bg_grad.stops_count, LV_GRADIENT_MAX_STOPS);
        for(uint8_t i = 0; i < cnt; i++) {
            stops[i] = dsc->bg_grad.stops[i].frac;

            col32[i] = lv_color_to_32(dsc->bg_grad.stops[i].color, dsc->bg_grad.stops[i].opa);
            colors[i] = vglite_get_color(col32[i], true);
        }

        lv_memzero(&gradient, sizeof(vg_lite_linear_gradient_t));

        err = vg_lite_init_grad(&gradient);
        LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Init gradient failed.");

        err = vg_lite_set_grad(&gradient, cnt, colors, stops);
        LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Set gradient failed.");

        err = vg_lite_update_grad(&gradient);
        LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Update gradient failed.");

        grad_matrix = vg_lite_get_grad_matrix(&gradient);
        vg_lite_identity(grad_matrix);
        vg_lite_translate((float)coords->x1, (float)coords->y1, grad_matrix);

        if(dsc->bg_grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_VER) {
            vg_lite_scale(1.0f, (float)height / 256.0f, grad_matrix);
            vg_lite_rotate(90.0f, grad_matrix);
        }
        else { /*LV_GRAD_DIR_HOR*/
            vg_lite_scale((float)width / 256.0f, 1.0f, grad_matrix);
        }

        err = vg_lite_draw_gradient(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, &gradient, VG_LITE_BLEND_SRC_OVER);
        LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Draw gradient failed.");
    }
    else {
        err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
        LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Draw triangle failed.");
    }

    vglite_run();

    vglite_disable_scissor();

    err = vg_lite_clear_path(&path);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Clear path failed.");

    if(has_gradient) {
        err = vg_lite_clear_grad(&gradient);
        LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Clear gradient failed.");
    }
}

#endif /*LV_USE_DRAW_VGLITE*/
