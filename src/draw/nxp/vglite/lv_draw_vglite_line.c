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
 * Draw line shape with effects
 *
 * @param[in] point1 Starting point with relative coordinates
 * @param[in] point2 Ending point with relative coordinates
 * @param[in] clip_area Clip area with relative coordinates to dest buff
 * @param[in] dsc Line description structure (width, rounded ending, opacity, ...)
 *
 */
static void _vglite_draw_line(const lv_point_t * point1, const lv_point_t * point2,
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

void lv_draw_vglite_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc)
{
    if(dsc->width == 0)
        return;
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    lv_area_t clip_area;
    clip_area.x1 = LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width / 2;
    clip_area.x2 = LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width / 2;
    clip_area.y1 = LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width / 2;
    clip_area.y2 = LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width / 2;

    if(!_lv_area_intersect(&clip_area, &clip_area, draw_unit->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_point_t point1 = {dsc->p1.x - layer->buf_area.x1, dsc->p1.y - layer->buf_area.y1};
    lv_point_t point2 = {dsc->p2.x - layer->buf_area.x1, dsc->p2.y - layer->buf_area.y1};

    _vglite_draw_line(&point1, &point2, &clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_draw_line(const lv_point_t * point1, const lv_point_t * point2,
                              const lv_area_t * clip_area, const lv_draw_line_dsc_t * dsc)
{
    vg_lite_path_t path;
    vg_lite_buffer_t * vgbuf = vglite_get_dest_buf();
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

    vg_lite_blend_t vgblend = vglite_get_blend_mode(dsc->blend_mode);

    /*** Init path ***/
    int32_t width = dsc->width;

    int32_t line_path[] = { /*VG line path*/
        VLC_OP_MOVE, point1->x, point1->y,
        VLC_OP_LINE, point2->x, point2->y,
        VLC_OP_END
    };

    VGLITE_CHECK_ERROR(vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(line_path), line_path,
                                         (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                                         ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f));

    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    /*** Draw line ***/
    VGLITE_CHECK_ERROR(vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH));

    VGLITE_CHECK_ERROR(vg_lite_set_stroke(&path, cap_style, join_style, width, 8, stroke_dash_pattern, stroke_dash_count,
                                          stroke_dash_phase, vgcol));

    VGLITE_CHECK_ERROR(vg_lite_update_stroke(&path));

    VGLITE_CHECK_ERROR(vg_lite_draw(vgbuf, &path, VG_LITE_FILL_NON_ZERO, &matrix, vgblend, vgcol));

    vglite_run();

    VGLITE_CHECK_ERROR(vg_lite_clear_path(&path));
}

#endif /*LV_USE_DRAW_VGLITE*/
