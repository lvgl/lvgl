/**
 * @file lv_draw_vg_lite_box_shadow.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_area_private.h"
#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"

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

void lv_draw_vg_lite_box_shadow(lv_draw_unit_t * draw_unit, const lv_draw_box_shadow_dsc_t * dsc,
                                const lv_area_t * coords)
{
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
    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &shadow_area, draw_unit->clip_area)) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_border_dsc_t border_dsc;
    lv_draw_border_dsc_init(&border_dsc);
    border_dsc.width = 3;
    border_dsc.color = dsc->color;
    border_dsc.radius = dsc->radius;

    lv_area_move(&draw_area, dsc->ofs_x, dsc->ofs_y);
    draw_area = core_area;
    int32_t half_w = dsc->width / 2;

    for(int32_t w = 0; w < half_w; w++) {
        border_dsc.opa = lv_map(w, 0, half_w, dsc->opa / 4, LV_OPA_0);
        border_dsc.radius++;
        lv_area_increase(&draw_area, 1, 1);
        lv_draw_vg_lite_border(draw_unit, &border_dsc, &draw_area);

        /* fill center */
        if(dsc->ofs_x || dsc->ofs_y) {
            lv_draw_fill_dsc_t fill_dsc;
            lv_draw_fill_dsc_init(&fill_dsc);
            fill_dsc.radius = dsc->radius;
            fill_dsc.opa = dsc->opa;
            fill_dsc.color = dsc->color;
            lv_draw_vg_lite_fill(draw_unit, &fill_dsc, &core_area);
        }
    }
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
