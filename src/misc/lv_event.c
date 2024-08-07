/**
 * @file lv_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_event_private.h"
#include "../core/lv_global.h"
#include "../stdlib/lv_mem.h"
#include "lv_assert.h"
#include "lv_types.h"

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

/* Traverse the list to delete the objects marked for deletion */
static void cleanup_event_list(lv_event_list_t * list);

inline static void event_mark_deleting(lv_event_list_t * list, lv_event_dsc_t * dsc);
inline static bool event_is_marked_deleting(lv_event_dsc_t * dsc);
inline static lv_array_t * event_array(lv_event_list_t * list);

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

void lv_event_push(lv_event_t * e)
{
    /*Build a simple linked list from the objects used in the events
     *It's important to know if this object was deleted by a nested event
     *called from this `event_cb`.*/
    e->prev = event_head;
    event_head = e;

}

void lv_event_pop(lv_event_t * e)
{
    event_head = e->prev;
}

lv_result_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool preprocess)
{
    if(list == NULL) return LV_RESULT_OK;

    /* Dealing with the problem of nested event deletion event */
    bool is_traversing = list->is_traversing;
    list->is_traversing = true;

    uint32_t i = 0;
    uint32_t size = lv_array_size(event_array(list));
    for(i = 0; i < size; i++) {
        lv_event_dsc_t * dsc = *(lv_event_dsc_t **)lv_array_at(event_array(list), i);
        if(dsc->cb == NULL) continue;
        if(event_is_marked_deleting(dsc)) continue;
        bool is_preprocessed = (dsc->filter & LV_EVENT_PREPROCESS) != 0;
        if(is_preprocessed != preprocess) continue;
        lv_event_code_t filter = dsc->filter & ~LV_EVENT_PREPROCESS;
        if(filter == LV_EVENT_ALL || filter == e->code) {
            e->user_data = dsc->user_data;
            dsc->cb(e);
            if(e->stop_processing) return LV_RESULT_OK;

            /*Stop if the object is deleted*/
            if(e->deleted) return LV_RESULT_INVALID;
        }
    }

    if(is_traversing) return LV_RESULT_OK;

    cleanup_event_list(list);

    list->is_traversing = false;

    return LV_RESULT_OK;
}

lv_event_dsc_t * lv_event_add(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter,
                              void * user_data)
{
    lv_event_dsc_t * dsc = lv_malloc(sizeof(lv_event_dsc_t));
    LV_ASSERT_NULL(dsc);

    dsc->cb = cb;
    dsc->filter = filter;
    dsc->user_data = user_data;

    if(lv_array_size(event_array(list)) == 0) {
        /*event list hasn't been initialized.*/
        lv_array_init(event_array(list), 1, sizeof(lv_event_dsc_t *));
    }

    lv_array_push_back(event_array(list), &dsc);
    return dsc;
}

bool lv_event_remove_dsc(lv_event_list_t * list, lv_event_dsc_t * dsc)
{
    LV_ASSERT_NULL(list);
    LV_ASSERT_NULL(dsc);

    int size = lv_array_size(event_array(list));
    lv_event_dsc_t ** events = lv_array_front(event_array(list));
    for(int i = 0; i < size; i++) {
        if(events[i] == dsc) {
            event_mark_deleting(list, events[i]);
            cleanup_event_list(list);
            return true;
        }
    }

    return false;
}

uint32_t lv_event_get_count(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    return lv_array_size(event_array(list));
}

lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index)
{
    LV_ASSERT_NULL(list);
    lv_event_dsc_t ** dsc;
    dsc = lv_array_at(event_array(list), index);
    return dsc ? *dsc : NULL;
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
    lv_event_dsc_t * dsc = lv_event_get_dsc(list, index);
    if(dsc == NULL) return false;
    event_mark_deleting(list, dsc);
    cleanup_event_list(list);
    return true;
}

void lv_event_remove_all(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    int size = lv_array_size(event_array(list));
    lv_event_dsc_t ** dsc = lv_array_front(event_array(list));
    for(int i = 0; i < size; i++)
        event_mark_deleting(list, dsc[i]);
    cleanup_event_list(list);
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

void lv_event_mark_deleted(void * target)
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

static void cleanup_event_list(lv_event_list_t * list)
{
    if(list->is_traversing) return;
    if(list->has_marked_deleting == false) return;

    uint32_t size = lv_array_size(event_array(list));
    uint32_t kept_count = 0;
    for(uint32_t i = 0; i < size; i++) {
        lv_event_dsc_t ** dsc_i = lv_array_at(event_array(list), i);
        lv_event_dsc_t ** dsc_kept = lv_array_at(event_array(list), kept_count);
        if(event_is_marked_deleting(*dsc_i) == false) {
            *dsc_kept = *dsc_i;
            kept_count++;
        }
        else lv_free(*dsc_i);
    }

    if(kept_count == 0) lv_array_deinit(event_array(list));
    else lv_array_resize(event_array(list), kept_count);

    list->has_marked_deleting = false;
}

inline static void event_mark_deleting(lv_event_list_t * list, lv_event_dsc_t * dsc)
{
    list->has_marked_deleting = true;
    dsc->filter |= LV_EVENT_MARKED_DELETING;
}
inline static bool event_is_marked_deleting(lv_event_dsc_t * dsc)
{
    return (dsc->filter & LV_EVENT_MARKED_DELETING) != 0;
}
inline static lv_array_t * event_array(lv_event_list_t * list)
{
    return &(list)->array;
}
