/**
 * @file lv_draw_vglite_line.c
 *
 */

/**
 * Copyright 2022-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_utils.h"

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
 * Set line path data
 *
 * @param[in/out] line_path Coordinates of the line
 * @param[in/out] path_data_size Size of path_data (bytes)
 * @param[in] p1 First point of the line
 * @patam[in] p2 Second point of the line
 *
 */
static void _vglite_set_line(int32_t * line_path, uint32_t * path_data_size,
                             const lv_point_t * point1, const lv_point_t * point2);

/**
 * Draw line shape with effects
 *
 * @param[in] point1 Starting point with relative coordinates
 * @param[in] point2 Ending point with relative coordinates
 * @param[in] clip_area Clip area with relative coordinates to dest buff
 * @param[in] dsc Line description structure (width, rounded ending, opacity, ...)
 *
 */
static void _vglite_draw_line(vglite_draw_task_t * vglite_task, const lv_point_t * point1, const lv_point_t * point2,
                              const lv_area_t * clip_area, const lv_draw_line_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_line(vglite_draw_task_t * vglite_task)
{
    const lv_draw_line_dsc_t * dsc = vglite_task->t->draw_dsc;

    if(dsc->width == 0)
        return;
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y)
        return;

    lv_layer_t * layer = vglite_task->t->target_layer;
    lv_area_t clip_area;
    clip_area.x1 = LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width / 2;
    clip_area.x2 = LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width / 2;
    clip_area.y1 = LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width / 2;
    clip_area.y2 = LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width / 2;

    if(!lv_area_intersect(&clip_area, &clip_area, &vglite_task->t->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_point_t point1 = {dsc->p1.x - layer->buf_area.x1, dsc->p1.y - layer->buf_area.y1};
    lv_point_t point2 = {dsc->p2.x - layer->buf_area.x1, dsc->p2.y - layer->buf_area.y1};

    _vglite_draw_line(vglite_task, &point1, &point2, &clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_set_line(int32_t * line_path, uint32_t * path_data_size, const lv_point_t * point1,
                             const lv_point_t * point2)
{
    uint32_t pidx = 0;
    line_path[pidx++] = VLC_OP_MOVE;
    line_path[pidx++] = point1->x;
    line_path[pidx++] = point1->y;
    line_path[pidx++] = VLC_OP_LINE;
    line_path[pidx++] = point2->x;
    line_path[pidx++] = point2->y;
    line_path[pidx++] = VLC_OP_END;

    *path_data_size = pidx * sizeof(int32_t);
}

static void _vglite_draw_line(vglite_draw_task_t * vglite_task, const lv_point_t * point1, const lv_point_t * point2,
                              const lv_area_t * clip_area, const lv_draw_line_dsc_t * dsc)
{
    vg_lite_path_t * path = lv_malloc_zeroed(sizeof(vg_lite_path_t));
    LV_ASSERT_MALLOC(path);
    vglite_task->path = path;
    vg_lite_buffer_t * dest_buf = vglite_get_dest_buf();
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

    /*** Init path ***/
    int32_t width = dsc->width;

    uint32_t path_data_size = 0;
    int32_t * line_path = lv_malloc_zeroed(7 * sizeof(int32_t));
    LV_ASSERT_MALLOC(line_path);
    vglite_task->path_data = line_path;
    _vglite_set_line(line_path, &path_data_size, point1, point2);

    VGLITE_CHECK_ERROR(vg_lite_init_path(path, VG_LITE_S32, VG_LITE_HIGH, path_data_size, line_path,
                                         (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                                         ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f));

    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    /*** Draw line ***/
    VGLITE_CHECK_ERROR(vg_lite_set_draw_path_type(path, VG_LITE_DRAW_STROKE_PATH));

    VGLITE_CHECK_ERROR(vg_lite_set_stroke(path, cap_style, join_style, width, 8, stroke_dash_pattern, stroke_dash_count,
                                          stroke_dash_phase, vgcol));

    VGLITE_CHECK_ERROR(vg_lite_update_stroke(path));

    VGLITE_CHECK_ERROR(vg_lite_draw(dest_buf, path, VG_LITE_FILL_NON_ZERO, NULL, VG_LITE_BLEND_SRC_OVER, vgcol));

    vglite_run();
}

#endif /*LV_USE_DRAW_VGLITE*/
