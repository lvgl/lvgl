/**
 * @file lv_draw_blur.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_private.h"
#include "lv_draw_blur.h"
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

void LV_ATTRIBUTE_FAST_MEM lv_draw_blur_dsc_init(lv_draw_blur_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_blur_dsc_t));
    dsc->base.dsc_size = sizeof(lv_draw_blur_dsc_t);
}

lv_draw_blur_dsc_t * lv_draw_task_get_blur_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_BLUR ? (lv_draw_blur_dsc_t *)task->draw_dsc : NULL;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_blur(lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->blur_radius <= 0) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_task_t * t = lv_draw_add_task(layer, coords, LV_DRAW_TASK_TYPE_BLUR);

    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
