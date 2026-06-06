/**
 * @file lv_draw_sifli_epic.c
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sifli_epic.h"

#if LV_USE_SIFLI_EPIC
#include "lv_sifli_epic_utils.h"
#include "lv_sifli_epic_cfg.h"
#include "../../../display/lv_display_private.h"
#include "../../../font/lv_font.h"
#include "../../../misc/lv_log.h"
#include "../../../misc/lv_text.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_SIFLI_EPIC 11

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Evaluate a task and set the score and preferred EPIC unit.
 * @param draw_unit Draw unit
 * @param task Draw task
 * @return 1 if task is preferred, 0 otherwise
 */
static int32_t _epic_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

/**
 * Dispatch a task to the EPIC unit.
 * @param draw_unit Draw unit
 * @param layer Layer to draw
 * @return 1 if task was dispatched, 0 otherwise
 */
static int32_t _epic_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

/**
 * Delete the EPIC draw unit.
 * @param draw_unit Draw unit
 * @return Always returns 0
 */
static int32_t _epic_delete(lv_draw_unit_t * draw_unit);

#if LV_USE_SIFLI_EPIC_DRAW_THREAD
    /**
    * Render thread callback.
    * @param ptr Pointer to draw unit
    */
    static void _epic_render_thread_cb(void * ptr);
#endif

/**
 * Wait for EPIC to finish current task.
 * @param draw_unit Draw unit
 * @return 1 on success
 */
#if !LV_USE_SIFLI_EPIC_DRAW_THREAD
    static int32_t _epic_wait_for_finish(lv_draw_unit_t * draw_unit);
#endif

/**
 * Execute drawing operation.
 * @param u EPIC draw unit
 */
static void _epic_execute_drawing(lv_draw_sifli_epic_unit_t * u);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool epic_draw_unit_initialized = false;
#if LV_USE_SIFLI_EPIC_DRAW_THREAD
    static uint32_t epic_dispatch_log_cnt;
    static uint32_t epic_render_start_log_cnt;
    static uint32_t epic_render_done_log_cnt;
    static bool epic_thread_start_logged;
#endif
/**********************
 *      MACROS
 **********************/


/* Vector glyphs are not handled by the EPIC label path below.
 * Keep the check lightweight and fall back only when the first glyph is vector. */
static bool _epic_label_has_vector_glyph(const lv_draw_label_dsc_t * draw_dsc)
{
#if LV_USE_FREETYPE && LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    if(draw_dsc == NULL || draw_dsc->font == NULL || draw_dsc->text == NULL || draw_dsc->text[0] == '\0') {
        return false;
    }

    lv_font_glyph_dsc_t glyph_dsc;
    uint32_t txt_ofs = 0;
    uint32_t letter = lv_text_encoded_next(draw_dsc->text, &txt_ofs);

    if(letter == '\0') {
        return false;
    }

    if(lv_font_get_glyph_dsc(draw_dsc->font, &glyph_dsc, letter, '\0') &&
       glyph_dsc.format == LV_FONT_GLYPH_FORMAT_VECTOR) {
        return true;
    }
#else
    LV_UNUSED(draw_dsc);
#endif

    return false;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sifli_epic_init(void)
{
    if(epic_draw_unit_initialized) {
        return;
    }

    /* Initialize EPIC hardware */
    lv_epic_init();
    if(!lv_epic_is_initialized()) {
        return;
    }

    /* Initialize draw buffer handlers */
    lv_draw_buf_sifli_epic_init_handlers();

    /* Create EPIC draw unit */
    lv_draw_sifli_epic_unit_t * draw_epic_unit = lv_draw_create_unit(sizeof(lv_draw_sifli_epic_unit_t));
    draw_epic_unit->base_unit.evaluate_cb = _epic_evaluate;
    draw_epic_unit->base_unit.dispatch_cb = _epic_dispatch;
    draw_epic_unit->base_unit.delete_cb = _epic_delete;
    draw_epic_unit->base_unit.name = "SiFli_EPIC";
#if !LV_USE_SIFLI_EPIC_DRAW_THREAD
    draw_epic_unit->base_unit.wait_for_finish_cb = _epic_wait_for_finish;
#endif

#if LV_USE_SIFLI_EPIC_DRAW_THREAD
    lv_thread_init(&draw_epic_unit->thread, "EPIC", LV_DRAW_THREAD_PRIO,
                   _epic_render_thread_cb, LV_DRAW_THREAD_STACK_SIZE, draw_epic_unit);
#endif

    epic_draw_unit_initialized = true;
}

void lv_draw_sifli_epic_deinit(void)
{
    if(!epic_draw_unit_initialized) {
        return;
    }

    lv_epic_deinit();
    epic_draw_unit_initialized = false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t _epic_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    switch(task->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * draw_dsc = (const lv_draw_fill_dsc_t *)task->draw_dsc;

                if(task->target_layer == NULL) {
                    return 0;
                }

                /* EPIC doesn't support rounded corners */
                if(draw_dsc->radius != 0) {
                    return 0;
                }

                /* Check gradient support */
                if(draw_dsc->grad.dir != LV_GRAD_DIR_NONE) {
                    if(draw_dsc->grad.dir != LV_GRAD_DIR_HOR && draw_dsc->grad.dir != LV_GRAD_DIR_VER) {
                        return 0;
                    }

                    /* Only support 2-stop gradients */
                    if(draw_dsc->grad.stops_count != 2) {
                        return 0;
                    }

                    /* Check if output format supports gradient */
                    lv_layer_t * target_layer = task->target_layer;
                    if(target_layer) {
                        lv_color_format_t dest_cf = target_layer->color_format;
                        uint32_t epic_cf = lv_img_cf_to_epic_cf(dest_cf);
                        if(!EPIC_SUPPROT_OUT_FORMAT(epic_cf)) {
                            return 0;
                        }
                    }
                }

                if(task->preference_score > 70) {
                    task->preference_score = 70;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_SIFLI_EPIC;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                const lv_draw_image_dsc_t * draw_dsc = (const lv_draw_image_dsc_t *)task->draw_dsc;

                if(draw_dsc->src == NULL) {
                    return 0;
                }

                if(draw_dsc->clip_radius != 0) {
                    return 0;
                }

                if(draw_dsc->blend_mode != LV_BLEND_MODE_NORMAL) {
                    return 0;
                }

                /* Check if recolor is used */
                bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
                if(has_recolor) {
                    return 0;
                }

                /* Check if transform is used */
                bool has_transform = (draw_dsc->rotation != 0 ||
                                      draw_dsc->scale_x != LV_SCALE_NONE ||
                                      draw_dsc->scale_y != LV_SCALE_NONE);

                /* Check color format support */
                if(!lv_epic_cf_supported(draw_dsc->header.cf, draw_dsc->header.flags)) {
                    return 0;
                }

                /* RGB565A8 with transform is not supported */
                if((LV_COLOR_FORMAT_RGB565A8 == draw_dsc->header.cf) && has_transform) {
                    return 0;
                }

                if(task->preference_score > 80) {
                    task->preference_score = 80;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_SIFLI_EPIC;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_BORDER: {
                const lv_draw_border_dsc_t * draw_dsc = (const lv_draw_border_dsc_t *)task->draw_dsc;

                if(draw_dsc->radius != 0) {
                    return 0;
                }

                if(task->preference_score > 90) {
                    task->preference_score = 90;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_SIFLI_EPIC;
                }
                return 1;
            }

#ifdef EPIC_SUPPORT_A8
        case LV_DRAW_TASK_TYPE_LABEL: {
                const lv_draw_label_dsc_t * draw_dsc = (const lv_draw_label_dsc_t *)task->draw_dsc;
                lv_layer_t * target_layer = task->target_layer;

                if(target_layer == NULL) {
                    return 0;
                }

                if(draw_dsc->font == NULL || draw_dsc->text == NULL || draw_dsc->text[0] == '\0') {
                    return 0;
                }

                if(_epic_label_has_vector_glyph(draw_dsc)) {
                    return 0;
                }

                if(!lv_epic_cf_supported(target_layer->color_format, 0)) {
                    return 0;
                }

                if(task->preference_score > 95) {
                    task->preference_score = 95;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_SIFLI_EPIC;
                }
                return 1;
            }
#endif

        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_image_dsc_t * draw_dsc = (const lv_draw_image_dsc_t *)task->draw_dsc;
                lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

                if(layer_to_draw == NULL) {
                    return 0;
                }

                if(layer_to_draw->draw_buf == NULL) {
                    return 0;
                }

                /* Check if recolor is used */
                bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
                if(has_recolor) {
                    return 0;
                }

                /* Check if transform is used */
                bool has_transform = (draw_dsc->rotation != 0 ||
                                      draw_dsc->scale_x != LV_SCALE_NONE ||
                                      draw_dsc->scale_y != LV_SCALE_NONE);

                /* Check color format support */
                if(!lv_epic_cf_supported(layer_to_draw->color_format, 0)) {
                    return 0;
                }

                /* RGB565A8 with transform is not supported */
                if((LV_COLOR_FORMAT_RGB565A8 == layer_to_draw->color_format) && has_transform) {
                    return 0;
                }

                if(task->preference_score > 80) {
                    task->preference_score = 80;
                    task->preferred_draw_unit_id = DRAW_UNIT_ID_SIFLI_EPIC;
                }
                return 1;
            }

        default:
            return 0;
    }

    return 0;
}

static int32_t _epic_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_sifli_epic_unit_t * draw_epic_unit = (lv_draw_sifli_epic_unit_t *)draw_unit;

    /* Try to get a ready to draw task */
    lv_draw_task_t * task = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_SIFLI_EPIC);

    if(task == NULL) {
        return LV_DRAW_UNIT_IDLE;
    }

    /* Return immediately if EPIC is busy */
    if(draw_epic_unit->task_act) {
        return 0;
    }

    /* Check if we have multiple draw units */
    if(lv_draw_get_unit_count() > 1) {
        /* Let the SW unit draw this task if not preferred for EPIC */
        if(task->preferred_draw_unit_id != DRAW_UNIT_ID_SIFLI_EPIC) {
            return LV_DRAW_UNIT_IDLE;
        }
    }
    else {
        /* Fake unsupported tasks as ready */
        if(task->preferred_draw_unit_id != DRAW_UNIT_ID_SIFLI_EPIC) {
            task->state = LV_DRAW_TASK_STATE_FINISHED;
            lv_draw_dispatch_request();
            return 1;
        }
    }

    /* Allocate buffer for layer */
    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL) {
        EPIC_ASSERT_MSG(false, "EPIC: Failed to allocate layer draw buffer");
        return LV_DRAW_UNIT_IDLE;
    }

    /* Mark task as in progress */
    task->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    task->draw_unit = &draw_epic_unit->base_unit;

#if LV_USE_SIFLI_EPIC_DRAW_THREAD
    if(epic_dispatch_log_cnt < 8) {
        LV_LOG_WARN("EPIC dispatch[%u]: type=%d inited=%d",
                    (unsigned int)epic_dispatch_log_cnt,
                    (int)task->type,
                    draw_epic_unit->inited ? 1 : 0);
        epic_dispatch_log_cnt++;
    }

    draw_epic_unit->task_act = task;
    /* Let the render thread work */
    if(draw_epic_unit->inited) {
        lv_thread_sync_signal(&draw_epic_unit->sync);
    }
#else
    draw_epic_unit->task_act = task;
    /* Execute drawing immediately */
    _epic_execute_drawing(draw_epic_unit);

    draw_epic_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    draw_epic_unit->task_act = NULL;

    /* Request new dispatching */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static int32_t _epic_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_SIFLI_EPIC_DRAW_THREAD
    lv_draw_sifli_epic_unit_t * draw_epic_unit = (lv_draw_sifli_epic_unit_t *)draw_unit;

    draw_epic_unit->exit_status = true;

    if(draw_epic_unit->inited) {
        lv_thread_sync_signal(&draw_epic_unit->sync);
    }

    return lv_thread_delete(&draw_epic_unit->thread);
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}

static void _epic_execute_drawing(lv_draw_sifli_epic_unit_t * u)
{
    lv_draw_task_t * task = u->task_act;
    lv_layer_t * layer = task->target_layer;

    /* Wait for previous operation to complete */
    lv_epic_wait();

    if(layer && layer->draw_buf) {
        lv_area_t draw_area;

        if(lv_area_intersect(&draw_area, &task->_real_area, &task->clip_area)) {
            lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
            lv_draw_buf_flush_cache(layer->draw_buf, &draw_area);
        }
    }

    /* Execute based on task type */
    switch(task->type) {
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_sifli_epic_border(task);
            break;

        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_sifli_epic_label(task);
            break;

        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_sifli_epic_fill(task);
            break;

        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_sifli_epic_img(task);
            break;

        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_sifli_epic_layer(task);
            break;

        default:
            break;
    }

    /* Wait for operation to complete */
    lv_epic_wait();

    if(layer && layer->draw_buf) {
        lv_area_t draw_area;

        if(lv_area_intersect(&draw_area, &task->_real_area, &task->clip_area)) {
            lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);
            lv_draw_buf_invalidate_cache(layer->draw_buf, &draw_area);
        }
    }
}

#if !LV_USE_SIFLI_EPIC_DRAW_THREAD
static int32_t _epic_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    LV_UNUSED(draw_unit);
    lv_epic_wait();
    return 1;
}
#endif

#if LV_USE_SIFLI_EPIC_DRAW_THREAD
static void _epic_render_thread_cb(void * ptr)
{
    lv_draw_sifli_epic_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);
    u->inited = true;

    if(!epic_thread_start_logged) {
        LV_LOG_WARN("EPIC render thread started");
        epic_thread_start_logged = true;
    }

    while(1) {
        /* Wait for sync if there is no task set */
        while(u->task_act == NULL) {
            if(u->exit_status) {
                break;
            }
            lv_thread_sync_wait(&u->sync);
        }

        if(u->exit_status) {
            break;
        }

        if(epic_render_start_log_cnt < 8) {
            LV_LOG_WARN("EPIC render start[%u]: type=%d",
                        (unsigned int)epic_render_start_log_cnt,
                        (int)u->task_act->type);
            epic_render_start_log_cnt++;
        }

        _epic_execute_drawing(u);

        if(epic_render_done_log_cnt < 8) {
            LV_LOG_WARN("EPIC render done[%u]: type=%d",
                        (unsigned int)epic_render_done_log_cnt,
                        (int)u->task_act->type);
            epic_render_done_log_cnt++;
        }

        /* Signal the ready state to dispatcher */
        u->task_act->state = LV_DRAW_TASK_STATE_FINISHED;

        /* Cleanup */
        u->task_act = NULL;

        /* Request new dispatching */
        lv_draw_dispatch_request();
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
}
#endif

#endif /*LV_USE_SIFLI_EPIC*/
