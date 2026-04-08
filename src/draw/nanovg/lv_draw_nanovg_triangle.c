/**
 * @file lv_draw_nanovg_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

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

void lv_draw_nanovg_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &tri_area, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    nvgBeginPath(u->vg);
    nvgMoveTo(u->vg, dsc->p[0].x, dsc->p[0].y);
    nvgLineTo(u->vg, dsc->p[1].x, dsc->p[1].y);
    nvgLineTo(u->vg, dsc->p[2].x, dsc->p[2].y);
    nvgClosePath(u->vg);

    if(dsc->grad.dir != LV_GRAD_DIR_NONE) {
#if LV_USE_VECTOR_GRAPHIC
        lv_nanovg_draw_grad_helper(u->vg, &tri_area, &dsc->grad, NVG_CCW, NVG_SOURCE_OVER);
#else
        LV_LOG_WARN("Gradient fill is not supported without VECTOR_GRAPHIC");
#endif
    }
    else { /* normal fill */
        lv_nanovg_fill(
            u->vg,
            NVG_CCW,
            NVG_SOURCE_OVER,
            lv_nanovg_color_convert(dsc->color, dsc->opa));
    }

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
