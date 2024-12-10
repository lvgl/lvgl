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

    uint32_t i = 0;
    lv_event_dsc_t ** dsc = lv_array_front(list);
    uint32_t size = lv_array_size(list);
    for(i = 0; i < size; i++) {
        if(dsc[i]->cb == NULL) continue;
        bool is_preprocessed = (dsc[i]->filter & LV_EVENT_PREPROCESS) != 0;
        if(is_preprocessed != preprocess) continue;
        lv_event_code_t filter = dsc[i]->filter & ~LV_EVENT_PREPROCESS;
        if(filter == LV_EVENT_ALL || filter == e->code) {
            e->user_data = dsc[i]->user_data;
            dsc[i]->cb(e);
            if(e->stop_processing) return LV_RESULT_OK;

            /*Stop if the object is deleted*/
            if(e->deleted) return LV_RESULT_INVALID;

        }
    }
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

    if(lv_array_size(list) == 0) {
        /*event list hasn't been initialized.*/
        lv_array_init(list, 1, sizeof(lv_event_dsc_t *));
    }

    lv_array_push_back(list, &dsc);
    return dsc;
}

bool lv_event_remove_dsc(lv_event_list_t * list, lv_event_dsc_t * dsc)
{
    LV_ASSERT_NULL(list);
    LV_ASSERT_NULL(dsc);

    int size = lv_array_size(list);
    lv_event_dsc_t ** events = lv_array_front(list);
    for(int i = 0; i < size; i++) {
        if(events[i] == dsc) {
            lv_free(dsc);
            lv_array_remove(list, i);
            return true;
        }
    }

    return false;
}

uint32_t lv_event_get_count(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    return lv_array_size(list);
}

lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index)
{
    LV_ASSERT_NULL(list);
    lv_event_dsc_t ** dsc;
    dsc = lv_array_at(list, index);
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
    lv_free(dsc);
    return lv_array_remove(list, index);
}

void lv_event_remove_all(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    int size = lv_array_size(list);
    lv_event_dsc_t ** dsc = lv_array_front(list);
    for(int i = 0; i < size; i++) {
        lv_free(dsc[i]);
    }
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

const char * lv_event_code_get_name(lv_event_code_t code)
{
    switch(code & ~LV_EVENT_PREPROCESS) {
        case LV_EVENT_ALL:
            return "ALL";

        /* Input device events*/
        case LV_EVENT_PRESSED:
            return "PRESSED";
        case LV_EVENT_PRESSING:
            return "PRESSING";
        case LV_EVENT_PRESS_LOST:
            return "PRESS_LOST";
        case LV_EVENT_SHORT_CLICKED:
            return "SHORT_CLICKED";
        case LV_EVENT_LONG_PRESSED:
            return "LONG_PRESSED";
        case LV_EVENT_LONG_PRESSED_REPEAT:
            return "LONG_PRESSED_REPEAT";
        case LV_EVENT_CLICKED:
            return "CLICKED";
        case LV_EVENT_RELEASED:
            return "RELEASED";
        case LV_EVENT_SCROLL_BEGIN:
            return "SCROLL_BEGIN";
        case LV_EVENT_SCROLL_THROW_BEGIN:
            return "SCROLL_THROW_BEGIN";
        case LV_EVENT_SCROLL_END:
            return "SCROLL_END";
        case LV_EVENT_SCROLL:
            return "SCROLL";
        case LV_EVENT_GESTURE:
            return "GESTURE";
        case LV_EVENT_KEY:
            return "KEY";
        case LV_EVENT_ROTARY:
            return "ROTARY";
        case LV_EVENT_FOCUSED:
            return "FOCUSED";
        case LV_EVENT_DEFOCUSED:
            return "DEFOCUSED";
        case LV_EVENT_LEAVE:
            return "LEAVE";
        case LV_EVENT_HIT_TEST:
            return "HIT_TEST";
        case LV_EVENT_INDEV_RESET:
            return "INDEV_RESET";
        case LV_EVENT_HOVER_OVER:
            return "HOVER_OVER";
        case LV_EVENT_HOVER_LEAVE:
            return "HOVER_LEAVE";

        /* Drawing events*/
        case LV_EVENT_COVER_CHECK:
            return "COVER_CHECK";
        case LV_EVENT_REFR_EXT_DRAW_SIZE:
            return "REFR_EXT_DRAW_SIZE";
        case LV_EVENT_DRAW_MAIN_BEGIN:
            return "DRAW_MAIN_BEGIN";
        case LV_EVENT_DRAW_MAIN:
            return "DRAW_MAIN";
        case LV_EVENT_DRAW_MAIN_END:
            return "DRAW_MAIN_END";
        case LV_EVENT_DRAW_POST_BEGIN:
            return "DRAW_POST_BEGIN";
        case LV_EVENT_DRAW_POST:
            return "DRAW_POST";
        case LV_EVENT_DRAW_POST_END:
            return "DRAW_POST_END";
        case LV_EVENT_DRAW_TASK_ADDED:
            return "DRAW_TASK_ADDED";

        /* Special events*/
        case LV_EVENT_VALUE_CHANGED:
            return "VALUE_CHANGED";
        case LV_EVENT_INSERT:
            return "INSERT";
        case LV_EVENT_REFRESH:
            return "REFRESH";
        case LV_EVENT_READY:
            return "READY";
        case LV_EVENT_CANCEL:
            return "CANCEL";

        /* Other events*/
        case LV_EVENT_CREATE:
            return "CREATE";
        case LV_EVENT_DELETE:
            return "DELETE";
        case LV_EVENT_CHILD_CHANGED:
            return "CHILD_CHANGED";
        case LV_EVENT_CHILD_CREATED:
            return "CHILD_CREATED";
        case LV_EVENT_CHILD_DELETED:
            return "CHILD_DELETED";
        case LV_EVENT_SCREEN_UNLOAD_START:
            return "SCREEN_UNLOAD_START";
        case LV_EVENT_SCREEN_LOAD_START:
            return "SCREEN_LOAD_START";
        case LV_EVENT_SCREEN_LOADED:
            return "SCREEN_LOADED";
        case LV_EVENT_SCREEN_UNLOADED:
            return "SCREEN_UNLOADED";
        case LV_EVENT_SIZE_CHANGED:
            return "SIZE_CHANGED";
        case LV_EVENT_STYLE_CHANGED:
            return "STYLE_CHANGED";
        case LV_EVENT_LAYOUT_CHANGED:
            return "LAYOUT_CHANGED";
        case LV_EVENT_GET_SELF_SIZE:
            return "GET_SELF_SIZE";

        /* Events of optional LVGL components*/
        case LV_EVENT_INVALIDATE_AREA:
            return "INVALIDATE_AREA";
        case LV_EVENT_RESOLUTION_CHANGED:
            return "RESOLUTION_CHANGED";
        case LV_EVENT_COLOR_FORMAT_CHANGED:
            return "COLOR_FORMAT_CHANGED";
        case LV_EVENT_REFR_REQUEST:
            return "REFR_REQUEST";
        case LV_EVENT_REFR_START:
            return "REFR_START";
        case LV_EVENT_REFR_READY:
            return "REFR_READY";
        case LV_EVENT_RENDER_START:
            return "RENDER_START";
        case LV_EVENT_RENDER_READY:
            return "RENDER_READY";
        case LV_EVENT_FLUSH_START:
            return "FLUSH_START";
        case LV_EVENT_FLUSH_FINISH:
            return "FLUSH_FINISH";
        case LV_EVENT_FLUSH_WAIT_START:
            return "FLUSH_WAIT_START";
        case LV_EVENT_FLUSH_WAIT_FINISH:
            return "FLUSH_WAIT_FINISH";

        case LV_EVENT_VSYNC:
            return "VSYNC";

        default:
            return "unknown";
    }
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
