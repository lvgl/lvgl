/**
 * @file lv_draw_vglite.c
 *
 */

/**
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"
#include "lv_vglite_utils.h"

#if LV_USE_PARALLEL_DRAW_DEBUG
    #include "../../../core/lv_global.h"
#endif

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_VGLITE 2

#if LV_USE_VGLITE_DRAW_ASYNC
    #define VGLITE_TASK_BUF_SIZE 10
#endif

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_VGLITE_DRAW_ASYNC
/**
 * Structure of pending vglite draw task
 */
typedef struct _vglite_draw_task_t {
    lv_draw_task_t * task;
    bool flushed;
} vglite_draw_tasks_t;
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*
 * Evaluate a task and set the score and preferred VGLite draw unit.
 * Return 1 if task is preferred, 0 otherwise (task is not supported).
 */
static int32_t _vglite_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

/*
 * Dispatch (assign) a task to VGLite draw unit (itself).
 * Return 1 if task was dispatched, 0 otherwise (task not supported).
 */
static int32_t _vglite_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

/*
 * Delete the VGLite draw unit.
 */
static int32_t _vglite_delete(lv_draw_unit_t * draw_unit);

#if LV_USE_OS
    static void _vglite_render_thread_cb(void * ptr);
#endif

static void _vglite_execute_drawing(lv_draw_vglite_unit_t * u);

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_PARALLEL_DRAW_DEBUG
    #define _draw_info LV_GLOBAL_DEFAULT()->draw_info
#endif

#if LV_USE_VGLITE_DRAW_ASYNC
    /*
    * Circular buffer to hold the queued and the flushed tasks.
    * Two indexes, _head and _tail, are used to signal the beginning
    * and the end of the valid tasks that are pending.
    */
    static vglite_draw_tasks_t _draw_task_buf[VGLITE_TASK_BUF_SIZE];
    static volatile int _head = 0;
    static volatile int _tail = 0;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_vglite_init(void)
{
    lv_draw_buf_vglite_init_handlers();

    lv_draw_vglite_unit_t * draw_vglite_unit = lv_draw_create_unit(sizeof(lv_draw_vglite_unit_t));
    draw_vglite_unit->base_unit.evaluate_cb = _vglite_evaluate;
    draw_vglite_unit->base_unit.dispatch_cb = _vglite_dispatch;
    draw_vglite_unit->base_unit.delete_cb = _vglite_delete;

#if LV_USE_OS
    lv_thread_init(&draw_vglite_unit->thread, LV_THREAD_PRIO_HIGH, _vglite_render_thread_cb, 2 * 1024, draw_vglite_unit);
#endif
}

void lv_draw_vglite_deinit(void)
{
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool _vglite_src_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
#if CHIPID == 0x255 || CHIPID == 0x555
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_I8:
#endif
        case LV_COLOR_FORMAT_A4:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_RGB565:
#if CHIPID == 0x555
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
#endif
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline bool _vglite_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_A8:
#if CHIPID == 0x255 || CHIPID == 0x555
        case LV_COLOR_FORMAT_L8:
#endif
        case LV_COLOR_FORMAT_RGB565:
#if CHIPTID == 0x555
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
#endif
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static int32_t _vglite_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);

    const lv_draw_dsc_base_t * draw_dsc_base = (lv_draw_dsc_base_t *) t->draw_dsc;

    if(!_vglite_dest_cf_supported(draw_dsc_base->layer->color_format))
        return 0;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            if(t->preference_score > 80) {
                t->preference_score = 80;
                t->preferred_draw_unit_id = DRAW_UNIT_ID_VGLITE;
            }
            return 1;

        case LV_DRAW_TASK_TYPE_LINE:
        case LV_DRAW_TASK_TYPE_ARC:
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            if(t->preference_score > 90) {
                t->preference_score = 90;
                t->preferred_draw_unit_id = DRAW_UNIT_ID_VGLITE;
            }
            return 1;

        case LV_DRAW_TASK_TYPE_LABEL:
            if(t->preference_score > 95) {
                t->preference_score = 95;
                t->preferred_draw_unit_id = DRAW_UNIT_ID_VGLITE;
            }
            return 1;

        case LV_DRAW_TASK_TYPE_BORDER: {
                const lv_draw_border_dsc_t * draw_dsc = (lv_draw_border_dsc_t *) t->draw_dsc;

                if(draw_dsc->side != (lv_border_side_t)LV_BORDER_SIDE_FULL)
                    return 0;

                if(t->preference_score > 90) {
                    t->preference_score = 90;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_VGLITE;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

#if LV_USE_VGLITE_BLIT_SPLIT
                bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                                      draw_dsc->scale_y != LV_SCALE_NONE);
#endif
                if(!_vglite_src_cf_supported(layer_to_draw->color_format)
#if LV_USE_VGLITE_BLIT_SPLIT
                   || has_transform
#endif
                  )
                    return 0;

                if(t->preference_score > 80) {
                    t->preference_score = 80;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_VGLITE;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                const lv_image_dsc_t * img_dsc = draw_dsc->src;

#if LV_USE_VGLITE_BLIT_SPLIT
                bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                                      draw_dsc->scale_y != LV_SCALE_NONE);
#endif

                if((!_vglite_src_cf_supported(img_dsc->header.cf))
#if LV_USE_VGLITE_BLIT_SPLIT
                   || has_transform
#endif
                   || (!vglite_buf_aligned(img_dsc->data, img_dsc->header.stride, img_dsc->header.cf))
                  )
                    return 0;

                if(t->preference_score > 80) {
                    t->preference_score = 80;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_VGLITE;
                }
                return 1;
            }
        default:
            return 0;
    }

    return 0;
}

static int32_t _vglite_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_vglite_unit_t * draw_vglite_unit = (lv_draw_vglite_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task. */
    if(draw_vglite_unit->task_act)
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_VGLITE);

    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_VGLITE)
        return -1;

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL)
        return -1;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_vglite_unit->base_unit.target_layer = layer;
    draw_vglite_unit->base_unit.clip_area = &t->clip_area;
    draw_vglite_unit->task_act = t;

#if LV_USE_OS
    /* Let the render thread work. */
    if(draw_vglite_unit->inited)
        lv_thread_sync_signal(&draw_vglite_unit->sync);
#else
    _vglite_execute_drawing(draw_vglite_unit);

    draw_vglite_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_vglite_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task. */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static int32_t _vglite_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_OS
    lv_draw_vglite_unit_t * draw_vglite_unit = (lv_draw_vglite_unit_t *) draw_unit;

    LV_LOG_INFO("Cancel VGLite draw thread.");
    draw_vglite_unit->exit_status = true;

    if(draw_vglite_unit->inited)
        lv_thread_sync_signal(&draw_vglite_unit->sync);

    lv_result_t res = lv_thread_delete(&draw_vglite_unit->thread);

    return res;
#else
    LV_UNUSED(draw_unit);

    return 0;
#endif
}

static void _vglite_execute_drawing(lv_draw_vglite_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_draw_unit_t * draw_unit = (lv_draw_unit_t *)u;
    lv_layer_t * layer = draw_unit->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    /* Set target buffer */
    vglite_set_dest_buf(draw_buf->data, draw_buf->header.w, draw_buf->header.h, draw_buf->header.stride,
                        draw_buf->header.cf);

    lv_area_t clip_area;
    lv_area_copy(&clip_area, draw_unit->clip_area);
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_area_t draw_area;
    lv_area_copy(&draw_area, &t->area);
    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);

    if(!_lv_area_intersect(&draw_area, &draw_area, &clip_area))
        return; /*Fully clipped, nothing to do*/

    /* Invalidate the drawing area */
    lv_draw_buf_invalidate_cache(draw_buf, &draw_area);

    /* Set scissor area, excluding the split blit case */
#if LV_USE_VGLITE_BLIT_SPLIT
    if(t->type != LV_DRAW_TASK_TYPE_IMAGE || t->type != LV_DRAW_TASK_TYPE_LAYER)
#endif
        vglite_set_scissor(&clip_area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_vglite_label(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_vglite_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_vglite_border(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_vglite_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_vglite_arc(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_vglite_line(draw_unit, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_vglite_layer(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_vglite_triangle(draw_unit, t->draw_dsc);
            break;
        default:
            break;
    }

    /* Disable scissor */
    vglite_set_scissor(&layer->buf_area);

#if LV_USE_PARALLEL_DRAW_DEBUG
    /*Layers manage it for themselves*/
    if(t->type != LV_DRAW_TASK_TYPE_LAYER) {
        lv_area_t draw_area;
        if(!_lv_area_intersect(&draw_area, &t->area, u->base_unit.clip_area))
            return;

        int32_t idx = 0;
        lv_draw_unit_t * draw_unit_tmp = _draw_info.unit_head;
        while(draw_unit_tmp != (lv_draw_unit_t *)u) {
            draw_unit_tmp = draw_unit_tmp->next;
            idx++;
        }
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_palette_main(idx % _LV_PALETTE_LAST);
        rect_dsc.border_color = rect_dsc.bg_color;
        rect_dsc.bg_opa = LV_OPA_10;
        rect_dsc.border_opa = LV_OPA_80;
        rect_dsc.border_width = 1;
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &draw_area);

        lv_point_t txt_size;
        lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

        lv_area_t txt_area;
        txt_area.x1 = draw_area.x1;
        txt_area.y1 = draw_area.y1;
        txt_area.x2 = draw_area.x1 + txt_size.x - 1;
        txt_area.y2 = draw_area.y1 + txt_size.y - 1;

        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_color_white();
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &txt_area);

        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d", idx);
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_color_black();
        label_dsc.text = buf;
        lv_draw_sw_label((lv_draw_unit_t *)u, &label_dsc, &txt_area);
    }
#endif
}

#if LV_USE_VGLITE_DRAW_ASYNC
static inline void _vglite_queue_task(lv_draw_task_t * task_act)
{
    _draw_task_buf[_tail].task = task_act;
    _draw_task_buf[_tail].flushed = false;
    _tail = (_tail + 1) % VGLITE_TASK_BUF_SIZE;
}

static inline void _vglite_signal_task_ready(lv_draw_task_t * task_act)
{
    if(vglite_cmd_buf_is_flushed()) {
        int end = (_head < _tail) ? _tail : _tail + VGLITE_TASK_BUF_SIZE;

        for(int i = _head; i < end; i++) {
            /* Previous flushed tasks are ready now. */
            if(_draw_task_buf[i % VGLITE_TASK_BUF_SIZE].flushed) {
                lv_draw_task_t * task = _draw_task_buf[i % VGLITE_TASK_BUF_SIZE].task;

                /* Signal the ready state to dispatcher. */
                task->state = LV_DRAW_TASK_STATE_READY;
                _head = (_head + 1) % VGLITE_TASK_BUF_SIZE;
                /* No need to cleanup the tasks in buffer as we advance with the _head. */
            }
            else {
                /* Those tasks have been flushed now. */
                _draw_task_buf[i % VGLITE_TASK_BUF_SIZE].flushed = true;
            }
        }
    }

    if(task_act)
        VGLITE_ASSERT_MSG(_tail != _head, "VGLite task buffer full.");
}
#endif

#if LV_USE_OS
static void _vglite_render_thread_cb(void * ptr)
{
    lv_draw_vglite_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);
    u->inited = true;

    while(1) {
        /* Wait for sync if there is no task set. */
        while(u->task_act == NULL
#if LV_USE_VGLITE_DRAW_ASYNC
              /*
               * Wait for sync if _draw_task_buf is empty.
               * The thread will have to run as much as there are pending tasks.
               */
              && _head == _tail
#endif
             ) {
            if(u->exit_status)
                break;

            lv_thread_sync_wait(&u->sync);
        }

        if(u->exit_status) {
            LV_LOG_INFO("Ready to exit VGLite draw thread.");
            break;
        }

        if(u->task_act) {
#if LV_USE_VGLITE_DRAW_ASYNC
            _vglite_queue_task((void *)u->task_act);
#endif
            _vglite_execute_drawing(u);
        }
#if LV_USE_VGLITE_DRAW_ASYNC
        else {
            /*
             * Update the flush status for last pending tasks.
             * vg_lite_flush() will early return if there is nothing to submit.
             */
            vglite_run();
        }
#endif
#if LV_USE_VGLITE_DRAW_ASYNC
        _vglite_signal_task_ready((void *)u->task_act);
#else
        /* Signal the ready state to dispatcher. */
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
#endif
        /* Cleanup. */
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
    LV_LOG_INFO("Exit VGLite draw thread.");
}
#endif

#endif /*LV_USE_DRAW_VGLITE*/
