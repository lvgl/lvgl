/**
 * @file lv_draw_eve.c
 *
 */

/*  Created on: 3 dic 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"
#if LV_USE_DRAW_EVE

#include "../../display/lv_display_private.h"
#include "lv_draw_eve_ram_g.h"
#include "lv_draw_eve.h"
#include "lv_eve.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_EVE 9

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void eve_execute_drawing(lv_draw_eve_unit_t * u);

static int32_t eve_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static int32_t eve_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

static void disp_delete_cb(lv_event_t * e);

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = lv_draw_eve_fill,
    .draw_border_cb = lv_draw_eve_border,
    .draw_label_cb = lv_draw_eve_label,
    .draw_image_cb = lv_draw_eve_image,
    .draw_arc_cb = lv_draw_eve_arc,
    .draw_line_cb = draw_line,
    .draw_triangle_cb = lv_draw_eve_triangle,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve_init(void)
{
    lv_draw_eve_unit_t * draw_eve_unit = lv_draw_create_unit(sizeof(lv_draw_eve_unit_t));
    draw_eve_unit->base_unit.dispatch_cb = eve_dispatch;
    draw_eve_unit->base_unit.evaluate_cb = eve_evaluate;
    draw_eve_unit->base_unit.draw_cbs = &draw_cbs;

    lv_draw_eve_unit_g = draw_eve_unit;
}

void lv_draw_eve_set_display_data(lv_display_t * disp, const lv_draw_eve_parameters_t * params,
                                  lv_draw_eve_operation_cb_t op_cb)
{
    if(lv_draw_eve_unit_g == NULL) {
        LV_LOG_WARN("lv_draw_eve is not initialized.");
        return;
    }

    lv_draw_eve_unit_g->disp = disp;
    lv_draw_eve_unit_g->params = *params; /* make a copy */
    lv_draw_eve_unit_g->op_cb = op_cb;

    lv_display_add_event_cb(disp, disp_delete_cb, LV_EVENT_DELETE, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t eve_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_eve_unit_t * draw_eve_unit = (lv_draw_eve_unit_t *) draw_unit;

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_EVE);
    if(t == NULL) return LV_DRAW_UNIT_IDLE;


    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_eve_unit->task_act = t;

    eve_execute_drawing(draw_eve_unit);

    draw_eve_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    draw_eve_unit->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();


    return 1;
}

static int32_t eve_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    /*EVE has no frame buffer that another unit could draw into,
     *so keep the other units from taking the tasks EVE can't draw: they are dropped*/
    task->preference_score = 0;
    task->preferred_draw_unit_id = LV_DRAW_UNIT_NONE;

    if(lv_draw_eve_unit_g == NULL || lv_draw_eve_unit_g->disp == NULL)
        return 0;

    if(((lv_draw_dsc_t *)task->draw_dsc)->user_data != NULL)
        return 0;

    switch(task->type) {
        case LV_DRAW_DSC_TYPE_FILL:
        case LV_DRAW_DSC_TYPE_BORDER:
        case LV_DRAW_DSC_TYPE_LABEL:
        case LV_DRAW_DSC_TYPE_IMAGE:
        case LV_DRAW_DSC_TYPE_ARC:
        case LV_DRAW_DSC_TYPE_LINE:
        case LV_DRAW_DSC_TYPE_TRIANGLE:
            task->preferred_draw_unit_id = DRAW_UNIT_ID_EVE;
            break;
        default:
            break;
    }
    return 0;
}

static void eve_execute_drawing(lv_draw_eve_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;

    lv_draw_unit_draw_task(&u->base_unit, t);
}

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    lv_draw_line_iterate(layer, clip_area, dsc, lv_draw_eve_line, NULL);
}

static void disp_delete_cb(lv_event_t * e)
{
    lv_draw_eve_unit_g->disp = NULL;
    lv_draw_eve_unit_g = NULL;
}


#endif /*LV_USE_DRAW_EVE*/
