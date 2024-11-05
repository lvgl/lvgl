/**
 * @file lv_draw_vg_lite_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_area_private.h"
#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "lv_vg_lite_utils.h"
#include "lv_vg_lite_grad.h"

/*********************
 *      DEFINES
 *********************/

#if LV_GRADIENT_MAX_STOPS > VLC_MAX_GRADIENT_STOPS
    #error "LV_GRADIENT_MAX_STOPS must be equal or less than VLC_MAX_GRADIENT_STOPS"
#endif

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

void lv_draw_vg_lite_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, draw_unit->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    vg_lite_matrix_t matrix = u->global_matrix;

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, dsc->radius == 0 ? VG_LITE_LOW : VG_LITE_HIGH);
    lv_vg_lite_path_set_bounding_box_area(path, &clip_area);
    lv_vg_lite_path_append_rect(path,
                                coords->x1, coords->y1,
                                lv_area_get_width(coords), lv_area_get_height(coords),
                                dsc->radius);
    lv_vg_lite_path_end(path);

    vg_lite_path_t * vg_lite_path = lv_vg_lite_path_get_path(path);

    LV_VG_LITE_ASSERT_DEST_BUFFER(&u->target_buffer);
    LV_VG_LITE_ASSERT_PATH(vg_lite_path);
    LV_VG_LITE_ASSERT_MATRIX(&matrix);

    if(dsc->grad.dir != LV_GRAD_DIR_NONE) {
#if LV_USE_VECTOR_GRAPHIC
        lv_vg_lite_draw_grad_helper(
            u,
            &u->target_buffer,
            vg_lite_path,
            coords,
            &dsc->grad,
            &matrix,
            VG_LITE_FILL_EVEN_ODD,
            VG_LITE_BLEND_SRC_OVER);
#else
        LV_LOG_WARN("Gradient fill is not supported without VECTOR_GRAPHIC");
#endif
    }
    else { /* normal fill */
        vg_lite_color_t color = lv_vg_lite_color(dsc->color, dsc->opa, true);
        LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_draw");
        LV_VG_LITE_CHECK_ERROR(vg_lite_draw(
                                   &u->target_buffer,
                                   vg_lite_path,
                                   VG_LITE_FILL_EVEN_ODD,
                                   &matrix,
                                   VG_LITE_BLEND_SRC_OVER,
                                   color));
        LV_PROFILER_DRAW_END_TAG("vg_lite_draw");
    }

    lv_vg_lite_path_drop(u, path);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
