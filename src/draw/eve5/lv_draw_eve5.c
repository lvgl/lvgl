/**
 * @file lv_draw_eve5.c
 *
 * EVE5 (BT820) Draw Unit for LVGL — Integration and Orchestration
 *
 * Architecture:
 * - Uses QUEUED state to accumulate tasks until all are added
 * - Each layer renders atomically when no WAITING tasks remain but QUEUED exist
 * - LVGL's dispatch-all-layers behavior ensures children complete before parents
 * - Each layer has its own complete display list cycle:
 *   dlStart -> commands -> display -> swap -> graphicsFinish
 * - Screen partial textures are handed to the display driver via draw_buf->vram_res
 * - Child layer textures are freed via lv_draw_buf_destroy -> vram_free_cb
 *
 * Contains LVGL entry points (init, evaluate, dispatch) and the top-level
 * layer rendering orchestrator. Render processing loops are in
 * lv_draw_eve5_render.c, SW fallback in lv_draw_eve5_sw_fallback.c.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../lv_draw_image.h"
#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#if LV_USE_OS
#include "../../drivers/display/eve5/lv_eve5.h"
#endif

/*********************
 * DEFINES
 *********************/
#define DRAW_UNIT_ID_EVE5 82

#if 0
#define EVE5_LOG(...) LV_LOG_INFO(__VA_ARGS__)
static const char *task_state_str(lv_draw_task_state_t state)
{
    switch(state) {
        case LV_DRAW_TASK_STATE_WAITING:     return "WAITING";
        case LV_DRAW_TASK_STATE_QUEUED:      return "QUEUED";
        case LV_DRAW_TASK_STATE_IN_PROGRESS: return "IN_PROGRESS";
        case LV_DRAW_TASK_STATE_FINISHED:    return "FINISHED";
        case LV_DRAW_TASK_STATE_BLOCKED:     return "BLOCKED";
        default:                              return "UNKNOWN";
    }
}
static const char *task_type_str(lv_draw_task_type_t type)
{
    switch(type) {
        case LV_DRAW_TASK_TYPE_FILL:       return "FILL";
        case LV_DRAW_TASK_TYPE_BORDER:     return "BORDER";
        case LV_DRAW_TASK_TYPE_LINE:       return "LINE";
        case LV_DRAW_TASK_TYPE_TRIANGLE:   return "TRIANGLE";
        case LV_DRAW_TASK_TYPE_LABEL:      return "LABEL";
        case LV_DRAW_TASK_TYPE_LETTER:     return "LETTER";
        case LV_DRAW_TASK_TYPE_IMAGE:      return "IMAGE";
        case LV_DRAW_TASK_TYPE_ARC:        return "ARC";
        case LV_DRAW_TASK_TYPE_LAYER:      return "LAYER";
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: return "BOX_SHADOW";
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE: return "MASK_RECT";
        default:                           return "OTHER";
    }
}
#else
#define EVE5_LOG(...) do {} while(0)
#endif

/*
 * QA Configuration: Force SW rendering for specific task types.
 * Set to 1 to render via software fallback, 0 for hardware rendering.
 * Useful for comparing HW vs SW output during QA testing.
 */
#ifndef LV_DRAW_EVE5_SW_FILL
#define LV_DRAW_EVE5_SW_FILL 0
#endif

#ifndef LV_DRAW_EVE5_SW_BORDER
#define LV_DRAW_EVE5_SW_BORDER 0
#endif

#ifndef LV_DRAW_EVE5_SW_LINE
#define LV_DRAW_EVE5_SW_LINE 0
#endif

#ifndef LV_DRAW_EVE5_SW_TRIANGLE
#define LV_DRAW_EVE5_SW_TRIANGLE 0
#endif

#ifndef LV_DRAW_EVE5_SW_LABEL
#define LV_DRAW_EVE5_SW_LABEL 0
#endif

#ifndef LV_DRAW_EVE5_SW_ARC
#define LV_DRAW_EVE5_SW_ARC 0
#endif

#ifndef LV_DRAW_EVE5_SW_BOX_SHADOW
#define LV_DRAW_EVE5_SW_BOX_SHADOW 0
#endif

#ifndef LV_DRAW_EVE5_SW_CANVAS
#define LV_DRAW_EVE5_SW_CANVAS 0  /* Set to 1 to let SW renderer handle canvas layers */
#endif

/**********************
 * STATIC PROTOTYPES
 **********************/
static int32_t dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer);
static int32_t evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task);
static void eve5_render_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer);

/**********************
 * GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve5_init(EVE_HalContext *hal, Esd_GpuAlloc *allocator)
{
    lv_draw_eve5_unit_t *unit = lv_draw_create_unit(sizeof(lv_draw_eve5_unit_t));
    unit->base_unit.dispatch_cb = dispatch;
    unit->base_unit.evaluate_cb = evaluate;
    unit->base_unit.name = "EVE5_BT820";
    unit->hal = hal;
    unit->allocator = allocator;
    unit->rendering_in_progress = false;
    unit->frame_alloc_count = 0;

    /* Initialize caches */
    lv_draw_eve5_image_cache_init(&unit->image_cache, EVE5_IMAGE_CACHE_CAPACITY);
    lv_draw_eve5_glyph_cache_init(&unit->glyph_cache, EVE5_GLYPH_CACHE_CAPACITY);
    lv_draw_eve5_sw_cache_init(unit);

    lv_draw_eve5_register_vram_callbacks(unit);

    /* Register lightweight image decoder for JPEG/PNG header parsing.
     * This allows lv_image_set_src("path.jpg") to succeed without a
     * full SW JPEG decoder — the EVE5 draw unit handles HW decode. */
#if EVE5_HW_IMAGE_DECODE
    lv_draw_eve5_register_image_decoder();
#endif

    LV_LOG_INFO("EVE5: Draw unit initialized, ID=%d", DRAW_UNIT_ID_EVE5);

    /* Log SW fallback configuration */
    LV_LOG_INFO("EVE5: SW fallback config: FILL=%d BORDER=%d LINE=%d TRI=%d LABEL=%d ARC=%d SHADOW=%d CANVAS=%d",
                LV_DRAW_EVE5_SW_FILL, LV_DRAW_EVE5_SW_BORDER, LV_DRAW_EVE5_SW_LINE,
                LV_DRAW_EVE5_SW_TRIANGLE, LV_DRAW_EVE5_SW_LABEL, LV_DRAW_EVE5_SW_ARC,
                LV_DRAW_EVE5_SW_BOX_SHADOW, LV_DRAW_EVE5_SW_CANVAS);
}

void lv_draw_eve5_deinit(void)
{
    LV_LOG_INFO("EVE5: Draw unit deinitialized");
}

/**********************
 * DISPATCH/EVALUATE
 **********************/

static int32_t evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task)
{
    LV_UNUSED(draw_unit);

    /* Skip tasks marked for SW fallback (re-issued with user_data set) */
    if(((lv_draw_dsc_base_t *)task->draw_dsc)->user_data != NULL) {
        EVE5_LOG("EVE5: Evaluate: type=%s -> SW fallback", task_type_str(task->type));
        return 0;
    }

#if LV_DRAW_EVE5_SW_CANVAS
    /* Decline tasks on canvas layers — let SW renderer handle them.
     * Canvas layer: draw_buf != NULL, parent == NULL, not the screen layer. */
    lv_layer_t *target = task->target_layer;
    if(target != NULL && target->draw_buf != NULL && target->parent == NULL) {
        lv_display_t *disp = lv_display_get_default();
        if(!disp || target != disp->layer_head) {
            EVE5_LOG("EVE5: Evaluate: type=%s -> declined (canvas layer)", task_type_str(task->type));
            return 0;
        }
    }
#endif

    task->preference_score = 10;
    task->preferred_draw_unit_id = DRAW_UNIT_ID_EVE5;

    EVE5_LOG("EVE5: Evaluate: type=%s -> claimed", task_type_str(task->type));

    return 0;
}

static int32_t dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer)
{
    lv_draw_eve5_unit_t *u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_draw_task_t *t;

    if(u->rendering_in_progress) {
        EVE5_LOG("EVE5: Dispatch skipped - render in progress");
        return 0;
    }

    /* Count task states for this layer */
    int32_t waiting_count = 0;
    int32_t queued_count = 0;
    int32_t blocked_count = 0;
    int32_t finished_count = 0;
    int32_t in_progress_count = 0;

    t = layer->draw_task_head;
    while(t) {
        if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5) {
            switch(t->state) {
                case LV_DRAW_TASK_STATE_WAITING:     waiting_count++;     break;
                case LV_DRAW_TASK_STATE_QUEUED:      queued_count++;      break;
                case LV_DRAW_TASK_STATE_BLOCKED:     blocked_count++;     break;
                case LV_DRAW_TASK_STATE_FINISHED:    finished_count++;    break;
                case LV_DRAW_TASK_STATE_IN_PROGRESS: in_progress_count++; break;
            }
        }
        t = t->next;
    }

    EVE5_LOG("EVE5: Dispatch layer=%p parent=%p | W=%d Q=%d B=%d F=%d P=%d",
             (void *)layer, (void *)layer->parent,
             waiting_count, queued_count, blocked_count, finished_count, in_progress_count);

    /* Debug: print each task */
    t = layer->draw_task_head;
    int task_idx = 0;
    while(t) {
        if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5) {
            EVE5_LOG("  [%d] type=%-10s state=%-11s area=(%d,%d)-(%d,%d)",
                   task_idx,
                   task_type_str(t->type),
                   task_state_str(t->state),
                   t->area.x1, t->area.y1, t->area.x2, t->area.y2);
        }
        task_idx++;
        t = t->next;
    }

    /* Case 1: New tasks arrived - claim them by marking QUEUED */
    if(waiting_count > 0) {
        EVE5_LOG("EVE5: -> Queueing %d tasks", waiting_count);

        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_WAITING) {
                t->state = LV_DRAW_TASK_STATE_QUEUED;
            } else if (t->state != LV_DRAW_TASK_STATE_QUEUED) {
              EVE5_LOG("EVE5: -> Skipping task type=%s state=%s",
                      task_type_str(t->type),
                    task_state_str(t->state));
            }
            t = t->next;
        }

        lv_draw_dispatch_request();
        return waiting_count;
    }

    /* Case 2: No new tasks, but we have queued work - render! */
    if (queued_count > 0 && layer->all_tasks_added && blocked_count == 0 && waiting_count == 0) {
        EVE5_LOG("EVE5: -> Rendering %d queued tasks atomically", queued_count);

        /* Allocate/migrate the layer target buffer for this draw unit */
        lv_draw_layer_alloc_buf(layer, draw_unit);

        /* Ensure all queued tasks' source buffers are resident */
        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                lv_draw_buf_ensure_task_sources_resident(t, draw_unit);
            }
            t = t->next;
        }

        eve5_render_layer(u, layer);
        lv_draw_dispatch_request();
        return 1;
    }

    /* Case 3: Nothing for us to do */
    EVE5_LOG("EVE5: -> IDLE");
    return LV_DRAW_UNIT_IDLE;
}

/**********************
 * LAYER RENDERING
 **********************/

static void eve5_render_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer)
{
    lv_draw_task_t *t;
    lv_display_t *disp = lv_display_get_default();
    bool is_screen = (layer->parent == NULL && disp && layer == disp->layer_head);

    u->rendering_in_progress = true;

#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif

    /* Detect canvas layer: pre-allocated draw_buf, no parent, not the screen layer.
     * Canvas layers require persistent GPU allocation and content preservation. */
    bool is_canvas = (layer->draw_buf != NULL && layer->parent == NULL && !is_screen);

    /* Check if layer format has alpha channel.
     * - Canvas layers: use draw_buf->header.cf (what user requested)
     * - Screen/child layers: use color_format (set by LVGL/display)
     * Non-alpha layers (RGB565, RGB888, XRGB8888) skip the alpha pass since their alpha
     * is discarded anyway — instead we fill alpha=255 for correct compositing. */
    lv_color_format_t layer_cf;
    if(is_canvas && layer->draw_buf) {
        layer_cf = layer->draw_buf->header.cf;
    }
    else {
        layer_cf = layer->color_format;
    }
    bool layer_has_alpha = lv_color_format_has_alpha(layer_cf);

    EVE5_LOG("EVE5: === RENDER START layer=%p is_screen=%d is_canvas=%d has_alpha=%d ===",
             (void *)layer, is_screen, is_canvas, layer_has_alpha);
    EVE5_LOG("EVE5: Layer buf_area=(%d,%d)-(%d,%d) clip=(%d,%d)-(%d,%d)",
             layer->buf_area.x1, layer->buf_area.y1,
             layer->buf_area.x2, layer->buf_area.y2,
             layer->_clip_area.x1, layer->_clip_area.y1,
             layer->_clip_area.x2, layer->_clip_area.y2);

    /* Canvas optimization: if canvas has a single simple image task, load directly.
     * This avoids the render pipeline overhead and keeps the image in native format. */
    if(is_canvas && lv_draw_eve5_try_canvas_direct_image(u, layer)) {
        u->rendering_in_progress = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return;
    }

    /* Canvas cache removed — VRAM lifecycle is managed by vram_res on draw_buf */

    /* Reset alpha repair tracking for this layer */
    u->has_alpha_opaque = false;
    u->has_alpha_trashed = false;
    u->alpha_needs_rendertarget = false;

    /* Opaque area pre-pass: determine largest opaque fill before rendering.
     * Only needed for layers with alpha (screen layers and non-alpha formats skip). */
    if(!is_screen && layer_has_alpha) {
        lv_draw_eve5_opaque_prepass(u, layer);
#if EVE5_USE_RENDERTARGET_ALPHA
        lv_draw_eve5_check_alpha_recovery(u, layer);
#endif
    }

#if EVE5_USE_RENDERTARGET_ALPHA
    /* Render-target alpha pass: when the layer contains tasks with inaccurate
     * stencil-based alpha recovery, render alpha coverage into a separate L8
     * render target BEFORE the main ARGB8 layer is initialized. The L8 result
     * is blitted into the ARGB8 alpha channel after the RGB pass, replacing
     * the direct-to-alpha correction pass entirely. */
    Esd_GpuHandle alpha_rt_handle = GA_HANDLE_INVALID;
    uint32_t alpha_rt_addr = GA_INVALID;
    int32_t alpha_rt_aw = 0, alpha_rt_ah = 0, alpha_rt_w = 0, alpha_rt_h = 0;

    if(!is_screen && layer_has_alpha && u->alpha_needs_rendertarget) {
        alpha_rt_w = lv_area_get_width(&layer->buf_area);
        alpha_rt_h = lv_area_get_height(&layer->buf_area);
        alpha_rt_aw = ALIGN_UP(alpha_rt_w, 16);
        alpha_rt_ah = ALIGN_UP(alpha_rt_h, 16);
        alpha_rt_handle = lv_draw_eve5_render_alpha_to_l8(u, layer,
                              alpha_rt_aw, alpha_rt_ah, alpha_rt_w, alpha_rt_h);
        alpha_rt_addr = Esd_GpuAlloc_Get(u->allocator, alpha_rt_handle);
        if(alpha_rt_addr == GA_INVALID) {
            u->alpha_needs_rendertarget = false;  /* fall back to direct-to-alpha */
        }
    }
#endif

    /* Initialize the layer (allocate ARGB8 texture, start display list) */
    lv_draw_eve5_hal_init_layer(u, layer, is_screen, is_canvas);

    /* Check for allocation failure */
    if(eve5_get_vram_res(layer) == NULL) {
        LV_LOG_ERROR("EVE5: Layer allocation failed!");

        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                t->state = LV_DRAW_TASK_STATE_FINISHED;
            }
            t = t->next;
        }
        u->rendering_in_progress = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return;
    }

    /* Advance SW cache frame counter */
    lv_draw_eve5_sw_cache_new_frame(u);

    /* RGB render pass: process all QUEUED tasks.
     * finish_tasks=true when no alpha pass will follow (tasks can be marked FINISHED
     * immediately): screen layer, non-alpha format, or L8 render-target path. */
    bool finish_tasks = is_screen || !layer_has_alpha
#if EVE5_USE_RENDERTARGET_ALPHA
                        || u->alpha_needs_rendertarget
#endif
                        ;
    int rendered_count = lv_draw_eve5_render_tasks(u, layer, is_screen, finish_tasks);

    /* Alpha recovery: either blit L8 render target or run direct-to-alpha pass.
     * Skip for screen layers — their alpha isn't used for compositing.
     * For non-alpha layers (RGB565, etc.), fill alpha=255 instead of full pass. */
    if(!is_screen) {
        if(layer_has_alpha) {
#if EVE5_USE_RENDERTARGET_ALPHA
            if(u->alpha_needs_rendertarget && alpha_rt_addr != GA_INVALID) {
                lv_draw_eve5_hal_blit_l8_to_alpha(u, alpha_rt_addr,
                                                   alpha_rt_aw, alpha_rt_ah,
                                                   alpha_rt_w, alpha_rt_h);
                track_frame_alloc(u, alpha_rt_handle);
            } else
#endif
            {
                lv_draw_eve5_alpha_pass(u, layer);
            }
        }
        /* else: Non-alpha format (RGB565, RGB8) rendered directly to native
         * render target — no alpha channel exists, nothing to fill. */
    }

    /* Deferred mask_rect tasks: process MASK_RECTANGLE tasks that were
     * skipped during the main loop (non-screen layers only). These run
     * after the alpha pass so they operate on fully corrected premultiplied
     * RGBA. The mask scales all four channels — not just alpha — because
     * premultiplied content requires RGB to be scaled alongside alpha to
     * avoid white fringing at partially masked edges. This is also more
     * efficient than adding a separate bitmap redraw in the alpha pass,
     * since the mask geometry is a simple rounded rect. */
    if(!is_screen) {
        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->type == LV_DRAW_TASK_TYPE_MASK_RECTANGLE &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
                lv_draw_eve5_hal_draw_mask_rect(u, t);
                t->state = LV_DRAW_TASK_STATE_FINISHED;
                rendered_count++;
            }
            t = t->next;
        }
    }

    /* Deferred bitmap mask: if this child layer's parent has a LAYER task
     * referencing us with bitmap_mask_src, apply the mask here — same
     * technique as mask_rect (scale all premultiplied RGBA uniformly).
     * Then clear bitmap_mask_src on the parent's task so any draw unit
     * compositing the LAYER task uses standard blend without double-masking.
     *
     * This must run after alpha correction so all channels are correct,
     * and before hal_finish_layer so the DL is still open. */
    if(!is_screen && !is_canvas && layer->parent != NULL) {
        lv_draw_task_t *pt = layer->parent->draw_task_head;
        while(pt) {
            if(pt->type == LV_DRAW_TASK_TYPE_LAYER) {
                lv_draw_image_dsc_t *layer_dsc = pt->draw_dsc;
                lv_layer_t *child_ref = (lv_layer_t *)layer_dsc->src;
                if(child_ref == layer && layer_dsc->bitmap_mask_src != NULL) {
                    lv_draw_eve5_apply_bitmap_mask(u, layer, layer_dsc);
                    layer_dsc->bitmap_mask_src = NULL;  /* Consumed — don't apply again */
                    break;
                }
            }
            pt = pt->next;
        }
    }

    /* Child layer GPU memory lifecycle is managed by lv_draw_buf_destroy
     * via vram_free_cb — no manual cleanup needed here. */

    EVE5_LOG("EVE5: Finishing layer, rendered %d tasks", rendered_count);
    lv_draw_eve5_hal_finish_layer(u, layer, is_screen, is_canvas);

    EVE5_LOG("EVE5: === RENDER END layer=%p ===", (void *)layer);

    u->rendering_in_progress = false;

#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif
}

#endif /* LV_USE_DRAW_EVE5 */
