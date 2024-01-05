/**
 * @file lv_draw_mask.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "lv_draw_mask.h"
#include "../core/lv_refr.h"
#include "../misc/lv_math.h"
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

LV_ATTRIBUTE_FAST_MEM void lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_mask_rect_dsc_t));
}

LV_ATTRIBUTE_FAST_MEM void lv_draw_mask_rect(lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc)
{
    if(!lv_color_format_has_alpha(layer->color_format)) {
        LV_LOG_WARN("Only layers with alpha channel can be masked");
        return;
    }
    LV_PROFILER_BEGIN;

    lv_draw_task_t * t = lv_draw_add_task(layer, &layer->buf_area);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_MASK_RECTANGLE;

    lv_draw_dsc_base_t * base_dsc = t->draw_dsc;
    base_dsc->layer = layer;

    if(base_dsc->obj && lv_obj_has_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS)) {
        /*Disable sending LV_EVENT_DRAW_TASK_ADDED first to avoid triggering recursive
         *event calls due draw task adds in the event*/
        lv_obj_remove_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
        lv_obj_send_event(dsc->base.obj, LV_EVENT_DRAW_TASK_ADDED, t);
        lv_obj_add_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    }

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
