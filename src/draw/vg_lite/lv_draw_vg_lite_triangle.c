/**
 * @file lv_draw_vg_lite_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_area_private.h"
#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_path.h"
#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_grad.h"

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

void lv_draw_vg_lite_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc)
{
    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    bool is_common;
    lv_area_t clip_area;
    is_common = lv_area_intersect(&clip_area, &tri_area, draw_unit->clip_area);
    if(!is_common) return;

    LV_PROFILER_BEGIN;

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_bonding_box_area(path, &clip_area);
    lv_vg_lite_path_move_to(path, dsc->p[0].x, dsc->p[0].y);
    lv_vg_lite_path_line_to(path, dsc->p[1].x, dsc->p[1].y);
    lv_vg_lite_path_line_to(path, dsc->p[2].x, dsc->p[2].y);
    lv_vg_lite_path_close(path);
    lv_vg_lite_path_end(path);

    vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(path);

    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);
    LV_VG_LITE_ASSERT_PATH(vg_lite_path);

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    lv_vg_lite_matrix_multiply(&matrix, &u->global_matrix);
    LV_VG_LITE_ASSERT_MATRIX(&matrix);

    if(dsc->bg_grad.dir != LV_GRAD_DIR_NONE) {
#if LV_USE_VECTOR_GRAPHIC
        lv_vg_lite_draw_grad_helper(
            u,
            &u->target_buffer,
            vg_lite_path,
            &tri_area,
            &dsc->bg_grad,
            &matrix,
            VG_LITE_FILL_EVEN_ODD,
            VG_LITE_BLEND_SRC_OVER);
#else
        LV_LOG_WARN("Gradient fill is not supported without VECTOR_GRAPHIC");
#endif
    }
    else { /* normal fill */
        vg_lite_color_t color = lv_vg_lite_color(dsc->bg_color, dsc->bg_opa, true);
        LV_PROFILER_BEGIN_TAG("vg_lite_draw");
        LV_VG_LITE_CHECK_ERROR(vg_lite_draw(
                                   &u->target_buffer,
                                   vg_lite_path,
                                   VG_LITE_FILL_EVEN_ODD,
                                   &matrix,
                                   VG_LITE_BLEND_SRC_OVER,
                                   color));
        LV_PROFILER_END_TAG("vg_lite_draw");
    }

    lv_vg_lite_path_drop(u, path);

    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
