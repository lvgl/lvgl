/**
 * @file lv_draw_g2d.c
 *
 */

/**
 * Copyright 2022, 2023，2024 G2D
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"

#if LV_USE_DRAW_G2D
#include "../../display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_G2D 3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/*
 * Dispatch a task to the G2D unit.
 * Return 1 if task was dispatched, 0 otherwise (task not supported).
 */
static int32_t _g2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

/*
 * Evaluate a task and set the score and preferred G2D unit.
 * Return 1 if task is preferred, 0 otherwise (task is not supported).
 */
static int32_t _g2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

#if LV_USE_OS
    static void _g2d_render_thread_cb(void * ptr);
#endif

static void _g2d_execute_drawing(lv_draw_g2d_unit_t * u);

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

void lv_draw_g2d_init(void)
{
    //lv_draw_buf_g2d_init_handlers();

    lv_draw_g2d_unit_t * draw_g2d_unit = lv_draw_create_unit(sizeof(lv_draw_g2d_unit_t));
    draw_g2d_unit->base_unit.dispatch_cb = _g2d_dispatch;
    draw_g2d_unit->base_unit.evaluate_cb = _g2d_evaluate;

    lv_g2d_init();

#if LV_USE_OS
    lv_thread_init(&draw_g2d_unit->thread, LV_THREAD_PRIO_HIGH, _g2d_render_thread_cb, 8 * 1024, draw_g2d_unit);
#endif
}

void lv_draw_g2d_deinit(void)
{
    lv_g2d_deinit();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool _g2d_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = (cf == LV_COLOR_FORMAT_RGB565 || cf == LV_COLOR_FORMAT_RGB888 ||
                            cf == LV_COLOR_FORMAT_ARGB8888 || cf == LV_COLOR_FORMAT_XRGB8888);

    return is_cf_supported;
}

static bool _g2d_draw_img_supported(const lv_draw_image_dsc_t * draw_dsc, int32_t size)
{
    const lv_image_dsc_t * img_dsc = draw_dsc->src;

    bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
    bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                          draw_dsc->scale_y != LV_SCALE_NONE);

    /* Recolor and transformation are not supported at the same time. */
    if(has_recolor && has_transform)
        return false;

    bool has_opa = (draw_dsc->opa < (lv_opa_t)LV_OPA_MAX);
    bool src_has_alpha = (img_dsc->header.cf == LV_COLOR_FORMAT_ARGB8888);

    /*
     * Recolor or transformation for images w/ opa or alpha channel can't
     * be obtained in a single G2D configuration. Two steps are required.
     */

    if(!has_opa && !src_has_alpha  && (size < sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_BLIT)))
        return false;

    if((has_opa || src_has_alpha)  && (size < sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_BLEND)))
        return false;

    /* G2D can only rotate at 90x angles. */
    if(draw_dsc->rotation || has_recolor)
        return false;

    if(((draw_dsc->scale_x != LV_SCALE_NONE) || (draw_dsc->scale_y != LV_SCALE_NONE)) &&
       (size < sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_SCALE)))
        return false;

    /*
     * G2D is set to process 16x16 blocks to optimize the system for memory
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
    /*if(has_transform && (img_dsc->header.w % 16 || img_dsc->header.h % 16))
        return false;*/
    return true;
}

static int32_t _g2d_evaluate(lv_draw_unit_t * u, lv_draw_task_t * t)
{
    LV_UNUSED(u);
    lv_area_t img_area;
    _lv_area_intersect(&img_area, &t->area, &t->clip_area);
    int32_t size = lv_area_get_width(&img_area) * lv_area_get_height(&img_area);
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * draw_dsc = (lv_draw_fill_dsc_t *) t->draw_dsc;

                /* Most simple case: just a plain rectangle (no radius, no gradient). */
                if((draw_dsc->radius != 0) || (draw_dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))
                    return 0;

                if((draw_dsc->opa >= LV_OPA_MAX) && (size < sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_FILL))) {
                    return 0;
                }
                if((draw_dsc->opa > LV_OPA_MIN) && (size < sunxifb_g2d_get_limit(SUNXI_G2D_LIMIT_OPA_FILL))) {
                    return 0;
                }

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_G2D;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;
                //lv_draw_buf_t * draw_buf = &layer_to_draw->draw_buf;

                if(!_g2d_cf_supported(layer_to_draw->color_format))
                    return 0;

                if(!_g2d_draw_img_supported(draw_dsc, size))
                    return 0;

                if(t->preference_score > 70) {
                    t->preference_score = 70;
                    t->preferred_draw_unit_id = DRAW_UNIT_ID_G2D;
                }
                return 1;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                const lv_image_dsc_t * img_dsc = draw_dsc->src;
                if(!_g2d_cf_supported(img_dsc->header.cf))
                    return 0;

                if(!_g2d_draw_img_supported(draw_dsc, size))
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

    /* Return if target buffer format is not supported. */
    if(!_g2d_cf_supported(layer->color_format))
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_G2D);

    /* Return 0 is no selection, some tasks can be supported only by other units. */
    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_G2D)
        return 0;

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(buf == NULL)
        return -1;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_g2d_unit->base_unit.target_layer = layer;
    draw_g2d_unit->base_unit.clip_area = &t->clip_area;
    draw_g2d_unit->task_act = t;

#if LV_USE_OS
    /* Let the render thread work. */
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

static void _g2d_execute_drawing(lv_draw_g2d_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_draw_unit_t * draw_unit = (lv_draw_unit_t *)u;
    /* Invalidate cache */
    lv_layer_t * layer = draw_unit->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &t->area, draw_unit->clip_area))
        return; /*Fully clipped, nothing to do*/
    lv_draw_buf_invalidate_cache(draw_buf->data, draw_buf->header.stride, draw_buf->header.cf, &draw_area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_g2d_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_g2d_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_g2d_layer(draw_unit, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}

#if LV_USE_OS
static void _g2d_render_thread_cb(void * ptr)
{
    lv_draw_g2d_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);

    while(1) {
        /*
         * Wait for sync if no task received or _draw_task_buf is empty.
         * The thread will have to run as much as there are pending tasks.
         */
        while(u->task_act == NULL) {
            lv_thread_sync_wait(&u->sync);
        }

        _g2d_execute_drawing(u);

        /* Cleanup. */
        u->task_act->state = LV_DRAW_TASK_STATE_READY;
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }
}
#endif

int lv_g2d_init()
{
    if(sunxi_g2d_open() < 0) {
        LV_LOG_WARN("Error: cannot open g2d device");
        return -1;
    }

    return true;
}

void lv_g2d_deinit(void)
{
    sunxi_g2d_close();
}

#endif /*LV_USE_DRAW_G2D*/
