/**
 * @file lv_draw_vglite_triangle.c
 *
 */

/**
 * Copyright 2023-2024 NXP
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
 * Set triangle path data
 *
 * @param[in/out] path_data Coordinates of the triangle
 * @param[in/out] path_data_size Size of path_data (bytes)
 * @param[in] p Points of the triangle
 *
 */
static void _vglite_set_triangle(int32_t * path_data, uint32_t * path_data_size,
                                 const lv_point_precise_t * p);

/**
 * Draw triangle shape with effects (opacity, gradient)
 *
 * @param[in] vglite_task The current vglite task
 * @param[in] coords Coordinates of the triangle (relative to dest buff)
 * @param[in] clip_area Clipping area with relative coordinates to dest buff
 * @param[in] dsc Description of the triangle
 *
 */
static void _vglite_draw_triangle(vglite_draw_task_t * vglite_task, const lv_area_t * coords,
                                  const lv_area_t * clip_area,
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

void lv_draw_vglite_triangle(vglite_draw_task_t * vglite_task)
{
    const lv_draw_triangle_dsc_t * dsc = vglite_task->t->draw_dsc;

    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = vglite_task->t->target_layer;
    lv_area_t clip_area;
    lv_area_copy(&clip_area, &vglite_task->t->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t coords;
    coords.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    coords.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    coords.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    coords.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_area_move(&coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &coords, &clip_area))
        return; /* Fully clipped, nothing to do */

    _vglite_draw_triangle(vglite_task, &coords, &clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_set_triangle(int32_t * path_data, uint32_t * path_data_size, const lv_point_precise_t * p)
{
    uint32_t pidx = 0;
    path_data[pidx++] = VLC_OP_MOVE;
    path_data[pidx++] = p[0].x;
    path_data[pidx++] = p[0].y;
    path_data[pidx++] = VLC_OP_LINE;
    path_data[pidx++] = p[1].x;
    path_data[pidx++] = p[1].y;
    path_data[pidx++] = VLC_OP_LINE;
    path_data[pidx++] = p[2].x;
    path_data[pidx++] = p[2].y;
    path_data[pidx++] = VLC_OP_LINE;
    path_data[pidx++] = p[0].x;
    path_data[pidx++] = p[0].y;
    path_data[pidx++] = VLC_OP_END;

    *path_data_size = pidx * sizeof(int32_t);
}

static void _vglite_draw_triangle(vglite_draw_task_t * vglite_task, const lv_area_t * coords,
                                  const lv_area_t * clip_area,
                                  const lv_draw_triangle_dsc_t * dsc)
{
    vg_lite_buffer_t * dest_buf = vglite_get_dest_buf();

    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    uint32_t width = lv_area_get_width(&tri_area);
    uint32_t height = lv_area_get_height(&tri_area);

    /* Init path */
    uint32_t path_data_size;
    int32_t * triangle_path = lv_malloc_zeroed(13 * sizeof(int32_t));
    LV_ASSERT_MALLOC(triangle_path);
    vglite_task->path_data = triangle_path;
    _vglite_set_triangle(triangle_path, &path_data_size, dsc->p);


    vg_lite_path_t * path = lv_malloc_zeroed(sizeof(vg_lite_path_t));
    LV_ASSERT_MALLOC(path);
    vglite_task->path = path;
    VGLITE_CHECK_ERROR(vg_lite_init_path(path, VG_LITE_S32, VG_LITE_HIGH, path_data_size, triangle_path,
                                         (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                                         ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f));

    /* Init Color */
    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vg_lite_linear_gradient_t * gradient;

    bool has_gradient = (dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE);

    /* Init Gradient*/
    if(has_gradient) {
        gradient = lv_malloc_zeroed(sizeof(vg_lite_linear_gradient_t));
        LV_ASSERT_MALLOC(gradient);
        vglite_task->gradient = gradient;

        vg_lite_matrix_t * grad_matrix;

        vg_lite_uint32_t colors[LV_GRADIENT_MAX_STOPS];
        vg_lite_uint32_t stops[LV_GRADIENT_MAX_STOPS];
        lv_color32_t col32[LV_GRADIENT_MAX_STOPS];

        /* Gradient Setup */
        vg_lite_uint32_t cnt = LV_MIN(dsc->grad.stops_count, LV_GRADIENT_MAX_STOPS);
        lv_opa_t opa;

        for(uint8_t i = 0; i < cnt; i++) {
            stops[i] = dsc->grad.stops[i].frac;
            opa = LV_OPA_MIX2(dsc->grad.stops[i].opa, dsc->opa);

            col32[i] = lv_color_to_32(dsc->grad.stops[i].color, opa);
            colors[i] = vglite_get_color(col32[i], true);
        }

        VGLITE_CHECK_ERROR(vg_lite_init_grad(gradient));

        VGLITE_CHECK_ERROR(vg_lite_set_grad(gradient, cnt, colors, stops));

        VGLITE_CHECK_ERROR(vg_lite_update_grad(gradient));

        grad_matrix = vg_lite_get_grad_matrix(gradient);
        VGLITE_CHECK_ERROR(vg_lite_identity(grad_matrix));
        VGLITE_CHECK_ERROR(vg_lite_translate((float)coords->x1, (float)coords->y1, grad_matrix));

        if(dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_VER) {
            VGLITE_CHECK_ERROR(vg_lite_scale(1.0f, (float)height / 256.0f, grad_matrix));
            VGLITE_CHECK_ERROR(vg_lite_rotate(90.0f, grad_matrix));
        }
        else { /*LV_GRAD_DIR_HOR*/
            VGLITE_CHECK_ERROR(vg_lite_scale((float)width / 256.0f, 1.0f, grad_matrix));
        }

        VGLITE_CHECK_ERROR(vg_lite_draw_gradient(dest_buf, path, VG_LITE_FILL_EVEN_ODD, NULL, gradient,
                                                 VG_LITE_BLEND_SRC_OVER));
    }
    else {
        VGLITE_CHECK_ERROR(vg_lite_draw(dest_buf, path, VG_LITE_FILL_EVEN_ODD, NULL, VG_LITE_BLEND_SRC_OVER, vgcol));
    }

    vglite_run();
}

#endif /*LV_USE_DRAW_VGLITE*/
