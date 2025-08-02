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
void lv_draw_vg_lite_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    vg_lite_matrix_t matrix = u->global_matrix;

    /* Improve GPU rendering efficiency using simpler fill modes */
    if(dsc->radius == 0 && dsc->opa >= LV_OPA_MAX && dsc->grad.dir == LV_GRAD_DIR_NONE &&
       lv_matrix_is_identity((lv_matrix_t *)&matrix)) {
        lv_vg_lite_clear(&u->target_buffer, &clip_area, lv_vg_lite_color(dsc->color, LV_OPA_COVER, false));
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_quality(path, dsc->radius == 0 ? VG_LITE_LOW : VG_LITE_HIGH);
    lv_vg_lite_path_set_bounding_box_area(path, &clip_area);
    lv_vg_lite_path_append_rect(path,
                                coords->x1, coords->y1,
                                lv_area_get_width(coords), lv_area_get_height(coords),
                                dsc->radius);
    lv_vg_lite_path_end(path);

    if(dsc->grad.dir != LV_GRAD_DIR_NONE) {
#if LV_USE_VECTOR_GRAPHIC
        lv_vg_lite_draw_grad_helper(
            u->grad_ctx,
            &u->target_buffer,
            lv_vg_lite_path_get_path(path),
            coords,
            &dsc->grad,
            &matrix,
            VG_LITE_FILL_EVEN_ODD,
            VG_LITE_BLEND_SRC_OVER);
#else
        LV_LOG_WARN("Gradient fill is not supported without VECTOR_GRAPHIC");
#endif
    }
    else {
        /* normal fill */
        lv_vg_lite_draw(
            &u->target_buffer,
            lv_vg_lite_path_get_path(path),
            VG_LITE_FILL_EVEN_ODD,
            &matrix,
            VG_LITE_BLEND_SRC_OVER,
            lv_vg_lite_color(dsc->color, dsc->opa, true));
    }

    lv_vg_lite_path_drop(u, path);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
