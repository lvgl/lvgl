/**
 * @file lv_draw_mask.c
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

void LV_ATTRIBUTE_FAST_MEM lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc)
{
    LV_CHECK_ARG(dsc != NULL, return);

    lv_memzero(dsc, sizeof(lv_draw_mask_rect_dsc_t));
    dsc->base.dsc_size = sizeof(lv_draw_mask_rect_dsc_t);
}

lv_draw_mask_rect_dsc_t * lv_draw_task_get_mask_rect_dsc(lv_draw_task_t * task)
{
    LV_CHECK_ARG(task != NULL, return NULL);

    return task->type == LV_DRAW_DSC_TYPE_MASK_RECTANGLE ? (lv_draw_mask_rect_dsc_t *)task->draw_dsc : NULL;
}

lv_draw_mask_rect_dsc_t * lv_draw_dsc_get_mask_rect_dsc(lv_draw_dsc_t * dsc)
{
    LV_CHECK_ARG(dsc != NULL, return NULL);
    LV_CHECK_ARG(dsc->type == LV_DRAW_DSC_TYPE_MASK_RECTANGLE, return NULL);

    return (lv_draw_mask_rect_dsc_t *)dsc;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_mask_rect(lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc)
{
    LV_CHECK_ARG(layer != NULL, return);
    LV_CHECK_ARG(dsc != NULL, return);

    if(!lv_color_format_has_alpha(layer->color_format)) {
        LV_LOG_WARN("Only layers with alpha channel can be masked");
        return;
    }
    LV_PROFILER_DRAW_BEGIN;

    lv_draw_task_t * t = lv_draw_add_task(layer, &layer->buf_area, LV_DRAW_DSC_TYPE_MASK_RECTANGLE);
    if(!t) {
        LV_LOG_WARN("Failed to create draw task");
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
