/**
 * @file lv_draw_vglite.c
 *
 */

/**
 * Copyright 2023 NXP
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

#include "../../../disp/lv_disp_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t lv_draw_vglite_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

#if LV_USE_OS
    static void _vglite_render_thread_cb(void * ptr);
#endif

static void _vglite_execute_drawing(lv_draw_vglite_unit_t * u);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_layer_t * lv_draw_vglite_layer_init(lv_disp_t * disp)
{
    lv_vglite_layer_t * vglite_layer = (lv_vglite_layer_t *)lv_draw_sw_layer_init(disp);

    return vglite_layer;
}

void lv_draw_vglite_layer_deinit(lv_disp_t * disp, lv_layer_t * layer)
{
    lv_draw_sw_layer_deinit(disp, layer);
}

void lv_draw_vglite_init(void)
{
    lv_draw_vglite_unit_t * draw_vglite_unit = lv_draw_create_unit(sizeof(lv_draw_vglite_unit_t));
    draw_vglite_unit->base_unit.dispatch = lv_draw_vglite_dispatch;

#if LV_USE_OS
    lv_thread_init(&draw_vglite_unit->thread, LV_THREAD_PRIO_HIGH, _vglite_render_thread_cb, 8 * 1024, draw_vglite_unit);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline bool _vglite_cf_supported(lv_color_format_t cf)
{
    // Add here the platform specific code for supported formats.

    bool is_cf_unsupported = (cf == LV_COLOR_FORMAT_RGB565A8 || cf == LV_COLOR_FORMAT_RGB888);

    return (!is_cf_unsupported);
}

static bool _vglite_task_supported(lv_draw_task_t * t)
{
    bool is_supported = true;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            break;

        case LV_DRAW_TASK_TYPE_BORDER: {
                const lv_draw_border_dsc_t * draw_dsc = (lv_draw_border_dsc_t *) t->draw_dsc;

                if(draw_dsc->side != (lv_border_side_t)LV_BORDER_SIDE_FULL)
                    is_supported = false;

                break;
            }

        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_img_dsc_t * draw_dsc = (lv_draw_img_dsc_t *) t->draw_dsc;
                const lv_img_dsc_t * img_dsc = draw_dsc->src;

                bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
#if VGLITE_BLIT_SPLIT_ENABLED
                bool has_transform = (draw_dsc->angle != 0 || draw_dsc->zoom != LV_ZOOM_NONE);
#endif

                if(has_recolor
#if VGLITE_BLIT_SPLIT_ENABLED
                   || has_transform
#endif
                   || (!_vglite_cf_supported(img_dsc->header.cf))
                   || (!vglite_buf_aligned(img_dsc->data, img_dsc->header.w))
                  )
                    is_supported = false;

                break;
            }
        default:
            is_supported = false;
            break;
    }

    return is_supported;
}

static int32_t lv_draw_vglite_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_vglite_unit_t * draw_vglite_unit = (lv_draw_vglite_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task. */
    if(draw_vglite_unit->task_act)
        return 0;

    /* Return if target buffer format is not supported. */
    if(!_vglite_cf_supported(layer->color_format))
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL);
    while(t != NULL) {
        if(_vglite_task_supported(t))
            break;

        t = lv_draw_get_next_available_task(layer, t);
    }

    /* Return 0 is no selection, some tasks can be supported only by SW. */
    if(t == NULL)
        return 0;

    /* If the buffer of the layer is not allocated yet, allocate it now. */
    if(layer->buf == NULL) {
        uint32_t px_size = lv_color_format_get_size(layer->color_format);
        uint32_t layer_size_byte = lv_area_get_size(&layer->buf_area) * px_size;

        uint8_t * buf = lv_malloc(layer_size_byte);
        if(buf == NULL) {
            LV_LOG_WARN("Allocating %d bytes of layer buffer failed. Try later", layer_size_byte);
            return -1;
        }
        LV_ASSERT_MALLOC(buf);
        lv_draw_add_used_layer_size(layer_size_byte < 1024 ? 1 : layer_size_byte >> 10);

        layer->buf = buf;

        if(lv_color_format_has_alpha(layer->color_format)) {
            layer->buffer_clear(layer, &layer->buf_area);
        }
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_vglite_unit->base_unit.target_layer = layer;
    draw_vglite_unit->base_unit.clip_area = &t->clip_area;
    draw_vglite_unit->task_act = t;

#if LV_USE_OS
    /* Let the render thread work. */
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

static void _vglite_execute_drawing(lv_draw_vglite_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;
    lv_draw_unit_t * draw_unit = (lv_draw_unit_t *)u;

    /* Set target buffer */
    lv_layer_t * layer = draw_unit->target_layer;
    vglite_set_dest_buf(layer->buf, &layer->buf_area, lv_area_get_width(&layer->buf_area), layer->color_format);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_vglite_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_vglite_border(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_vglite_img(draw_unit, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}

#if LV_USE_OS
static void _vglite_render_thread_cb(void * ptr)
{
    lv_draw_vglite_unit_t * u = ptr;

    lv_thread_sync_init(&u->sync);

    while(1) {
        /*
         * Wait for sync if no task received or _draw_task_buf is empty.
         * The thread will have to run as much as there are pending tasks.
         */
        while(u->task_act == NULL) {
            lv_thread_sync_wait(&u->sync);
        }

        _vglite_execute_drawing(u);

        u->task_act->state = LV_DRAW_TASK_STATE_READY;

        /* Cleanup. */
        u->task_act = NULL;

        /* The draw unit is free now. Request a new dispatching as it can get a new task. */
        lv_draw_dispatch_request();
    }
}
#endif

#endif /*LV_USE_DRAW_VGLITE*/
