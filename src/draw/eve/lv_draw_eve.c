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

#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE

#include "../../core/lv_refr.h"
#include "../../display/lv_display_private.h"
#include "../../stdlib/lv_string.h"
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

/**********************
 *  GLOBAL PROTOTYPES
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

void lv_draw_eve_init(void)
{
    lv_draw_eve_unit_t * draw_eve_unit = lv_draw_create_unit(sizeof(lv_draw_eve_unit_t));
    draw_eve_unit->base_unit.dispatch_cb = eve_dispatch;
    draw_eve_unit->base_unit.evaluate_cb = eve_evaluate;

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

    if(((lv_draw_dsc_base_t *)task->draw_dsc)->user_data == NULL) {
        task->preference_score = 0;
        task->preferred_draw_unit_id = DRAW_UNIT_ID_EVE;
    }
    return 0;
}

static void eve_execute_drawing(lv_draw_eve_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;

    uint8_t coprocessor_status;
    do {
        coprocessor_status = EVE_busy();
    } while(coprocessor_status != E_OK && coprocessor_status != EVE_FIFO_HALF_EMPTY);

    EVE_start_cmd_burst();

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_eve_line(t, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_eve_border(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_eve_fill(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_eve_image(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_eve_label(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_eve_arc(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_eve_triangle(t, t->draw_dsc);
            break;
        default:
            break;
    }

    EVE_end_cmd_burst();
}

static void disp_delete_cb(lv_event_t * e)
{
    lv_draw_eve_unit_g->disp = NULL;
    lv_draw_eve_unit_g = NULL;
}


#endif /*LV_USE_DRAW_EVE*/
