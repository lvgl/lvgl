/**
 * @file lv_draw_eve5.c
 *
 * EVE5 (BT820) Draw Unit for LVGL
 * 
 * Architecture:
 * - Uses QUEUED state to accumulate tasks until all are added
 * - Each layer renders atomically when no WAITING tasks remain but QUEUED exist
 * - LVGL's dispatch-all-layers behavior ensures children complete before parents
 * - Each layer has its own complete display list cycle:
 *   dlStart -> commands -> display -> swap -> graphicsFinish
 * - Screen partial textures are handed to the display driver via layer->user_data
 * - Child layer textures are freed after compositing into their parent
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"

#define printf(...) do{}while(0)

/*********************
 * DEFINES
 *********************/
#define DRAW_UNIT_ID_EVE5 82

/**********************
 * STATIC PROTOTYPES
 **********************/
static int32_t dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer);
static int32_t evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task);
static void eve5_render_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer);
static const char *task_state_str(lv_draw_task_state_t state);
static const char *task_type_str(lv_draw_task_type_t type);

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
    unit->texture_cache = NULL; /* TODO: Initialize image cache */
	unit->rendering_in_progress = false;
	unit->frame_alloc_count = 0;
    
    printf("[EVE5] Draw unit initialized, ID=%d\n", DRAW_UNIT_ID_EVE5);
}

void lv_draw_eve5_deinit(void)
{
    printf("[EVE5] Draw unit deinitialized\n");
    /* TODO: Cleanup texture cache */
}

/**********************
 * SW FALLBACK RENDERING
 **********************/

// TODO: Use SW render cache similar to SDL implementation

/**
 * Render a draw task using SW fallback and return the result buffer.
 * The caller must free the returned buffer with lv_free().
 *
 * @param u          Draw unit (for display context)
 * @param t          Task to render via SW
 * @param out_w      Output: buffer width
 * @param out_h      Output: buffer height
 * @param out_area   Output: screen coordinates of buffer
 * @return           ARGB8888 pixel data, or NULL on failure
 */
static uint8_t *render_task_sw_fallback(lv_draw_eve5_unit_t *u,
                                         const lv_draw_task_t *t,
                                         int32_t *out_w, int32_t *out_h,
                                         lv_area_t *out_area)
{
    LV_UNUSED(u);

    /* Use task's _real_area which includes any expansion (blur, spread, etc.) */
    int32_t buf_w = lv_area_get_width(&t->_real_area);
    int32_t buf_h = lv_area_get_height(&t->_real_area);

    if(buf_w <= 0 || buf_h <= 0) {
        LV_LOG_WARN("EVE5: SW fallback area invalid");
        return NULL;
    }

    /* Allocate CPU buffer */
    uint32_t buf_stride = buf_w * 4;
    uint32_t buf_size = buf_stride * buf_h;
    uint8_t *buf_data = lv_malloc(buf_size);
    if(!buf_data) {
        LV_LOG_ERROR("EVE5: Failed to allocate SW fallback buffer (%"PRIu32" bytes)", buf_size);
        return NULL;
    }
    lv_memzero(buf_data, buf_size);

    /* Initialize draw buffer */
    lv_draw_buf_t sw_buf;
    lv_draw_buf_init(&sw_buf, buf_w, buf_h,
                     LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO,
                     buf_data, buf_size);

    /* Create temporary layer */
    lv_layer_t temp_layer;
    lv_memzero(&temp_layer, sizeof(temp_layer));
    temp_layer.draw_buf = &sw_buf;
    temp_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    temp_layer.buf_area = t->_real_area;
    temp_layer._clip_area = t->_real_area;
    temp_layer.phy_clip_area = t->_real_area;

    /* Re-issue the draw command based on task type.
     * Setting user_data != NULL prevents EVE5 from claiming it. */
    switch(t->type) {
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: {
            lv_draw_box_shadow_dsc_t *dsc = t->draw_dsc;
            
            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
            rect_dsc.bg_opa = LV_OPA_TRANSP;
            rect_dsc.radius = dsc->radius;
            rect_dsc.shadow_color = dsc->color;
            rect_dsc.shadow_opa = dsc->opa;
            rect_dsc.shadow_width = dsc->width;
            rect_dsc.shadow_spread = dsc->spread;
            rect_dsc.shadow_offset_x = dsc->ofs_x;
            rect_dsc.shadow_offset_y = dsc->ofs_y;
            
            lv_draw_rect(&temp_layer, &rect_dsc, &t->area);
            break;
        }

        /* Add other fallback types here as needed */

        default:
            LV_LOG_WARN("EVE5: No SW fallback for task type %d", t->type);
            lv_free(buf_data);
            return NULL;
    }

    /* Dispatch to SW renderer synchronously */
    lv_display_t *disp = lv_refr_get_disp_refreshing();
    while(temp_layer.draw_task_head) {
        lv_draw_dispatch_layer(disp, &temp_layer);
        if(temp_layer.draw_task_head) {
            lv_draw_dispatch_wait_for_request();
        }
    }

    /* Return results */
    *out_w = buf_w;
    *out_h = buf_h;
    *out_area = t->_real_area;

    LV_LOG_INFO("EVE5: SW fallback rendered %"PRId32"x%"PRId32" for task type %d",
                buf_w, buf_h, t->type);

    return buf_data;
}

/**********************
 * STATIC FUNCTIONS
 **********************/

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
        case LV_DRAW_TASK_TYPE_FILL:      return "FILL";
        case LV_DRAW_TASK_TYPE_BORDER:    return "BORDER";
        case LV_DRAW_TASK_TYPE_LINE:      return "LINE";
        case LV_DRAW_TASK_TYPE_TRIANGLE:  return "TRIANGLE";
        case LV_DRAW_TASK_TYPE_LABEL:     return "LABEL";
        case LV_DRAW_TASK_TYPE_IMAGE:     return "IMAGE";
        case LV_DRAW_TASK_TYPE_ARC:       return "ARC";
        case LV_DRAW_TASK_TYPE_LAYER:     return "LAYER";
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: return "BOX_SHADOW";
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE: return "MASK_RECT";
        default:                          return "OTHER";
    }
}

static int32_t evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task)
{
    LV_UNUSED(draw_unit);

    /* Skip tasks marked for SW fallback (re-issued with user_data set) */
    if(((lv_draw_dsc_base_t *)task->draw_dsc)->user_data != NULL) {
		printf("[EVE5] Evaluate: type=%s -> SW fallback\n", task_type_str(task->type));
        return 0;
    }

    task->preference_score = 10;
    task->preferred_draw_unit_id = DRAW_UNIT_ID_EVE5;

    printf("[EVE5] Evaluate: type=%s -> claimed\n", task_type_str(task->type));

    return 0;
}

static int32_t dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer)
{
    lv_draw_eve5_unit_t *u = (lv_draw_eve5_unit_t *)draw_unit;
    lv_draw_task_t *t;

	/* Prevent re-entry during SW fallback dispatch.
	* When we call lv_draw_dispatch_layer() for the temp SW layer,
	* LVGL will call our dispatch for ALL layers including the main
	* layer we're currently rendering. We must not start a new render. */
	if(u->rendering_in_progress) {
		printf("[EVE5] Dispatch skipped - render in progress\n");
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
    
    printf("[EVE5] Dispatch layer=%p parent=%p | W=%d Q=%d B=%d F=%d P=%d\n",
           (void *)layer, (void *)layer->parent,
           waiting_count, queued_count, blocked_count, finished_count, in_progress_count);
    
    /* Debug: print each task */
    t = layer->draw_task_head;
    int task_idx = 0;
    while(t) {
        if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5) {
            printf("  [%d] type=%-10s state=%-11s area=(%d,%d)-(%d,%d)\n",
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
        printf("[EVE5] -> Queueing %d tasks\n", waiting_count);
        
        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_WAITING) {
                t->state = LV_DRAW_TASK_STATE_QUEUED;
            }
            t = t->next;
        }
        
        lv_draw_dispatch_request();
        return waiting_count;
    }
    
    /* Case 2: No new tasks, but we have queued work - ALL TASKS ARE IN, render! */
    if(queued_count > 0) {
        printf("[EVE5] -> Rendering %d queued tasks atomically\n", queued_count);
        eve5_render_layer(u, layer);
        lv_draw_dispatch_request();
        return 1;
    }
    
    /* Case 3: Nothing for us to do */
    printf("[EVE5] -> IDLE\n");
    return LV_DRAW_UNIT_IDLE;
}

static void eve5_render_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer)
{
    lv_draw_task_t *t;
    bool is_screen = (layer->parent == NULL);

	/* Set re-entrancy guard before starting display list */
	u->rendering_in_progress = true;
    
    printf("[EVE5] === RENDER START layer=%p is_screen=%d ===\n", 
           (void *)layer, is_screen);
    printf("[EVE5] Layer buf_area=(%d,%d)-(%d,%d) clip=(%d,%d)-(%d,%d)\n",
           layer->buf_area.x1, layer->buf_area.y1,
           layer->buf_area.x2, layer->buf_area.y2,
           layer->_clip_area.x1, layer->_clip_area.y1,
           layer->_clip_area.x2, layer->_clip_area.y2);
    
    /* Initialize the layer (allocate texture, start display list) */
    lv_draw_eve5_hal_init_layer(u, layer, is_screen);
    
    if(layer->user_data == NULL) {
        printf("[EVE5] ERROR: Layer allocation failed!\n");
        
        /* Mark all queued tasks as finished to prevent infinite loop */
        t = layer->draw_task_head;
        while(t) {
            if(t->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
               t->state == LV_DRAW_TASK_STATE_QUEUED) {
                t->state = LV_DRAW_TASK_STATE_FINISHED;
            }
            t = t->next;
        }
        return;
    }
    
    /* Process all QUEUED tasks for this layer */
    t = layer->draw_task_head;
    int rendered_count = 0;
    
    while(t) {
        /* Skip tasks not assigned to us or not queued */
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED) {
            t = t->next;
            continue;
        }
        
        t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
        
        printf("[EVE5] Render task: type=%-10s area=(%d,%d)-(%d,%d)\n",
               task_type_str(t->type),
               t->area.x1, t->area.y1, t->area.x2, t->area.y2);
        
        switch(t->type) {
            case LV_DRAW_TASK_TYPE_FILL:
                lv_draw_eve5_hal_draw_fill(u, t);
                break;

            case LV_DRAW_TASK_TYPE_BORDER:
                lv_draw_eve5_hal_draw_border(u, t);
                break;

            case LV_DRAW_TASK_TYPE_LINE:
                lv_draw_eve5_hal_draw_line(u, t);
                break;

            case LV_DRAW_TASK_TYPE_TRIANGLE:
                lv_draw_eve5_hal_draw_triangle(u, t);
                break;

            case LV_DRAW_TASK_TYPE_LABEL:
                lv_draw_eve5_hal_draw_label(u, t);
                break;

            case LV_DRAW_TASK_TYPE_IMAGE:
                lv_draw_eve5_hal_draw_image(u, t);
                break;

            case LV_DRAW_TASK_TYPE_ARC:
                lv_draw_eve5_hal_draw_arc(u, t);
                break;

            case LV_DRAW_TASK_TYPE_LAYER:
                /* Child layer should already be a finished texture in RAM_G.
                 * Just composite it like a regular image. */
                printf("[EVE5]   LAYER task: child=%p\n", 
                       ((lv_draw_image_dsc_t *)t->draw_dsc)->src);
                lv_draw_eve5_hal_draw_image(u, t);
                break;

            case LV_DRAW_TASK_TYPE_BOX_SHADOW: {
                int32_t sw_w, sw_h;
                lv_area_t sw_area;
                uint8_t *sw_buf = render_task_sw_fallback(u, t, &sw_w, &sw_h, &sw_area);
                
                if(sw_buf) {
                    lv_draw_eve5_hal_composite_buffer(u, t, sw_buf, sw_w, sw_h, &sw_area);
                    lv_free(sw_buf);
                }
                break;
            }

            default:
                printf("[EVE5]   Unhandled task type %d, skipping\n", t->type);
                break;
        }
        
        t->state = LV_DRAW_TASK_STATE_FINISHED;
        rendered_count++;
        t = t->next;
    }
    
    /* Finalize the display list */
    printf("[EVE5] Finishing layer, rendered %d tasks\n", rendered_count);
    lv_draw_eve5_hal_finish_layer(u, layer, is_screen);
    
    /* For child layers: texture stays in user_data for parent to use.
     * For screen layers: texture ownership transfers to display driver.
     * 
     * Child texture cleanup happens automatically when LVGL processes
     * the LAYER task in cleanup_task() - but we allocated via Esd_GpuAlloc,
     * so we need to handle it ourselves. 
     *
     * The parent will have already rendered by the time cleanup_task()
     * is called for the LAYER task (because we just finished the parent),
     * so we're safe.
     *
     * Actually - with the QUEUED pattern, LVGL handles the ordering:
     * - Child finishes and gets cleaned up 
     * - ONLY THEN parent's LAYER task becomes WAITING
     * - Parent queues and renders
     *
     * So by the time parent renders, child texture is still valid!
     * We need to free child textures AFTER parent's graphicsFinish().
     * 
     * For now, let's track this via layer->user_data and free in
     * the HAL finish or via a separate cleanup pass.
     */
    
    printf("[EVE5] === RENDER END layer=%p ===\n", (void *)layer);

	/* Clear guard after display list is complete */
	u->rendering_in_progress = false;
}

#endif /* LV_USE_DRAW_EVE5 */
