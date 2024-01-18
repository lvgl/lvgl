/**
 * @file lv_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_event.h"
#include "../core/lv_global.h"
#include "../stdlib/lv_mem.h"
#include "lv_assert.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

#define event_head LV_GLOBAL_DEFAULT()->event_header
#define event_last_id LV_GLOBAL_DEFAULT()->event_last_register_id

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#if LV_USE_LOG && LV_LOG_TRACE_EVENT
    #define LV_TRACE_EVENT(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_EVENT(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_event_push(lv_event_t * e)
{
    /*Build a simple linked list from the objects used in the events
     *It's important to know if this object was deleted by a nested event
     *called from this `event_cb`.*/
    e->prev = event_head;
    event_head = e;

}

void _lv_event_pop(lv_event_t * e)
{
    event_head = e->prev;
}

lv_result_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool preprocess)
{
    if(list == NULL) return LV_RESULT_OK;

    uint32_t i = 0;
    lv_event_dsc_t * dsc = lv_array_at(list, 0);
    for(i = 0; i < lv_array_size(list); i++) {
        if(dsc[i].cb == NULL) continue;
        bool is_preprocessed = (dsc[i].filter & LV_EVENT_PREPROCESS) != 0;
        if(is_preprocessed != preprocess) continue;
        lv_event_code_t filter = dsc[i].filter & ~LV_EVENT_PREPROCESS;
        if(filter == LV_EVENT_ALL || filter == e->code) {
            e->user_data = dsc[i].user_data;
            dsc[i].cb(e);
            if(e->stop_processing) return LV_RESULT_OK;

            /*Stop if the object is deleted*/
            if(e->deleted) return LV_RESULT_INVALID;

        }
    }
    return LV_RESULT_OK;
}

void lv_event_add(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter,
                  void * user_data)
{
    lv_event_dsc_t dsc = { 0 };
    dsc.cb = cb;
    dsc.filter = filter;
    dsc.user_data = user_data;

    if(lv_array_size(list) == 0) {
        /*event list hasn't been initialized.*/
        lv_array_init(list, 1, sizeof(lv_event_dsc_t));
    }

    lv_array_push_back(list, &dsc);
}

uint32_t lv_event_get_count(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    return lv_array_size(list);
}

lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index)
{
    LV_ASSERT_NULL(list);
    return lv_array_at(list, index);
}

lv_event_cb_t lv_event_dsc_get_cb(lv_event_dsc_t * dsc)
{
    LV_ASSERT_NULL(dsc);
    return dsc->cb;
}

void * lv_event_dsc_get_user_data(lv_event_dsc_t * dsc)
{
    LV_ASSERT_NULL(dsc);
    return dsc->user_data;

}

bool lv_event_remove(lv_event_list_t * list, uint32_t index)
{
    LV_ASSERT_NULL(list);
    return lv_array_remove(list, index);
}

void lv_event_remove_all(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    lv_array_deinit(list);
}

void * lv_event_get_current_target(lv_event_t * e)
{
    return e->current_target;
}

void * lv_event_get_target(lv_event_t * e)
{
    return e->original_target;
}

lv_event_code_t lv_event_get_code(lv_event_t * e)
{
    return e->code & ~LV_EVENT_PREPROCESS;
}

void * lv_event_get_param(lv_event_t * e)
{
    return e->param;
}

void * lv_event_get_user_data(lv_event_t * e)
{
    return e->user_data;
}

void lv_event_stop_bubbling(lv_event_t * e)
{
    e->stop_bubbling = 1;
}

void lv_event_stop_processing(lv_event_t * e)
{
    e->stop_processing = 1;
}

uint32_t lv_event_register_id(void)
{
    event_last_id ++;
    return event_last_id;
}

void _lv_event_mark_deleted(void * target)
{
    lv_event_t * e = event_head;

    while(e) {
        if(e->original_target == target || e->current_target == target) e->deleted = 1;
        e = e->prev;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
