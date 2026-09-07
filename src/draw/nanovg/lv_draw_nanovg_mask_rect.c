/**
 * @file lv_draw_nanovg_mask_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "../../misc/lv_area_private.h"
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

void lv_draw_nanovg_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_area_t draw_area;

    if(!lv_area_intersect(&draw_area, &dsc->area, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_area_t outer_area = dsc->area;
    if(dsc->keep_outside) {
        lv_nanovg_set_clip_area(u->vg, &draw_area);
    }
    else {
        lv_area_join(&outer_area, &outer_area, &t->clip_area);
    }

    /* Handle anti-aliasing */
    lv_area_increase(&outer_area, 1, 1);

    nvgBeginPath(u->vg);

    /*The area to erase*/
    lv_nanovg_path_append_area(u->vg, &outer_area);

    /*The rounded rectangle to keep, as a hole*/
    lv_nanovg_path_append_rect(
        u->vg,
        dsc->area.x1, dsc->area.y1,
        lv_area_get_width(&dsc->area), lv_area_get_height(&dsc->area),
        dsc->radius);

    /*Use NVG_DESTINATION_OUT (D * (1 - Sa)) with an opaque source to clear the filled region. */
    lv_nanovg_fill(
        u->vg,
        NVG_CW,
        NVG_DESTINATION_OUT,
        nvgRGBA(0, 0, 0, 255));

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
