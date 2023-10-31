/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "lv_draw_arc.h"
#include "../core/lv_obj_event.h"
#include "../stdlib/lv_string.h"

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

void lv_draw_arc_dsc_init(lv_draw_arc_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_arc_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
}

void lv_draw_arc(lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->start_angle == dsc->end_angle) return;

    LV_PROFILER_BEGIN;
    lv_area_t a;
    a.x1 = dsc->center.x - dsc->radius;
    a.y1 = dsc->center.y - dsc->radius;
    a.x2 = dsc->center.x + dsc->radius - 1;
    a.y2 = dsc->center.y + dsc->radius - 1;
    lv_draw_task_t * t = lv_draw_add_task(layer, &a);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_ARC;

    lv_draw_finalize_task_creation(layer, t);

    LV_PROFILER_END;
}

void lv_draw_arc_get_area(int32_t x, int32_t y, uint16_t radius,  lv_value_precise_t start_angle,
                          lv_value_precise_t end_angle,
                          int32_t w, bool rounded, lv_area_t * area)
{
    int32_t rout = radius;
    int32_t start_angle_int = (int32_t) start_angle;
    int32_t end_angle_int = (int32_t) end_angle;

    /*Special case: full arc invalidation */
    if(end_angle_int == start_angle_int + 360) {
        area->x1 = x - rout;
        area->y1 = y - rout;
        area->x2 = x + rout;
        area->y2 = y + rout;
        return;
    }

    if(start_angle_int > 360) start_angle_int -= 360;
    if(end_angle_int > 360) end_angle_int -= 360;

    int32_t rin = radius - w;
    int32_t extra_area = rounded ? w / 2 + 1 : 0;
    uint8_t start_quarter = start_angle_int / 90;
    uint8_t end_quarter = end_angle_int / 90;

    /*360 deg still counts as quarter 3 (360 / 90 would be 4)*/
    if(start_quarter == 4) start_quarter = 3;
    if(end_quarter == 4) end_quarter = 3;

    if(start_quarter == end_quarter && start_angle_int <= end_angle_int) {
        if(start_quarter == 0) {
            area->y1 = y + ((lv_trigo_sin(start_angle_int) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lv_trigo_sin(start_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;

            area->y2 = y + ((lv_trigo_sin(end_angle_int) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->x1 = x + ((lv_trigo_sin(end_angle_int + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        }
        else if(start_quarter == 1) {
            area->y2 = y + ((lv_trigo_sin(start_angle_int) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->x2 = x + ((lv_trigo_sin(start_angle_int + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lv_trigo_sin(end_angle_int) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->x1 = x + ((lv_trigo_sin(end_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        }
        else if(start_quarter == 2) {
            area->x1 = x + ((lv_trigo_sin(start_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            area->y2 = y + ((lv_trigo_sin(start_angle_int) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lv_trigo_sin(end_angle_int) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lv_trigo_sin(end_angle_int + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        }
        else if(start_quarter == 3) {
            area->x1 = x + ((lv_trigo_sin(start_angle_int + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->y1 = y + ((lv_trigo_sin(start_angle_int) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            area->x2 = x + ((lv_trigo_sin(end_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->y2 = y + ((lv_trigo_sin(end_angle_int) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        }
    }
    else if(start_quarter == 0 && end_quarter == 1) {
        area->x1 = x + ((lv_trigo_sin(end_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((LV_MIN(lv_trigo_sin(end_angle_int),
                                lv_trigo_sin(start_angle_int))  * rin) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((lv_trigo_sin(start_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + rout + extra_area;
    }
    else if(start_quarter == 1 && end_quarter == 2) {
        area->x1 = x - rout - extra_area;
        area->y1 = y + ((lv_trigo_sin(end_angle_int) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((LV_MAX(lv_trigo_sin(start_angle_int + 90),
                                lv_trigo_sin(end_angle_int + 90)) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + ((lv_trigo_sin(start_angle_int) * rout) >> LV_TRIGO_SHIFT) + extra_area;
    }
    else if(start_quarter == 2 && end_quarter == 3) {
        area->x1 = x + ((lv_trigo_sin(start_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y - rout - extra_area;
        area->x2 = x + ((lv_trigo_sin(end_angle_int + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + (LV_MAX(lv_trigo_sin(end_angle_int) * rin,
                               lv_trigo_sin(start_angle_int) * rin) >> LV_TRIGO_SHIFT) + extra_area;
    }
    else if(start_quarter == 3 && end_quarter == 0) {
        area->x1 = x + ((LV_MIN(lv_trigo_sin(end_angle_int + 90),
                                lv_trigo_sin(start_angle_int + 90)) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((lv_trigo_sin(start_angle_int) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + rout + extra_area;
        area->y2 = y + ((lv_trigo_sin(end_angle_int) * rout) >> LV_TRIGO_SHIFT) + extra_area;

    }
    else {
        area->x1 = x - rout;
        area->y1 = y - rout;
        area->x2 = x + rout;
        area->y2 = y + rout;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
