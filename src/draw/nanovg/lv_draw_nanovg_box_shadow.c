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

void lv_draw_nanovg_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

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
    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &shadow_area, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    const NVGcolor icol = lv_nanovg_color_convert(dsc->color, dsc->opa);
    const NVGcolor ocol = lv_nanovg_color_convert(lv_color_black(), 0);

    const int32_t core_w = lv_area_get_width(&core_area);
    const int32_t core_h = lv_area_get_height(&core_area);
    const int32_t shadow_w = lv_area_get_width(&shadow_area);
    const int32_t shadow_h = lv_area_get_height(&shadow_area);

    /*Get the clamped radius. `dsc->radius` can be `LV_RADIUS_CIRCLE` which
     *would degenerate the box gradient and make the shadow disappear.*/
    int32_t r_sh = dsc->radius;
    int32_t short_side = LV_MIN(core_w, core_h);
    if(r_sh > short_side >> 1) r_sh = short_side >> 1;

    /*Consider 1 px smaller bg to be sure the edge will be covered by the shadow*/
    lv_area_t bg_area = *coords;
    lv_area_increase(&bg_area, -1, -1);
    const int32_t bg_w = lv_area_get_width(&bg_area);
    const int32_t bg_h = lv_area_get_height(&bg_area);

    /*Get the clamped radius of the bg*/
    int32_t r_bg = dsc->radius;
    short_side = LV_MIN(bg_w, bg_h);
    if(r_bg > short_side >> 1) r_bg = short_side >> 1;

    NVGpaint paint = nvgBoxGradient(
                         u->vg,
                         core_area.x1, core_area.y1,
                         core_w, core_h,
                         r_sh, dsc->width, icol, ocol);

    nvgBeginPath(u->vg);
    lv_nanovg_path_append_rect(u->vg, shadow_area.x1, shadow_area.y1, shadow_w, shadow_h, r_sh);

    /*Remove the shadow from the bg area. Without this the shadow is also painted
     *behind the object and shows through if the bg is not fully opaque.*/
    if(bg_w > 0 && bg_h > 0) {
        lv_nanovg_path_append_rect(u->vg, bg_area.x1, bg_area.y1, bg_w, bg_h, r_bg);
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
