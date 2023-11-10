/**
 * @file lv_draw_vglite_line.c
 *
 */

/**
 * Copyright 2022, 2023 NXP
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
 * @param[in] clip_area Clipping area with relative coordinates to dest buff
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
    lv_area_t rel_clip_area;
    rel_clip_area.x1 = LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width / 2;
    rel_clip_area.x2 = LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width / 2;
    rel_clip_area.y1 = LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width / 2;
    rel_clip_area.y2 = LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width / 2;

    if(!_lv_area_intersect(&rel_clip_area, &rel_clip_area, draw_unit->clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_area_move(&rel_clip_area, -layer->draw_buf_ofs.x, -layer->draw_buf_ofs.y);

    lv_point_t rel_point1 = {dsc->p1.x - layer->draw_buf_ofs.x, dsc->p1.y - layer->draw_buf_ofs.y};
    lv_point_t rel_point2 = {dsc->p2.x - layer->draw_buf_ofs.x, dsc->p2.y - layer->draw_buf_ofs.y};

    _vglite_draw_line(&rel_point1, &rel_point2, &rel_clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_draw_line(const lv_point_t * point1, const lv_point_t * point2,
                              const lv_area_t * clip_area, const lv_draw_line_dsc_t * dsc)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
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

    /* Choose vglite blend mode based on given lvgl blend mode */
    vg_lite_blend_t vglite_blend_mode = vglite_get_blend_mode(dsc->blend_mode);

    /*** Init path ***/
    int32_t width = dsc->width;

    int32_t line_path[] = { /*VG line path*/
        VLC_OP_MOVE, point1->x, point1->y,
        VLC_OP_LINE, point2->x, point2->y,
        VLC_OP_END
    };

    err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_HIGH, sizeof(line_path), line_path,
                            (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                            ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Init path failed.");

    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    /*** Draw line ***/
    err = vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Set draw path type failed.");

    err = vg_lite_set_stroke(&path, cap_style, join_style, width, 8, stroke_dash_pattern, stroke_dash_count,
                             stroke_dash_phase, vgcol);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Set stroke failed.");

    err = vg_lite_update_stroke(&path);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Update stroke failed.");

    vglite_set_scissor(clip_area);

    err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_NON_ZERO, &matrix, vglite_blend_mode, vgcol);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Draw line failed.");

    vglite_run();

    vglite_disable_scissor();

    err = vg_lite_clear_path(&path);
    LV_ASSERT_MSG(err == VG_LITE_SUCCESS, "Clear path failed.");
}

#endif /*LV_USE_DRAW_VGLITE*/
