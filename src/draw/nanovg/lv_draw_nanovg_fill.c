/**
 * @file lv_draw_nanovg_fill.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

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
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    float xform[6];
    lv_matrix_to_nvg_transform(xform, &u->global_matrix);
    nvgResetTransform(u->vg);
    nvgTransform(u->vg, xform[0], xform[1], xform[2], xform[3], xform[4], xform[5]);

    // nvgSave(u->vg);

    nvgBeginPath(u->vg);

    const int32_t w = lv_area_get_width(coords);
    const int32_t h = lv_area_get_height(coords);

    if(dsc->radius > 0) {
        const float half_w = w / 2.0f;
        const float half_h = h / 2.0f;

        /*clamping cornerRadius by minimum size*/
        const float r_max = LV_MIN(half_w, half_h);

        nvgRoundedRect(u->vg, coords->x1, coords->y1, w, h, dsc->radius > r_max ? r_max : dsc->radius);
    }
    else {
        nvgRect(u->vg, coords->x1, coords->y1, w, h);
    }

    if(0 && dsc->grad.dir != LV_GRAD_DIR_NONE) {

    }
    else {
        // nvgPathWinding(u->vg, NVG_CCW);
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
