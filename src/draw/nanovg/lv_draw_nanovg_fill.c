/**
 * @file lv_draw_nanovg_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_draw_nanovg_utils.h"

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

void lv_draw_nanovg_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_nanovg_transform(u->vg, &u->global_matrix);

    // nvgSave(u->vg);

    nvgBeginPath(u->vg);

    lv_nanovg_path_append_rect(u->vg,
                               coords->x1, coords->y1,
                               lv_area_get_width(coords), lv_area_get_height(coords),
                               dsc->radius);

    if(0 && dsc->grad.dir != LV_GRAD_DIR_NONE) {

    }
    else {
        nvgPathWinding(u->vg, NVG_CCW);
        nvgFillColor(u->vg, nvgRGBA(dsc->color.red, dsc->color.green, dsc->color.blue, dsc->opa));
        nvgFill(u->vg);
    }

    // nvgRestore(u->vg);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/
