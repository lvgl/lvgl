/**
 * @file lv_draw.c
 *
 */

/**
 * Modified by NXP in 2024
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area_private.h"
#include "../misc/lv_assert.h"
#include "lv_draw_private.h"
#include "sw/lv_draw_sw.h"
#include "../display/lv_display_private.h"
#include "../core/lv_global.h"
#include "../core/lv_refr_private.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool is_independent(lv_layer_t * layer, lv_draw_task_t * t_check);
static void lv_cleanup_task(lv_draw_task_t * t, lv_display_t * disp);

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
static inline uint32_t get_layer_size_kb(uint32_t size_byte)
{
    return (size_byte + 1023) >> 10;
}
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

void lv_draw_init(void)
{
#if LV_USE_OS
    lv_thread_sync_init(&_draw_info.sync);
#endif
}

void lv_draw_deinit(void)
{
#if LV_USE_OS
    lv_thread_sync_delete(&_draw_info.sync);
#endif

    lv_draw_unit_t * u = _draw_info.unit_head;
    while(u) {
        lv_draw_unit_t * cur_unit = u;
        u = u->next;

        if(cur_unit->delete_cb) cur_unit->delete_cb(cur_unit);
        lv_free(cur_unit);
    }
    _draw_info.unit_head = NULL;
}

void * lv_draw_create_unit(size_t size)
{
    lv_draw_unit_t * new_unit = lv_malloc_zeroed(size);
    LV_ASSERT_MALLOC(new_unit);
    new_unit->next = _draw_info.unit_head;
    _draw_info.unit_head = new_unit;
    _draw_info.unit_cnt++;

    return new_unit;
}

lv_draw_task_t * lv_draw_add_task(lv_layer_t * layer, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_task_t * new_task = lv_malloc_zeroed(sizeof(lv_draw_task_t));
    LV_ASSERT_MALLOC(new_task);
    new_task->area = *coords;
    new_task->_real_area = *coords;
    new_task->clip_area = layer->_clip_area;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    new_task->matrix = layer->matrix;
#endif
    new_task->state = LV_DRAW_TASK_STATE_QUEUED;

    /*Find the tail*/
    if(layer->draw_task_head == NULL) {
        layer->draw_task_head = new_task;
    }
    else {
        lv_draw_task_t * tail = layer->draw_task_head;
        while(tail->next) tail = tail->next;

        tail->next = new_task;
    }

    LV_PROFILER_DRAW_END;
    return new_task;
}

void lv_draw_finalize_task_creation(lv_layer_t * layer, lv_draw_task_t * t)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_dsc_base_t * base_dsc = t->draw_dsc;
    base_dsc->layer = layer;

    lv_draw_global_info_t * info = &_draw_info;

    /*Send LV_EVENT_DRAW_TASK_ADDED and dispatch only on the "main" draw_task
     *and not on the draw tasks added in the event.
     *Sending LV_EVENT_DRAW_TASK_ADDED events might cause recursive event sends and besides
     *dispatching might remove the "main" draw task while it's still being used in the event*/

    if(info->task_running == false) {
        if(base_dsc->obj && lv_obj_has_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS)) {
            info->task_running = true;
            lv_obj_send_event(base_dsc->obj, LV_EVENT_DRAW_TASK_ADDED, t);
            info->task_running = false;
        }

        /*Let the draw units set their preference score*/
        t->preference_score = 100;
        t->preferred_draw_unit_id = 0;
        lv_draw_unit_t * u = info->unit_head;
        while(u) {
            if(u->evaluate_cb) {
                LV_PROFILER_DRAW_BEGIN_TAG("evaluate_cb");
                LV_PROFILER_DRAW_BEGIN_TAG(u->name);
                u->evaluate_cb(u, t);
                LV_PROFILER_DRAW_END_TAG(u->name);
                LV_PROFILER_DRAW_END_TAG("evaluate_cb");
            }
            u = u->next;
        }
        if(t->preferred_draw_unit_id == LV_DRAW_UNIT_NONE) {
            LV_LOG_WARN("the draw task was not taken by any units");
            t->state = LV_DRAW_TASK_STATE_READY;
        }
        else {
            lv_draw_dispatch();
        }
    }
    else {
        /*Let the draw units set their preference score*/
        t->preference_score = 100;
        t->preferred_draw_unit_id = 0;
        lv_draw_unit_t * u = info->unit_head;
        while(u) {
            if(u->evaluate_cb) {
                LV_PROFILER_DRAW_BEGIN_TAG("evaluate_cb");
                LV_PROFILER_DRAW_BEGIN_TAG(u->name);
                u->evaluate_cb(u, t);
                LV_PROFILER_DRAW_END_TAG(u->name);
                LV_PROFILER_DRAW_END_TAG("evaluate_cb");
            }
            u = u->next;
        }
    }
    LV_PROFILER_DRAW_END;
}

void lv_draw_wait_for_finish(void)
{
#if LV_USE_OS
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_unit_t * u = _draw_info.unit_head;
    while(u) {
        if(u->wait_for_finish_cb) {
            LV_PROFILER_DRAW_BEGIN_TAG("wait_for_finish_cb");
            LV_PROFILER_DRAW_BEGIN_TAG(u->name);
            u->wait_for_finish_cb(u);
            LV_PROFILER_DRAW_END_TAG(u->name);
            LV_PROFILER_DRAW_END_TAG("wait_for_finish_cb");
        }
        u = u->next;
    }
    LV_PROFILER_DRAW_END;
#endif
}

void lv_draw_dispatch(void)
{
    LV_PROFILER_DRAW_BEGIN;
    bool task_dispatched = false;
    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        lv_layer_t * layer = disp->layer_head;
        while(layer) {
            if(lv_draw_dispatch_layer(disp, layer))
                task_dispatched = true;
            layer = layer->next;
        }
        if(!task_dispatched) {
            lv_draw_wait_for_finish();
            lv_draw_dispatch_request();
        }
        disp = lv_display_get_next(disp);
    }
    LV_PROFILER_DRAW_END;
}

bool lv_draw_dispatch_layer(lv_display_t * disp, lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    /*Remove the finished tasks first*/
    lv_draw_task_t * t_prev = NULL;
    lv_draw_task_t * t = layer->draw_task_head;
    lv_draw_task_t * t_next;
    while(t) {
        t_next = t->next;
        if(t->state == LV_DRAW_TASK_STATE_READY) {
            lv_cleanup_task(t, disp);
            if(t_prev != NULL)
                t_prev->next = t_next;
            else
                layer->draw_task_head = t_next;
        }
        else {
            t_prev = t;
        }
        t = t_next;
    }

    bool task_dispatched = false;

    /*This layer is ready, enable blending its buffer*/
    if(layer->parent && layer->all_tasks_added && layer->draw_task_head == NULL) {
        /*Find a draw task with TYPE_LAYER in the layer where the src is this layer*/
        lv_draw_task_t * t_src = layer->parent->draw_task_head;
        while(t_src) {
            if(t_src->type == LV_DRAW_TASK_TYPE_LAYER && t_src->state == LV_DRAW_TASK_STATE_WAITING) {
                lv_draw_image_dsc_t * draw_dsc = t_src->draw_dsc;
                if(draw_dsc->src == layer) {
                    t_src->state = LV_DRAW_TASK_STATE_QUEUED;
                    lv_draw_dispatch_request();
                    break;
                }
            }
            t_src = t_src->next;
        }
    }
    /*Assign draw tasks to the draw_units*/
    else {
        /*Find a draw unit which is not busy and can take at least one task*/
        /*Let all draw units to pick draw tasks*/
        lv_draw_unit_t * u = _draw_info.unit_head;
        while(u) {
            LV_PROFILER_DRAW_BEGIN_TAG("dispatch_cb");
            LV_PROFILER_DRAW_BEGIN_TAG(u->name);
            int32_t taken_cnt = u->dispatch_cb(u, layer);
            LV_PROFILER_DRAW_END_TAG(u->name);
            LV_PROFILER_DRAW_END_TAG("dispatch_cb");
            if(taken_cnt != LV_DRAW_UNIT_IDLE) task_dispatched = true;
            u = u->next;
        }
    }

    LV_PROFILER_DRAW_END;
    return task_dispatched;
}

void lv_draw_dispatch_wait_for_request(void)
{
    LV_PROFILER_DRAW_BEGIN;
#if LV_USE_OS
    lv_thread_sync_wait(&_draw_info.sync);
#else
    while(!_draw_info.dispatch_req);
    _draw_info.dispatch_req = 0;
#endif
    LV_PROFILER_DRAW_END;
}

void lv_draw_dispatch_request(void)
{
    LV_PROFILER_DRAW_BEGIN;
#if LV_USE_OS
    lv_thread_sync_signal(&_draw_info.sync);
#else
    _draw_info.dispatch_req = 1;
#endif
    LV_PROFILER_DRAW_END;
}

uint32_t lv_draw_get_unit_count(void)
{
    return _draw_info.unit_cnt;
}

lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev, uint8_t draw_unit_id)
{
    LV_PROFILER_DRAW_BEGIN;

    /* If there is only 1 draw unit the task can be consumed linearly as
     * they are added in the correct order. However, it can happen that
     * there is a `LV_DRAW_TASK_TYPE_LAYER` which can be blended only when
     * all its tasks are ready. As other areas might be on top of that
     * layer-to-blend don't skip it. Instead stop there, so that the
     * draw tasks of that layer can be consumed and can be finished.
     * After that this layer-to-blenf will have `LV_DRAW_TASK_STATE_QUEUED`
     * so it can be blended normally.*/
    if(_draw_info.unit_cnt <= 1) {
        lv_draw_task_t * t = layer->draw_task_head;
        while(t) {
            /*Not queued yet, leave this layer while the first task will be queued*/
            if(t->state != LV_DRAW_TASK_STATE_QUEUED) {
                t = NULL;
                break;
            }
            /*It's a supported and queued task, process it*/
            else {
                break;
            }
            t = t->next;
        }
        LV_PROFILER_DRAW_END;
        return t;
    }

    /*Handle the case of multiply draw units*/

    /*If the first task is screen sized, there cannot be independent areas*/
    if(layer->draw_task_head) {
        int32_t hor_res = lv_display_get_horizontal_resolution(lv_refr_get_disp_refreshing());
        int32_t ver_res = lv_display_get_vertical_resolution(lv_refr_get_disp_refreshing());
        lv_draw_task_t * t = layer->draw_task_head;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED &&
           t->area.x1 <= 0 && t->area.x2 >= hor_res - 1 &&
           t->area.y1 <= 0 && t->area.y2 >= ver_res - 1) {
            LV_PROFILER_DRAW_END;
            return NULL;
        }
    }

    lv_draw_task_t * t = t_prev ? t_prev->next : layer->draw_task_head;
    while(t) {
        /*Find a queued and independent task*/
        if(t->state == LV_DRAW_TASK_STATE_QUEUED &&
           (t->preferred_draw_unit_id == LV_DRAW_UNIT_NONE || t->preferred_draw_unit_id == draw_unit_id) &&
           is_independent(layer, t)) {
            LV_PROFILER_DRAW_END;
            return t;
        }
        t = t->next;
    }

    LV_PROFILER_DRAW_END;
    return NULL;
}

uint32_t lv_draw_get_dependent_count(lv_draw_task_t * t_check)
{
    if(t_check == NULL) return 0;
    if(t_check->next == NULL) return 0;

    LV_PROFILER_DRAW_BEGIN;
    uint32_t cnt = 0;

    lv_draw_task_t * t = t_check->next;
    while(t) {
        if((t->state == LV_DRAW_TASK_STATE_QUEUED || t->state == LV_DRAW_TASK_STATE_WAITING) &&
           lv_area_is_on(&t_check->area, &t->area)) {
            cnt++;
        }

        t = t->next;
    }
    LV_PROFILER_DRAW_END;
    return cnt;
}

void lv_layer_init(lv_layer_t * layer)
{
    LV_ASSERT_NULL(layer);
    lv_memzero(layer, sizeof(lv_layer_t));
    lv_layer_reset(layer);
}

void lv_layer_reset(lv_layer_t * layer)
{
    LV_ASSERT_NULL(layer);
#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_matrix_identity(&layer->matrix);
#endif
    layer->opa = LV_OPA_COVER;
}

lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_layer_t * new_layer = lv_malloc_zeroed(sizeof(lv_layer_t));
    LV_ASSERT_MALLOC(new_layer);
    if(new_layer == NULL) {
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    lv_draw_layer_init(new_layer, parent_layer, color_format, area);

    /*Inherits transparency from parent*/
    if(parent_layer) {
        new_layer->opa = parent_layer->opa;
    }

    LV_PROFILER_DRAW_END;
    return new_layer;
}

void lv_draw_layer_init(lv_layer_t * layer, lv_layer_t * parent_layer, lv_color_format_t color_format,
                        const lv_area_t * area)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_layer_init(layer);
    lv_display_t * disp = lv_refr_get_disp_refreshing();

    layer->parent = parent_layer;
    layer->_clip_area = *area;
    layer->buf_area = *area;
    layer->phy_clip_area = *area;
    layer->color_format = color_format;

    if(disp->layer_init) disp->layer_init(disp, layer);

    if(disp->layer_head) {
        lv_layer_t * tail = disp->layer_head;
        while(tail->next) tail = tail->next;
        tail->next = layer;
    }
    else {
        disp->layer_head = layer;
    }

    LV_PROFILER_DRAW_END;
}


void * lv_draw_layer_alloc_buf(lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    /*If the buffer of the layer is already allocated return it*/
    if(layer->draw_buf != NULL) {
        LV_PROFILER_DRAW_END;
        return layer->draw_buf->data;
    }

    /*If the buffer of the layer is not allocated yet, allocate it now*/
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    uint32_t layer_size_byte = h * lv_draw_buf_width_to_stride(w, layer->color_format);

#if LV_DRAW_LAYER_MAX_MEMORY > 0
    /* Do not allocate the layer if the sum of allocated layer sizes
     * will exceed `LV_DRAW_LAYER_MAX_MEMORY` */
    if((_draw_info.used_memory_for_layers + layer_size_byte) > LV_DRAW_LAYER_MAX_MEMORY) {
        LV_LOG_WARN("LV_DRAW_LAYER_MAX_MEMORY was reached when allocating the layer.");
        return NULL;
    }
#endif

    layer->draw_buf = lv_draw_buf_create(w, h, layer->color_format, 0);

    if(layer->draw_buf == NULL) {
        LV_LOG_WARN("Allocating layer buffer failed. Try later");
        LV_PROFILER_DRAW_END;
        return NULL;
    }

    _draw_info.used_memory_for_layers += layer_size_byte;
    LV_LOG_INFO("Layer memory used: %" LV_PRIu32 " kB", get_layer_size_kb(_draw_info.used_memory_for_layers));

    if(lv_color_format_has_alpha(layer->color_format)) {
        lv_draw_buf_clear(layer->draw_buf, NULL);
    }

    LV_PROFILER_DRAW_END;
    return layer->draw_buf->data;
}

void * lv_draw_layer_go_to_xy(lv_layer_t * layer, int32_t x, int32_t y)
{
    return lv_draw_buf_goto_xy(layer->draw_buf, x, y);
}

lv_draw_task_type_t lv_draw_task_get_type(const lv_draw_task_t * t)
{
    return t->type;
}

void * lv_draw_task_get_draw_dsc(const lv_draw_task_t * t)
{
    return t->draw_dsc;
}

void lv_draw_task_get_area(const lv_draw_task_t * t, lv_area_t * area)
{
    *area = t->area;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Check if there are older draw task overlapping the area of `t_check`
 * @param layer      the draw ctx to search in
 * @param t_check       check this task if it overlaps with the older ones
 * @return              true: `t_check` is not overlapping with older tasks so it's independent
 */
static bool is_independent(lv_layer_t * layer, lv_draw_task_t * t_check)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_task_t * t = layer->draw_task_head;

    /*If t_check is outside of the older tasks then it's independent*/
    while(t && t != t_check) {
        if(t->state != LV_DRAW_TASK_STATE_READY) {
            lv_area_t a;
            if(lv_area_intersect(&a, &t->_real_area, &t_check->_real_area)) {
                LV_PROFILER_DRAW_END;
                return false;
            }
        }
        t = t->next;
    }
    LV_PROFILER_DRAW_END;

    return true;
}

/**
 * Clean-up resources allocated by a finished task
 * @param t         pointer to a draw task
 * @param disp      pointer to a display on which the task was drawn
 */
static void lv_cleanup_task(lv_draw_task_t * t, lv_display_t * disp)
{
    LV_PROFILER_DRAW_BEGIN;
    /*If it was layer drawing free the layer too*/
    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        lv_draw_image_dsc_t * draw_image_dsc = t->draw_dsc;
        lv_layer_t * layer_drawn = (lv_layer_t *)draw_image_dsc->src;

        if(layer_drawn->draw_buf) {
            int32_t h = lv_area_get_height(&layer_drawn->buf_area);
            uint32_t layer_size_byte = h * layer_drawn->draw_buf->header.stride;

            if(_draw_info.used_memory_for_layers >= layer_size_byte) {
                _draw_info.used_memory_for_layers -= layer_size_byte;
            }
            else {
                _draw_info.used_memory_for_layers = 0;
                LV_LOG_WARN("More layers were freed than allocated");
            }
            LV_LOG_INFO("Layer memory used: %" LV_PRIu32 " kB", get_layer_size_kb(_draw_info.used_memory_for_layers));
            lv_draw_buf_destroy(layer_drawn->draw_buf);
            layer_drawn->draw_buf = NULL;
        }

        /*Remove the layer from  the display's*/
        if(disp) {
            lv_layer_t * l2 = disp->layer_head;
            while(l2) {
                if(l2->next == layer_drawn) {
                    l2->next = layer_drawn->next;
                    break;
                }
                l2 = l2->next;
            }

            if(disp->layer_deinit) {
                LV_PROFILER_DRAW_BEGIN_TAG("layer_deinit");
                disp->layer_deinit(disp, layer_drawn);
                LV_PROFILER_DRAW_END_TAG("layer_deinit");
            }
            lv_free(layer_drawn);
        }
    }
    lv_draw_label_dsc_t * draw_label_dsc = lv_draw_task_get_label_dsc(t);
    if(draw_label_dsc && draw_label_dsc->text_local) {
        lv_free((void *)draw_label_dsc->text);
        draw_label_dsc->text = NULL;
    }

    lv_free(t->draw_dsc);
    lv_free(t);
    LV_PROFILER_DRAW_END;
}
