/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../core/lv_refr.h"
#include "../misc/lv_math.h"

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

LV_ATTRIBUTE_FAST_MEM void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_line_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
}

LV_ATTRIBUTE_FAST_MEM void lv_draw_line(struct _lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    lv_area_t a;
    a.x1 = LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width;
    a.x2 = LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width;
    a.y1 = LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width;
    a.y2 = LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width;

    lv_draw_task_t * t = lv_draw_add_task(layer, &a);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_LINE;

    lv_draw_dsc_base_t * base_dsc = t->draw_dsc;
    base_dsc->layer = layer;

    if(base_dsc->obj && lv_obj_has_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS)) {
        /*Disable sending LV_EVENT_DRAW_TASK_ADDED first to avoid triggering recursive
         *event calls due draw task adds in the event*/
        lv_obj_clear_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
        lv_obj_send_event(dsc->base.obj, LV_EVENT_DRAW_TASK_ADDED, t);
        lv_obj_add_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    }

    lv_draw_dispatch();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
