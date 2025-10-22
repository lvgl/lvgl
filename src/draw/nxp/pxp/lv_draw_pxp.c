/**
 * @file lv_draw_pxp.c
 *
 */

/**
 * Copyright 2022-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_pxp.h"

#if LV_USE_PXP
#if LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP
#include "lv_pxp_cfg.h"
#include "lv_pxp_utils.h"

#if LV_USE_PARALLEL_DRAW_DEBUG
    #include "../../../core/lv_global.h"
#endif

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
 * Evaluate a task and set the score and preferred PXP unit.
 * Return 1 if task is preferred, 0 otherwise (task is not supported).
 */
static int32_t _pxp_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);

/*
 * Dispatch a task to the PXP unit.
 * Return 1 if task was dispatched, 0 otherwise (task not supported).
 */
static int32_t _pxp_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

/*
 * Delete the PXP draw unit.
 */
static int32_t _pxp_delete(lv_draw_unit_t * draw_unit);

#if LV_USE_PXP_DRAW_THREAD
    static void _pxp_render_thread_cb(void * ptr);
#endif

static void _pxp_execute_drawing(lv_draw_pxp_unit_t * u);

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_PARALLEL_DRAW_DEBUG
    #define _draw_info LV_GLOBAL_DEFAULT()->draw_info
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_pxp_init(void)
{
    lv_pxp_init();

#if LV_USE_DRAW_PXP
    lv_draw_buf_pxp_init_handlers();

    lv_draw_pxp_unit_t * draw_pxp_unit = lv_draw_create_unit(sizeof(lv_draw_pxp_unit_t));
    draw_pxp_unit->base_unit.evaluate_cb = _pxp_evaluate;
    draw_pxp_unit->base_unit.dispatch_cb = _pxp_dispatch;
    draw_pxp_unit->base_unit.delete_cb = _pxp_delete;
    draw_pxp_unit->base_unit.name = "NXP_PXP";

#if LV_USE_PXP_DRAW_THREAD
    lv_thread_init(&draw_pxp_unit->thread, "pxpdraw", LV_DRAW_THREAD_PRIO, _pxp_render_thread_cb, 2 * 1024, draw_pxp_unit);
#endif
#endif /*LV_USE_DRAW_PXP*/
}

void lv_draw_pxp_deinit(void)
{
    lv_pxp_deinit();
}

void lv_draw_pxp_rotate(const void * src_buf, void * dest_buf, int32_t src_width, int32_t src_height,
                        int32_t src_stride, int32_t dest_stride, lv_display_rotation_t rotation,
                        lv_color_format_t cf)
{
    lv_pxp_reset();

    /* Convert rotation angle
     * To be in sync with CPU, the received angle is counterclockwise
     * and the PXP constants are for clockwise rotation
     *
     *    counterclockwise          clockwise
     * LV_DISPLAY_ROTATION_90  -> kPXP_Rotate270
     * LV_DISPLAY_ROTATION_270 -> kPXP_Rotate90
     */
    pxp_rotate_degree_t pxp_rotation;
    switch(rotation) {
        case LV_DISPLAY_ROTATION_0:
            pxp_rotation = kPXP_Rotate0;
            break;
        case LV_DISPLAY_ROTATION_270:
            pxp_rotation = kPXP_Rotate90;
            break;
        case LV_DISPLAY_ROTATION_180:
            pxp_rotation = kPXP_Rotate180;
            break;
        case LV_DISPLAY_ROTATION_90:
            pxp_rotation = kPXP_Rotate270;
            break;
        default:
            pxp_rotation = kPXP_Rotate0;
            break;
    }
    PXP_SetRotateConfig(PXP_ID, kPXP_RotateOutputBuffer, pxp_rotation, kPXP_FlipDisable);

    /*Simple blit, no effect - Disable PS buffer*/
    PXP_SetProcessSurfacePosition(PXP_ID, 0xFFFFU, 0xFFFFU, 0U, 0U);

    /*AS buffer - source image*/
    pxp_as_buffer_config_t asBufferConfig = {
        .pixelFormat = pxp_get_as_px_format(cf),
        .bufferAddr = (uint32_t)src_buf,
        .pitchBytes = src_stride
    };
    PXP_SetAlphaSurfaceBufferConfig(PXP_ID, &asBufferConfig);
    PXP_SetAlphaSurfacePosition(PXP_ID, 0U, 0U, src_width - 1U, src_height - 1U);
    PXP_EnableAlphaSurfaceOverlayColorKey(PXP_ID, false);

    /*Output buffer.*/
    pxp_output_buffer_config_t outputBufferConfig = {
        .pixelFormat = pxp_get_out_px_format(cf),
        .interlacedMode = kPXP_OutputProgressive,
        .buffer0Addr = (uint32_t)dest_buf,
        .buffer1Addr = (uint32_t)0U,
        .pitchBytes = dest_stride,
        .width = src_width,
        .height = src_height
    };
    PXP_SetOutputBufferConfig(PXP_ID, &outputBufferConfig);

    lv_pxp_run();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if LV_USE_DRAW_PXP
static inline bool _pxp_src_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static inline bool _pxp_dest_cf_supported(lv_color_format_t cf)
{
    bool is_cf_supported = false;

    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            is_cf_supported = true;
            break;
        default:
            break;
    }

    return is_cf_supported;
}

static bool _pxp_draw_img_supported(const lv_draw_image_dsc_t * draw_dsc)
{
    const lv_image_dsc_t * img_dsc = draw_dsc->src;

    bool has_recolor = (draw_dsc->recolor_opa > LV_OPA_MIN);
    bool has_transform = (draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                          draw_dsc->scale_y != LV_SCALE_NONE);

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

    const lv_draw_dsc_base_t * draw_dsc_base = (lv_draw_dsc_base_t *) t->draw_dsc;

    if(!_pxp_dest_cf_supported(draw_dsc_base->layer->color_format))
        return 0;

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

        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) t->draw_dsc;
                lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

                if(!_pxp_src_cf_supported(layer_to_draw->color_format))
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

                if(img_dsc->header.cf >= LV_COLOR_FORMAT_PROPRIETARY_START)
                    return 0;

                if((!_pxp_src_cf_supported(img_dsc->header.cf)) ||
                   (!pxp_buf_aligned(img_dsc->data, img_dsc->header.stride)))
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

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_PXP);

    if(t == NULL || t->preferred_draw_unit_id != DRAW_UNIT_ID_PXP)
        return LV_DRAW_UNIT_IDLE;

    if(lv_draw_layer_alloc_buf(layer) == NULL)
        return LV_DRAW_UNIT_IDLE;

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_pxp_unit->task_act = t;

#if LV_USE_PXP_DRAW_THREAD
    /* Let the render thread work. */
    if(draw_pxp_unit->inited)
        lv_thread_sync_signal(&draw_pxp_unit->sync);
#else
    _pxp_execute_drawing(draw_pxp_unit);

    draw_pxp_unit->task_act->state = LV_DRAW_TASK_STATE_FINISHED;
    draw_pxp_unit->task_act = NULL;

    /* The draw unit is free now. Request a new dispatching as it can get a new task. */
    lv_draw_dispatch_request();
#endif

    return 1;
}

static int32_t _pxp_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_PXP_DRAW_THREAD
    lv_draw_pxp_unit_t * draw_pxp_unit = (lv_draw_pxp_unit_t *) draw_unit;

    LV_LOG_INFO("Cancel PXP draw thread.");
    draw_pxp_unit->exit_status = true;

    if(draw_pxp_unit->inited)
        lv_thread_sync_signal(&draw_pxp_unit->sync);

    lv_result_t res = lv_thread_delete(&draw_pxp_unit->thread);

    return res;
#else
    LV_UNUSED(draw_unit);

    return 0;
#endif
}

static void _pxp_execute_drawing(lv_draw_pxp_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_layer_t * layer = t->target_layer;
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &t->area, &t->clip_area))
        return; /*Fully clipped, nothing to do*/

    /* Make area relative to the buffer */
    lv_area_move(&draw_area, -layer->buf_area.x1, -layer->buf_area.y1);

    /* Invalidate only the drawing area */
    lv_draw_buf_invalidate_cache(draw_buf, &draw_area);

#if LV_USE_PARALLEL_DRAW_DEBUG
    t->draw_unit = &u->base_unit;
#endif

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_pxp_fill(t);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_pxp_img(t);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_pxp_layer(t);
            break;
        default:
            break;
    }

#if LV_USE_PARALLEL_DRAW_DEBUG
    /*Layers manage it for themselves*/
    if(t->type != LV_DRAW_TASK_TYPE_LAYER) {
        lv_area_t draw_area;
        if(!lv_area_intersect(&draw_area, &t->area, &t->clip_area))
            return;

        int32_t idx = u->base_unit.idx;

        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.bg_color = lv_palette_main(idx % LV_PALETTE_LAST);
        rect_dsc.border_color = rect_dsc.bg_color;
        rect_dsc.bg_opa = LV_OPA_10;
        rect_dsc.border_opa = LV_OPA_80;
        rect_dsc.border_width = 1;
        lv_draw_sw_fill((lv_draw_unit_t *)u, &rect_dsc, &draw_area);

        lv_point_t txt_size;
        lv_text_get_size_attributes(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

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

#if LV_USE_PXP_DRAW_THREAD
static void _pxp_render_thread_cb(void * ptr)
{
    lv_draw_pxp_unit_t * u = ptr;

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
            LV_LOG_INFO("Ready to exit PXP draw thread.");
            break;
        }

        _pxp_execute_drawing(u);

        /* Signal the ready state to dispatcher. */
        u->task_act->state = LV_DRAW_TASK_STATE_FINISHED;

        /* Cleanup. */
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }

    u->inited = false;
    lv_thread_sync_delete(&u->sync);
    LV_LOG_INFO("Exit PXP draw thread.");
}
#endif
#endif /*LV_USE_DRAW_PXP*/
#endif /*LV_USE_DRAW_PXP || LV_USE_ROTATE_PXP*/
#endif /*LV_USE_PXP*/
