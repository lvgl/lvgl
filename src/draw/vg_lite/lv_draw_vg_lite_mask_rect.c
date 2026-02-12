/**
 * @file lv_draw_vg_lite_mask_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_utils.h"
#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_path.h"
#include "../../misc/lv_area_private.h"
#include "../lv_draw_mask.h"

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
void lv_draw_vg_lite_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc,
                               const lv_area_t * coords)
{
    LV_UNUSED(coords);
    lv_area_t draw_area;

    if(!lv_area_intersect(&draw_area, &dsc->area, &t->clip_area)) {
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_vg_lite_unit_t * u = (lv_draw_vg_lite_unit_t *)t->draw_unit;

    lv_vg_lite_path_t * path = lv_vg_lite_path_get(u, VG_LITE_FP32);
    lv_vg_lite_path_set_bounding_box_area(path, &t->clip_area);

    /* Nesting cropping regions using rounded rectangles and normal rectangles */
    lv_vg_lite_path_append_rect(
        path,
        dsc->area.x1, dsc->area.y1,
        lv_area_get_width(&dsc->area), lv_area_get_height(&dsc->area),
        dsc->radius);
    lv_vg_lite_path_append_rect(
        path,
        t->clip_area.x1, t->clip_area.y1,
        lv_area_get_width(&t->clip_area), lv_area_get_height(&t->clip_area),
        0);
    lv_vg_lite_path_end(path);

    vg_lite_matrix_t matrix = u->global_matrix;

    /* Use VG_LITE_BLEND_DST_IN (Sa * D) blending mode to make the corners transparent */
    lv_vg_lite_draw(
        &u->target_buffer,
        lv_vg_lite_path_get_path(path),
        VG_LITE_FILL_EVEN_ODD,
        &matrix,
        VG_LITE_BLEND_DST_IN,
        0);

    lv_vg_lite_path_drop(u, path);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
