/**
 * @file lv_draw_nanovg_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_math.h"
#include "lv_nanovg_utils.h"

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

void lv_draw_nanovg_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    float start_angle = dsc->start_angle;
    float end_angle = dsc->end_angle;
    float sweep_angle = end_angle - start_angle;

    while(sweep_angle < 0) {
        sweep_angle += 360;
    }

    while(sweep_angle > 360) {
        sweep_angle -= 360;
    }

    /*If the angles are the same then there is nothing to draw*/
    if(math_zero(sweep_angle)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    if(dsc->img_src) {
        /* Not supported yet */
    }
    else {
        nvgBeginPath(u->vg);
        nvgArc(u->vg, dsc->center.x, dsc->center.y, dsc->radius, MATH_RADIANS(start_angle), MATH_RADIANS(end_angle), NVG_CW);
        nvgStrokeColor(u->vg, lv_nanovg_color_convert(dsc->color, dsc->opa));
        nvgStrokeWidth(u->vg, dsc->width);
        nvgLineCap(u->vg, dsc->rounded ? NVG_ROUND : NVG_BUTT);
        nvgStroke(u->vg);
    }

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
