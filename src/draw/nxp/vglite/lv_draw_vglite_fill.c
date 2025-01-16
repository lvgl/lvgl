/**
 * @file lv_draw_vglite_fill.c
 *
 */

/**
 * Copyright 2020-2024 NXP
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
 * Fill area, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dsc Description of the area to fill (color, opa)
 *
 */
static void _vglite_fill(const lv_area_t * dest_area, const lv_draw_fill_dsc_t * dsc);

/**
 * Draw rectangle background with effects (rounded corners, gradient)
 *
 * @param[in] coords Coordinates of the rectangle background (relative to dest buff)
 * @param[in] clip_area Clip area with relative coordinates to dest buff
 * @param[in] dsc Description of the rectangle background
 *
 */
static void _vglite_draw_rect(const lv_area_t * coords, const lv_area_t * clip_area,
                              const lv_draw_fill_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                         const lv_area_t * coords)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    lv_layer_t * layer = draw_unit->target_layer;
    lv_area_t relative_coords;
    lv_area_copy(&relative_coords, coords);
    lv_area_move(&relative_coords, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_unit->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, &relative_coords, &clip_area))
        return; /*Fully clipped, nothing to do*/

    /*
     * Most simple case: just a plain rectangle (no radius, no gradient)
     */
    if((dsc->radius == 0) && (dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE))
        _vglite_fill(&clipped_coords, dsc);
    else
        _vglite_draw_rect(&relative_coords, &clip_area, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _vglite_fill(const lv_area_t * dest_area, const lv_draw_fill_dsc_t * dsc)
{
    vg_lite_buffer_t * vgbuf = vglite_get_dest_buf();

    lv_color32_t col32 = lv_color_to_32(dsc->color, dsc->opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    if(dsc->opa >= (lv_opa_t)LV_OPA_MAX) {   /*Opaque fill*/
        vg_lite_rectangle_t rect = {
            .x = dest_area->x1,
            .y = dest_area->y1,
            .width = lv_area_get_width(dest_area),
            .height = lv_area_get_height(dest_area)
        };

        VGLITE_CHECK_ERROR(vg_lite_clear(vgbuf, &rect, vgcol));

        vglite_run();
    }
    else {   /*fill with transparency*/

        vg_lite_path_t path;
        int32_t path_data[] = { /*VG rectangular path*/
            VLC_OP_MOVE, dest_area->x1,  dest_area->y1,
            VLC_OP_LINE, dest_area->x2 + 1,  dest_area->y1,
            VLC_OP_LINE, dest_area->x2 + 1,  dest_area->y2 + 1,
            VLC_OP_LINE, dest_area->x1,  dest_area->y2 + 1,
            VLC_OP_LINE, dest_area->x1,  dest_area->y1,
            VLC_OP_END
        };

        VGLITE_CHECK_ERROR(vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_MEDIUM, sizeof(path_data), path_data,
                                             (vg_lite_float_t) dest_area->x1, (vg_lite_float_t) dest_area->y1,
                                             ((vg_lite_float_t) dest_area->x2) + 1.0f, ((vg_lite_float_t) dest_area->y2) + 1.0f));

        /*Draw rectangle*/
        VGLITE_CHECK_ERROR(vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, NULL, VG_LITE_BLEND_SRC_OVER, vgcol));

        vglite_run();

        VGLITE_CHECK_ERROR(vg_lite_clear_path(&path));
    }
}

static void _vglite_draw_rect(const lv_area_t * coords, const lv_area_t * clip_area,
                              const lv_draw_fill_dsc_t * dsc)
{
    int32_t width = lv_area_get_width(coords);
    int32_t height = lv_area_get_height(coords);
    int32_t radius = dsc->radius;
    lv_opa_t opa = dsc->opa;
    vg_lite_buffer_t * vgbuf = vglite_get_dest_buf();

    if(dsc->radius < 0)
        return;

    /*** Init path ***/
    int32_t path_data[RECT_PATH_DATA_MAX_SIZE];
    uint32_t path_data_size;
    vglite_create_rect_path_data(path_data, &path_data_size, radius, coords);
    vg_lite_quality_t path_quality = dsc->radius > 0 ? VG_LITE_HIGH : VG_LITE_MEDIUM;

    vg_lite_path_t path;
    VGLITE_CHECK_ERROR(vg_lite_init_path(&path, VG_LITE_S32, path_quality, path_data_size, path_data,
                                         (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                                         ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f));

    /*** Init Color ***/
    lv_color32_t col32 = lv_color_to_32(dsc->color, opa);
    vg_lite_color_t vgcol = vglite_get_color(col32, false);

    vg_lite_linear_gradient_t gradient;
    bool has_gradient = (dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE);

    /*** Init Gradient ***/
    if(has_gradient) {
        vg_lite_matrix_t * grad_matrix;

        vg_lite_uint32_t colors[LV_GRADIENT_MAX_STOPS];
        vg_lite_uint32_t stops[LV_GRADIENT_MAX_STOPS];
        lv_color32_t col32[LV_GRADIENT_MAX_STOPS];

        /* Gradient setup */
        vg_lite_uint32_t cnt = LV_MAX(dsc->grad.stops_count, LV_GRADIENT_MAX_STOPS);
        lv_opa_t opa;

        for(uint8_t i = 0; i < cnt; i++) {
            stops[i] = dsc->grad.stops[i].frac;

            opa = LV_OPA_MIX2(dsc->grad.stops[i].opa, dsc->opa);

            col32[i] = lv_color_to_32(dsc->grad.stops[i].color, opa);
            colors[i] = vglite_get_color(col32[i], true);
        }

        lv_memzero(&gradient, sizeof(vg_lite_linear_gradient_t));

        VGLITE_CHECK_ERROR(vg_lite_init_grad(&gradient));

        VGLITE_CHECK_ERROR(vg_lite_set_grad(&gradient, cnt, colors, stops));

        VGLITE_CHECK_ERROR(vg_lite_update_grad(&gradient));

        grad_matrix = vg_lite_get_grad_matrix(&gradient);
        vg_lite_identity(grad_matrix);
        vg_lite_translate((float)coords->x1, (float)coords->y1, grad_matrix);

        if(dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_VER) {
            vg_lite_scale(1.0f, (float)height / 256.0f, grad_matrix);
            vg_lite_rotate(90.0f, grad_matrix);
        }
        else {   /*LV_GRAD_DIR_HOR*/
            vg_lite_scale((float)width / 256.0f, 1.0f, grad_matrix);
        }

        VGLITE_CHECK_ERROR(vg_lite_draw_gradient(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, NULL, &gradient,
                                                 VG_LITE_BLEND_SRC_OVER));
    }
    else {
        VGLITE_CHECK_ERROR(vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, NULL, VG_LITE_BLEND_SRC_OVER, vgcol));
    }

    vglite_run();

    VGLITE_CHECK_ERROR(vg_lite_clear_path(&path));

    if(has_gradient)
        VGLITE_CHECK_ERROR(vg_lite_clear_grad(&gradient));
}

#endif /*LV_USE_DRAW_VGLITE*/
