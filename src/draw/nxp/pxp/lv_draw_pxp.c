/**
 * @file lv_draw_pxp.c
 *
 */

/**
 * Copyright 2022, 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_pxp.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_cfg.h"
#include "../../../display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_PXP 3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*
 * Dispatch a task to the PXP unit.
 * Return 1 if task was dispatched, 0 otherwise (task not supported).
 */
static int32_t _pxp_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

/*
 * Evaluate a task and set the score and preferred PXP unit.
 * Return 1 if task is preferred, 0 otherwise (task is not supported).
 */
static int32_t _pxp_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

#if LV_USE_OS
    static void _pxp_render_thread_cb(void * ptr);
#endif

static void _pxp_execute_drawing(lv_draw_pxp_unit_t * u);

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

void lv_draw_pxp_init(void)
{
    lv_draw_buf_pxp_init_handlers();

    lv_draw_pxp_unit_t * draw_pxp_unit = lv_draw_create_unit(sizeof(lv_draw_pxp_unit_t));
    draw_pxp_unit->base_unit.dispatch_cb = _pxp_dispatch;
    draw_pxp_unit->base_unit.evaluate_cb = _pxp_evaluate;

    lv_pxp_init();

#if LV_USE_OS
    lv_thread_init(&draw_pxp_unit->thread, LV_THREAD_PRIO_HIGH, _pxp_render_thread_cb, 8 * 1024, draw_pxp_unit);
#endif
}

void lv_draw_pxp_deinit(void)
{
    lv_pxp_deinit();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool _pxp_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = (cf == LV_COLOR_FORMAT_RGB565 || cf == LV_COLOR_FORMAT_RGB888 ||
                            cf == LV_COLOR_FORMAT_ARGB8888 || cf == LV_COLOR_FORMAT_XRGB8888);

    return is_cf_supported;
}

static bool _pxp_draw_img_supported(const lv_draw_image_dsc_t * draw_dsc)
{
    const lv_image_dsc_t * img_dsc = draw_dsc->src;

    bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
    bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->zoom != LV_SCALE_NONE);

    /* Recolor and transformation are not supported at the same time. */
    if(has_recolor && has_transform)
        return false;

    bool has_opa = (draw_dsc->opa < (lv_opa_t)LV_OPA_MAX);
    bool src_has_alpha = (img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888);

    /*
     * Recolor or transformation for images w/ opa or alpha channel can't
     * be obtained in a single PXP configuration. Two steps are required.
     */
    if((has_recolor || has_transform) && (has_opa || src_has_alpha))
        return false;

    /* PXP can only rotate at 90x angles. */
    if(draw_dsc->rotation % 900)
        return false;

    /*
     * PXP is set to process 16x16 blocks to optimize the system for memory
     * bandwidth and image processing time.
     * The output engine essentially truncates any output pixels after the
     * desired number of pixels has been written.
     * When rotating a source image and the output is not divisible by the block
     * size, the incorrect pixels could be truncated and the final output image
     * can look shifted.
     *
     * No combination of rotate with flip, scaling or decimation is possible
     * if buffer is unaligned.
     */
    if(has_transform && (img_dsc->header.w % 16 || img_dsc->header.h % 16))
        return false;

    return true;
}

static int32_t _pxp_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * draw_dsc = (lv_draw_fill_dsc_t *) t->draw_dsc;

                /* Most simple case: just a plain rectangle (no radius, no gradient). */
                if((draw_dsc->radius != 0) || (draw_dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))
                    return 0;

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PXP;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_BG_IMG: {
                const lv_draw_bg_image_dsc_t * draw_dsc = (lv_draw_bg_image_dsc_t *) t->draw_dsc;
                lv_image_src_t src_type = lv_image_src_get_type(draw_dsc->src);

                if(src_type == LV_IMAGE_SRC_SYMBOL)
                    return 0;

                if(!_pxp_cf_supported(draw_dsc->img_header.cf))
                    return 0;

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PXP;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;
                lv_draw_buf_t * draw_buf = &layer_to_draw->draw_buf;

                if(!_pxp_cf_supported(draw_buf->color_format))
                    return 0;

                if(!_pxp_draw_img_supported(draw_dsc))
                    return 0;

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PXP;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                const lv_image_dsc_t * img_dsc = draw_dsc->src;

                if(!_pxp_cf_supported(img_dsc->header.cf))
                    return 0;

                if(!_pxp_draw_img_supported(draw_dsc))
                    return 0;

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_PXP;
                }
                return 1;
            }
        default:
            return 0;
    }

    return 0;
}

static int32_t _pxp_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_pxp_unit_t * draw_pxp_unit = (lv_draw_pxp_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task. */
    if(draw_pxp_unit->task_act)
        return 0;

    /* Return if target buffer format is not supported. */
    if(!_pxp_cf_supported(layer->draw_buf.color_format))
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_PXP);

    /* Return 0 is no selection, some tasks can be supported only by other units. */
    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_PXP)
        return 0;

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL)
        return -1;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_pxp_unit->base_unit.target_layer = layer;
    draw_pxp_unit->base_unit.clip_area = &t->clip_area;
    draw_pxp_unit->task_act = t;

#if LV_USE_OS
    /* Let the render thread work. */
    lv_thread_sync_signal(&draw_pxp_unit->sync);
#else
    _pxp_execute_drawing(draw_pxp_unit);

    draw_pxp_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_pxp_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task. */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static void _pxp_execute_drawing(lv_draw_pxp_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_draw_unit_t * draw_unit = (lv_draw_unit_t *)u;

    /* Invalidate cache */
    lv_layer_t * layer = draw_unit->target_layer;
    lv_draw_buf_invalidate_cache(&layer->draw_buf, (const char *)&t->area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_pxp_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BG_IMG:
            lv_draw_pxp_bg_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_pxp_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_pxp_layer(draw_unit, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}

#if LV_USE_OS
static void _pxp_render_thread_cb(void * ptr)
{
    lv_draw_pxp_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);

    while(1) {
        /*
         * Wait for sync if no task received or _draw_task_buf is empty.
         * The thread will have to run as much as there are pending tasks.
         */
        while(u->task_act == NULL) {
            lv_thread_sync_wait(&u->sync);
        }

        _pxp_execute_drawing(u);

        /* Cleanup. */
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }
}
#endif

#endif /*LV_USE_DRAW_PXP*/
