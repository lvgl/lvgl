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

static int32_t lv_draw_pxp_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

#if LV_USE_OS
    static void _pxp_render_thread_cb(void * ptr);
#endif

static void _pxp_execute_drawing(lv_draw_pxp_unit_t * u);

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if 0
static void lv_draw_pxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_color_format_t cf);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_layer_t * lv_draw_pxp_layer_init(lv_disp_t * disp)
{
    lv_pxp_layer_t * pxp_layer = (lv_pxp_layer_t *)lv_draw_sw_layer_init(disp);

    pxp_layer->buffer_copy = lv_draw_pxp_buffer_copy;

    return pxp_layer;
}

void lv_draw_pxp_layer_deinit(lv_disp_t * disp, lv_layer_t * layer)
{
    lv_draw_sw_layer_deinit(disp, layer);
}

void lv_draw_pxp_init(void)
{
    lv_draw_pxp_unit_t * draw_pxp_unit = lv_draw_create_unit(sizeof(lv_draw_pxp_unit_t));
    draw_pxp_unit->base_unit.dispatch = lv_draw_pxp_dispatch;

    lv_pxp_init();

#if LV_USE_OS
    lv_thread_init(&draw_pxp_unit->thread, LV_THREAD_PRIO_HIGH, _pxp_render_thread_cb, 8 * 1024, draw_pxp_unit);
#endif
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

static bool _pxp_task_supported(lv_draw_task_t * t)
{
    bool is_supported = true;

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                const lv_draw_fill_dsc_t * draw_dsc = (lv_draw_fill_dsc_t *) t->draw_dsc;

                /* Most simple case: just a plain rectangle (no radius, no gradient). */
                if((draw_dsc->radius != 0) || (draw_dsc->grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE))
                    is_supported = false;

                break;
            }
#if 0
        case LV_DRAW_TASK_TYPE_BG_IMG: {
                const lv_draw_bg_img_dsc_t * draw_dsc = (lv_draw_bg_img_dsc_t *) t->draw_dsc;
                lv_img_src_t src_type = lv_img_src_get_type(draw_dsc->src);

                if(src_type != LV_IMG_SRC_SYMBOL) {
                    bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);

                    if(has_recolor
                       || (!_pxp_cf_supported(draw_dsc->img_header.cf))
                      )
                        is_supported = false;
                }

                break;
            }

        case LV_DRAW_TASK_TYPE_LAYER: {
                const lv_draw_img_dsc_t * draw_dsc = (lv_draw_img_dsc_t *) t->draw_dsc;
                lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

                /* Return supported so this task is marked as complete once it gets in execution. */
                if(layer_to_draw->buf == NULL)
                    break;

                bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);

                if(has_recolor
                   || (!_pxp_cf_supported(layer_to_draw->color_format))
                  )
                    is_supported = false;

                break;
            }
#endif
        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_img_dsc_t * draw_dsc = (lv_draw_img_dsc_t *) t->draw_dsc;
                const lv_img_dsc_t * img_dsc = draw_dsc->src;

                bool has_recolor = (draw_dsc->recolor_opa != LV_OPA_TRANSP);
                bool has_transform = (draw_dsc->angle != 0 || draw_dsc->zoom != LV_ZOOM_NONE);

                if(has_recolor || has_transform
                   || (!_pxp_cf_supported(img_dsc->header.cf))
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

static int32_t lv_draw_pxp_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_pxp_unit_t * draw_pxp_unit = (lv_draw_pxp_unit_t *) draw_unit;

    /* Return immediately if it's busy with draw task. */
    if(draw_pxp_unit->task_act)
        return 0;

    /* Return if target buffer format is not supported. */
    if(!_pxp_cf_supported(layer->color_format))
        return 0;

    /* Try to get an ready to draw. */
    lv_draw_task_t * t = lv_draw_get_next_available_task(layer, NULL);
    while(t != NULL) {
        if(_pxp_task_supported(t))
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

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_pxp_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BG_IMG:
            //lv_draw_pxp_bg_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_pxp_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            //lv_draw_pxp_layer(draw_unit, t->draw_dsc, &t->area);
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

#if 0
static void lv_draw_pxp_img_decoded(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                                    const lv_area_t * coords, const uint8_t * map_p, lv_color_format_t cf)
{
    if(dsc->opa <= (lv_opa_t)LV_OPA_MIN)
        return;

    const lv_color_t * src_buf = (const lv_color_t *)map_p;
    if(!src_buf) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t blend_area;

    bool has_mask = lv_draw_mask_is_any(&blend_area);
    bool has_scale = (dsc->zoom != LV_ZOOM_NONE);
    bool has_rotation = (dsc->angle != 0);
    bool unsup_rotation = false;

    if(has_rotation)
        lv_area_copy(&blend_area, &rel_coords);
    else if(!_lv_area_intersect(&blend_area, &rel_coords, &rel_clip_area))
        return; /*Fully clipped, nothing to do*/

    lv_coord_t src_width = lv_area_get_width(coords);
    lv_coord_t src_height = lv_area_get_height(coords);

    if(has_rotation) {
        /*
         * PXP can only rotate at 90x angles.
         */
        if(dsc->angle % 900) {
            PXP_LOG_TRACE("Rotation angle %d is not supported. PXP can rotate only 90x angle.", dsc->angle);
            unsup_rotation = true;
        }

        /*
         * PXP is set to process 16x16 blocks to optimize the system for memory
         * bandwidth and image processing time.
         * The output engine essentially truncates any output pixels after the
         * desired number of pixels has been written.
         * When rotating a source image and the output is not divisible by the block
         * size, the incorrect pixels could be truncated and the final output image
         * can look shifted.
         */
        if(src_width % 16 || src_height % 16) {
            PXP_LOG_TRACE("Rotation is not supported for image w/o alignment to block size 16x16.");
            unsup_rotation = true;
        }
    }

    if(has_mask || has_scale || unsup_rotation || lv_area_get_size(&blend_area) < LV_GPU_NXP_PXP_SIZE_LIMIT
#if LV_COLOR_DEPTH != 32
       || lv_color_format_has_alpha(cf)
#endif
      ) {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }

    lv_color_t * dest_buf = draw_ctx->buf;
    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

    lv_area_t src_area;
    src_area.x1 = blend_area.x1 - (coords->x1 - draw_ctx->buf_area->x1);
    src_area.y1 = blend_area.y1 - (coords->y1 - draw_ctx->buf_area->y1);
    src_area.x2 = src_area.x1 + src_width - 1;
    src_area.y2 = src_area.y1 + src_height - 1;
    lv_coord_t src_stride = lv_area_get_width(coords);

    lv_gpu_nxp_pxp_blit_transform(dest_buf, &blend_area, dest_stride, src_buf, &src_area, src_stride,
                                  dsc, cf);
}
#endif

#endif /*LV_USE_DRAW_PXP*/
