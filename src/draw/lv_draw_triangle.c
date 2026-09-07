/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_private.h"

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

void lv_draw_triangle_dsc_init(lv_draw_triangle_dsc_t * draw_dsc)
{
    LV_CHECK_ARG(draw_dsc != NULL, return);

    LV_PROFILER_DRAW_BEGIN;
    lv_memzero(draw_dsc, sizeof(lv_draw_triangle_dsc_t));
    draw_dsc->color = lv_color_white();
    draw_dsc->grad.stops[0].color = lv_color_white();
    draw_dsc->grad.stops[1].color = lv_color_black();
    draw_dsc->grad.stops[1].frac = 0xFF;
    draw_dsc->grad.stops_count = 2;
    draw_dsc->opa = LV_OPA_COVER;
    draw_dsc->base.dsc_size = sizeof(lv_draw_triangle_dsc_t);
    LV_PROFILER_DRAW_END;
}

lv_draw_triangle_dsc_t * lv_draw_task_get_triangle_dsc(lv_draw_task_t * task)
{
    LV_CHECK_ARG(task != NULL, return NULL);

    return task->type == LV_DRAW_TASK_TYPE_TRIANGLE ? (lv_draw_triangle_dsc_t *)task->draw_dsc : NULL;
}

void lv_draw_triangle(lv_layer_t * layer, const lv_draw_triangle_dsc_t * draw_dsc)
{
    LV_CHECK_ARG(layer != NULL, return);
    LV_CHECK_ARG(draw_dsc != NULL, return);

    if(draw_dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_area_t a;
    a.x1 = (int32_t)LV_MIN3(draw_dsc->p[0].x, draw_dsc->p[1].x, draw_dsc->p[2].x);
    a.y1 = (int32_t)LV_MIN3(draw_dsc->p[0].y, draw_dsc->p[1].y, draw_dsc->p[2].y);
    a.x2 = (int32_t)LV_MAX3(draw_dsc->p[0].x, draw_dsc->p[1].x, draw_dsc->p[2].x);
    a.y2 = (int32_t)LV_MAX3(draw_dsc->p[0].y, draw_dsc->p[1].y, draw_dsc->p[2].y);

    if(draw_dsc->base.drop_shadow_opa) {
        lv_layer_t * ds_layer = lv_draw_layer_create_drop_shadow(layer, &draw_dsc->base, &a);
        LV_ASSERT_NULL(ds_layer);
        lv_draw_triangle_dsc_t ds_dsc = *draw_dsc;
        ds_dsc.base.drop_shadow_opa = 0; /*Disable drop shadow so rendering below will render plain triangle*/
        lv_draw_triangle(ds_layer, &ds_dsc);
        lv_draw_layer_finish_drop_shadow(ds_layer, &draw_dsc->base);
    }


    lv_draw_task_t * t = lv_draw_add_task(layer, &a, LV_DRAW_TASK_TYPE_TRIANGLE);

    lv_memcpy(t->draw_dsc, draw_dsc, sizeof(*draw_dsc));

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
