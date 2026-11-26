/**
 * @file lv_draw_blur.c
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

void LV_ATTRIBUTE_FAST_MEM lv_draw_blur_dsc_init(lv_draw_blur_dsc_t * dsc)
{
    LV_CHECK_ARG(dsc != NULL, return);

    lv_memzero(dsc, sizeof(lv_draw_blur_dsc_t));
    dsc->base.dsc_size = sizeof(lv_draw_blur_dsc_t);
}

lv_draw_blur_dsc_t * lv_draw_task_get_blur_dsc(lv_draw_task_t * task)
{
    LV_CHECK_ARG(task != NULL, return NULL);

    return task->type == LV_DRAW_DSC_TYPE_BLUR ? (lv_draw_blur_dsc_t *)task->draw_dsc : NULL;
}

lv_draw_blur_dsc_t * lv_draw_dsc_get_blur_dsc(lv_draw_dsc_t * dsc)
{
    LV_CHECK_ARG(dsc != NULL, return NULL);
    LV_CHECK_ARG(dsc->type == LV_DRAW_DSC_TYPE_BLUR, return NULL);

    return (lv_draw_blur_dsc_t *)dsc;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_blur(lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    LV_CHECK_ARG(layer != NULL, return);
    LV_CHECK_ARG(dsc != NULL, return);
    LV_CHECK_ARG(coords != NULL, return);

    if(dsc->blur_radius <= 0) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_blur_dsc_t local = *dsc;
    lv_draw_dsc_submit(&local.base, LV_DRAW_DSC_TYPE_BLUR, layer, coords);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
