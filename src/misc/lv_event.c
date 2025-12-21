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
static void cleanup_event_list_core(lv_array_t * array);

static void event_mark_deleting(lv_event_list_t * list, lv_event_dsc_t * dsc);
static bool event_is_marked_deleting(lv_event_dsc_t * dsc);
static uint32_t event_array_size(lv_event_list_t * list);
static lv_event_dsc_t ** event_array_at(lv_event_list_t * list, uint32_t index);

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

#if LV_USE_EXT_DATA
void lv_event_desc_set_external_data(lv_event_dsc_t * dsc, void * data, void (* free_cb)(void * data))
{
    if(!dsc) {
        LV_LOG_WARN("Can't attach external user data and destructor callback to a NULL event descriptor");
        return;
    }

    dsc->ext_data.data = data;
    dsc->ext_data.free_cb = free_cb;
}
#endif

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

lv_result_t lv_event_push_and_send(lv_event_list_t * event_list, lv_event_code_t code, void * original_target,
                                   void * param)
{
    LV_ASSERT_NULL(event_list);
    lv_event_t e;
    lv_memzero(&e, sizeof(e));
    e.code = code;
    e.current_target = original_target;
    e.original_target = original_target;
    e.param = param;

    lv_event_push(&e);
    lv_result_t res = lv_event_send(event_list, &e, true);
    if(res != LV_RESULT_OK) goto ret;

    res = lv_event_send(event_list, &e, false);
    if(res != LV_RESULT_OK) goto ret;

ret:
    lv_event_pop(&e);
    return res;
}

lv_result_t lv_event_send(lv_event_list_t * list, lv_event_t * e, bool preprocess)
{
    if(list == NULL) return LV_RESULT_OK;
    if(e->deleted) return LV_RESULT_INVALID;

    /* When obj is deleted in its own event, it will cause the `list->array` header to be released,
     * but the content still exists, which leads to memory leakage.
     * Therefore, back up the header in advance,
     * which can strive to release the memory and prevent used-after-free. */
    lv_array_t back_array_head = list->array;

    /* Dealing with the problem of nested event deletion event */
    const bool is_traversing = list->is_traversing;
    list->is_traversing = true;

    lv_result_t res = LV_RESULT_OK;
    const uint32_t size = event_array_size(list);
    for(uint32_t i = 0; i < size && !e->deleted; i++) {
        lv_event_dsc_t * dsc = *event_array_at(list, i);
        if(dsc->cb == NULL) continue;
        if(event_is_marked_deleting(dsc)) continue;
        const bool is_preprocessed = (dsc->filter & LV_EVENT_PREPROCESS) != 0;
        if(is_preprocessed != preprocess) continue;
        lv_event_code_t filter = dsc->filter & ~LV_EVENT_PREPROCESS;
        if(filter == LV_EVENT_ALL || filter == e->code) {
            e->user_data = dsc->user_data;
#if LV_USE_EXT_DATA
            e->ext_data.data = dsc->ext_data.data;
#endif
            dsc->cb(e);
            if(e->stop_processing) break;

            /*Stop if the object is deleted*/
            if(e->deleted) {
                res = LV_RESULT_INVALID;
                break;
            }
        }
    }

    if(is_traversing) return res;

    if(e->deleted) cleanup_event_list_core(&back_array_head);
    else {
        list->is_traversing = false;
        cleanup_event_list(list);
    }

    return res;
}

lv_event_dsc_t * lv_event_add(lv_event_list_t * list, lv_event_cb_t cb, lv_event_code_t filter,
                              void * user_data)
{
    lv_event_dsc_t * dsc = lv_malloc(sizeof(lv_event_dsc_t));
    LV_ASSERT_NULL(dsc);

    dsc->cb = cb;
    dsc->filter = filter;
    dsc->user_data = user_data;
#if LV_USE_EXT_DATA
    dsc->ext_data.free_cb = NULL;
    dsc->ext_data.data = NULL;
#endif

    if(event_array_size(list) == 0) {
        /*event list hasn't been initialized.*/
        lv_array_init(&list->array, 1, sizeof(lv_event_dsc_t *));
    }

    lv_array_push_back(&list->array, &dsc);
    return dsc;
}

bool lv_event_remove_dsc(lv_event_list_t * list, lv_event_dsc_t * dsc)
{
    LV_ASSERT_NULL(list);
    LV_ASSERT_NULL(dsc);

    const uint32_t size = event_array_size(list);
    for(uint32_t i = 0; i < size; i++) {
        lv_event_dsc_t * event = *event_array_at(list, i);
        if(event == dsc) {
#if LV_USE_EXT_DATA
            if(dsc->ext_data.free_cb) {
                dsc->ext_data.free_cb(dsc->ext_data.data);
                dsc->ext_data.data = NULL;
            }
#endif
            event_mark_deleting(list, event);
            cleanup_event_list(list);
            return true;
        }
    }

    return false;
}

uint32_t lv_event_get_count(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    return event_array_size(list);
}

lv_event_dsc_t * lv_event_get_dsc(lv_event_list_t * list, uint32_t index)
{
    LV_ASSERT_NULL(list);
    lv_event_dsc_t ** dsc = event_array_at(list, index);
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
#if LV_USE_EXT_DATA
    if(dsc->ext_data.free_cb) {
        dsc->ext_data.free_cb(dsc->ext_data.data);
        dsc->ext_data.data = NULL;
    }
#endif
    event_mark_deleting(list, dsc);
    cleanup_event_list(list);
    return true;
}

void lv_event_remove_all(lv_event_list_t * list)
{
    LV_ASSERT_NULL(list);
    const uint32_t size = event_array_size(list);
    for(uint32_t i = 0; i < size; i++) {
#if LV_USE_EXT_DATA
        lv_event_dsc_t * dsc = lv_event_get_dsc(list, i);
        if(dsc && dsc->ext_data.free_cb) {
            dsc->ext_data.free_cb(dsc->ext_data.data);
            dsc->ext_data.data = NULL;
        }
#endif
        event_mark_deleting(list, *event_array_at(list, i));
    }

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

void lv_event_stop_trickling(lv_event_t * e)
{
    e->stop_trickling = 1;
}

void lv_event_stop_processing(lv_event_t * e)
{
    e->stop_processing = 1;
}

void lv_event_free_user_data_cb(lv_event_t * e)
{
    void * p = lv_event_get_user_data(e);
    lv_free(p);
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

const char * lv_event_code_get_name(lv_event_code_t code)
{
    /*Remove the preprocess flag*/
    code &= ~LV_EVENT_PREPROCESS;

#define ENUM_CASE(x) case LV_##x: return #x

    switch(code) {
            ENUM_CASE(EVENT_ALL);

            /** Input device events*/
            ENUM_CASE(EVENT_PRESSED);
            ENUM_CASE(EVENT_PRESSING);
            ENUM_CASE(EVENT_PRESS_LOST);
            ENUM_CASE(EVENT_SHORT_CLICKED);
            ENUM_CASE(EVENT_SINGLE_CLICKED);
            ENUM_CASE(EVENT_DOUBLE_CLICKED);
            ENUM_CASE(EVENT_TRIPLE_CLICKED);
            ENUM_CASE(EVENT_LONG_PRESSED);
            ENUM_CASE(EVENT_LONG_PRESSED_REPEAT);
            ENUM_CASE(EVENT_CLICKED);
            ENUM_CASE(EVENT_RELEASED);
            ENUM_CASE(EVENT_SCROLL_BEGIN);
            ENUM_CASE(EVENT_SCROLL_THROW_BEGIN);
            ENUM_CASE(EVENT_SCROLL_END);
            ENUM_CASE(EVENT_SCROLL);
            ENUM_CASE(EVENT_GESTURE);
            ENUM_CASE(EVENT_KEY);
            ENUM_CASE(EVENT_ROTARY);
            ENUM_CASE(EVENT_FOCUSED);
            ENUM_CASE(EVENT_DEFOCUSED);
            ENUM_CASE(EVENT_LEAVE);
            ENUM_CASE(EVENT_HIT_TEST);
            ENUM_CASE(EVENT_INDEV_RESET);
            ENUM_CASE(EVENT_HOVER_OVER);
            ENUM_CASE(EVENT_HOVER_LEAVE);

            /** Drawing events*/
            ENUM_CASE(EVENT_COVER_CHECK);
            ENUM_CASE(EVENT_REFR_EXT_DRAW_SIZE);
            ENUM_CASE(EVENT_DRAW_MAIN_BEGIN);
            ENUM_CASE(EVENT_DRAW_MAIN);
            ENUM_CASE(EVENT_DRAW_MAIN_END);
            ENUM_CASE(EVENT_DRAW_POST_BEGIN);
            ENUM_CASE(EVENT_DRAW_POST);
            ENUM_CASE(EVENT_DRAW_POST_END);
            ENUM_CASE(EVENT_DRAW_TASK_ADDED);

            /** Special events*/
            ENUM_CASE(EVENT_VALUE_CHANGED);
            ENUM_CASE(EVENT_INSERT);
            ENUM_CASE(EVENT_REFRESH);
            ENUM_CASE(EVENT_READY);
            ENUM_CASE(EVENT_CANCEL);
            ENUM_CASE(EVENT_STATE_CHANGED);

            /** Other events*/
            ENUM_CASE(EVENT_CREATE);
            ENUM_CASE(EVENT_DELETE);
            ENUM_CASE(EVENT_CHILD_CHANGED);
            ENUM_CASE(EVENT_CHILD_CREATED);
            ENUM_CASE(EVENT_CHILD_DELETED);
            ENUM_CASE(EVENT_SCREEN_UNLOAD_START);
            ENUM_CASE(EVENT_SCREEN_LOAD_START);
            ENUM_CASE(EVENT_SCREEN_LOADED);
            ENUM_CASE(EVENT_SCREEN_UNLOADED);
            ENUM_CASE(EVENT_SIZE_CHANGED);
            ENUM_CASE(EVENT_STYLE_CHANGED);
            ENUM_CASE(EVENT_LAYOUT_CHANGED);
            ENUM_CASE(EVENT_GET_SELF_SIZE);
            ENUM_CASE(EVENT_UPDATE_LAYOUT_COMPLETED);

            /** Events of optional LVGL components*/
            ENUM_CASE(EVENT_INVALIDATE_AREA);
            ENUM_CASE(EVENT_RESOLUTION_CHANGED);
            ENUM_CASE(EVENT_COLOR_FORMAT_CHANGED);
            ENUM_CASE(EVENT_REFR_REQUEST);
            ENUM_CASE(EVENT_REFR_START);
            ENUM_CASE(EVENT_REFR_READY);
            ENUM_CASE(EVENT_RENDER_START);
            ENUM_CASE(EVENT_RENDER_READY);
            ENUM_CASE(EVENT_FLUSH_START);
            ENUM_CASE(EVENT_FLUSH_FINISH);
            ENUM_CASE(EVENT_FLUSH_WAIT_START);
            ENUM_CASE(EVENT_FLUSH_WAIT_FINISH);

            ENUM_CASE(EVENT_VSYNC);
            ENUM_CASE(EVENT_VSYNC_REQUEST);

#if LV_USE_TRANSLATION
            ENUM_CASE(EVENT_TRANSLATION_LANGUAGE_CHANGED);
#endif /*LV_USE_TRANSLATION*/

        /* Special event flags */
        case LV_EVENT_LAST:
        case LV_EVENT_PREPROCESS:
        case LV_EVENT_MARKED_DELETING:
            break;

            /* Note that default is not added here because when adding new event code,
             * if forget to add case, the compiler will automatically report a warning.
             */
    }

#undef ENUM_CASE

    return "EVENT_UNKNOWN";
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void cleanup_event_list_core(lv_array_t * array)
{
    const uint32_t size = lv_array_size(array);
    uint32_t kept_count = 0;
    for(uint32_t i = 0; i < size; i++) {
        lv_event_dsc_t ** dsc_i = lv_array_at(array, i);
        lv_event_dsc_t ** dsc_kept = lv_array_at(array, kept_count);
        if(event_is_marked_deleting(*dsc_i)) lv_free(*dsc_i);
        else {
            *dsc_kept = *dsc_i;
            kept_count++;
        }
    }

    if(kept_count == 0) lv_array_deinit(array);
    else lv_array_resize(array, kept_count);
}

static void cleanup_event_list(lv_event_list_t * list)
{
    if(list->is_traversing) return;
    if(list->has_marked_deleting == false) return;

    cleanup_event_list_core(&list->array);

    list->has_marked_deleting = false;
}

static void event_mark_deleting(lv_event_list_t * list, lv_event_dsc_t * dsc)
{
    list->has_marked_deleting = true;
    dsc->filter |= LV_EVENT_MARKED_DELETING;
}
static bool event_is_marked_deleting(lv_event_dsc_t * dsc)
{
    return (dsc->filter & LV_EVENT_MARKED_DELETING) != 0;
}
static uint32_t event_array_size(lv_event_list_t * list)
{
    return lv_array_size(&list->array);
}
static lv_event_dsc_t ** event_array_at(lv_event_list_t * list, uint32_t index)
{
    return lv_array_at(&list->array, index);
}
