/**
 * @file lv_draw_eve5.c
 *
 * EVE5 (BT820) Draw Unit for LVGL
 */

#include "lv_draw_eve5_private.h"

#if LV_USE_DRAW_EVE5

#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"

/*********************
 * DEFINES
 *********************/
#define DRAW_UNIT_ID_EVE5 82

#if 0
#define EVE5_LOG(...) LV_LOG_INFO(__VA_ARGS__)
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

/**********************
 * STATIC PROTOTYPES
 **********************/
static int32_t dispatch(lv_draw_unit_t *draw_unit, lv_layer_t *layer);
static int32_t evaluate(lv_draw_unit_t *draw_unit, lv_draw_task_t *task);
static void eve5_render_layer(lv_draw_eve5_unit_t *u, lv_layer_t *layer);
static const char *task_state_str(lv_draw_task_state_t state);
static const char *task_type_str(lv_draw_task_type_t type);

/* SW fallback helpers - declared before use */
static const void *get_dsc_cache_data(const lv_draw_task_t *t, uint32_t *out_size);
static uint8_t *render_sw_to_buffer(lv_draw_eve5_unit_t *u,
                                     const lv_draw_task_t *t,
                                     int32_t buf_w, int32_t buf_h);
static Esd_GpuHandle render_task_sw_cached(lv_draw_eve5_unit_t *u,
                                            const lv_draw_task_t *t,
                                            int32_t *out_w, int32_t *out_h,
                                            uint32_t *out_stride,
                                            bool *out_from_cache);
static void render_task_via_sw(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t);

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

    LV_LOG_INFO("EVE5: Draw unit initialized, ID=%d", DRAW_UNIT_ID_EVE5);

    /* Log SW fallback configuration */
    LV_LOG_INFO("EVE5: SW fallback config: FILL=%d BORDER=%d LINE=%d TRI=%d LABEL=%d ARC=%d SHADOW=%d",
                LV_DRAW_EVE5_SW_FILL, LV_DRAW_EVE5_SW_BORDER, LV_DRAW_EVE5_SW_LINE,
                LV_DRAW_EVE5_SW_TRIANGLE, LV_DRAW_EVE5_SW_LABEL, LV_DRAW_EVE5_SW_ARC,
                LV_DRAW_EVE5_SW_BOX_SHADOW);
}

void lv_draw_eve5_deinit(void)
{
    LV_LOG_INFO("EVE5: Draw unit deinitialized");
}

/**********************
 * SW FALLBACK HELPERS
 **********************/

/**
 * Get the descriptor data pointer and size for cache comparison.
 * Returns pointer to data AFTER the base descriptor.
 */
static const void *get_dsc_cache_data(const lv_draw_task_t *t, uint32_t *out_size)
{
    const lv_draw_dsc_base_t *base = t->draw_dsc;
    uint32_t full_size = base->dsc_size;
    uint32_t base_size = sizeof(lv_draw_dsc_base_t);

    if(full_size <= base_size) {
        *out_size = 0;
        return NULL;
    }

    *out_size = full_size - base_size;
    return (const uint8_t *)base + base_size;
}

/**
 * Helper: render task to a CPU buffer using SW fallback.
 * Caller must free returned buffer.
 */
static uint8_t *render_sw_to_buffer(lv_draw_eve5_unit_t *u,
                                     const lv_draw_task_t *t,
                                     int32_t buf_w, int32_t buf_h)
{
    LV_UNUSED(u);

    uint32_t buf_stride = buf_w * 4;
    uint32_t buf_size = buf_stride * buf_h;
    uint8_t *buf_data = lv_malloc(buf_size);

    if(!buf_data) {
        LV_LOG_ERROR("EVE5: Failed to allocate SW buffer (%"PRIu32" bytes)", buf_size);
        return NULL;
    }
    lv_memzero(buf_data, buf_size);

    lv_draw_buf_t sw_buf;
    lv_draw_buf_init(&sw_buf, buf_w, buf_h,
                     LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO,
                     buf_data, buf_size);

    /* Normalized coordinates (0,0 origin) */
    lv_area_t norm_area;
    lv_area_set(&norm_area, 0, 0, buf_w - 1, buf_h - 1);

    lv_layer_t temp_layer;
    lv_memzero(&temp_layer, sizeof(temp_layer));
    temp_layer.draw_buf = &sw_buf;
    temp_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    temp_layer.buf_area = norm_area;
    temp_layer._clip_area = norm_area;
    temp_layer.phy_clip_area = norm_area;

    int32_t ofs_x = t->_real_area.x1;
    int32_t ofs_y = t->_real_area.y1;

    bool render_ok = false;

    switch(t->type) {
#if LV_DRAW_EVE5_SW_FILL
        case LV_DRAW_TASK_TYPE_FILL: {
            lv_draw_fill_dsc_t *src_dsc = t->draw_dsc;

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
            rect_dsc.bg_color = src_dsc->color;
            rect_dsc.bg_grad = src_dsc->grad;
            rect_dsc.radius = src_dsc->radius;
            rect_dsc.bg_opa = src_dsc->opa;

            lv_draw_rect(&temp_layer, &rect_dsc, &norm_task_area);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_BORDER
        case LV_DRAW_TASK_TYPE_BORDER: {
            lv_draw_border_dsc_t *src_dsc = t->draw_dsc;

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
            rect_dsc.bg_opa = LV_OPA_TRANSP;
            rect_dsc.radius = src_dsc->radius;
            rect_dsc.border_color = src_dsc->color;
            rect_dsc.border_opa = src_dsc->opa;
            rect_dsc.border_side = src_dsc->side;
            rect_dsc.border_width = src_dsc->width;

            lv_draw_rect(&temp_layer, &rect_dsc, &norm_task_area);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_LINE
        case LV_DRAW_TASK_TYPE_LINE: {
            lv_draw_line_dsc_t line_dsc;
            lv_memcpy(&line_dsc, t->draw_dsc, sizeof(line_dsc));
            line_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            /* Normalize coordinates */
            line_dsc.p1.x -= ofs_x;
            line_dsc.p1.y -= ofs_y;
            line_dsc.p2.x -= ofs_x;
            line_dsc.p2.y -= ofs_y;

            lv_draw_line(&temp_layer, &line_dsc);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_TRIANGLE
        case LV_DRAW_TASK_TYPE_TRIANGLE: {
            lv_draw_triangle_dsc_t tri_dsc;
            lv_memcpy(&tri_dsc, t->draw_dsc, sizeof(tri_dsc));
            tri_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            /* Normalize coordinates */
            tri_dsc.p[0].x -= ofs_x;
            tri_dsc.p[0].y -= ofs_y;
            tri_dsc.p[1].x -= ofs_x;
            tri_dsc.p[1].y -= ofs_y;
            tri_dsc.p[2].x -= ofs_x;
            tri_dsc.p[2].y -= ofs_y;

            lv_draw_triangle(&temp_layer, &tri_dsc);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_LABEL
        case LV_DRAW_TASK_TYPE_LABEL: {
            lv_draw_label_dsc_t label_dsc;
            lv_memcpy(&label_dsc, t->draw_dsc, sizeof(label_dsc));
            label_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_label(&temp_layer, &label_dsc, &norm_task_area);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_ARC
        case LV_DRAW_TASK_TYPE_ARC: {
            lv_draw_arc_dsc_t arc_dsc;
            lv_memcpy(&arc_dsc, t->draw_dsc, sizeof(arc_dsc));
            arc_dsc.base.user_data = (void *)1;  /* SW fallback marker */

            /* Normalize center coordinates */
            arc_dsc.center.x -= ofs_x;
            arc_dsc.center.y -= ofs_y;

            lv_draw_arc(&temp_layer, &arc_dsc);
            render_ok = true;
            break;
        }
#endif

#if LV_DRAW_EVE5_SW_BOX_SHADOW
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: {
            lv_draw_box_shadow_dsc_t *src_dsc = t->draw_dsc;

            lv_area_t norm_task_area;
            norm_task_area.x1 = t->area.x1 - ofs_x;
            norm_task_area.y1 = t->area.y1 - ofs_y;
            norm_task_area.x2 = t->area.x2 - ofs_x;
            norm_task_area.y2 = t->area.y2 - ofs_y;

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.base.user_data = (void *)1;  /* SW fallback marker */
            rect_dsc.bg_opa = LV_OPA_TRANSP;
            rect_dsc.radius = src_dsc->radius;
            rect_dsc.shadow_color = src_dsc->color;
            rect_dsc.shadow_opa = src_dsc->opa;
            rect_dsc.shadow_width = src_dsc->width;
            rect_dsc.shadow_spread = src_dsc->spread;
            rect_dsc.shadow_offset_x = src_dsc->ofs_x;
            rect_dsc.shadow_offset_y = src_dsc->ofs_y;

            lv_draw_rect(&temp_layer, &rect_dsc, &norm_task_area);
            render_ok = true;
            break;
        }
#endif

        default:
            LV_LOG_WARN("EVE5: No SW fallback for task type %d", t->type);
            break;
    }

    if(!render_ok) {
        lv_free(buf_data);
        return NULL;
    }

    /* Dispatch to SW renderer */
    lv_display_t *disp = lv_refr_get_disp_refreshing();
    while(temp_layer.draw_task_head) {
        lv_draw_dispatch_layer(disp, &temp_layer);
        if(temp_layer.draw_task_head) {
            lv_draw_dispatch_wait_for_request();
        }
    }

    return buf_data;
}

/**
 * Render a task via SW fallback, with caching.
 */
static Esd_GpuHandle render_task_sw_cached(lv_draw_eve5_unit_t *u,
                                            const lv_draw_task_t *t,
                                            int32_t *out_w, int32_t *out_h,
                                            uint32_t *out_stride,
                                            bool *out_from_cache)
{
    *out_from_cache = false;

    int32_t buf_w = lv_area_get_width(&t->_real_area);
    int32_t buf_h = lv_area_get_height(&t->_real_area);

    if(buf_w <= 0 || buf_h <= 0) {
        return GA_HANDLE_INVALID;
    }

    /* Get descriptor data for cache key (excluding base) */
    uint32_t dsc_size;
    const void *dsc_data = get_dsc_cache_data(t, &dsc_size);

    /* Check cache first */
    Esd_GpuHandle cached_handle;
    uint32_t cached_stride;

    if(lv_draw_eve5_sw_cache_lookup(u, t->type, buf_w, buf_h,
                                     dsc_data, dsc_size,
                                     &cached_handle, &cached_stride)) {
        *out_w = buf_w;
        *out_h = buf_h;
        *out_stride = cached_stride;
        *out_from_cache = true;
        return cached_handle;
    }

    /* Cache miss - render via SW */
    uint8_t *buf_data = render_sw_to_buffer(u, t, buf_w, buf_h);
    if(!buf_data) {
        return GA_HANDLE_INVALID;
    }

    /* Upload to RAM_G via HAL */
    uint32_t eve_stride;
    Esd_GpuHandle handle = lv_draw_eve5_hal_upload_texture(u, buf_data,
                                                           buf_w, buf_h,
                                                           &eve_stride);
    lv_free(buf_data);

    if(handle.Id == GA_HANDLE_INVALID.Id) {
        return GA_HANDLE_INVALID;
    }

    /* Insert into cache */
    lv_draw_eve5_sw_cache_insert(u, t->type, buf_w, buf_h,
                                  dsc_data, dsc_size, handle, eve_stride);

    *out_w = buf_w;
    *out_h = buf_h;
    *out_stride = eve_stride;
    *out_from_cache = true;

    LV_LOG_INFO("EVE5: SW fallback rendered and cached %"PRId32"x%"PRId32" type=%d",
                buf_w, buf_h, t->type);

    return handle;
}

/**
 * Helper: render a task via SW fallback and blit to screen.
 */
static void render_task_via_sw(lv_draw_eve5_unit_t *u, const lv_draw_task_t *t)
{
    int32_t tex_w, tex_h;
    uint32_t tex_stride;
    bool from_cache;

    Esd_GpuHandle handle = render_task_sw_cached(u, t, &tex_w, &tex_h, &tex_stride, &from_cache);

    if(handle.Id == GA_HANDLE_INVALID.Id) {
        LV_LOG_WARN("EVE5: SW fallback failed for task type %d", t->type);
        return;
    }

    uint32_t addr = Esd_GpuAlloc_Get(u->allocator, handle);
    lv_draw_eve5_hal_draw_texture(u, t, addr, tex_w, tex_h, tex_stride, &t->_real_area);
}

/**********************
 * DEBUG HELPERS
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
        case LV_DRAW_TASK_TYPE_FILL:       return "FILL";
        case LV_DRAW_TASK_TYPE_BORDER:     return "BORDER";
        case LV_DRAW_TASK_TYPE_LINE:       return "LINE";
        case LV_DRAW_TASK_TYPE_TRIANGLE:   return "TRIANGLE";
        case LV_DRAW_TASK_TYPE_LABEL:      return "LABEL";
        case LV_DRAW_TASK_TYPE_IMAGE:      return "IMAGE";
        case LV_DRAW_TASK_TYPE_ARC:        return "ARC";
        case LV_DRAW_TASK_TYPE_LAYER:      return "LAYER";
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: return "BOX_SHADOW";
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE: return "MASK_RECT";
        default:                           return "OTHER";
    }
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
        if (layer->parent == NULL) {
            layer->all_tasks_added = false;
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
    bool is_screen = (layer->parent == NULL);

    u->rendering_in_progress = true;

    EVE5_LOG("EVE5: === RENDER START layer=%p is_screen=%d ===",
             (void *)layer, is_screen);
    EVE5_LOG("EVE5: Layer buf_area=(%d,%d)-(%d,%d) clip=(%d,%d)-(%d,%d)",
             layer->buf_area.x1, layer->buf_area.y1,
             layer->buf_area.x2, layer->buf_area.y2,
             layer->_clip_area.x1, layer->_clip_area.y1,
             layer->_clip_area.x2, layer->_clip_area.y2);

    /* Initialize the layer (allocate texture, start display list) */
    lv_draw_eve5_hal_init_layer(u, layer, is_screen);

    if(layer->user_data == NULL) {
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
        return;
    }

    /* Advance SW cache frame counter */
    lv_draw_eve5_sw_cache_new_frame(u);

    /* Process all QUEUED tasks for this layer */
    t = layer->draw_task_head;
    int rendered_count = 0;

    while(t) {
        if(t->preferred_draw_unit_id != DRAW_UNIT_ID_EVE5 ||
           t->state != LV_DRAW_TASK_STATE_QUEUED) {
            t = t->next;
            continue;
        }

        t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

        EVE5_LOG("EVE5: Render task: type=%-10s area=(%d,%d)-(%d,%d)",
                 task_type_str(t->type),
                 t->area.x1, t->area.y1, t->area.x2, t->area.y2);

        switch(t->type) {
case LV_DRAW_TASK_TYPE_FILL: {
                /* Check if next task is a matching BORDER from same lv_draw_rect() */
                lv_draw_task_t *next = t->next;
                bool has_matching_border = false;

                if(next &&
                   next->preferred_draw_unit_id == DRAW_UNIT_ID_EVE5 &&
                   next->state == LV_DRAW_TASK_STATE_QUEUED &&
                   next->type == LV_DRAW_TASK_TYPE_BORDER &&
                   next->target_layer == t->target_layer) {

                    const lv_draw_fill_dsc_t *fill_dsc = t->draw_dsc;
                    const lv_draw_border_dsc_t *border_dsc = next->draw_dsc;

                    /* LVGL shrinks fill by 1px when border is opaque with radius.
                     * Fill should be inside or equal to border area. */
                    int32_t dx1 = t->area.x1 - next->area.x1;
                    int32_t dy1 = t->area.y1 - next->area.y1;
                    int32_t dx2 = t->area.x2 - next->area.x2;
                    int32_t dy2 = t->area.y2 - next->area.y2;

                    bool area_match = (dx1 >= 0 && dx1 <= 2) &&
                                      (dy1 >= 0 && dy1 <= 2) &&
                                      (dx2 <= 0 && dx2 >= -2) &&
                                      (dy2 <= 0 && dy2 >= -2);

                    bool radius_match = (fill_dsc->radius == border_dsc->radius);

                    has_matching_border = area_match && radius_match;
                }

                if(has_matching_border) {
                    EVE5_LOG("EVE5: Unified FILL+BORDER rendering");

                    next->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

#if LV_DRAW_EVE5_SW_FILL || LV_DRAW_EVE5_SW_BORDER
                    render_task_via_sw(u, t);
                    render_task_via_sw(u, next);
#else
                    lv_draw_eve5_hal_draw_fill_with_border(u, t, next);
#endif
                    next->state = LV_DRAW_TASK_STATE_FINISHED;
                    rendered_count++;
                }
                else {
#if LV_DRAW_EVE5_SW_FILL
                    render_task_via_sw(u, t);
#else
                    lv_draw_eve5_hal_draw_fill(u, t);
#endif
                }
                break;
            }

            case LV_DRAW_TASK_TYPE_BORDER:
#if LV_DRAW_EVE5_SW_BORDER
                render_task_via_sw(u, t);
#else
                lv_draw_eve5_hal_draw_border(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LINE:
#if LV_DRAW_EVE5_SW_LINE
                render_task_via_sw(u, t);
#else
                lv_draw_eve5_hal_draw_line(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_TRIANGLE:
#if LV_DRAW_EVE5_SW_TRIANGLE
                render_task_via_sw(u, t);
#else
                lv_draw_eve5_hal_draw_triangle(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LABEL:
#if LV_DRAW_EVE5_SW_LABEL
                render_task_via_sw(u, t);
#else
                lv_draw_eve5_hal_draw_label(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_IMAGE:
                lv_draw_eve5_hal_draw_image(u, t);
                break;

            case LV_DRAW_TASK_TYPE_ARC:
#if LV_DRAW_EVE5_SW_ARC
                render_task_via_sw(u, t);
#else
                lv_draw_eve5_hal_draw_arc(u, t);
#endif
                break;

            case LV_DRAW_TASK_TYPE_LAYER: {
                lv_draw_image_dsc_t *dsc = t->draw_dsc;
                lv_layer_t *child = (lv_layer_t *)dsc->src;

                if(child->user_data == NULL) {
                    /* Child layer was never rendered (empty or clipped out).
                    * Either allocate a transparent texture or skip entirely. */

                    if(child->draw_buf == NULL) {
                        /* Truly empty layer - nothing to composite */
						LV_LOG_WARN("EVE5: Skipping empty LAYER task for child %p", (void *)child);
                        break;  /* Skip rendering, task will be marked FINISHED */
                    }

                    /* Child has a CPU buffer but no GPU texture - upload it now */
                    // lv_draw_eve5_hal_upload_layer_buffer(u, child);
                }

                if(child->user_data != NULL) {
                    lv_draw_eve5_hal_draw_image(u, t);
                }
                break;
            }

            case LV_DRAW_TASK_TYPE_BOX_SHADOW:
#if LV_DRAW_EVE5_SW_BOX_SHADOW
                render_task_via_sw(u, t);
#else
                lv_draw_eve5_hal_draw_box_shadow(u, t);
#endif
                break;

            default:
                EVE5_LOG("EVE5:   Unhandled task type %d, skipping", t->type);
                break;
        }

        t->state = LV_DRAW_TASK_STATE_FINISHED;
        rendered_count++;
        t = t->next;
    }

    EVE5_LOG("EVE5: Finishing layer, rendered %d tasks", rendered_count);
    lv_draw_eve5_hal_finish_layer(u, layer, is_screen);

    EVE5_LOG("EVE5: === RENDER END layer=%p ===", (void *)layer);

    u->rendering_in_progress = false;
}

#endif /* LV_USE_DRAW_EVE5 */
