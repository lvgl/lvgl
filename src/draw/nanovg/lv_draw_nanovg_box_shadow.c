/**
 * @file lv_draw_nanovg_box_shadow.c
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

void lv_draw_nanovg_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                               const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_UNUSED(layer);

    /*Calculate the rectangle which is blurred to get the shadow in `shadow_area`*/
    lv_area_t core_area;
    core_area.x1 = coords->x1  + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2  + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1  + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2  + dsc->ofs_y + dsc->spread;

    /*Calculate the bounding box of the shadow*/
    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - dsc->width / 2 - 1;
    shadow_area.x2 = core_area.x2 + dsc->width / 2 + 1;
    shadow_area.y1 = core_area.y1 - dsc->width / 2 - 1;
    shadow_area.y2 = core_area.y2 + dsc->width / 2 + 1;

    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `shadow_area`*/
    lv_area_t clipped_area;
    if(!lv_area_intersect(&clipped_area, &shadow_area, clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)draw_unit;

    const NVGcolor icol = lv_nanovg_color_convert(dsc->color, dsc->opa);
    const NVGcolor ocol = lv_nanovg_color_convert(lv_color_black(), 0);

    const int32_t core_w = lv_area_get_width(&core_area);
    const int32_t core_h = lv_area_get_height(&core_area);
    const int32_t shadow_w = lv_area_get_width(&shadow_area);
    const int32_t shadow_h = lv_area_get_height(&shadow_area);

    NVGpaint paint = nvgBoxGradient(
                         u->vg,
                         core_area.x1, core_area.y1,
                         core_w, core_h,
                         dsc->radius, dsc->width, icol, ocol);

    nvgBeginPath(u->vg);
    if(dsc->bg_cover) {
        lv_nanovg_path_append_rect(u->vg, shadow_area.x1, shadow_area.y1, shadow_w, shadow_h, dsc->radius);
    }
    else {
        /*Don't draw the shadow under the object. Same 1 px smaller area as in the SW renderer.
         *The hole must be inside the outer path; the paint is transparent outside `shadow_area`.*/
        lv_area_t bg_area = *coords;
        lv_area_increase(&bg_area, -1, -1);
        lv_area_t outer_area;
        lv_area_join(&outer_area, &shadow_area, &bg_area);
        lv_nanovg_path_append_area(u->vg, &outer_area);
        lv_nanovg_path_append_rect(u->vg, bg_area.x1, bg_area.y1,
                                   lv_area_get_width(&bg_area), lv_area_get_height(&bg_area), dsc->radius);
        nvgPathWinding(u->vg, NVG_HOLE);
    }
    nvgFillPaint(u->vg, paint);
    nvgFill(u->vg);

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
