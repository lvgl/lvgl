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
#include "lv_g2d_buf_map.h"

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

static void _g2d_execute_drawing(lv_draw_task_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_g2d_init(void)
{
    lv_draw_buf_g2d_init_handlers();

    lv_draw_g2d_unit_t * draw_g2d_unit = lv_draw_create_unit(sizeof(lv_draw_g2d_unit_t));
    draw_g2d_unit->base_unit.evaluate_cb = _g2d_evaluate;
    draw_g2d_unit->base_unit.dispatch_cb = _g2d_dispatch;
    draw_g2d_unit->base_unit.delete_cb = _g2d_delete;
    draw_g2d_unit->base_unit.name = "G2D";
    g2d_create_buf_map();
    if(g2d_open(&draw_g2d_unit->g2d_handle)) {
        LV_LOG_ERROR("g2d_open fail.\n");
    }
#if LV_USE_G2D_DRAW_THREAD
    lv_draw_sw_thread_dsc_t * thread_dsc = &draw_g2d_unit->thread_dsc;
    thread_dsc->idx = 0;
    thread_dsc->draw_unit = (void *) draw_g2d_unit;
    lv_thread_init(&thread_dsc->thread, "g2ddraw", LV_DRAW_THREAD_PRIO, _g2d_render_thread_cb, LV_DRAW_THREAD_STACK_SIZE,
                   thread_dsc);
#endif
}

void lv_draw_g2d_deinit(void)
{
    g2d_free_buf_map();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool _g2d_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline bool _g2d_src_cf_supported(lv_draw_unit_t * drawunit, lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        case LV_COLOR_FORMAT_RGB565: {
                int32_t hw_pxp = 0;
                lv_draw_g2d_unit_t * u = (lv_draw_g2d_unit_t *)drawunit;
                g2d_query_hardware(u->g2d_handle, G2D_HARDWARE_PXP, &hw_pxp);
                if(!hw_pxp) {
                    is_cf_supported = true;
                }
            }
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static bool _g2d_draw_img_supported(const lv_draw_image_dsc_t * draw_dsc)
{
    bool is_tiled = draw_dsc->tile;
    /* Tiled image (repeat image) is currently not supported. */
    if(is_tiled)
        return false;

    bool has_recolor = (draw_dsc->recolor_opa > LV_OPA_MIN);
    bool has_rotation = (draw_dsc->rotation != 0);

    /* Recolor or rotation are not supported. */
    if(has_recolor || has_rotation)
        return false;

    return true;
}

static int32_t _g2d_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);

    const lv_draw_dsc_base_t * draw_dsc_base = (lv_draw_dsc_base_t *) t->draw_dsc;
    lv_draw_buf_t * draw_buf = draw_dsc_base->layer->draw_buf;

    if(!_g2d_dest_cf_supported(draw_dsc_base->layer->color_format))
        return 0;

    if(draw_buf && !g2d_search_buf_map(draw_buf->data))
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
        case LV_DRAW_TASK_TYPE_IMAGE: {
                const lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;

                if(!_g2d_src_cf_supported(u, draw_dsc->header.cf))
                    return 0;

                if(!_g2d_draw_img_supported(draw_dsc))
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

#if LV_USE_OS
    lv_draw_sw_thread_dsc_t * thread_dsc = &draw_g2d_unit->thread_dsc;

    /* Return immediately if it's busy with draw task. */
    if(thread_dsc->task_act)
        return 0;
#else
    /* Return immediately if it's busy with draw task. */
    if(draw_g2d_unit->task_act)
        return 0;
#endif

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_G2D);

    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_G2D)
        return LV_DRAW_UNIT_IDLE;

    if(lv_draw_layer_alloc_buf(layer) == NULL)
        return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    t->draw_unit = draw_unit;

#if LV_USE_G2D_DRAW_THREAD
    thread_dsc->task_act = t;

    /* Let the render thread work. */
    if(thread_dsc->inited)
        lv_thread_sync_signal(&thread_dsc->sync);
#else
    draw_g2d_unit->task_act = t;

    _g2d_execute_drawing(t);

    draw_g2d_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_g2d_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task. */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static int32_t _g2d_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_g2d_unit_t * draw_g2d_unit = (lv_draw_g2d_unit_t *) draw_unit;
    lv_result_t res = LV_RESULT_OK;

#if LV_USE_G2D_DRAW_THREAD
    lv_draw_sw_thread_dsc_t * thread_dsc = &draw_g2d_unit->thread_dsc;
    LV_LOG_INFO("Cancel G2D draw thread.");
    thread_dsc->exit_status = true;

    if(thread_dsc->inited)
        lv_thread_sync_signal(&thread_dsc->sync);

    res = lv_thread_delete(&thread_dsc->thread);
#endif
    g2d_close(draw_g2d_unit->g2d_handle);

    return res;
}

static void _g2d_execute_drawing(lv_draw_task_t * t)
{
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    /* Invalidate only the drawing area */
    lv_draw_buf_invalidate_cache(draw_buf, NULL);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_g2d_fill(t);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_g2d_img(t);
            break;
        default:
            break;
    }
}

#if LV_USE_G2D_DRAW_THREAD
static void _g2d_render_thread_cb(void * ptr)
{
    lv_draw_sw_thread_dsc_t * thread_dsc = ptr;

    lv_thread_sync_init(&thread_dsc->sync);
    thread_dsc->inited = true;

    while(1) {
        /* Wait for sync if there is no task set. */
        while(thread_dsc->task_act == NULL) {
            if(thread_dsc->exit_status)
                break;

            lv_thread_sync_wait(&thread_dsc->sync);
        }

        if(thread_dsc->exit_status) {
            LV_LOG_INFO("Ready to exit G2D draw thread.");
            break;
        }

        _g2d_execute_drawing(thread_dsc->task_act);

        /* Signal the ready state to dispatcher. */
        thread_dsc->task_act->state = LV_DRAW_TASK_STATE_READY;

        /* Cleanup. */
        thread_dsc->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }

    thread_dsc->inited = false;
    lv_thread_sync_delete(&thread_dsc->sync);
    LV_LOG_INFO("Exit G2D draw thread.");
}
#endif

#endif /*LV_USE_DRAW_G2D*/
