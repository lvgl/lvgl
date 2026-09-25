/**
 * @file lv_draw_eve5.c
 *
 * EVE5 (BT820) Draw Unit for LVGL: Integration and Orchestration
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
 * Alpha Recovery:
 * EVE hardware applies the same blend equation to all four channels. With standard
 * SRC_ALPHA/ONE_MINUS_SRC_ALPHA blending, this produces:
 *
 *   dst_a = src_a * src_a + dst_a * (1 - src_a)
 *
 * But correct Porter-Duff "over" compositing requires:
 *
 *   dst_a = src_a + dst_a * (1 - src_a)
 *
 * The source alpha gets squared, making semi-transparent content increasingly
 * more transparent than intended. Additionally, some effects (rounded corners,
 * complex masks) use the alpha channel as a temporary mask buffer during RGB
 * rendering, completely overwriting alpha with unrelated mask data.
 *
 * For layers that will be composited, we must recover correct alpha values.
 * Two methods handle the two types of corruption:
 *
 * 1. Direct-to-Alpha (default):
 *    Replays alpha-affecting tasks into the alpha channel after RGB rendering.
 *    Works for squared-alpha errors since the math is predictable and reversible.
 *
 * 2. L8 Render-Target (EVE5_USE_RENDERTARGET_ALPHA=1, when needed):
 *    When any task uses alpha-as-mask, the alpha channel is irrecoverably trashed.
 *    This path renders alpha coverage to a separate L8 texture BEFORE RGB rendering,
 *    then blits the result into the ARGB8 alpha channel afterward. Requires extra
 *    VRAM but handles all cases correctly.
 *
 * The opaque pre-pass (lv_draw_eve5_opaque_prepass) and alpha recovery check
 * (lv_draw_eve5_check_alpha_recovery) analyze queued tasks to determine which
 * method is required. Screen layers skip alpha recovery entirely since their
 * alpha isn't used for compositing.
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

#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#include "../../drivers/display/eve5/lv_eve5_image_private.h"

/*********************
 * DEFINES
 *********************/

#if EVE5_DEBUG_LOG
static const char * task_state_str(lv_draw_task_state_t state)
{
    switch(state) {
        case LV_DRAW_TASK_STATE_WAITING:
            return "WAITING";
        case LV_DRAW_TASK_STATE_QUEUED:
            return "QUEUED";
        case LV_DRAW_TASK_STATE_IN_PROGRESS:
            return "IN_PROGRESS";
        case LV_DRAW_TASK_STATE_FINISHED:
            return "FINISHED";
        case LV_DRAW_TASK_STATE_BLOCKED:
            return "BLOCKED";
        default:
            return "UNKNOWN";
    }
}
static const char * task_type_str(lv_draw_task_type_t type)
{
    switch(type) {
        case LV_DRAW_TASK_TYPE_FILL:
            return "FILL";
        case LV_DRAW_TASK_TYPE_BORDER:
            return "BORDER";
        case LV_DRAW_TASK_TYPE_LINE:
            return "LINE";
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            return "TRIANGLE";
        case LV_DRAW_TASK_TYPE_LABEL:
            return "LABEL";
        case LV_DRAW_TASK_TYPE_LETTER:
            return "LETTER";
        case LV_DRAW_TASK_TYPE_IMAGE:
            return "IMAGE";
        case LV_DRAW_TASK_TYPE_ARC:
            return "ARC";
        case LV_DRAW_TASK_TYPE_LAYER:
            return "LAYER";
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            return "BOX_SHADOW";
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            return "MASK_RECT";
        default:
            return "OTHER";
    }
}
#endif

/**********************
 * STATIC PROTOTYPES
 **********************/
#ifdef EVE_SUPPORT_RENDERTARGET
/* Render-target dispatch path. Requires CMD_RENDERTARGET, the alpha-pass /
 * blend / gaussian helpers (themselves RT-only), and BT820+ format
 * identifiers used inside those helpers. The whole RT side of this file is
 * compiled out below the same guard. */
static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static lv_draw_task_t * eve5_render_slice(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                          bool is_screen, bool layer_has_alpha,
                                          const lv_draw_eve5_slice_t * slice, bool apply_bitmap_mask);
static void eve5_render_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer);
static lv_draw_task_t * eve5_find_blend_task(lv_draw_task_t * cursor, lv_draw_task_t * end);
#endif

/* Non-render-target dispatch path for EVE generations without CMD_RENDERTARGET
 * (EVE1-EVE4). Single DL per frame, screen-layer only, no slicing or alpha pass. */
static int32_t dispatch_nort(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t evaluate_nort(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static void eve5_render_layer_nort(lv_draw_eve5_unit_t * u, lv_layer_t * layer);

static bool s_eve5_enabled = true;


/**********************
 * GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve5_init(EVE_HalContext *hal, EVE_GpuAlloc *allocator)
{
	EVE_HalContext *phost = hal;
	LV_UNUSED(phost); /* Only the multi-target macro expansions use it */

    lv_draw_eve5_unit_t * unit = lv_draw_create_unit(sizeof(lv_draw_eve5_unit_t));
#ifdef EVE_SUPPORT_RENDERTARGET
    if(EVE_Hal_supportRenderTarget(hal)) {
        unit->base_unit.dispatch_cb = dispatch;
        unit->base_unit.evaluate_cb = evaluate;
        unit->base_unit.name = "EVE5_BT820";
    }
    else
#endif
    {
        unit->base_unit.dispatch_cb = dispatch_nort;
        unit->base_unit.evaluate_cb = evaluate_nort;
        unit->base_unit.name = "EVE_NORT";
        LV_LOG_INFO("EVE5: No render-target support, using non-RT dispatch (screen-layer only)");
    }
    unit->hal = hal;
    unit->allocator = allocator;
    unit->rendering_in_progress = false;
    unit->alloc_canvas_hint = false;
    unit->font_list = NULL;

#if LV_DRAW_EVE5_SW_FALLBACK
    lv_draw_eve5_sw_cache_init(unit);
#endif

    lv_draw_eve5_ring_init(unit);

    lv_draw_eve5_handle_pool_init(unit);
    lv_draw_eve5_rom_font_init(unit);

    lv_draw_eve5_register_vram_callbacks(unit);
    unit->base_unit.vram_font_free_cb = lv_draw_eve5_vram_font_free;

#if EVE5_HW_IMAGE_DECODE
    lv_draw_eve5_register_image_decoder(unit);
#endif

    /* Wire the draw unit into the display's full_buf vram_res so LVGL can
     * dispatch vram callbacks (vram_check, vram_free, etc.). lv_eve5_create*
     * stashed the display on hal->UserContext for exactly this lookup —
     * one display per HAL context per draw unit. Also register the
     * coprocessor-reset cache-invalidation hook so the display driver can
     * dispatch into us on a narrow reset without depending on this unit's
     * header. */
    if(hal != NULL && hal->UserContext != NULL) {
        lv_display_t * disp = (lv_display_t *)hal->UserContext;
        lv_eve5_link_draw_unit(disp, &unit->base_unit);
        lv_eve5_set_coprocessor_reset_handler(disp, lv_draw_eve5_handle_coprocessor_reset);
    }

    LV_LOG_INFO("EVE5: Draw unit initialized, ID=%d", DRAW_UNIT_ID_EVE5);
    LV_LOG_INFO("EVE5: SW fallback config: FILL=%d BORDER=%d LINE=%d TRI=%d LABEL=%d ARC=%d SHADOW=%d CANVAS=%d VECTOR=%d",
                LV_DRAW_EVE5_SW_FILL, LV_DRAW_EVE5_SW_BORDER, LV_DRAW_EVE5_SW_LINE,
                LV_DRAW_EVE5_SW_TRIANGLE, LV_DRAW_EVE5_SW_LABEL, LV_DRAW_EVE5_SW_ARC,
                LV_DRAW_EVE5_SW_BOX_SHADOW, LV_DRAW_EVE5_SW_CANVAS, LV_DRAW_EVE5_SW_VECTOR);
}

void lv_draw_eve5_deinit(void)
{
    LV_LOG_INFO("EVE5: Draw unit deinitialized");
}

void lv_draw_eve5_set_enabled(bool enabled)
{
    LV_LOG_WARN("EVE5: set_enabled %d -> %d", (int)s_eve5_enabled, (int)enabled);
    s_eve5_enabled = enabled;
}

bool lv_draw_eve5_get_enabled(void)
{
    return s_eve5_enabled;
}

/**********************
 * DISPATCH/EVALUATE
 **********************/

#ifdef EVE_SUPPORT_RENDERTARGET

/* Render-target dispatch path. Everything from here down to
 * "NON-RENDER-TARGET DISPATCH" depends on CMD_RENDERTARGET, SWAPCHAIN_0, the
 * alpha-pass / blend / gaussian helpers, and the BT820+ format identifiers
 * those helpers reference (ARGB8, PALETTEDARGB8, ...). Compiled out entirely
 * on chips without render-target support. */

static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    if(!s_eve5_enabled) return 0;

    /* Skip tasks marked for SW fallback (re-issued with user_data set) */
    if(((lv_draw_dsc_base_t *)task->draw_dsc)->user_data != NULL) {
        EVE5_LOG("EVE5: Evaluate: type=%s -> SW fallback", task_type_str(task->type));
        return 0;
    }

#if LV_DRAW_EVE5_SW_CANVAS
    /* Decline tasks on canvas layers (draw_buf != NULL, parent == NULL, not screen) */
    lv_layer_t * target = task->target_layer;
    if(target != NULL && target->draw_buf != NULL && target->parent == NULL) {
        lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
        lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);
        if(!disp || target != disp->layer_head) {
            EVE5_LOG("EVE5: Evaluate: type=%s -> declined (canvas layer)", task_type_str(task->type));
            return 0;
        }
    }
#else
    LV_UNUSED(draw_unit);
#endif

    task->preference_score = 10;
    task->preferred_draw_unit_id = DRAW_UNIT_ID_EVE5;

    EVE5_LOG("EVE5: Evaluate: type=%s -> claimed", task_type_str(task->type));

    return 0;
}

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_draw_task_t * t;

    if(u->rendering_in_progress) {
        EVE5_LOG("EVE5: Dispatch skipped - render in progress");
        return 0;
    }

    int32_t waiting_count = 0;
    int32_t queued_count = 0;
    int32_t blocked_count = 0;
    int32_t finished_count = 0;
    int32_t in_progress_count = 0;

    t = layer->draw_task_head;
    while(t) {
        if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5) {
            switch(t->state) {
                case LV_DRAW_TASK_STATE_WAITING:
                    waiting_count++;
                    break;
                case LV_DRAW_TASK_STATE_QUEUED:
                    queued_count++;
                    break;
                case LV_DRAW_TASK_STATE_BLOCKED:
                    blocked_count++;
                    break;
                case LV_DRAW_TASK_STATE_FINISHED:
                    finished_count++;
                    break;
                case LV_DRAW_TASK_STATE_IN_PROGRESS:
                    in_progress_count++;
                    break;
            }
        }
        t = t->next;
    }

    EVE5_LOG("EVE5: Dispatch layer=%p parent=%p | W=%d Q=%d B=%d F=%d P=%d",
             (void *)layer, (void *)layer->parent,
             waiting_count, queued_count, blocked_count, finished_count, in_progress_count);

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

    /* New tasks arrived - claim them */
    if(waiting_count > 0) {
        EVE5_LOG("EVE5: -> Queueing %d tasks", waiting_count);

        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_WAITING) {
                t->state = LV_DRAW_TASK_STATE_QUEUED;
            }
            else if(t->state != LV_DRAW_TASK_STATE_QUEUED) {
                EVE5_LOG("EVE5: -> Skipping task type=%s state=%s",
                         task_type_str(t->type),
                         task_state_str(t->state));
            }
            t = t->next;
        }

        lv_draw_dispatch_request();
        return waiting_count;
    }

    /* All tasks queued and ready - render atomically */
    if(queued_count > 0 && layer->all_tasks_added && blocked_count == 0 && waiting_count == 0) {
        EVE5_LOG("EVE5: -> Rendering %d queued tasks atomically", queued_count);

#if LV_DRAW_EVE5_OPAQUE_CANVAS_YCBCR
        /* Tell vram_alloc_cb whether this buffer belongs to a canvas layer
         * (draw_buf-backed, parentless, not the screen), so the opaque-canvas
         * YCBCR policy applies on the first allocation instead of through an
         * init_layer realloc. */
        {
            lv_display_t * hint_disp = lv_eve5_disp_from_hal(u->hal);
            u->alloc_canvas_hint = (layer->draw_buf != NULL && layer->parent == NULL
                                    && (hint_disp == NULL || layer != hint_disp->layer_head));
        }
#endif
        lv_draw_layer_alloc_buf(layer, draw_unit);
#if LV_DRAW_EVE5_OPAQUE_CANVAS_YCBCR
        u->alloc_canvas_hint = false;
#endif

        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                if(!lv_draw_buf_ensure_task_sources_resident(t, draw_unit)) {
                    LV_LOG_WARN("EVE5: Failed to ensure task sources resident, skipping task type %d", t->type);
                    t->state = LV_DRAW_TASK_STATE_FINISHED;
                }
            }
            t = t->next;
        }

        eve5_render_layer(u, layer);
        lv_draw_dispatch_request();
        return 1;
    }

    EVE5_LOG("EVE5: -> IDLE");
    return LV_DRAW_UNIT_IDLE;
}

/**********************
 * SLICE RENDERING
 **********************/

/**
 * Render one slice of the task queue: prepass, init, draw, alpha, finish.
 * Each slice is a complete render-target display list cycle. When the tasks
 * do not all fit the display list, the slice ends early; the caller renders
 * the rest as a new slice on top of this one's output.
 *
 * @param apply_bitmap_mask  true for the last slice only (applies parent's bitmap mask)
 * @return the first task not rendered: slice_in->end when the slice is complete
 */
static lv_draw_task_t * eve5_render_slice(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                                          bool is_screen, bool layer_has_alpha,
                                          const lv_draw_eve5_slice_t * slice_in, bool apply_bitmap_mask)
{
    lv_draw_task_t * t;

    /* The range shrinks when the display list budget ends the slice early */
    lv_draw_eve5_slice_t slice_range = *slice_in;
    lv_draw_eve5_slice_t * slice = &slice_range;

    /* Pre-pass: check if any task in the slice would produce visible output.
     * If all tasks are no-ops (e.g., all LAYER tasks with empty children),
     * skip the entire render-target cycle. This propagates recursively:
     * an empty child → parent's LAYER task is a no-op → parent stays empty. */
    {
        bool has_visible_tasks = false;
        t = eve5_slice_first(slice, layer);
        while(t && t != slice->end) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
                    lv_draw_image_dsc_t * dsc = t->draw_dsc;
                    lv_layer_t * child = (lv_layer_t *)dsc->src;
                    lv_eve5_vram_res_t * child_vr = eve5_get_vram_res(child);
                    if(child_vr != NULL && child_vr->has_content) {
                        has_visible_tasks = true;
                        break;
                    }
                }
                else {
                    /* Non-LAYER task — assume it produces output.
                     * Individual draw functions handle their own opa/size checks. */
                    has_visible_tasks = true;
                    break;
                }
            }
            t = t->next;
        }

        if(!has_visible_tasks) {
            /* Mark all tasks as finished without rendering.
             * No init_layer/finish_layer — has_content stays unchanged
             * (false for fresh layers, true for existing canvas content). */
            t = eve5_slice_first(slice, layer);
            while(t && t != slice->end) {
                if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
                   t->state == LV_DRAW_TASK_STATE_QUEUED) {
                    t->state = LV_DRAW_TASK_STATE_FINISHED;
                }
                t = t->next;
            }

            /* The previous slice's output is the layer content. The swapchain
             * cannot take it over, nor can a layer in another format; blit it
             * with an otherwise empty slice. */
            lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
            if(!slice->isolated && slice->prev_handle.Id != GA_HANDLE_INVALID.Id && vr != NULL) {
                if(vr->is_swapchain || slice->prev_eve_format != 0) {
                    bool is_swapchain = vr->is_swapchain;
                    lv_draw_eve5_hal_init_layer(u, layer, is_screen, slice);
                    vr = eve5_get_vram_res(layer);
                    if(vr != NULL) {
                        lv_draw_eve5_hal_finish_layer(u, layer, is_screen, 0);
                        if(!is_swapchain) vr->has_content = true;
                    }
                }
                else {
                    EVE_GpuAlloc_ScopedFree(u->allocator, vr->gpu_handle);
                    vr->gpu_handle = slice->prev_handle;
                    vr->has_content = true;
                }
            }
            return slice_in->end;
        }
    }

    /* Pre-initialize canvas_orig from prev_handle so the L8 alpha render-target
     * pass (which runs before init_layer) can include the previous slice's alpha
     * as its background. init_layer will overwrite these with the same values. */
    if(slice->isolated) {
        u->canvas_orig_addr = GA_INVALID;
        u->canvas_orig_palette = GA_INVALID;
    }
    /* Sentinel check: prev_handle is GA_HANDLE_INVALID when no previous slice was assigned */
    else if(slice->prev_handle.Id != GA_HANDLE_INVALID.Id) {
        uint32_t prev_addr = EVE_GpuAlloc_Get(u->allocator, slice->prev_handle);
        if(prev_addr != GA_INVALID) {
            lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
            u->canvas_orig_addr = prev_addr;
            u->canvas_orig_format = slice->prev_eve_format ? slice->prev_eve_format : (vr ? vr->eve_format : ARGB8);
            u->canvas_orig_stride = slice->prev_eve_format ? slice->prev_stride : (vr ? vr->stride : 0);
            u->canvas_orig_palette = GA_INVALID;
            u->canvas_orig_w = lv_area_get_width(&layer->buf_area);
            u->canvas_orig_h = lv_area_get_height(&layer->buf_area);
        }
        else {
            u->canvas_orig_addr = GA_INVALID;
            u->canvas_orig_palette = GA_INVALID;
        }
    }
    else {
        /* Without a previous slice, the layer's content, such as a canvas's,
         * is the background, unless init_layer discards it */
        lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
        bool discard = layer->draw_buf != NULL
                       && lv_draw_buf_has_flag(layer->draw_buf, LV_IMAGE_FLAGS_CLEARZERO | LV_IMAGE_FLAGS_DISCARDABLE);
        uint32_t base = (vr != NULL && vr->has_content && !vr->is_swapchain && !discard)
                        ? EVE_GpuAlloc_Get(u->allocator, vr->gpu_handle) : GA_INVALID;
        if(base != GA_INVALID) {
            u->canvas_orig_addr = base + vr->source_offset;
            u->canvas_orig_format = vr->eve_format;
            u->canvas_orig_stride = vr->stride;
            u->canvas_orig_palette = vr->palette_offset != GA_INVALID ? base + vr->palette_offset : GA_INVALID;
            u->canvas_orig_w = lv_area_get_width(&layer->buf_area);
            u->canvas_orig_h = lv_area_get_height(&layer->buf_area);
        }
        else {
            u->canvas_orig_addr = GA_INVALID;
            u->canvas_orig_palette = GA_INVALID;
        }
    }

    u->has_alpha_opaque = false;
    u->has_alpha_trashed = false;
    u->alpha_needs_rendertarget = false;

    /* Analyze tasks for alpha recovery method selection */
    if(!is_screen && layer_has_alpha) {
        lv_draw_eve5_opaque_prepass(u, layer, slice);
#if EVE5_USE_RENDERTARGET_ALPHA
        lv_draw_eve5_check_alpha_recovery(u, layer, slice);
#endif
    }

#if EVE5_USE_RENDERTARGET_ALPHA
    /* L8 render-target pass: render alpha to separate texture before RGB pass */
    EVE_GpuHandle alpha_rt_handle = GA_HANDLE_INVALID;
    uint32_t alpha_rt_addr = GA_INVALID;
    int32_t alpha_rt_aw = 0, alpha_rt_ah = 0, alpha_rt_w = 0, alpha_rt_h = 0;

    if(!is_screen && layer_has_alpha && u->alpha_needs_rendertarget) {
        alpha_rt_w = lv_area_get_width(&layer->buf_area);
        alpha_rt_h = lv_area_get_height(&layer->buf_area);
        alpha_rt_aw = ALIGN_UP(alpha_rt_w, 16);
        alpha_rt_ah = ALIGN_UP(alpha_rt_h, 16);
        lv_draw_task_t * l8_stop = slice->end;
        alpha_rt_handle = lv_draw_eve5_render_alpha_to_l8(u, layer,
                                                          alpha_rt_aw, alpha_rt_ah, alpha_rt_w, alpha_rt_h, slice,
                                                          EVE5_DL_INIT_LAYER + EVE5_DL_BLIT_L8 + EVE5_DL_FINISH_LAYER
                                                          + (apply_bitmap_mask ? EVE5_DL_BITMAP_MASK : 0),
                                                          &l8_stop);
        alpha_rt_addr = EVE_GpuAlloc_Get(u->allocator, alpha_rt_handle);
        if(alpha_rt_addr == GA_INVALID) {
            u->alpha_needs_rendertarget = false;
        }
        else {
            slice->end = l8_stop;
        }
    }
#endif

    lv_draw_eve5_hal_init_layer(u, layer, is_screen, slice);

    if(eve5_get_vram_res(layer) == NULL) {
        LV_LOG_ERROR("EVE5: Layer allocation failed!");

        t = eve5_slice_first(slice, layer);
        while(t && t != slice_in->end) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                t->state = LV_DRAW_TASK_STATE_FINISHED;
            }
            t = t->next;
        }
        return slice_in->end;
    }

#if LV_DRAW_EVE5_SW_FALLBACK
    lv_draw_eve5_sw_cache_new_frame(u);
#endif

    /* RGB render pass. finish_tasks=true when no alpha pass follows. */
    bool l8_alpha = false;
#if EVE5_USE_RENDERTARGET_ALPHA
    l8_alpha = u->alpha_needs_rendertarget;
#endif
    bool finish_tasks = is_screen || !layer_has_alpha || l8_alpha;

    /* Display list budget. The L8 alpha pass has already fixed the slice end,
     * and the swapchain is only rendered to when all of it fits. */
    lv_draw_eve5_budget_t budget;
    lv_eve5_vram_res_t * target_vr = eve5_get_vram_res(layer);
    budget.alpha_pass = !finish_tasks;
    budget.can_split = !l8_alpha && !target_vr->is_swapchain;
    budget.reserve = EVE5_DL_FINISH_LAYER;
    if(!is_screen && layer_has_alpha) budget.reserve += l8_alpha ? EVE5_DL_BLIT_L8 : EVE5_DL_ALPHA_PASS;
    if(apply_bitmap_mask && !is_screen && layer->parent != NULL) budget.reserve += EVE5_DL_BITMAP_MASK;
    budget.stop = slice->end;

    int rendered_count = lv_draw_eve5_render_tasks(u, layer, is_screen, finish_tasks, slice, &budget);
    slice->end = budget.stop;

    /* Alpha recovery pass (skip for screen layers) */
    if(!is_screen) {
        if(layer_has_alpha) {
#if EVE5_USE_RENDERTARGET_ALPHA
            if(u->alpha_needs_rendertarget && alpha_rt_addr != GA_INVALID) {
                lv_draw_eve5_hal_blit_l8_to_alpha(u, alpha_rt_addr,
                                                  alpha_rt_aw, alpha_rt_ah,
                                                  alpha_rt_w, alpha_rt_h);
                EVE_GpuAlloc_ScopedFree(u->allocator, alpha_rt_handle);
            }
            else
#endif
            {
                lv_draw_eve5_alpha_pass(u, layer, slice);
            }
        }
    }

    /* Deferred mask_rect: runs after alpha pass on fully corrected premultiplied RGBA.
     * Scales all four channels to avoid white fringing at partially masked edges.
     * Scoped to the slice range, and so to the content drawn before it. LVGL
     * adds masks after everything else in a layer, and masks multiply, so a
     * slice boundary between them does not change the result. */
    if(!is_screen) {
        t = eve5_slice_first(slice, layer);
        while(t && t != slice->end) {
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

    /* Deferred bitmap mask: apply parent's bitmap_mask_src if present, then clear it
     * to prevent double-masking during compositing. Must run after alpha correction.
     * Only applied on the last slice to avoid double-masking across slices. */
    if(apply_bitmap_mask && slice->end == slice_in->end && !is_screen && layer->parent != NULL) {
        lv_draw_task_t * pt = layer->parent->draw_task_head;
        while(pt) {
            if(pt->type == LV_DRAW_TASK_TYPE_LAYER) {
                lv_draw_image_dsc_t * layer_dsc = pt->draw_dsc;
                lv_layer_t * child_ref = (lv_layer_t *)layer_dsc->src;
                if(child_ref == layer && layer_dsc->bitmap_mask_src != NULL) {
                    lv_draw_eve5_apply_bitmap_mask(u, layer, layer_dsc);
                    layer_dsc->bitmap_mask_src = NULL;
                    break;
                }
            }
            pt = pt->next;
        }
    }

    EVE5_LOG("EVE5: Finishing slice, rendered %d tasks", rendered_count);
    lv_draw_eve5_hal_finish_layer(u, layer, is_screen, rendered_count);

    return slice->end;
}

/* Mark the queued EVE5 tasks in [start, end) finished without rendering them */
static void eve5_finish_queued(lv_draw_task_t * start, const lv_draw_task_t * end)
{
    for(lv_draw_task_t * t = start; t != NULL && t != end; t = t->next) {
        if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
           t->state == LV_DRAW_TASK_STATE_QUEUED) {
            t->state = LV_DRAW_TASK_STATE_FINISHED;
        }
    }
}

/**
 * Render the task range of `range` as consecutive slices that each fit the
 * display list budget, the first on top of range->prev_handle (consumed).
 * Each further slice continues in a fresh buffer on top of the previous
 * slice's output. The output stays in the layer's buffer.
 */
static void eve5_render_range(lv_draw_eve5_unit_t * u, lv_layer_t * layer,
                              bool is_screen, bool layer_has_alpha,
                              const lv_draw_eve5_slice_t * range, bool apply_bitmap_mask)
{
    lv_draw_eve5_slice_t slice = *range;

    for(;;) {
        lv_draw_task_t * stop = eve5_render_slice(u, layer, is_screen, layer_has_alpha, &slice, apply_bitmap_mask);
        if(stop == range->end) return;

        lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
        EVE_GpuHandle next = EVE_GpuAlloc_Alloc(u->allocator, vr->base.size, GA_ALIGN_128);
        if(EVE_GpuAlloc_Get(u->allocator, next) == GA_INVALID) {
            LV_LOG_ERROR("EVE5: Failed to allocate buffer for the next slice, dropping its tasks");
            eve5_finish_queued(stop, range->end);
            return;
        }

        EVE5_LOG("EVE5: Continuing layer %p in a new slice", (void *)layer);
        slice.start = stop;
        slice.prev_handle = vr->gpu_handle;
        slice.prev_eve_format = 0;
        slice.prev_stride = 0;
        vr->gpu_handle = next;
        vr->has_content = false;
    }
}

/* Layer state while its slices render to ARGB8 intermediates. The blend math,
 * and the intermediates between slices, are ARGB8: a layer in another format
 * (an opaque canvas in RGB565, RGB8 or L8) converts its content in and its
 * result back. The full-mode screen does the same, as the swapchain can only
 * take its final frame. */
typedef struct {
    bool is_swapchain;
    EVE_GpuHandle gpu_handle;   /**< The swapchain, or the layer's buffer until its content is converted */
    uint16_t eve_format;
    uint32_t stride;
    uint32_t base_size;
    bool has_content;
    bool is_premultiplied;
    bool sample_as_luminance;
    lv_color_format_t layer_cf;
    lv_color_format_t buf_cf;
    uint16_t buf_flags;
    uint32_t inter_stride;      /**< Stride of the ARGB8 intermediates */
} eve5_argb8_state_t;

/**
 * Point the layer at a fresh ARGB8 intermediate so its slices render there,
 * like an ARGB8 layer, starting from the layer's content. Returns false when
 * the allocation fails.
 */
static bool eve5_argb8_detach(lv_draw_eve5_unit_t * u, lv_layer_t * layer, lv_eve5_vram_res_t * vr,
                              eve5_argb8_state_t * saved)
{
    int32_t aw = ALIGN_UP(lv_area_get_width(&layer->buf_area), 16);
    int32_t ah = ALIGN_UP(lv_area_get_height(&layer->buf_area), 16);
    uint32_t stride = (uint32_t)aw * 4;
    uint32_t size = stride * (uint32_t)ah;

    EVE_GpuHandle handle = EVE_GpuAlloc_Alloc(u->allocator, size, GA_ALIGN_128);
    if(EVE_GpuAlloc_Get(u->allocator, handle) == GA_INVALID) {
        LV_LOG_ERROR("EVE5: Failed to allocate ARGB8 intermediate for slicing (%" LV_PRIu32 " bytes)", size);
        return false;
    }

    saved->is_swapchain = vr->is_swapchain;
    saved->gpu_handle = vr->gpu_handle;
    saved->eve_format = vr->eve_format;
    saved->stride = vr->stride;
    saved->base_size = vr->base.size;
    saved->has_content = vr->has_content;
    saved->is_premultiplied = vr->is_premultiplied;
    saved->sample_as_luminance = vr->sample_as_luminance;
    saved->layer_cf = layer->color_format;
    saved->buf_cf = layer->draw_buf ? layer->draw_buf->header.cf : LV_COLOR_FORMAT_UNKNOWN;
    saved->buf_flags = layer->draw_buf ? (uint16_t)layer->draw_buf->header.flags : 0;
    saved->inter_stride = stride;

    vr->is_swapchain = false;
    vr->gpu_handle = handle;
    vr->eve_format = ARGB8;
    vr->stride = stride;
    vr->base.size = size;
    vr->has_content = false;
    vr->is_premultiplied = false;
    vr->sample_as_luminance = false;
    layer->color_format = LV_COLOR_FORMAT_ARGB8888;
    if(layer->draw_buf) layer->draw_buf->header.cf = LV_COLOR_FORMAT_ARGB8888;

    /* The layer's content, converted, is the base of the first slice. The
     * conversion consumes the layer's buffer. */
    if(!saved->is_swapchain && saved->has_content) {
        lv_draw_eve5_slice_t conv;
        lv_memzero(&conv, sizeof(conv));
        conv.prev_handle = saved->gpu_handle;
        conv.prev_eve_format = saved->eve_format;
        conv.prev_stride = saved->stride;
        conv.prev_luminance = saved->sample_as_luminance;
        lv_draw_eve5_hal_init_layer(u, layer, false, &conv);
        if(eve5_get_vram_res(layer) != NULL) {
            lv_draw_eve5_hal_finish_layer(u, layer, false, 0);
            vr->has_content = true;
            vr->is_premultiplied = true;
        }
        saved->gpu_handle = GA_HANDLE_INVALID;
    }
    return true;
}

/**
 * Give the layer its own format back. The swapchain returns as it was; a
 * layer gets a new buffer in its format, for the last slice or the
 * conversion of the result. Returns the current intermediate, which the
 * caller now owns.
 */
static EVE_GpuHandle eve5_argb8_attach(lv_draw_eve5_unit_t * u, lv_layer_t * layer, lv_eve5_vram_res_t * vr,
                                       const eve5_argb8_state_t * saved)
{
    EVE_GpuHandle inter = vr->gpu_handle;
    vr->is_swapchain = saved->is_swapchain;
    vr->eve_format = saved->eve_format;
    vr->is_premultiplied = saved->is_premultiplied;
    vr->sample_as_luminance = saved->sample_as_luminance;
    if(saved->is_swapchain) {
        vr->gpu_handle = saved->gpu_handle;
        vr->stride = saved->stride;
        vr->base.size = saved->base_size;
        vr->has_content = saved->has_content;
    }
    else {
        /* An unconverted buffer, which had no content, isn't needed anymore */
        EVE_GpuAlloc_ScopedFree(u->allocator, saved->gpu_handle);
        int32_t aw = ALIGN_UP(lv_area_get_width(&layer->buf_area), 16);
        int32_t ah = ALIGN_UP(lv_area_get_height(&layer->buf_area), 16);
        vr->stride = (uint32_t)aw * (uint32_t)eve5_format_bpp(saved->eve_format);
        vr->base.size = eve5_rt_surface_size(saved->eve_format, vr->stride, (uint32_t)ah);
        vr->gpu_handle = EVE_GpuAlloc_Alloc(u->allocator, vr->base.size, GA_ALIGN_128);
        vr->source_offset = 0;
        vr->palette_offset = GA_INVALID;
        vr->has_content = false;
    }
    layer->color_format = saved->layer_cf;
    if(layer->draw_buf) {
        /* init_layer marks the intermediate's format premultiplied */
        layer->draw_buf->header.cf = saved->buf_cf;
        layer->draw_buf->header.flags = saved->buf_flags;
    }
    return inter;
}

/**
 * Put an ARGB8 intermediate (consumed) in the attached layer with an otherwise
 * empty slice: present it on the swapchain, or convert it to the layer's
 * format. Without one, the swapchain is cleared to keep its rotation, and a
 * layer stays empty.
 */
static void eve5_argb8_finish(lv_draw_eve5_unit_t * u, lv_layer_t * layer, EVE_GpuHandle inter,
                              const eve5_argb8_state_t * saved)
{
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);

    if(inter.Id != GA_HANDLE_INVALID.Id) {
        lv_draw_eve5_slice_t slice_empty;
        lv_memzero(&slice_empty, sizeof(slice_empty));
        slice_empty.prev_handle = inter;
        slice_empty.prev_eve_format = ARGB8;
        slice_empty.prev_stride = saved->inter_stride;
        lv_draw_eve5_hal_init_layer(u, layer, saved->is_swapchain, &slice_empty);
        lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
        if(vr != NULL) {
            lv_draw_eve5_hal_finish_layer(u, layer, saved->is_swapchain, 0);
            if(!saved->is_swapchain) vr->has_content = true;
        }
    }
    else if(saved->is_swapchain) {
        EVE_HalContext * phost = u->hal;
        lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
        EVE_CoCmd_renderTarget(phost, SWAPCHAIN_0, vr->eve_format, w, h);
        EVE_CoCmd_dlStart(phost);
        EVE_CoDl_scissorXY(phost, 0, 0);
        EVE_CoDl_scissorSize(phost, w, h);
        EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
        EVE_CoDl_clear(phost, 1, 1, 1);
        EVE_CoDl_display(phost);
        EVE_CoCmd_swap(phost);
        EVE_CoCmd_graphicsFinish(phost);
    }
}

/**********************
 * LAYER RENDERING
 **********************/

static void eve5_render_layer(lv_draw_eve5_unit_t * u, lv_layer_t * layer)
{
    lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);
    bool is_screen = (layer->parent == NULL && disp && layer == disp->layer_head);

    u->rendering_in_progress = true;

#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif

    /* Determine layer color format for alpha handling */
    lv_color_format_t layer_cf;
    if(layer->draw_buf != NULL) {
        layer_cf = layer->draw_buf->header.cf;
    }
    else {
        layer_cf = layer->color_format;
    }
    bool layer_has_alpha = lv_color_format_has_alpha(layer_cf);

    EVE5_LOG("EVE5: === RENDER START layer=%p is_screen=%d has_alpha=%d ===",
             (void *)layer, is_screen, layer_has_alpha);
    EVE5_LOG("EVE5: Layer buf_area=(%d,%d)-(%d,%d) clip=(%d,%d)-(%d,%d)",
             layer->buf_area.x1, layer->buf_area.y1,
             layer->buf_area.x2, layer->buf_area.y2,
             layer->_clip_area.x1, layer->_clip_area.y1,
             layer->_clip_area.x2, layer->_clip_area.y2);

    /* Canvas optimization: single image task can load directly without render pipeline */
    if(!is_screen && layer->draw_buf != NULL && layer->parent == NULL
       && lv_draw_eve5_try_canvas_direct_image(u, layer)) {
        u->rendering_in_progress = false;
#if LV_USE_OS
        lv_eve5_hal_unlock(disp);
#endif
        return;
    }

    lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
    bool is_swapchain = is_screen && vr != NULL && vr->is_swapchain;

    /* Slice-boundary splitting: scan for tasks that require splitting the render
     * queue into slices. Two kinds:
     * - Blend modes (IMAGE/LAYER with non-standard blend): isolated render + per-channel blend math
     * - Blur: mipmap downsample chain on the accumulated content
     * Each such task becomes a slice boundary. Between boundaries, the display
     * list budget may split the tasks further (eve5_render_range). */
    lv_draw_task_t * blend_task = eve5_find_blend_task(layer->draw_task_head, NULL);

    if(blend_task == NULL) {
        lv_draw_eve5_slice_t range;
        lv_memzero(&range, sizeof(range));
        range.prev_handle = GA_HANDLE_INVALID;

        if(!is_swapchain || lv_draw_eve5_range_fits_dl(u, layer->draw_task_head, NULL,
                                                       EVE5_DL_INIT_LAYER + EVE5_DL_FINISH_LAYER)) {
            eve5_render_range(u, layer, is_screen, layer_has_alpha, &range, true);
        }
        else {
            /* The screen may not fit one display list, and the swapchain
             * cannot continue a slice: render the slices to an intermediate
             * and present that */
            eve5_argb8_state_t saved;
            if(!eve5_argb8_detach(u, layer, vr, &saved)) {
                eve5_finish_queued(layer->draw_task_head, NULL);
                goto render_done;
            }
            eve5_render_range(u, layer, true, false, &range, true);
            bool has_content = vr->has_content;
            EVE_GpuHandle inter = eve5_argb8_attach(u, layer, vr, &saved);
            if(!has_content) {
                EVE_GpuAlloc_ScopedFree(u->allocator, inter);
                inter = GA_HANDLE_INVALID;
            }
            eve5_argb8_finish(u, layer, inter, &saved);
        }
        goto render_done;
    }

    {
        EVE5_LOG("EVE5: Slice split at task %p (type=%d)", (void *)blend_task, blend_task->type);

        /* Slicing intermediates are ARGB8 buffers, as the blend math is. An
         * ARGB8 layer's own vram_res plays that role. The full-mode screen's
         * vram_res is virtual (is_swapchain), and a layer in another format
         * can't hold them, so these swap it for a fresh ARGB8 allocation for
         * the duration of the slice loop (eve5_argb8_detach). The tail slice
         * then restores the layer, and renders into its own format with the
         * ARGB8 intermediate as its base. A partial-mode screen tile renders
         * ARGB8 whatever its format says (init_layer). */
        bool argb8_sliced = is_swapchain || (!is_screen && vr != NULL && vr->eve_format != ARGB8);
        eve5_argb8_state_t saved;
        /* LVGL blends on an L8 layer's luminances */
        bool blend_luminance = false;

        /* Per-iteration eve5_render_slice parameters. For non-screen-layer
         * slicing, intermediates and the tail share the outer is_screen /
         * layer_has_alpha. For full-mode-screen, intermediates render as a
         * regular ARGB8 layer (so blend math / alpha pass behave correctly)
         * and only the tail runs with is_screen=true so init_layer's
         * swapchain branch fires. */
        bool inter_is_screen = is_screen;
        bool inter_layer_has_alpha = layer_has_alpha;

        if(argb8_sliced) {
            if(!eve5_argb8_detach(u, layer, vr, &saved)) {
                /* Frame is dropped */
                eve5_finish_queued(layer->draw_task_head, NULL);
                goto render_done;
            }
            inter_is_screen = false;
            inter_layer_has_alpha = true;
            /* Only where the layer is stored as L8: a screen tile in another
             * format stays in color */
            blend_luminance = saved.layer_cf == LV_COLOR_FORMAT_L8 && saved.eve_format == L8;
        }

        EVE_GpuHandle prev = GA_HANDLE_INVALID;
        lv_draw_task_t * cursor = layer->draw_task_head;

        while(cursor) {
            /* Find next slice-boundary task from cursor */
            blend_task = eve5_find_blend_task(cursor, NULL);

            if(blend_task == NULL) {
                /* No more blend tasks — render remainder as final slice */
                lv_draw_eve5_slice_t slice_tail;
                lv_memzero(&slice_tail, sizeof(slice_tail));
                slice_tail.start = cursor;
                slice_tail.end = NULL;
                slice_tail.prev_handle = prev;
                slice_tail.isolated = false;

                if(argb8_sliced && (!is_swapchain
                                    || lv_draw_eve5_range_fits_dl(u, cursor, NULL,
                                                                  EVE5_DL_INIT_LAYER + EVE5_DL_FINISH_LAYER))) {
                    /* Render the tail directly into the layer's own format,
                     * or to the swapchain. Free the fresh intermediate
                     * reserved for it. No guard Get: it would re-stamp the
                     * entry to the upcoming epoch and defer the release past
                     * the tail slice; ScopedFree validates internally. */
                    EVE_GpuAlloc_ScopedFree(u->allocator, eve5_argb8_attach(u, layer, vr, &saved));

                    /* prev was rendered as ARGB8 — tell init_layer's
                     * prev_handle blit to decode the source as ARGB8 */
                    slice_tail.prev_eve_format = ARGB8;
                    slice_tail.prev_stride = saved.inter_stride;
                    eve5_render_range(u, layer, is_screen, layer_has_alpha, &slice_tail, true);
                }
                else if(argb8_sliced) {
                    /* The screen's tail may need several slices: render them
                     * to the intermediate as a screen, then present it */
                    eve5_render_range(u, layer, true, false, &slice_tail, true);
                    bool has_content = vr->has_content;
                    EVE_GpuHandle inter = eve5_argb8_attach(u, layer, vr, &saved);
                    if(!has_content) {
                        EVE_GpuAlloc_ScopedFree(u->allocator, inter);
                        inter = GA_HANDLE_INVALID;
                    }
                    eve5_argb8_finish(u, layer, inter, &saved);
                }
                else {
                    eve5_render_range(u, layer, is_screen, layer_has_alpha, &slice_tail, true);
                }
                break;
            }

            /* Slice A: render tasks before the blend task */
            bool has_pre_tasks = false;
            {
                lv_draw_task_t * t = cursor;
                while(t && t != blend_task) {
                    if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
                       t->state == LV_DRAW_TASK_STATE_QUEUED) {
                        has_pre_tasks = true;
                        break;
                    }
                    t = t->next;
                }
            }

            if(has_pre_tasks) {
                lv_draw_eve5_slice_t slice_pre;
                lv_memzero(&slice_pre, sizeof(slice_pre));
                slice_pre.start = cursor;
                slice_pre.end = blend_task;
                slice_pre.prev_handle = prev;
                slice_pre.isolated = false;
                eve5_render_range(u, layer, inter_is_screen, inter_layer_has_alpha, &slice_pre, false);

                /* Capture slice_pre output as dst, allocate new buffer for next slice.
                 * If the slice produced no content (all tasks were no-ops, e.g. empty
                 * child layers), treat it as if has_pre_tasks was false — prev stays
                 * unchanged and no buffer swap is needed. */
                if(vr != NULL && vr->has_content) {
                    EVE_GpuHandle dst_handle = vr->gpu_handle;
                    EVE_GpuHandle new_handle = EVE_GpuAlloc_Alloc(u->allocator, vr->base.size, GA_ALIGN_128);
                    if(EVE_GpuAlloc_Get(u->allocator, new_handle) != GA_INVALID) {
                        vr->gpu_handle = new_handle;
                        vr->has_content = false;
                        prev = dst_handle;
                    }
                    else {
                        LV_LOG_ERROR("EVE5: Failed to allocate buffer after pre-blend slice");
                        prev = GA_HANDLE_INVALID;
                    }
                }
            }
            /* else: no tasks before blend task, prev stays as-is (from previous iteration or INVALID) */

            /* Blur: modify prev in place, no isolation slice or blend math needed */
            if(blend_task->type == LV_DRAW_TASK_TYPE_BLUR) {
                if(prev.Id != GA_HANDLE_INVALID.Id) {
                    lv_draw_eve5_gaussian_blur(u, layer, prev, blend_task);
                }
                blend_task->state = LV_DRAW_TASK_STATE_FINISHED;
                cursor = blend_task->next;
                continue;
            }

            /* If prev is still INVALID (first task in queue is a blend task with
             * nothing before), the dst is the layer's content, such as a
             * canvas's: an empty slice renders it into an ARGB8 render target,
             * premultiplied, as the blend math takes it. */
            if(prev.Id == GA_HANDLE_INVALID.Id && vr != NULL && vr->has_content) {
                lv_draw_eve5_slice_t slice_base;
                lv_memzero(&slice_base, sizeof(slice_base));
                slice_base.prev_handle = GA_HANDLE_INVALID;
                lv_draw_eve5_hal_init_layer(u, layer, inter_is_screen, &slice_base);
                if(eve5_get_vram_res(layer) != NULL) {
                    lv_draw_eve5_hal_finish_layer(u, layer, inter_is_screen, 0);
                    EVE_GpuHandle new_handle = EVE_GpuAlloc_Alloc(u->allocator, vr->base.size, GA_ALIGN_128);
                    if(EVE_GpuAlloc_Get(u->allocator, new_handle) != GA_INVALID) {
                        prev = vr->gpu_handle;
                        vr->gpu_handle = new_handle;
                        vr->has_content = false;
                    }
                }
            }

            /* Without content, the dst is transparent black. Allocate and clear
             * an ARGB8 buffer so blend math has a defined dst. */
            if(prev.Id == GA_HANDLE_INVALID.Id) {
                int32_t aw = ALIGN_UP(lv_area_get_width(&layer->buf_area), 16);
                int32_t ah = ALIGN_UP(lv_area_get_height(&layer->buf_area), 16);
                uint32_t sz = (uint32_t)aw * (uint32_t)ah * 4;
                prev = EVE_GpuAlloc_Alloc(u->allocator, sz, GA_ALIGN_128);
                uint32_t pa = EVE_GpuAlloc_Get(u->allocator, prev);
                if(pa != GA_INVALID) {
                    /* Clear-only DL: samples no allocator-managed memory,
                     * so no epoch scope is needed around this segment */
                    EVE_CoCmd_renderTarget(u->hal, pa, ARGB8, aw, ah);
                    EVE_CoCmd_dlStart(u->hal);
                    EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
                    EVE_CoDl_clearColorA(u->hal, 0);
                    EVE_CoDl_clear(u->hal, 1, 1, 1);
                    EVE_CoDl_display(u->hal);
                    EVE_CoCmd_swap(u->hal);
                    EVE_CoCmd_graphicsFinish(u->hal);
                }
            }

            /* Slice B: render the blend task in isolation */
            lv_draw_eve5_slice_t slice_iso;
            lv_memzero(&slice_iso, sizeof(slice_iso));
            slice_iso.start = blend_task;
            slice_iso.end = blend_task->next;
            slice_iso.prev_handle = GA_HANDLE_INVALID;
            slice_iso.isolated = true;
            eve5_render_slice(u, layer, inter_is_screen, inter_layer_has_alpha,
                              &slice_iso, false);

            /* Capture isolated output as src.
             * If the slice produced no content (empty child layer), src stays
             * GA_HANDLE_INVALID and the blend math is skipped below. */
            EVE_GpuHandle src_handle = GA_HANDLE_INVALID;
            if(vr != NULL && vr->has_content) {
                src_handle = vr->gpu_handle;
                /* Allocate fresh buffer for the next phase */
                EVE_GpuHandle new_handle = EVE_GpuAlloc_Alloc(u->allocator, vr->base.size, GA_ALIGN_128);
                if(EVE_GpuAlloc_Get(u->allocator, new_handle) != GA_INVALID) {
                    vr->gpu_handle = new_handle;
                    vr->has_content = false;
                }
                else {
                    LV_LOG_ERROR("EVE5: Failed to allocate buffer after isolated slice");
                    src_handle = GA_HANDLE_INVALID;
                }
            }

            /* Blend math: per-channel blend between dst and src */
            if(EVE_GpuAlloc_Get(u->allocator, prev) != GA_INVALID &&
               EVE_GpuAlloc_Get(u->allocator, src_handle) != GA_INVALID) {
                const lv_draw_image_dsc_t * dsc = blend_task->draw_dsc;
                EVE_GpuHandle result = GA_HANDLE_INVALID;
                bool blend_attempted = false;

                if(dsc->blend_mode == LV_BLEND_MODE_MULTIPLY) {
                    lv_draw_eve5_blend_multiply(u, layer, prev, src_handle, blend_luminance, &result);
                    blend_attempted = true;
                }
                else if(dsc->blend_mode == LV_BLEND_MODE_SUBTRACTIVE) {
                    lv_draw_eve5_blend_subtractive(u, layer, prev, src_handle, blend_luminance, &result);
                    blend_attempted = true;
                }
                else if(dsc->blend_mode == LV_BLEND_MODE_DIFFERENCE) {
                    lv_draw_eve5_blend_difference(u, layer, prev, src_handle, blend_luminance, &result);
                    blend_attempted = true;
                }

                if(!blend_attempted) {
                    /* Unimplemented blend mode: composite src over dst normally.
                     * This is a standard premultiplied blit as fallback. */
                    EVE_HalContext *phost = u->hal;
                    LV_LOG_INFO("EVE5: Blend mode %d not implemented, compositing normally",
                                dsc->blend_mode);
                    int32_t bw = lv_area_get_width(&layer->buf_area);
                    int32_t bh = lv_area_get_height(&layer->buf_area);
                    int32_t baw = ALIGN_UP(bw, 16);
                    int32_t bah = ALIGN_UP(bh, 16);
                    uint32_t bstride = (uint32_t)baw * 4;
                    uint32_t bsize = bstride * (uint32_t)bah;

                    result = EVE_GpuAlloc_Alloc(u->allocator, bsize, GA_ALIGN_128);
                    uint32_t result_addr = EVE_GpuAlloc_Get(u->allocator, result);
                    uint32_t prev_addr = EVE_GpuAlloc_Get(u->allocator, prev);
                    uint32_t src_addr = EVE_GpuAlloc_Get(u->allocator, src_handle);

                    if(result_addr != GA_INVALID && prev_addr != GA_INVALID && src_addr != GA_INVALID) {
                        /* Epoch scope for the fallback composite segment;
                         * the prev/src Gets above carry its epoch, gating
                         * their scoped frees below on this DL's sync */
                        EVE_GpuAlloc_OpenScope(u->allocator);

                        EVE_CoCmd_renderTarget(u->hal, result_addr, ARGB8, baw, bah);
                        EVE_CoCmd_dlStart(u->hal);
                        EVE_CoDl_scissorXY(u->hal, 0, 0);
                        EVE_CoDl_scissorSize(u->hal, bw, bh);
                        EVE_CoDl_clearColorRgb(u->hal, 0, 0, 0);
                        EVE_CoDl_clearColorA(u->hal, 0);
                        EVE_CoDl_clear(u->hal, 1, 1, 1);
                        EVE_CoDl_vertexFormat(u->hal, 0);
                        EVE_CoDl_colorArgb_ex(u->hal, 0xFFFFFFFF);
                        EVE_CoDl_bitmapTransform_identity(u->hal);
                        EVE_CoDl_bitmapHandle(u->hal, EVE_CO_SCRATCH_HANDLE);

                        /* Blit dst */
                        EVE_CoDl_blendFunc(u->hal, ONE, ZERO);
                        EVE_CoDl_bitmapSource(u->hal, prev_addr);
                        EVE_CoDl_bitmapLayout(u->hal, ARGB8, bstride, bh);
                        EVE_CoDl_bitmapSize(u->hal, NEAREST, BORDER, BORDER, bw, bh);
                        EVE_CoDl_begin(u->hal, BITMAPS);
                        EVE_CoDl_vertex2f_0(u->hal, 0, 0);
                        EVE_CoDl_end(u->hal);

                        /* Blit src with premultiplied composite */
                        EVE_CoDl_blendFunc(u->hal, ONE, ONE_MINUS_SRC_ALPHA);
                        EVE_CoDl_bitmapSource(u->hal, src_addr);
                        EVE_CoDl_begin(u->hal, BITMAPS);
                        EVE_CoDl_vertex2f_0(u->hal, 0, 0);
                        EVE_CoDl_end(u->hal);

                        EVE_CoDl_display(u->hal);
                        EVE_CoCmd_swap(u->hal);
                        EVE_CoCmd_graphicsFinish(u->hal);

                        EVE_GpuAlloc_CloseScope(u->allocator, EVE_Cmd_sync(u->hal));
                    }
                    else {
                        result = GA_HANDLE_INVALID;
                    }
                }

                EVE_GpuAlloc_ScopedFree(u->allocator, prev);
                EVE_GpuAlloc_ScopedFree(u->allocator, src_handle);

                /* Sentinel: result is specifically GA_HANDLE_INVALID when blend was not attempted or failed */
                if(result.Id != GA_HANDLE_INVALID.Id) {
                    prev = result;
                }
                else {
                    LV_LOG_WARN("EVE5: Blend operation failed");
                    prev = GA_HANDLE_INVALID;
                }
            }
            else {
                EVE_GpuAlloc_ScopedFree(u->allocator, prev);
                EVE_GpuAlloc_ScopedFree(u->allocator, src_handle);
                prev = GA_HANDLE_INVALID;
            }

            cursor = blend_task->next;
        }

        /* Edge case: cursor became NULL because the very last task was a
         * blend (no remaining tasks for a tail slice).
         * ARGB8 layer: store prev on vr so the parent can composite it.
         * Others: free the unused fresh buffer, restore the layer, then
         *   present prev or convert it to the layer's format. */
        if(cursor == NULL) {
            if(argb8_sliced) {
                /* No guard Get: it would re-stamp the entry and defer the
                 * release; ScopedFree validates internally. */
                EVE_GpuAlloc_ScopedFree(u->allocator, eve5_argb8_attach(u, layer, vr, &saved));
                eve5_argb8_finish(u, layer, prev, &saved);
            }
            else if(prev.Id != GA_HANDLE_INVALID.Id) {
                if(vr != NULL) {
                    EVE_GpuAlloc_ScopedFree(u->allocator, vr->gpu_handle);
                    vr->gpu_handle = prev;
                    vr->has_content = true;
                }
            }
        }
    }

render_done:
    EVE5_LOG("EVE5: === RENDER END layer=%p ===", (void *)layer);

    u->rendering_in_progress = false;

#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif
}

/**********************
 * SLICE-BOUNDARY HELPERS
 **********************/

/**
 * Find the next blend/blur task (slice boundary) in [cursor, end). Returns
 * NULL if none. Centralizes the search used by eve5_render_layer and
 * eve5_render_layer_full_screen_sliced.
 */
static lv_draw_task_t * eve5_find_blend_task(lv_draw_task_t * cursor, lv_draw_task_t * end)
{
    for(lv_draw_task_t * t = cursor; t != end; t = t->next) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5) continue;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED) continue;
        if(t->type == LV_DRAW_TASK_TYPE_BLUR) {
            return t;
        }
        if(t->type == LV_DRAW_TASK_TYPE_IMAGE || t->type == LV_DRAW_TASK_TYPE_LAYER) {
            const lv_draw_image_dsc_t * dsc = t->draw_dsc;
            if(dsc->blend_mode != LV_BLEND_MODE_NORMAL &&
               dsc->blend_mode != LV_BLEND_MODE_ADDITIVE) {
                return t;
            }
        }
    }
    return NULL;
}

#endif /* EVE_SUPPORT_RENDERTARGET — RT dispatch path */

/**********************
 * NON-RENDER-TARGET DISPATCH (EVE1-EVE4)
 *
 * For chips without CMD_RENDERTARGET, the screen renders as a single display
 * list each frame. There's no swapchain to flip and no per-layer render targets.
 *
 * Strategy: claim only screen-layer tasks; sub-layer tasks fall through to
 * LVGL's SW renderer, which writes them into CPU buffers. When the screen DL
 * encounters a LAYER task, ensure_resident migrates the SW-rendered child to
 * GPU via vram_upload_cb. The composite DL then samples the uploaded bitmap
 * like any other image source.
 *
 * Unsupported tasks (BLUR, MASK_RECTANGLE) are skipped via the existing
 * render_tasks default case. Blend modes other than NORMAL/ADDITIVE on
 * IMAGE/LAYER tasks degrade to NORMAL — non-RT can't isolate them per slice.
 **********************/

static int32_t evaluate_nort(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    if(!s_eve5_enabled) return 0;

    /* Skip tasks marked for SW fallback (re-issued with user_data set) */
    if(((lv_draw_dsc_base_t *)task->draw_dsc)->user_data != NULL) {
        EVE5_LOG("EVE5 NORT: Evaluate: type=%s -> SW fallback", task_type_str(task->type));
        return 0;
    }

    /* Only the screen layer is rendered on hardware. Sub-layers (canvases,
     * opa_layered, transformed widgets, etc.) all fall through to SW. */
    lv_layer_t * target = task->target_layer;
    if(target == NULL) return 0;

    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);
    if(disp == NULL || target != disp->layer_head) {
        EVE5_LOG("EVE5 NORT: Evaluate: type=%s -> declined (not screen layer)", task_type_str(task->type));
        return 0;
    }

    task->preference_score = 10;
    task->preferred_draw_unit_id = DRAW_UNIT_ID_EVE5;

    EVE5_LOG("EVE5 NORT: Evaluate: type=%s -> claimed", task_type_str(task->type));
    return 0;
}

static int32_t dispatch_nort(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_eve5_unit_t * u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_draw_task_t * t;

    if(u->rendering_in_progress) return 0;

    int32_t waiting_count = 0;
    int32_t queued_count = 0;
    int32_t blocked_count = 0;

    for(t = layer->draw_task_head; t != NULL; t = t->next) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5) continue;
        switch(t->state) {
            case LV_DRAW_TASK_STATE_WAITING:
                waiting_count++;
                break;
            case LV_DRAW_TASK_STATE_QUEUED:
                queued_count++;
                break;
            case LV_DRAW_TASK_STATE_BLOCKED:
                blocked_count++;
                break;
            default:
                break;
        }
    }

    EVE5_LOG("EVE5 NORT: Dispatch layer=%p W=%d Q=%d B=%d",
             (void *)layer, waiting_count, queued_count, blocked_count);

    /* New tasks arrived — claim them, wait for upstream */
    if(waiting_count > 0) {
        for(t = layer->draw_task_head; t != NULL; t = t->next) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_WAITING) {
                t->state = LV_DRAW_TASK_STATE_QUEUED;
            }
        }
        lv_draw_dispatch_request();
        return waiting_count;
    }

    /* All queued and ready — render atomically */
    if(queued_count > 0 && layer->all_tasks_added && blocked_count == 0) {
        lv_draw_layer_alloc_buf(layer, draw_unit);

        for(t = layer->draw_task_head; t != NULL; t = t->next) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                if(!lv_draw_buf_ensure_task_sources_resident(t, draw_unit)) {
                    LV_LOG_WARN("EVE5 NORT: Failed to ensure task sources resident, type=%d", t->type);
                    t->state = LV_DRAW_TASK_STATE_FINISHED;
                }
            }
        }

        eve5_render_layer_nort(u, layer);
        lv_draw_dispatch_request();
        return 1;
    }

    return LV_DRAW_UNIT_IDLE;
}

/**
 * Single-DL render of all queued screen-layer tasks.
 *
 * Reuses lv_draw_eve5_render_tasks with a synthesized full slice and
 * is_screen=true. The existing render_tasks switch handles all primitive
 * types; unsupported types (BLUR) hit the default case and are no-ops.
 *
 * No render target, no alpha pass, no slicing. Output goes straight into
 * the implicit framebuffer that CMD_SWAP latches for scanout.
 */
static void eve5_render_layer_nort(lv_draw_eve5_unit_t * u, lv_layer_t * layer)
{
    EVE_HalContext * phost = u->hal;
    lv_display_t * disp = lv_eve5_disp_from_hal(u->hal);
    int32_t sw = lv_area_get_width(&layer->buf_area);
    int32_t sh = lv_area_get_height(&layer->buf_area);

    u->rendering_in_progress = true;

#if LV_USE_OS
    lv_eve5_hal_lock(disp);
#endif

    EVE5_LOG("EVE5 NORT: === RENDER START layer=%p %dx%d ===",
             (void *)layer, (int)sw, (int)sh);

    /* Scope the alloc-failure counter to this frame; checked after Update
     * to decide whether to retry via screen invalidate. */
    EVE_GpuAlloc_ClearAllocFailedCount(u->allocator);

    /* Reset alpha tracking — the per-task draw functions still call the
     * track_alpha_* helpers, but no alpha pass consumes them. Keep state
     * hygienic across frames. */
    u->has_alpha_opaque = false;
    u->has_alpha_trashed = false;
    u->alpha_needs_rendertarget = false;
    u->canvas_orig_addr = GA_INVALID;
    u->canvas_orig_palette = GA_INVALID;

#if LV_DRAW_EVE5_SW_FALLBACK
    lv_draw_eve5_sw_cache_new_frame(u);
#endif

    /* Open DL targeting the implicit framebuffer (no CMD_RENDERTARGET) */
    EVE_CoCmd_dlStart(phost);
    EVE_CoDl_scissorXY(phost, 0, 0);
    EVE_CoDl_scissorSize(phost, sw, sh);
    EVE_CoDl_clearColorRgb(phost, 0, 0, 0);
    EVE_CoDl_clearColorA(phost, 255);
    EVE_CoDl_clear(phost, 1, 1, 1);
    EVE_CoDl_colorArgb_ex(phost, 0xFFFFFFFF);
    EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);

    /* Synthesize a full-range slice so render_tasks iterates layer->draw_task_head
     * to NULL. is_screen=true skips MASK_RECTANGLE deferral; finish_tasks=true
     * marks each rendered task FINISHED. */
    lv_draw_eve5_slice_t slice;
    lv_memzero(&slice, sizeof(slice));
    slice.prev_handle = GA_HANDLE_INVALID;
    /* Without render targets the frame cannot be split; tasks that may not
     * fit the display list are only reported */
    lv_draw_eve5_budget_t budget;
    budget.reserve = EVE5_DL_FINISH_LAYER;
    budget.alpha_pass = false;
    budget.can_split = false;
    budget.stop = NULL;
    int rendered_count = lv_draw_eve5_render_tasks(u, layer, /*is_screen=*/true,
                                                   /*finish_tasks=*/true, &slice, &budget);

    EVE_CoDl_display(phost);
    EVE_CoCmd_swap(phost);
    /* No sync marker needed here: this is the pre-BT820 NORT path, where the
     * allocator is GA3 — frees ride the Update sweep below, not the sync
     * pipeline. (CMD_SWAP itself is blocking on these gens anyway.) */

    /* Advance the scratch ring's in-use boundary to match the just-issued
     * CMD_SWAP. The frame we just built is now the "currently scanned out"
     * region; the frame before it is released. */
    lv_draw_eve5_ring_swap(u);

    /* Run the GA3 mark-and-sweep GC between fullscreen swaps. Every Get()
     * call this frame refreshed GA_USED_FLAG on handles in use; Update frees
     * GC-flagged handles whose USED bit wasn't set, then clears USED for the
     * next cycle. Two-frame lifecycle: a handle survives one Update with
     * USED set, then gets freed on the next Update if not refreshed.
     *
     * Timing: this must run AFTER the previous swap has completed, so the
     * frame whose handles we might free is no longer being scanned out by
     * the GPU. waitFlush blocks until the coprocessor drains its FIFO; on
     * pre-BT820, CMD_SWAP itself is blocking until V-sync, so once waitFlush
     * returns, the swap has happened and the old DL bank is inactive.
     *
     * Distinct from EVE_GpuAlloc_UpdateFree (sync-based deferred queue, GA5).
     * No-op on BT820 — GA5 has no frame-based GC and we don't take this path. */
    EVE_Cmd_waitFlush(phost);
    bool gc_freed_any = EVE_GpuAlloc_Update(u->allocator);

    /* Frame had alloc failures + GC just freed something → pending content
     * might fit next frame. Defer to LV_EVENT_REFR_READY; lv_obj_invalidate
     * during rendering trips the rendering_in_progress assert. */
    if(gc_freed_any && EVE_GpuAlloc_GetAllocFailedCount(u->allocator) > 0) {
        LV_LOG_INFO("EVE5 NORT: %u alloc failures + GC freed memory — requesting screen invalidate",
                    (unsigned)EVE_GpuAlloc_GetAllocFailedCount(u->allocator));
        lv_eve5_request_invalidate(disp);
    }

    /* Bookkeeping mirrors lv_draw_eve5_hal_finish_layer */
    lv_eve5_vram_res_t * vr = eve5_get_vram_res(layer);
    if(vr != NULL && rendered_count > 0) {
        vr->is_premultiplied = true;
        vr->has_content = true;
    }

    /* Tell the display driver this frame's swap was the screen swap, so
     * flush_cb's FULL-mode HW branch reclaims deferred frees instead of
     * trying to upload a SW-rendered px_map. EVE_CMD_SYNC_INVALID is
     * correct only while this path drains the frame host-side (the
     * waitFlush above); a pipelined NORT loop must place a sync marker
     * after CMD_SWAP and record that instead. */
    lv_eve5_record_frame_sync(disp, EVE_CMD_SYNC_INVALID);

    EVE5_LOG("EVE5 NORT: === RENDER END layer=%p rendered=%d ===",
             (void *)layer, rendered_count);

    u->rendering_in_progress = false;

#if LV_USE_OS
    lv_eve5_hal_unlock(disp);
#endif
}

#endif /* LV_USE_DRAW_EVE5 */
