/**
 * @file lv_draw_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "lv_draw_arc.h"

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
    lv_memset_00(dsc, sizeof(lv_draw_arc_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
}

void lv_draw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center, uint16_t radius,
                 uint16_t start_angle, uint16_t end_angle)
{
    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(start_angle == end_angle) return;

    draw_ctx->draw_arc(draw_ctx, dsc, center, radius, start_angle, end_angle);

    //    const lv_draw_backend_t * backend = lv_draw_backend_get();
    //    backend->draw_arc(center_x, center_y, radius, start_angle, end_angle, clip_area, dsc);
}

void lv_draw_arc_get_area(lv_coord_t x, lv_coord_t y, uint16_t radius,  uint16_t start_angle, uint16_t end_angle,
                          lv_coord_t w, bool rounded, lv_area_t * area)
{
    lv_coord_t rout = radius;

    /*Special case: full arc invalidation */
    if(end_angle == start_angle + 360) {
        area->x1 = x - rout;
        area->y1 = y - rout;
        area->x2 = x + rout;
        area->y2 = y + rout;
        return;
    }

    if(start_angle > 360) start_angle -= 360;
    if(end_angle > 360) end_angle -= 360;

    lv_coord_t rin = radius - w;
    lv_coord_t extra_area = rounded ? w / 2 + 1 : 0;
    uint8_t start_quarter = start_angle / 90;
    uint8_t end_quarter = end_angle / 90;

    /*360 deg still counts as quarter 3 (360 / 90 would be 4)*/
    if(start_quarter == 4) start_quarter = 3;
    if(end_quarter == 4) end_quarter = 3;

    if(start_quarter == end_quarter && start_angle <= end_angle) {
        if(start_quarter == 0) {
            area->y1 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;

            area->y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->x1 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        }
        else if(start_quarter == 1) {
            area->y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->x2 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        }
        else if(start_quarter == 2) {
            area->x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            area->y2 = y + ((lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;

            area->y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
            area->x2 = x + ((lv_trigo_sin(end_angle + 90) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        }
        else if(start_quarter == 3) {
            area->x1 = x + ((lv_trigo_sin(start_angle + 90) * rin) >> LV_TRIGO_SHIFT) - extra_area;
            area->y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;

            area->x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
            area->y2 = y + ((lv_trigo_sin(end_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        }
    }
    else if(start_quarter == 0 && end_quarter == 1) {
        area->x1 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((LV_MIN(lv_trigo_sin(end_angle),
                                lv_trigo_sin(start_angle))  * rin) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + rout + extra_area;
    }
    else if(start_quarter == 1 && end_quarter == 2) {
        area->x1 = x - rout - extra_area;
        area->y1 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + ((LV_MAX(lv_trigo_sin(start_angle + 90),
                                lv_trigo_sin(end_angle + 90)) * rin) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;
    }
    else if(start_quarter == 2 && end_quarter == 3) {
        area->x1 = x + ((lv_trigo_sin(start_angle + 90) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y - rout - extra_area;
        area->x2 = x + ((lv_trigo_sin(end_angle + 90) * rout) >> LV_TRIGO_SHIFT) + extra_area;
        area->y2 = y + (LV_MAX(lv_trigo_sin(end_angle) * rin,
                               lv_trigo_sin(start_angle) * rin) >> LV_TRIGO_SHIFT) + extra_area;
    }
    else if(start_quarter == 3 && end_quarter == 0) {
        area->x1 = x + ((LV_MIN(lv_trigo_sin(end_angle + 90),
                                lv_trigo_sin(start_angle + 90)) * rin) >> LV_TRIGO_SHIFT) - extra_area;
        area->y1 = y + ((lv_trigo_sin(start_angle) * rout) >> LV_TRIGO_SHIFT) - extra_area;
        area->x2 = x + rout + extra_area;
        area->y2 = y + ((lv_trigo_sin(end_angle) * rout) >> LV_TRIGO_SHIFT) + extra_area;

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
