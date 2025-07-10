/**
 * @file lv_draw_m2d.c
 *
 */

/*********************
*      INCLUDES
*********************/

#include "lv_draw_m2d_private.h"
#include "lv_draw_m2d.h"

#if LV_USE_DRAW_M2D

#include "../../misc/lv_area_private.h"
#include <m2d/m2d.h>

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_M2D         9
#define DRAW_UNIT_M2D_PREF_SCORE 70

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t m2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t m2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static void m2d_execute_drawing(lv_draw_task_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_m2d_init(void)
{
    lv_draw_buf_m2d_init_handlers();

    lv_draw_m2d_unit_t * draw_m2d_unit = lv_draw_create_unit(sizeof(lv_draw_m2d_unit_t));
    draw_m2d_unit->base_unit.evaluate_cb = m2d_evaluate;
    draw_m2d_unit->base_unit.dispatch_cb = m2d_dispatch;
    draw_m2d_unit->base_unit.name = "M2D";
    lv_draw_m2d_init_buf_map();

    if(m2d_init()) {
        LV_LOG_ERROR("m2d_init() failed");
        return;
    }

    m2d_blend_functions(M2D_FUNC_ADD, M2D_FUNC_ADD);
}

void lv_draw_m2d_deinit(void)
{
    lv_draw_m2d_cleanup_buf_map();
    m2d_cleanup();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t m2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    bool claim = false;
    switch(task->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            claim = lv_draw_m2d_fill_is_supported(task, task->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            claim = lv_draw_m2d_image_is_supported(task, task->draw_dsc);
            break;

        case LV_DRAW_TASK_TYPE_LAYER:
            claim = lv_draw_m2d_layer_is_supported(task, task->draw_dsc);
            break;

        default:
            break;
    }

    if(!claim) {
        lv_draw_m2d_buf_t * target = (lv_draw_m2d_buf_t *)task->target_layer->draw_buf;

        if(target) lv_draw_m2d_buf_sync_for_cpu(target);
        return 0;
    }

    if(task->preference_score > DRAW_UNIT_M2D_PREF_SCORE) {
        task->preference_score = DRAW_UNIT_M2D_PREF_SCORE;
        task->preferred_draw_unit_id = DRAW_UNIT_ID_M2D;
    }

    return 0;
}

static int32_t m2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_m2d_unit_t * draw_m2d_unit = (lv_draw_m2d_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task */
    if(draw_m2d_unit->task_act) {
        LV_PROFILER_DRAW_END;
        return 0;
    }

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_M2D);
    if(t == NULL) {
        LV_PROFILER_DRAW_END;
        return LV_DRAW_UNIT_IDLE;  /* Couldn't start rendering */
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        LV_PROFILER_DRAW_END;
        return LV_DRAW_UNIT_IDLE;  /* Couldn't start rendering */
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_m2d_unit->task_act = t;

    m2d_execute_drawing(t);
    draw_m2d_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    draw_m2d_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task */
    lv_draw_dispatch_request();

    LV_PROFILER_DRAW_END;
    return 1;
}

static void m2d_execute_drawing(lv_draw_task_t * t)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_layer_t * layer = t->target_layer;
    lv_area_t area;

    if(!lv_area_intersect(&area, &t->area, &t->clip_area)) return;
    lv_area_move(&area, -layer->buf_area.x1, -layer->buf_area.y1);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_m2d_fill(t, t->draw_dsc, &area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_m2d_image(t, t->draw_dsc, &area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_m2d_layer(t, t->draw_dsc, &area);
            break;
        default:
            break;
    }

    LV_PROFILER_DRAW_END;
}

#endif /*LV_USE_DRAW_M2D*/
