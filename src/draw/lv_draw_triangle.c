/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "lv_draw_triangle.h"
#include "../misc/lv_math.h"
#include "../stdlib/lv_mem.h"
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

void lv_draw_triangle_dsc_init(lv_draw_triangle_dsc_t * dsc)
{
    LV_PROFILER_BEGIN;
    lv_memzero(dsc, sizeof(lv_draw_triangle_dsc_t));
    dsc->bg_color = lv_color_white();
    dsc->bg_grad.stops[0].color = lv_color_white();
    dsc->bg_grad.stops[1].color = lv_color_black();
    dsc->bg_grad.stops[1].frac = 0xFF;
    dsc->bg_grad.stops_count = 2;
    dsc->bg_opa = LV_OPA_COVER;
    LV_PROFILER_END;
}

void lv_draw_triangle(lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc)
{
    LV_PROFILER_BEGIN;
    lv_area_t a;
    a.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    a.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    a.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    a.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_draw_task_t * t = lv_draw_add_task(layer, &a);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_TRIANGLE;

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
