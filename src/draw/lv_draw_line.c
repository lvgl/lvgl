/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_private.h"
#include "../core/lv_refr.h"
#include "../misc/lv_math.h"
#include "../misc/lv_types.h"
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

void LV_ATTRIBUTE_FAST_MEM lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_line_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
    dsc->base.dsc_size = sizeof(lv_draw_line_dsc_t);
}

lv_draw_line_dsc_t * lv_draw_task_get_line_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_LINE ? (lv_draw_line_dsc_t *)task->draw_dsc : NULL;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_line(lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_area_t a;
    a.x1 = (int32_t)LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width;
    a.x2 = (int32_t)LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width;
    a.y1 = (int32_t)LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width;
    a.y2 = (int32_t)LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width;

    lv_draw_task_t * t = lv_draw_add_task(layer, &a, LV_DRAW_TASK_TYPE_LINE);

    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
