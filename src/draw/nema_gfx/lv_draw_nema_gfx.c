/**
 * MIT License
 *
 * -----------------------------------------------------------------------------
 * Copyright (c) 2008-24 Think Silicon Single Member PC
 * -----------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file lv_draw_nema_gfx.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../core/lv_refr.h"

#if LV_USE_NEMA_GFX

#include "lv_draw_nema_gfx.h"
#include "../../font/lv_font.h"
#include "../../font/lv_font_fmt_txt.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_NEMA_GFX 7

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_OS
/**
 * Structure of pending nema_gfx draw task
 */
typedef struct _nema_gfx_draw_task_t {
    lv_draw_task_t * task;
    bool flushed;
} nema_gfx_draw_task_t;
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_USE_OS
    static void nema_gfx_render_thread_cb(void * ptr);
#endif

static void nema_gfx_execute_drawing(lv_draw_nema_gfx_unit_t * u);

static int32_t nema_gfx_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static int32_t nema_gfx_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

static int32_t nema_gfx_delete(lv_draw_unit_t * draw_unit);

static int32_t nema_gfx_wait_for_finish(lv_draw_unit_t * draw_unit);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nema_gfx_init(void)
{
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = lv_draw_create_unit(sizeof(lv_draw_nema_gfx_unit_t));
    /*Initialize NemaGFX*/
    nema_init();

    draw_nema_gfx_unit->base_unit.dispatch_cb = nema_gfx_dispatch;
    draw_nema_gfx_unit->base_unit.evaluate_cb = nema_gfx_evaluate;
    draw_nema_gfx_unit->base_unit.delete_cb = nema_gfx_delete;
    draw_nema_gfx_unit->base_unit.wait_for_finish_cb = nema_gfx_wait_for_finish;
    draw_nema_gfx_unit->base_unit.name = "NEMA_GFX";

#if LV_USE_NEMA_VG
    /*Initialize NemaVG */
    nema_vg_init(LV_NEMA_GFX_MAX_RESX, LV_NEMA_GFX_MAX_RESY);
    /* Allocate VG Buffers*/
    draw_nema_gfx_unit->paint = nema_vg_paint_create();
    draw_nema_gfx_unit->gradient = nema_vg_grad_create();
    draw_nema_gfx_unit->path = nema_vg_path_create();
    /*Initialize Freetype Support*/
    lv_draw_nema_gfx_label_init(&(draw_nema_gfx_unit->base_unit));
#endif
    /*Create GPU Command List*/
    draw_nema_gfx_unit->cl = nema_cl_create();
    /*Bind Command List*/
    nema_cl_bind_circular(&(draw_nema_gfx_unit->cl));


#if LV_USE_OS
    lv_thread_init(&draw_nema_gfx_unit->thread, "nemagfx", LV_DRAW_THREAD_PRIO, nema_gfx_render_thread_cb, 2 * 1024,
                   draw_nema_gfx_unit);
#endif
}

void lv_draw_nema_gfx_deinit(void)
{
    return;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t nema_gfx_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)draw_unit;
    nema_cl_submit(&(draw_nema_gfx_unit->cl));
    nema_cl_wait(&(draw_nema_gfx_unit->cl));
    return 1;
}

static int32_t nema_gfx_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *)draw_unit;

    switch(task->type) {
        case LV_DRAW_TASK_TYPE_LAYER: {
                if(task->preference_score > 80) {
                    task->preference_score = 80;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                }
                return 1;
            }
#if LV_USE_NEMA_VG
        case LV_DRAW_TASK_TYPE_TRIANGLE:
        case LV_DRAW_TASK_TYPE_ARC:
        case LV_DRAW_TASK_TYPE_FILL: {
                if(task->preference_score > 80) {
                    task->preference_score = 80;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                }
                return 1;
            }
#else
        case LV_DRAW_TASK_TYPE_FILL: {
                lv_draw_fill_dsc_t * draw_fill_dsc = (lv_draw_fill_dsc_t *) task->draw_dsc;
                if((draw_fill_dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE)) {
                    if(task->preference_score > 80) {
                        task->preference_score = 80;
                        task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                    }
                    return 1;
                }
                break;
            }
        case LV_DRAW_TASK_TYPE_TRIANGLE: {
                lv_draw_triangle_dsc_t * draw_triangle_dsc = (lv_draw_triangle_dsc_t *) task->draw_dsc;
                if((draw_triangle_dsc->grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE)) {
                    if(task->preference_score > 80) {
                        task->preference_score = 80;
                        task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                    }
                    return 1;
                }
                break;
            }
#endif
        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * draw_image_dsc = (lv_draw_image_dsc_t *) task->draw_dsc;
                /*Guard for previous NemaGFX Version*/
#ifndef NEMA_BLOP_RECOLOR
                if(draw_image_dsc->recolor_opa > LV_OPA_MIN)
                    break;
#endif
                const lv_image_dsc_t * img_dsc = draw_image_dsc->src;
                if(!lv_nemagfx_is_cf_supported(img_dsc->header.cf))
                    break;

                if(draw_image_dsc->blend_mode != LV_BLEND_MODE_SUBTRACTIVE) {
                    if(task->preference_score > 80) {
                        task->preference_score = 80;
                        task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                    }
                    return 1;
                }
                break;
            }
        case LV_DRAW_TASK_TYPE_LABEL: {
                lv_draw_label_dsc_t * draw_label_dsc = (lv_draw_label_dsc_t *) task->draw_dsc;
                lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)(draw_label_dsc->font->dsc);
                if(fdsc->bitmap_format != LV_FONT_FMT_TXT_COMPRESSED) {
                    if(task->preference_score > 80) {
                        task->preference_score = 80;
                        task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                    }
                    return 1;
                }
                break;
            }
        case LV_DRAW_TASK_TYPE_LINE: {
                lv_draw_line_dsc_t * draw_line_dsc = (lv_draw_line_dsc_t *) task->draw_dsc;
                bool is_dashed = (draw_line_dsc->dash_width && draw_line_dsc->dash_gap);
                if(!is_dashed && !(draw_line_dsc->round_end || draw_line_dsc->round_start)) {
                    if(task->preference_score > 80) {
                        task->preference_score = 80;
                        task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                    }
                    return 1;
                }
                break;
            }
        case LV_DRAW_TASK_TYPE_BORDER: {
                const lv_draw_border_dsc_t * draw_dsc = (lv_draw_border_dsc_t *) task->draw_dsc;
                if((!(draw_dsc->side != (lv_border_side_t)LV_BORDER_SIDE_FULL && draw_dsc->radius > 0)) &&
                   (draw_dsc->radius > draw_dsc->width)) {
                    if(task->preference_score > 80) {
                        task->preference_score = 80;
                        task->preferred_draw_unit_id = DRAW_UNIT_ID_NEMA_GFX;
                    }
                    return 1;
                }
                break;
            }
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
        case LV_DRAW_TASK_TYPE_MASK_BITMAP:
#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_TASK_TYPE_VECTOR:
#endif
        default:
            break;
    }
    nema_cl_wait(&(draw_nema_gfx_unit->cl));
    return 0;
}

static int32_t nema_gfx_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task. */
    if(draw_nema_gfx_unit->task_act)
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_NEMA_GFX);

    /* Return 0 is no selection, some tasks can be supported by other units. */
    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_NEMA_GFX)
        return LV_DRAW_UNIT_IDLE;

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL)
        return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_nema_gfx_unit->task_act = t;

#if LV_USE_OS
    /* Let the render thread work. */
    if(draw_nema_gfx_unit->inited)
        lv_thread_sync_signal(&draw_nema_gfx_unit->sync);
#else
    nema_gfx_execute_drawing(draw_nema_gfx_unit);

    draw_nema_gfx_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_nema_gfx_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task. */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static void nema_gfx_execute_drawing(lv_draw_nema_gfx_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    /* remember draw unit for access to unit's context */
    t->draw_unit = (lv_draw_unit_t *)u;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_nema_gfx_fill(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_nema_gfx_img(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_nema_gfx_triangle(t, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_nema_gfx_label(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_nema_gfx_layer(t, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_nema_gfx_line(t, t->draw_dsc);
            break;
#if LV_USE_NEMA_VG
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_nema_gfx_arc(t, t->draw_dsc, &t->area);
            break;
#endif
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_nema_gfx_border(t, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}

static int32_t nema_gfx_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_NEMA_VG
    /*Free VG Buffers*/
    lv_draw_nema_gfx_unit_t * draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *) draw_unit;
    nema_vg_paint_destroy(draw_nema_gfx_unit->paint);
    nema_vg_path_destroy(draw_nema_gfx_unit->path);
    nema_vg_grad_destroy(draw_nema_gfx_unit->gradient);
#endif

#if LV_USE_OS
    lv_draw_nema_gfx_unit_t * _draw_nema_gfx_unit = (lv_draw_nema_gfx_unit_t *) draw_unit;
    LV_LOG_INFO("Cancel NemaGFX draw thread.");
    _draw_nema_gfx_unit->exit_status = true;

    if(_draw_nema_gfx_unit->inited)
        lv_thread_sync_signal(&_draw_nema_gfx_unit->sync);

    lv_result_t res = lv_thread_delete(&_draw_nema_gfx_unit->thread);

    return res;
#endif

#if LV_USE_NEMA_VG == 0 && LV_USE_OS == LV_OS_NONE
    LV_UNUSED(draw_unit);
#endif
    return 0;
}

#if LV_USE_OS
static void nema_gfx_render_thread_cb(void * ptr)
{
    lv_draw_nema_gfx_unit_t * u = ptr;

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
            LV_LOG_INFO("Ready to exit NemaGFX draw thread.");
            break;
        }

        if(u->task_act) {
            nema_gfx_execute_drawing(u);
        }
        /* Signal the ready state to dispatcher. */
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        /* Cleanup. */
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
    LV_LOG_INFO("Exit NemaGFX draw thread.");
}
#endif

#endif
