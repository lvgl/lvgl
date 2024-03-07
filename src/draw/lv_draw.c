/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "sw/lv_draw_sw.h"
#include "../display/lv_display_private.h"
#include "../core/lv_global.h"
#include "../core/lv_refr.h"
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

static inline uint32_t get_layer_size_kb(uint32_t size_byte)
{
    return size_byte < 1024 ? 1 : size_byte >> 10;
}
/**********************
 *  STATIC VARIABLES
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

    new_unit->next = _draw_info.unit_head;
    _draw_info.unit_head = new_unit;

    return new_unit;
}

lv_draw_task_t * lv_draw_add_task(lv_layer_t * layer, const lv_area_t * coords)
{
    LV_PROFILER_BEGIN;
    lv_draw_task_t * new_task = lv_malloc_zeroed(sizeof(lv_draw_task_t));

    new_task->area = *coords;
    new_task->_real_area = *coords;
    new_task->clip_area = layer->_clip_area;
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

    LV_PROFILER_END;
    return new_task;
}

void lv_draw_finalize_task_creation(lv_layer_t * layer, lv_draw_task_t * t)
{
    LV_PROFILER_BEGIN;
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
            if(u->evaluate_cb) u->evaluate_cb(u, t);
            u = u->next;
        }

        lv_draw_dispatch();
    }
    else {
        /*Let the draw units set their preference score*/
        t->preference_score = 100;
        t->preferred_draw_unit_id = 0;
        lv_draw_unit_t * u = info->unit_head;
        while(u) {
            if(u->evaluate_cb) u->evaluate_cb(u, t);
            u = u->next;
        }
    }
    LV_PROFILER_END;
}

void lv_draw_dispatch(void)
{
    LV_PROFILER_BEGIN;
    bool render_running = false;
    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        lv_layer_t * layer = disp->layer_head;
        while(layer) {
            if(lv_draw_dispatch_layer(disp, layer))
                render_running = true;
            layer = layer->next;
        }
        if(!render_running) {
            lv_draw_dispatch_request();
        }
        disp = lv_display_get_next(disp);
    }
    LV_PROFILER_END;
}

bool lv_draw_dispatch_layer(lv_display_t * disp, lv_layer_t * layer)
{
    LV_PROFILER_BEGIN;
    /*Remove the finished tasks first*/
    lv_draw_task_t * t_prev = NULL;
    lv_draw_task_t * t = layer->draw_task_head;
    while(t) {
        lv_draw_task_t * t_next = t->next;
        if(t->state == LV_DRAW_TASK_STATE_READY) {
            if(t_prev) t_prev->next = t->next;      /*Remove by it by assigning the next task to the previous*/
            else layer->draw_task_head = t_next;    /*If it was the head, set the next as head*/

            /*If it was layer drawing free the layer too*/
            if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
                lv_draw_image_dsc_t * draw_image_dsc = t->draw_dsc;
                lv_layer_t * layer_drawn = (lv_layer_t *)draw_image_dsc->src;

                if(layer_drawn->draw_buf) {
                    int32_t h = lv_area_get_height(&layer_drawn->buf_area);
                    int32_t w = lv_area_get_width(&layer_drawn->buf_area);
                    uint32_t layer_size_byte = h * lv_draw_buf_width_to_stride(w, layer_drawn->color_format);

                    _draw_info.used_memory_for_layers_kb -= get_layer_size_kb(layer_size_byte);
                    LV_LOG_INFO("Layer memory used: %" LV_PRIu32 " kB\n", _draw_info.used_memory_for_layers_kb);
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

                    if(disp->layer_deinit) disp->layer_deinit(disp, layer_drawn);
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
        }
        else {
            t_prev = t;
        }
        t = t_next;
    }

    bool render_running = false;

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
            int32_t taken_cnt = u->dispatch_cb(u, layer);
            if(taken_cnt >= 0) render_running = true;
            u = u->next;
        }
    }

    LV_PROFILER_END;
    return render_running;
}

void lv_draw_dispatch_wait_for_request(void)
{
#if LV_USE_OS
    lv_thread_sync_wait(&_draw_info.sync);
#else
    while(!_draw_info.dispatch_req);
    _draw_info.dispatch_req = 0;
#endif
}

void lv_draw_dispatch_request(void)
{
#if LV_USE_OS
    lv_thread_sync_signal(&_draw_info.sync);
#else
    _draw_info.dispatch_req = 1;
#endif
}

lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev, uint8_t draw_unit_id)
{
    LV_PROFILER_BEGIN;
    /*If the first task is screen sized, there cannot be independent areas*/
    if(layer->draw_task_head) {
        int32_t hor_res = lv_display_get_horizontal_resolution(_lv_refr_get_disp_refreshing());
        int32_t ver_res = lv_display_get_vertical_resolution(_lv_refr_get_disp_refreshing());
        lv_draw_task_t * t = layer->draw_task_head;
        if(t->state != LV_DRAW_TASK_STATE_QUEUED &&
           t->area.x1 <= 0 && t->area.x2 >= hor_res - 1 &&
           t->area.y1 <= 0 && t->area.y2 >= ver_res - 1) {
            LV_PROFILER_END;
            return NULL;
        }
    }

    lv_draw_task_t * t = t_prev ? t_prev->next : layer->draw_task_head;
    while(t) {
        /*Find a queued and independent task*/
        if(t->state == LV_DRAW_TASK_STATE_QUEUED &&
           (t->preferred_draw_unit_id == LV_DRAW_UNIT_ID_ANY || t->preferred_draw_unit_id == draw_unit_id) &&
           is_independent(layer, t)) {
            LV_PROFILER_END;
            return t;
        }
        t = t->next;
    }

    LV_PROFILER_END;
    return NULL;
}

uint32_t lv_draw_get_dependent_count(lv_draw_task_t * t_check)
{
    if(t_check == NULL) return 0;
    if(t_check->next == NULL) return 0;

    LV_PROFILER_BEGIN;
    uint32_t cnt = 0;

    lv_draw_task_t * t = t_check->next;
    while(t) {
        if((t->state == LV_DRAW_TASK_STATE_QUEUED || t->state == LV_DRAW_TASK_STATE_WAITING) &&
           _lv_area_is_on(&t_check->area, &t->area)) {
            cnt++;
        }

        t = t->next;
    }
    LV_PROFILER_END;
    return cnt;
}

lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area)
{
    lv_display_t * disp = _lv_refr_get_disp_refreshing();
    lv_layer_t * new_layer = lv_malloc_zeroed(sizeof(lv_layer_t));
    LV_ASSERT_MALLOC(new_layer);
    if(new_layer == NULL) return NULL;

    new_layer->parent = parent_layer;
    new_layer->_clip_area = *area;
    new_layer->buf_area = *area;
    new_layer->color_format = color_format;

    if(disp->layer_head) {
        lv_layer_t * tail = disp->layer_head;
        while(tail->next) tail = tail->next;
        tail->next = new_layer;
    }
    else {
        disp->layer_head = new_layer;
    }

    return new_layer;
}

void * lv_draw_layer_alloc_buf(lv_layer_t * layer)
{
    /*If the buffer of the layer is already allocated return it*/
    if(layer->draw_buf != NULL) {
        return layer->draw_buf->data;
    }

    /*If the buffer of the layer is not allocated yet, allocate it now*/
    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    uint32_t layer_size_byte = h * lv_draw_buf_width_to_stride(w, layer->color_format);

    layer->draw_buf = lv_draw_buf_create(w, h, layer->color_format, 0);

    if(layer->draw_buf == NULL) {
        LV_LOG_WARN("Allocating layer buffer failed. Try later");
        return NULL;
    }

    _draw_info.used_memory_for_layers_kb += get_layer_size_kb(layer_size_byte);
    LV_LOG_INFO("Layer memory used: %" LV_PRIu32 " kB\n", _draw_info.used_memory_for_layers_kb);

    if(lv_color_format_has_alpha(layer->color_format)) {
        lv_draw_buf_clear(layer->draw_buf, NULL);
    }

    return layer->draw_buf->data;
}

void * lv_draw_layer_go_to_xy(lv_layer_t * layer, int32_t x, int32_t y)
{
    return lv_draw_buf_goto_xy(layer->draw_buf, x, y);
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
    LV_PROFILER_BEGIN;
    lv_draw_task_t * t = layer->draw_task_head;

    /*If t_check is outside of the older tasks then it's independent*/
    while(t && t != t_check) {
        if(t->state != LV_DRAW_TASK_STATE_READY) {
            lv_area_t a;
            if(_lv_area_intersect(&a, &t->_real_area, &t_check->_real_area)) {
                LV_PROFILER_END;
                return false;
            }
        }
        t = t->next;
    }
    LV_PROFILER_END;

    return true;
}
