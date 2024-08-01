/**
 * @file lv_draw_g2d.c
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_g2d.h"

#if LV_USE_DRAW_G2D
#include "../../../misc/lv_area_private.h"
#include "g2d.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_G2D 8

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Evaluate a task and set the score and preferred G2D unit.
 * Return 1 if task is preferred, 0 otherwise (task is not supported).
 */
static int32_t _g2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

/**
 * Dispatch a task to the G2D unit.
 * Return 1 if task was dispatched, 0 otherwise (task not supported).
 */
static int32_t _g2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

/**
 * Delete the G2D draw unit.
 */
static int32_t _g2d_delete(lv_draw_unit_t * draw_unit);

#if LV_USE_G2D_DRAW_THREAD
    static void _g2d_render_thread_cb(void * ptr);
#endif

static void _g2d_execute_drawing(lv_draw_g2d_unit_t * u);

/**********************
 *  STATIC VARIABLES
 **********************/

void * g2d_handle = NULL;

extern struct g2d_buf * ctx_buf1;
extern struct g2d_buf * ctx_buf2;
extern struct g2d_buf * temp_buf;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_init(void)
{
    lv_draw_g2d_unit_t * draw_g2d_unit = lv_draw_create_unit(sizeof(lv_draw_g2d_unit_t));
    draw_g2d_unit->base_unit.evaluate_cb = _g2d_evaluate;
    draw_g2d_unit->base_unit.dispatch_cb = _g2d_dispatch;
    draw_g2d_unit->base_unit.delete_cb = _g2d_delete;
    if(g2d_open(&g2d_handle)) {
        LV_LOG_ERROR("g2d_open fail.\n");
    }

#if LV_USE_G2D_DRAW_THREAD
    lv_thread_init(&draw_g2d_unit->thread, "g2ddraw", LV_THREAD_PRIO_HIGH, _g2d_render_thread_cb, 2 * 1024, draw_g2d_unit);
#endif
}

void lv_draw_g2d_deinit(void)
{
    g2d_free(temp_buf);
    g2d_free(ctx_buf1);
    g2d_free(ctx_buf2);
    g2d_close(g2d_handle);

    temp_buf = 0;
    ctx_buf1 = 0;
    ctx_buf2 = 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool _g2d_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static int32_t _g2d_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);

    const lv_draw_dsc_base_t * draw_dsc_base = (lv_draw_dsc_base_t *) t->draw_dsc;

    if(!_g2d_dest_cf_supported(draw_dsc_base->layer->color_format))
        return 0;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * draw_dsc = (lv_draw_fill_dsc_t *) t->draw_dsc;

                /* Most simple case: just a plain rectangle (no radius, no gradient). */
                if((draw_dsc->radius != 0) || (draw_dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))
                    return 0;

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_G2D;
                }
                return 1;
            }
        default:
            return 0;
    }

    return 0;

}

static int32_t _g2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_g2d_unit_t * draw_g2d_unit = (lv_draw_g2d_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task. */
    if(draw_g2d_unit->task_act)
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_G2D);

    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_G2D)
        return LV_DRAW_UNIT_IDLE;

    if(lv_draw_layer_alloc_buf(layer) == NULL)
        return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_g2d_unit->task_act = t;

#if LV_USE_G2D_DRAW_THREAD
    /* Let the render thread work. */
    if(draw_g2d_unit->inited)
        lv_thread_sync_signal(&draw_g2d_unit->sync);
#else
    _g2d_execute_drawing(draw_g2d_unit);

    draw_g2d_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_g2d_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task. */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static int32_t _g2d_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_G2D_DRAW_THREAD
    lv_draw_g2d_unit_t * draw_g2d_unit = (lv_draw_g2d_unit_t *) draw_unit;

    LV_LOG_INFO("Cancel G2D draw thread.");
    draw_g2d_unit->exit_status = true;

    if(draw_g2d_unit->inited)
        lv_thread_sync_signal(&draw_g2d_unit->sync);

    lv_result_t res = lv_thread_delete(&draw_g2d_unit->thread);

    return res;
#else
    LV_UNUSED(draw_unit);

    return 0;
#endif
}

static void _g2d_execute_drawing(lv_draw_g2d_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_draw_unit_t * draw_unit = (lv_draw_unit_t *)u;
    lv_layer_t * layer = draw_unit->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    t->draw_unit = (lv_draw_unit_t *)u;

    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &t->area, &t->clip_area))
        return; /*Fully clipped, nothing to do*/

    /* Make area relative to the buffer */
    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);

    /* Invalidate only the drawing area */
    lv_draw_buf_invalidate_cache(draw_buf, &draw_area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_g2d_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }

    /* Invalidate only the drawing area */
    lv_draw_buf_invalidate_cache(draw_buf, &draw_area);
}

#if LV_USE_G2D_DRAW_THREAD
static void _g2d_render_thread_cb(void * ptr)
{
    lv_draw_g2d_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);
    u->inited = true;

    while(1) {
        /* Wait for sync if there is no task set. */
        while(u->task_act == NULL) {
            if(u->exit_status)
                break;

            lv_thread_sync_wait(&u->sync);
        }

        if(u->exit_status) {
            LV_LOG_INFO("Ready to exit G2D draw thread.");
            break;
        }

        _g2d_execute_drawing(u);

        /* Signal the ready state to dispatcher. */
        u->task_act->state = LV_DRAW_TASK_STATE_READY;

        /* Cleanup. */
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
    LV_LOG_INFO("Exit G2D draw thread.");
}
#endif

#endif /*LV_USE_DRAW_G2D*/