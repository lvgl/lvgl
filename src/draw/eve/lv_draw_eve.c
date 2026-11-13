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

static bool eve_takes(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_dsc_t * dsc);
static bool eve_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool eve_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool eve_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool eve_can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool eve_can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);
static bool eve_can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
static bool eve_can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc);

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
    .can_draw_fill_cb = eve_can_draw_fill,
    .draw_border_cb = lv_draw_eve_border,
    .can_draw_border_cb = eve_can_draw_border,
    .draw_label_cb = lv_draw_eve_label,
    .can_draw_label_cb = eve_can_draw_label,
    .draw_image_cb = lv_draw_eve_image,
    .can_draw_image_cb = eve_can_draw_image,
    .draw_arc_cb = lv_draw_eve_arc,
    .can_draw_arc_cb = eve_can_draw_arc,
    .draw_line_cb = draw_line,
    .can_draw_line_cb = eve_can_draw_line,
    .draw_triangle_cb = lv_draw_eve_triangle,
    .can_draw_triangle_cb = eve_can_draw_triangle,
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
    draw_eve_unit->base_unit.draw_cbs = &draw_cbs;
    draw_eve_unit->base_unit.id = DRAW_UNIT_ID_EVE;
    /*EVE has no frame buffer that another unit could draw into*/
    draw_eve_unit->base_unit.fallback = NULL;

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

static bool eve_takes(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);

    if(lv_draw_eve_unit_g == NULL || lv_draw_eve_unit_g->disp == NULL)
        return false;

    return dsc->user_data == NULL;
}

static bool eve_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
}

static bool eve_can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
}

static bool eve_can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
}

static bool eve_can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
}

static bool eve_can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
}

static bool eve_can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
}

static bool eve_can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc)
{
    return eve_takes(draw_unit, layer, &dsc->base);
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
