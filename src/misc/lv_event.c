/**
 * @file lv_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_event.h"
#include "lv_mem.h"
#include "lv_assert.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_event_dsc_t {
    lv_event_cb_t cb;
    void * user_data;
    uint32_t filter;
} lv_event_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void remove_dsc(lv_event_list_t * list, uint32_t idx);

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

lv_res_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool prerpocess)
{
    if(list == NULL) return LV_RES_OK;

    uint32_t i = 0;
    for(i = 0; i < list->cnt; i++) {
        if(list->dsc[i].cb == NULL) continue;
        bool is_preprocessed = (list->dsc[i].filter & LV_OBJ_EVENT_PREPROCESS) != 0;
        if(is_preprocessed != prerpocess) continue;
        lv_event_code_t filter = list->dsc[i].filter & ~LV_OBJ_EVENT_PREPROCESS;
        if(filter == LV_OBJ_EVENT_ALL || filter == e->code) {
            e->user_data = list->dsc[i].user_data;
            list->dsc[i].cb(e);
            if(e->stop_processing) return LV_RES_OK;

            /*Stop if the object is deleted*/
            if(e->deleted) return LV_RES_INV;

        }
    }
    return LV_RES_OK;
}

void lv_event_add_callback(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter,
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

bool lv_event_remove_callback(lv_event_list_t * list, lv_event_cb_t cb)
{
    uint32_t i = 0;
    for(i = 0; i < list->cnt; i++) {
        if(cb == NULL || list->dsc[i].cb == cb) {
            remove_dsc(list, i);
        }
    }

    /*No event handler found*/
    return false;
}

bool lv_event_remove_callback_with_user_data(lv_event_list_t * list, lv_event_cb_t cb, const void * user_data)
{
    uint32_t i = 0;
    for(i = 0; i < list->cnt; i++) {
        if((cb == NULL || list->dsc[i].cb == cb) && list->dsc[i].user_data == user_data) {
            remove_dsc(list, i);
            return true;
        }
    }
    /*No event handler found*/
    return false;
}

void * lv_event_get_user_data_of_callback(lv_event_list_t * list, lv_event_cb_t event_cb)
{
    uint32_t i = 0;
    for(i = 0; i < list->cnt; i++) {
        if(event_cb == list->dsc[i].cb) return list->dsc[i].user_data;
    }
    return NULL;
}

void * lv_event_get_target(lv_event_t * e)
{
    return e->target;
}

void * lv_event_get_current_target(lv_event_t * e)
{
    return e->current_target;
}

lv_event_code_t lv_event_get_code(lv_event_t * e)
{
    return e->code & ~LV_OBJ_EVENT_PREPROCESS;
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
    static uint32_t last_id = _LV_OBJ_EVENT_LAST;
    last_id ++;
    return last_id;
}

void _lv_event_mark_deleted(void * target)
{
    lv_event_t * e = event_head;

    while(e) {
        if(e->current_target == target || e->target == target) e->deleted = 1;
        e = e->prev;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void remove_dsc(lv_event_list_t * list, uint32_t idx)
{
    /*Shift the remaining event handlers forward*/
    uint32_t i;
    for(i = idx; i < list->cnt - 1; i++) {
        list->dsc[i] = list->dsc[i + 1];
    }
    list->cnt--;
    list->dsc = lv_realloc(list->dsc, list->cnt * sizeof(lv_event_dsc_t));
    LV_ASSERT_MALLOC(list->dsc);
}
