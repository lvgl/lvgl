/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "sw/lv_draw_sw.h"
#include "../disp/lv_disp_private.h"
#include "../core/lv_refr.h"
#include "../stdlib/lv_string.h"
#include "../misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool is_independent(lv_layer_t * layer, lv_draw_task_t * t_check);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OS
    static lv_thread_sync_t sync;
#else
    static int dispatch_req = 0;
#endif

static uint32_t used_memory_for_layers_kb = 0;

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
    lv_thread_sync_init(&sync);
#endif
}

void * lv_draw_create_unit(size_t size)
{
    lv_draw_unit_t * new_unit = lv_malloc(size);
    lv_memzero(new_unit, size);

    new_unit->next = LV_GC_ROOT(_lv_draw_unit_head);
    LV_GC_ROOT(_lv_draw_unit_head) = new_unit;

    return new_unit;
}

lv_draw_task_t * lv_draw_add_task(lv_layer_t * layer, const lv_area_t * coords)
{
    LV_PROFILER_BEGIN;
    lv_draw_task_t * new_task = lv_malloc(sizeof(lv_draw_task_t));
    lv_memzero(new_task, sizeof(*new_task));

    new_task->area = *coords;
    new_task->clip_area = layer->clip_area;
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
    lv_draw_dsc_base_t * base_dsc = t->draw_dsc;
    base_dsc->layer = layer;

    /*Send LV_EVENT_DRAW_TASK_ADDED and dispatch only on the "main" draw_task
     *and not on the draw tasks added in the event.
     *Sending LV_EVENT_DRAW_TASK_ADDED events might cause recursive event sends
     *Dispatching might remove the "main" draw task while it's still being used in the event*/
    static bool running = false;
    if(running == false) {
        running = true;
        if(base_dsc->obj && lv_obj_has_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS)) {
            lv_obj_send_event(base_dsc->obj, LV_EVENT_DRAW_TASK_ADDED, t);
        }
        lv_draw_dispatch();
        running = false;
    }
}

void lv_draw_dispatch(void)
{
    LV_PROFILER_BEGIN;
    bool one_taken = false;
    lv_disp_t * disp = lv_disp_get_next(NULL);
    while(disp) {
        lv_layer_t * layer = disp->layer_head;
        while(layer) {
            bool ret = lv_draw_dispatch_layer(disp, layer);
            if(ret) one_taken = true;
            layer = layer->next;
        }

        if(!one_taken) {
            lv_draw_dispatch_request();
        }
        disp = lv_disp_get_next(disp);
    }
    LV_PROFILER_END;
}

bool lv_draw_dispatch_layer(struct _lv_disp_t * disp, lv_layer_t * layer)
{
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
                lv_draw_img_dsc_t * draw_img_dsc = t->draw_dsc;
                lv_layer_t * layer_drawn = (lv_layer_t *)draw_img_dsc->src;

                if(layer_drawn->buf) {
                    uint32_t layer_px_size = lv_color_format_get_size(layer_drawn->color_format);
                    uint32_t layer_size_byte = lv_area_get_size(&layer_drawn->buf_area) * layer_px_size;
                    used_memory_for_layers_kb -= layer_size_byte < 1024 ? 1 : layer_size_byte >> 10;
                    LV_LOG_INFO("Layer memory used: %d kB\n", used_memory_for_layers_kb);
                    lv_free(layer_drawn->buf);
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

                    disp->layer_deinit(disp, layer_drawn);
                    lv_free(layer_drawn);
                }
            }
            if(t->type == LV_DRAW_TASK_TYPE_LABEL) {
                lv_draw_label_dsc_t * draw_label_dsc = t->draw_dsc;
                if(draw_label_dsc->text_local) {
                    lv_free((void *)draw_label_dsc->text);
                    draw_label_dsc->text = NULL;
                }
            }

            lv_free(t->draw_dsc);
            lv_free(t);
        }
        else {
            t_prev = t;
        }
        t = t_next;
    }

    bool one_taken = false;

    /*This layer is ready, enable blending its buffer*/
    if(layer->parent && layer->all_tasks_added && layer->draw_task_head == NULL) {
        /*Find a draw task with TYPE_LAYER in the layer where the src is this layer*/
        lv_draw_task_t * t_src = layer->parent->draw_task_head;
        while(t_src) {
            if(t_src->type == LV_DRAW_TASK_TYPE_LAYER && t_src->state == LV_DRAW_TASK_STATE_WAITING) {
                lv_draw_img_dsc_t * draw_dsc = t_src->draw_dsc;
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
        bool layer_ok = true;
        if(layer->buf == NULL) {
            uint32_t px_size = lv_color_format_get_size(layer->color_format);
            uint32_t layer_size_byte = lv_area_get_size(&layer->buf_area) * px_size;
            uint32_t kb = layer_size_byte < 1024 ? 1 : layer_size_byte >> 10;
            if(used_memory_for_layers_kb + kb > LV_LAYER_MAX_MEMORY_USAGE) {
                layer_ok = false;
            }
        }

        if(layer_ok) {
            /*Find a draw unit which is not busy and can take at least one task*/
            /*Let all draw units to pick draw tasks*/
            lv_draw_unit_t * u = LV_GC_ROOT(_lv_draw_unit_head);
            while(u) {
                int32_t taken_cnt = u->dispatch(u, layer);
                if(taken_cnt < 0) {
                    break;
                }
                if(taken_cnt > 0) one_taken = true;
                u = u->next;
            }
        }
    }

    return one_taken;

}

void lv_draw_dispatch_wait_for_request(void)
{
#if LV_USE_OS
    lv_thread_sync_wait(&sync);
#else
    while(!dispatch_req);
    dispatch_req = 0;
#endif
}

void lv_draw_dispatch_request(void)
{
#if LV_USE_OS
    lv_thread_sync_signal(&sync);
#else
    dispatch_req = 1;
#endif
}

lv_draw_task_t * lv_draw_get_next_available_task(lv_layer_t * layer, lv_draw_task_t * t_prev)
{
    LV_PROFILER_BEGIN;
    /*If the first task is screen sized, there cannot be independent areas*/
    if(layer->draw_task_head) {
        lv_coord_t hor_res = lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
        lv_coord_t ver_res = lv_disp_get_ver_res(_lv_refr_get_disp_refreshing());
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
        if(t->state == LV_DRAW_TASK_STATE_QUEUED && is_independent(layer, t)) {
            LV_PROFILER_END;
            return t;
        }
        t = t->next;
    }

    LV_PROFILER_END;
    return NULL;
}

lv_layer_t * lv_draw_layer_create(lv_layer_t * parent_layer, lv_color_format_t color_format, const lv_area_t * area)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_layer_t * new_layer = disp->layer_init(disp);
    LV_ASSERT_MALLOC(new_layer);
    if(new_layer == NULL) return NULL;

    new_layer->buf = NULL;
    new_layer->parent = parent_layer;
    new_layer->color_format = color_format;
    new_layer->buf_area = *area;
    new_layer->clip_area = *area;

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

void lv_draw_add_used_layer_size(uint32_t kb)
{
    used_memory_for_layers_kb += kb;
    LV_LOG_INFO("Layer memory used: %d kB\n", used_memory_for_layers_kb);
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
            if(_lv_area_intersect(&a, &t->area, &t_check->area)) {
                LV_PROFILER_END;
                return false;
            }
        }
        t = t->next;
    }
    LV_PROFILER_END;

    return true;
}

