/**
 * @file lv_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_event.h"
#include "../stdlib/lv_mem.h"
#include "lv_assert.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_event_t * event_head;

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_EVENT
    #define EVENT_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define EVENT_TRACE(...)
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

lv_res_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool preprocess)
{
    if(list == NULL) return LV_RES_OK;

    uint32_t i = 0;
    for(i = 0; i < list->cnt; i++) {
        if(list->dsc[i].cb == NULL) continue;
        bool is_preprocessed = (list->dsc[i].filter & LV_EVENT_PREPROCESS) != 0;
        if(is_preprocessed != preprocess) continue;
        lv_event_code_t filter = list->dsc[i].filter & ~LV_EVENT_PREPROCESS;
        if(filter == LV_EVENT_ALL || filter == e->code) {
            e->user_data = list->dsc[i].user_data;
            list->dsc[i].cb(e);
            if(e->stop_processing) return LV_RES_OK;

            /*Stop if the object is deleted*/
            if(e->deleted) return LV_RES_INV;

        }
    }
    return LV_RES_OK;
}

void lv_event_add(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter,
                  void * user_data)
{
    list->cnt++;
    list->dsc = lv_realloc(list->dsc, list->cnt * sizeof(lv_event_dsc_t));
    LV_ASSERT_MALLOC(list->dsc);
    if(list->dsc == NULL) return;

    list->dsc[list->cnt - 1].cb = cb;
    list->dsc[list->cnt - 1].filter = filter;
    list->dsc[list->cnt - 1].user_data = user_data;
}


uint32_t lv_event_get_count(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    return list->cnt;
}


lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index)
{
    LV_ASSERT_NULL(list);
    if(index >= list->cnt) return NULL;
    else return &list->dsc[index];
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
    if(index >= list->cnt) return false;

    /*Shift the remaining event handlers forward*/
    uint32_t i;
    for(i = index; i < list->cnt - 1; i++) {
        list->dsc[i] = list->dsc[i + 1];
    }
    list->cnt--;
    list->dsc = lv_realloc(list->dsc, list->cnt * sizeof(lv_event_dsc_t));
    LV_ASSERT_MALLOC(list->dsc);
    return true;
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
    static uint32_t last_id = _LV_EVENT_LAST;
    last_id ++;
    return last_id;
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

