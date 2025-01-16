/**
 * @file lv_draw_vglite_border.c
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
#include "lv_vglite_path.h"
#include "lv_vglite_utils.h"

#include <math.h>

/*********************
 *      DEFINES
 *********************/

/*********************
 *      DEFINES
 *********************/

/*** Define maximum numbers of rectangles needed to clip partial borders ***/
#define MAX_NUM_RECTANGLES 4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Draw rectangle border/outline shape with effects (rounded corners, opacity)
 *
 * @param[in] coords Coordinates of the rectangle border/outline (relative to dest buff)
 * @param[in] clip_area Clip area with relative coordinates to dest buff
 * @param[in] dsc Description of the rectangle border/outline
 *
 */
static void _vglite_draw_border(const lv_area_t * coords, const lv_area_t * clip_area,
                                const lv_draw_border_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                           const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(dsc->side == (lv_border_side_t)LV_BORDER_SIDE_NONE)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    lv_area_t inward_coords;
    int32_t width = dsc->width;

    /* Move border inwards to align with software rendered border */
    inward_coords.x1 = coords->x1 + ceil(width / 2.0f);
    inward_coords.x2 = coords->x2 - floor(width / 2.0f);
    inward_coords.y1 = coords->y1 + ceil(width / 2.0f);
    inward_coords.y2 = coords->y2 - floor(width / 2.0f);

    lv_area_move(&inward_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_unit->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &inward_coords, &clip_area))
        return; /*Fully clipped, nothing to do*/

    _vglite_draw_border(&inward_coords, &clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_draw_border(const lv_area_t * coords, const lv_area_t * clip_area,
                                const lv_draw_border_dsc_t * dsc)
{
    int32_t radius = dsc->radius;
    vg_lite_buffer_t * vgbuf = vglite_get_dest_buf();

    if(radius < 0)
        return;

    int32_t border_half = (int32_t)floor(dsc->width / 2.0f);
    if(radius > border_half)
        radius = radius - border_half;

    vg_lite_cap_style_t cap_style = (radius) ? VG_LITE_CAP_ROUND : VG_LITE_CAP_BUTT;
    vg_lite_join_style_t join_style = (radius) ? VG_LITE_JOIN_ROUND : VG_LITE_JOIN_MITER;

    /*** Init path ***/
    int32_t path_data[RECT_PATH_DATA_MAX_SIZE];
    uint32_t path_data_size;
    vglite_create_rect_path_data(path_data, &path_data_size, radius, coords);
    vg_lite_quality_t path_quality = radius > 0 ? VG_LITE_HIGH : VG_LITE_MEDIUM;

    vg_lite_path_t path;
    VGLITE_CHECK_ERROR(vg_lite_init_path(&path, VG_LITE_S32, path_quality, path_data_size, path_data,
                                         (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                                         ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f));

    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    int32_t line_width = dsc->width;
    lv_border_side_t border_side = dsc->side;

    if(border_side == LV_BORDER_SIDE_FULL)
        border_side = LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT;

    uint32_t num_rect = 0;
    vg_lite_rectangle_t rect[MAX_NUM_RECTANGLES];
    int32_t rect_width = coords->x2 - coords->x1;
    int32_t rect_height = coords->y2 - coords->y1;
    int32_t shortest_side = LV_MIN(rect_width, rect_height);
    int32_t final_radius = LV_MIN(radius, shortest_side / 2);

    if(border_side & LV_BORDER_SIDE_TOP) {
        rect[num_rect].x = coords->x1 - ceil(line_width / 2.0f);
        rect[num_rect].y = coords->y1 - ceil(line_width / 2.0f);
        rect[num_rect].width = coords->x2 - coords->x1 + line_width;
        rect[num_rect].height = final_radius + ceil(line_width / 2.0f);
        num_rect++;
    }

    if(border_side & LV_BORDER_SIDE_LEFT) {
        rect[num_rect].x = coords->x1 - ceil(line_width / 2.0f);
        rect[num_rect].y = coords->y1 - ceil(line_width / 2.0f);
        rect[num_rect].width = final_radius + ceil(line_width / 2.0f);
        rect[num_rect].height = coords->y2 - coords->y1 + line_width + 1;
        num_rect++;
    }

    if(border_side & LV_BORDER_SIDE_RIGHT) {
        rect[num_rect].x = coords->x2 - final_radius + 1;
        rect[num_rect].y = coords->y1 - ceil(line_width / 2.0f);
        rect[num_rect].width = final_radius + ceil(line_width / 2.0f);
        rect[num_rect].height = coords->y2 - coords->y1 + line_width + 1;
        num_rect++;
    }

    if(border_side & LV_BORDER_SIDE_BOTTOM) {
        rect[num_rect].x = coords->x1 - ceil(line_width / 2.0f);
        rect[num_rect].y = coords->y2 - final_radius + 1;
        rect[num_rect].width = coords->x2 - coords->x1 + line_width;
        rect[num_rect].height = final_radius + ceil(line_width / 2.0f);
        num_rect++;
    }

    /*** Enable scissor and apply scissor rects ***/
    VGLITE_CHECK_ERROR(vg_lite_enable_scissor());
    VGLITE_CHECK_ERROR(vg_lite_scissor_rects(vgbuf, num_rect, rect));

    /*** Draw border ***/
    VGLITE_CHECK_ERROR(vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH));

    VGLITE_CHECK_ERROR(vg_lite_set_stroke(&path, cap_style, join_style, line_width, 8, NULL, 0, 0, vgcol));

    VGLITE_CHECK_ERROR(vg_lite_update_stroke(&path));

    VGLITE_CHECK_ERROR(vg_lite_draw(vgbuf, &path, VG_LITE_FILL_NON_ZERO, NULL, VG_LITE_BLEND_SRC_OVER, vgcol));

    vglite_run();

    VGLITE_CHECK_ERROR(vg_lite_clear_path(&path));

    /*** Disable scissor ***/
    VGLITE_CHECK_ERROR(vg_lite_disable_scissor());
}

#endif /*LV_USE_DRAW_VGLITE*/
