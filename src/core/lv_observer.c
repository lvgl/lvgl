/**
 * @file lv_observer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_observer_private.h"
#if LV_USE_OBSERVER

#include "../lvgl_public.h"
#include "../core/lv_obj_private.h"
#include "../misc/lv_event_private.h"
/*********************
 *      DEFINES
 *********************/

#ifndef FLT_MAX
    #define FLT_MAX 3.402823466e+38F /* float max value */
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    FLAG_COND_EQ = 0,
    FLAG_COND_GT = 1,
    FLAG_COND_GE = 2
} flag_cond_t;

typedef struct {
    uint32_t flag;
    lv_subject_value_t value;
    uint32_t inv     : 1;
    flag_cond_t cond : 3;
} flag_and_cond_t;

typedef struct {
    lv_subject_t * subject;
    int32_t value;
} subject_set_int_user_data_t;

typedef struct {
    lv_subject_t * subject;
    float value;
} subject_set_float_user_data_t;

typedef struct {
    lv_subject_t * subject;
    const char * value;
} subject_set_string_user_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void subject_toggle_cb(lv_event_t * e);
static void subject_set_int_cb(lv_event_t * e);
#if LV_USE_FLOAT
    static void subject_set_float_cb(lv_event_t * e);
#endif

static void subject_set_string_cb(lv_event_t * e);
static void subject_increment_cb(lv_event_t * e);

static void unsubscribe_on_delete_cb(lv_event_t * e);
static void group_notify_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_observer_t * bind_to_bitfield(lv_subject_t * subject, lv_obj_t * obj, lv_observer_cb_t cb, uint32_t flag,
                                        int32_t ref_value, bool inv, flag_cond_t cond);

static void obj_flag_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void obj_state_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void obj_value_changed_event_cb(lv_event_t * e);

static void lv_subject_notify_if_changed(lv_subject_t * subject);

static void subject_set_string_free_user_data_event_cb(lv_event_t * e);

static void set_bool_observer(lv_observer_t * observer, lv_subject_t * subject);
static void set_int_observer(lv_observer_t * observer, lv_subject_t * subject);
#if LV_USE_FLOAT
    static void set_float_observer(lv_observer_t * observer, lv_subject_t * subject);
#endif /*LV_USE_FLOAT*/

static void set_string_observer(lv_observer_t * observer, lv_subject_t * subject);
static void set_color_observer(lv_observer_t * observer, lv_subject_t * subject);
static void set_pointer_observer(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_subject_t * lv_subject_create(lv_subject_type_t type)
{
    lv_subject_t * subject = lv_malloc(sizeof(lv_subject_t));
    LV_ASSERT_MALLOC(subject);
    if(subject == NULL) return NULL;

    switch(type) {
        case LV_SUBJECT_TYPE_INT:
            lv_subject_init_int(subject, 0);
            break;
#if LV_USE_FLOAT
        case LV_SUBJECT_TYPE_FLOAT:
            lv_subject_init_float(subject, 0.0f);
            break;
#endif
        case LV_SUBJECT_TYPE_STRING:
            lv_subject_init_string(subject, NULL, NULL, 0, "");
            break;
        case LV_SUBJECT_TYPE_POINTER:
            lv_subject_init_pointer(subject, NULL);
            break;
        case LV_SUBJECT_TYPE_COLOR:
            lv_subject_init_color(subject, lv_color_black());
            break;
        case LV_SUBJECT_TYPE_GROUP:
            lv_subject_init_group(subject, NULL, 0);
            break;
        default:
            LV_LOG_WARN("Invalid subject type: %d", type);
            lv_free(subject);
            return NULL;
    }

    return subject;
}

void lv_subject_delete(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return);

    lv_subject_deinit(subject);
    lv_free(subject);
}

void lv_subject_init_int(lv_subject_t * subject, int32_t value)
{
    LV_CHECK_ARG(subject != NULL, return);

    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_INT;
    subject->value.num = value;
    subject->prev_value.num = value;
    subject->min_value.num = INT32_MIN;
    subject->max_value.num = INT32_MAX;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_int(lv_subject_t * subject, int32_t value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return);

    value = LV_CLAMP(subject->min_value.num, value, subject->max_value.num);

    subject->prev_value.num = subject->value.num;
    subject->value.num = value;
    lv_subject_notify_if_changed(subject);
}

int32_t lv_subject_get_int(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return 0);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return 0);

    return subject->value.num;
}

int32_t lv_subject_get_previous_int(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return 0);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return 0);

    return subject->prev_value.num;
}

void lv_subject_set_min_value_int(lv_subject_t * subject, int32_t min_value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return);

    subject->min_value.num = min_value;
}

void lv_subject_set_max_value_int(lv_subject_t * subject, int32_t max_value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return);

    subject->max_value.num = max_value;
}

#if LV_USE_FLOAT

void lv_subject_init_float(lv_subject_t * subject, float value)
{
    LV_CHECK_ARG(subject != NULL, return);

    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_FLOAT;
    subject->value.float_v = value;
    subject->prev_value.float_v = value;
    subject->min_value.float_v = -FLT_MAX;
    subject->max_value.float_v = FLT_MAX;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_float(lv_subject_t * subject, float value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return);

    value = LV_CLAMP(subject->min_value.float_v, value, subject->max_value.float_v);

    subject->prev_value.float_v = subject->value.float_v;
    subject->value.float_v = value;
    lv_subject_notify_if_changed(subject);
}

float lv_subject_get_float(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return 0.0);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return 0.0);

    return subject->value.float_v;
}

float lv_subject_get_previous_float(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return 0.0);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return 0.0);

    return subject->prev_value.float_v;
}

void lv_subject_set_min_value_float(lv_subject_t * subject, float min_value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return);

    subject->min_value.float_v = min_value;
}

void lv_subject_set_max_value_float(lv_subject_t * subject, float max_value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return);

    subject->max_value.float_v = max_value;
}


#endif /*LV_USE_FLOAT*/

void lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(value != NULL, return);

    lv_memzero(subject, sizeof(lv_subject_t));
    lv_strlcpy(buf, value, size);
    if(prev_buf) lv_strlcpy(prev_buf, value, size);

    subject->type = LV_SUBJECT_TYPE_STRING;
    subject->size = (uint32_t)size;
    subject->value.pointer = buf;
    subject->prev_value.pointer = prev_buf;

    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_buf(lv_subject_t * subject, char * buf, char * prev_buf, size_t size)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return);

    if(buf) {
        if(subject->value.pointer) lv_strlcpy(buf, subject->value.pointer, size);
        else buf[0] = '\0';
    }
    if(prev_buf) {
        if(subject->prev_value.pointer) lv_strlcpy(prev_buf, subject->prev_value.pointer, size);
        else prev_buf[0] = '\0';
    }

    subject->size = (uint32_t)size;
    subject->value.pointer = buf;
    subject->prev_value.pointer = prev_buf;
}

void lv_subject_copy_string(lv_subject_t * subject, const char * buf)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return);
    LV_CHECK_ARG(buf != NULL, return);

    if(subject->size == 0) return;
    if(subject->prev_value.pointer) {
        lv_strlcpy((char *)subject->prev_value.pointer, subject->value.pointer, subject->size);
    }

    lv_strlcpy((char *)subject->value.pointer, buf, subject->size);

    lv_subject_notify_if_changed(subject);
}

void lv_subject_snprintf(lv_subject_t * subject, const char * format, ...)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return);
    LV_CHECK_ARG(format != NULL, return);

    if(subject->size < 1U) return;

    if(subject->prev_value.pointer) {
        lv_strlcpy((char *)subject->prev_value.pointer, subject->value.pointer, subject->size);
    }

    va_list va;
    va_start(va, format);
    const int ret = lv_vsnprintf((char *)subject->value.pointer, subject->size, format, va);
    LV_UNUSED(ret);
    va_end(va);

    lv_subject_notify_if_changed(subject);
}

const char * lv_subject_get_string(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return NULL);

    return subject->value.pointer;
}

const char * lv_subject_get_previous_string(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return NULL);

    return subject->prev_value.pointer;
}

void lv_subject_init_pointer(lv_subject_t * subject, void * value)
{
    LV_CHECK_ARG(subject != NULL, return);

    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_POINTER;
    subject->value.pointer = value;
    subject->prev_value.pointer = value;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_pointer(lv_subject_t * subject, void * ptr)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_POINTER, return);

    subject->prev_value.pointer = subject->value.pointer;
    subject->value.pointer = ptr;
    lv_subject_notify_if_changed(subject);
}

const void * lv_subject_get_pointer(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_POINTER, return NULL);


    return subject->value.pointer;
}

const void * lv_subject_get_previous_pointer(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_POINTER, return NULL);

    return subject->prev_value.pointer;
}

void lv_subject_init_color(lv_subject_t * subject, lv_color_t color)
{
    LV_CHECK_ARG(subject != NULL, return);

    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_COLOR;
    subject->value.color = color;
    subject->prev_value.color = color;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_color(lv_subject_t * subject, lv_color_t color)
{
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_COLOR, return);

    subject->prev_value.color = subject->value.color;
    subject->value.color = color;
    lv_subject_notify_if_changed(subject);
}

lv_color_t lv_subject_get_color(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return lv_color_black());
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_COLOR, return lv_color_black());

    return subject->value.color;
}

lv_color_t lv_subject_get_previous_color(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return lv_color_black());
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_COLOR, return lv_color_black());

    return subject->prev_value.color;
}

void lv_subject_init_group(lv_subject_t * group_subject, lv_subject_t * list[], uint32_t list_len)
{
    LV_CHECK_ARG(group_subject != NULL, return);

    lv_memzero(group_subject, sizeof(lv_subject_t));
    group_subject->type = LV_SUBJECT_TYPE_GROUP;
    lv_ll_init(&(group_subject->subs_ll), sizeof(lv_observer_t));

    /* Bind all list[] subjects to `group_subject`. */
    lv_subject_set_group_list(group_subject, list, list_len);
}

void lv_subject_set_group_list(lv_subject_t * group_subject, lv_subject_t * list[], uint32_t list_len)
{
    LV_CHECK_ARG(group_subject != NULL, return);
    LV_CHECK_ARG(group_subject->type == LV_SUBJECT_TYPE_GROUP, return);

    /* Unbind all previous subjects from `group_subject`. */
    uint32_t i;
    for(i = 0; i < group_subject->size; i++) {
        lv_subject_t * sub = ((lv_subject_t **)(group_subject->value.pointer))[i];
        lv_observer_t * observer;
        LV_LL_READ(&sub->subs_ll, observer) {
            if(observer->cb == group_notify_cb && observer->user_data == group_subject) {
                lv_observer_remove(observer);
                break;
            }
        }
    }

    group_subject->size = list_len;
    group_subject->value.pointer = list;
    /* Bind all new subjects in `list[]` to `group_subject`. */
    for(i = 0; i < list_len; i++) {
        lv_subject_add_observer(list[i], group_notify_cb, group_subject);
    }

    lv_subject_notify(group_subject);
}

lv_subject_t * lv_subject_get_group_element(lv_subject_t * subject, int32_t index)
{
    LV_CHECK_ARG(subject != NULL, return NULL);

    if(subject->type != LV_SUBJECT_TYPE_GROUP) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_GROUP");
        return NULL;
    }

    if(index >= (int32_t)subject->size)  return NULL;
    if(index < 0)  return NULL;

    return ((lv_subject_t **)(subject->value.pointer))[index];
}

void lv_subject_deinit(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return);

    /*Unsubscribe all subjects from the group */
    if(subject->type == LV_SUBJECT_TYPE_GROUP) {
        lv_subject_set_group_list(subject, NULL, 0);
    }

    lv_observer_t * observer = lv_ll_get_head(&subject->subs_ll);
    while(observer) {
        lv_observer_t * observer_next = lv_ll_get_next(&subject->subs_ll, observer);

        lv_observer_remove(observer);
        observer = observer_next;
    }

    lv_ll_clear(&subject->subs_ll);
}

lv_observer_t * lv_subject_add_observer(lv_subject_t * subject, lv_observer_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(cb != NULL, return NULL);

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, cb, NULL, user_data);
    if(observer == NULL) return NULL;

    observer->for_obj = 0;
    return observer;
}

lv_observer_t * lv_subject_add_observer_obj(lv_subject_t * subject, lv_observer_cb_t cb, lv_obj_t * obj,
                                            void * user_data)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type != LV_SUBJECT_TYPE_INVALID, return NULL);

    lv_observer_t * observer = lv_ll_ins_tail(&(subject->subs_ll));
    LV_ASSERT_MALLOC(observer);
    if(observer == NULL) return NULL;

    lv_memzero(observer, sizeof(*observer));

    observer->subject = subject;
    observer->cb = cb;
    observer->user_data = user_data;
    observer->target = obj;
    observer->for_obj = 1;
    /* subscribe to delete event of the object */
    if(obj != NULL) {
        lv_obj_add_event_cb(obj, unsubscribe_on_delete_cb, LV_EVENT_DELETE, observer);
    }

    /* Update Observer immediately. */
    if(observer->cb) observer->cb(observer, subject);

    return observer;
}

lv_observer_t * lv_subject_add_observer_with_target(lv_subject_t * subject, lv_observer_cb_t cb, void * target,
                                                    void * user_data)
{
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type != LV_SUBJECT_TYPE_INVALID, return NULL);

    lv_observer_t * observer = lv_ll_ins_tail(&(subject->subs_ll));
    LV_ASSERT_MALLOC(observer);
    if(observer == NULL) return NULL;

    lv_memzero(observer, sizeof(*observer));

    observer->subject = subject;
    observer->cb = cb;
    observer->user_data = user_data;
    observer->target = target;

    /* Update Observer immediately. */
    if(observer->cb) observer->cb(observer, subject);

    return observer;
}

void lv_observer_remove(lv_observer_t * observer)
{
    if(observer == NULL) return;

    if(observer->for_obj && observer->target) {
        lv_obj_remove_event_cb_with_user_data(observer->target, unsubscribe_on_delete_cb, observer);
        lv_obj_remove_event_cb_with_user_data(observer->target, NULL, observer->subject);
    }

    observer->subject->notify_restart_query = 1;

#if LV_USE_EXT_DATA
    if(observer->subject->ext_data.free_cb) {
        observer->subject->ext_data.free_cb(observer->subject->ext_data);
        observer->subject->ext_data = NULL;
    }
#endif

    lv_ll_remove(&(observer->subject->subs_ll), observer);

    if(observer->auto_free_user_data) {
        lv_free(observer->user_data);
    }
    lv_free(observer);
}

void lv_obj_remove_from_subject(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_CHECK_ARG(obj != NULL, return);
    /* subject == NULL is documented as valid: remove from ALL subjects */

    /*
     * Look for the `observer` that connects `obj` and `subject`
     * Since the obj is associated with the subject,
     *  the `obj` will have an LV_EVENT_REMOVE event with the `unsubscribe_on_delete_cb` callback
     *  associated.
     * From the event we can then find the observer in the event's `user_data` field
     */
    int32_t i;
    int32_t event_cnt = (int32_t)(obj->spec_attr ? lv_event_get_count(&obj->spec_attr->event_list) : 0);
    for(i = event_cnt - 1; i >= 0; i--) {
        lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(obj, i);
        if(event_dsc->cb == unsubscribe_on_delete_cb) {
            lv_observer_t * observer = event_dsc->user_data;
            if(subject == NULL || subject == observer->subject) {
                /* lv_observer_remove handles the deletion of all possible event callbacks */
                lv_observer_remove(observer);
            }
        }
    }
    /* Gracefully de-couple `subject` from Widget by deleting any existing
     * `LV_EVENT_VALUE_CHANGED` event associated with `subject` in case
     * one of the `..._bind_value()` functions was used. */
    lv_obj_remove_event_cb_with_user_data(obj, NULL, subject);

}

void * lv_observer_get_target(lv_observer_t * observer)
{
    LV_CHECK_ARG(observer != NULL, return NULL);

    return observer->target;
}

void lv_subject_notify(lv_subject_t * subject)
{
    LV_CHECK_ARG(subject != NULL, return);

    lv_observer_t * observer;
    LV_LL_READ(&(subject->subs_ll), observer) {
        observer->notified = 0;
    }

    do {
        subject->notify_restart_query = 0;
        LV_LL_READ(&(subject->subs_ll), observer) {
            if(observer->cb && observer->notified == 0) {
                observer->cb(observer, subject);
                if(subject->notify_restart_query) break;
                observer->notified = 1;
            }
        }
    } while(subject->notify_restart_query);
}

#if LV_USE_EXT_DATA
void lv_subject_set_external_data(lv_subject_t * subject, void * data, void (* free_cb)(void * data))
{
    LV_CHECK_ARG(subject != NULL, return);

    subject->ext_data = data;
    subject->ext_data.free_cb = free_cb;
}
#endif

lv_subject_increment_dsc_t * lv_obj_add_subject_increment_event(lv_obj_t * obj, lv_subject_t * subject,
                                                                lv_event_code_t trigger, int32_t step)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT || subject->type == LV_SUBJECT_TYPE_FLOAT, return NULL);

    lv_subject_increment_dsc_t * user_data = lv_malloc(sizeof(lv_subject_increment_dsc_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data in in <lv_obj-subject_increment>");
        return NULL;
    }

    user_data->step = step;
    user_data->subject = subject;
    user_data->rollover = false;
    user_data->min_value = INT32_MIN;
    user_data->max_value = INT32_MAX;
    lv_obj_add_event_cb(obj, subject_increment_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, lv_event_free_user_data_cb, LV_EVENT_DELETE, user_data);

    return user_data;
}

void lv_obj_set_subject_increment_event_min_value(lv_obj_t * obj, lv_subject_increment_dsc_t * dsc, int32_t min_value)
{
    LV_UNUSED(obj);
    LV_CHECK_ARG(dsc != NULL, return);

    dsc->min_value = min_value;
    if(dsc->subject->type == LV_SUBJECT_TYPE_INT) {
        if(dsc->subject->value.num < min_value) {
            lv_subject_set_int(dsc->subject, min_value);
        }
    }
#if LV_USE_FLOAT
    else if(dsc->subject->type == LV_SUBJECT_TYPE_FLOAT) {
        if(dsc->subject->value.float_v < (float)min_value) {
            lv_subject_set_float(dsc->subject, (float)min_value);
        }
    }
#endif
}

void lv_obj_set_subject_increment_event_max_value(lv_obj_t * obj, lv_subject_increment_dsc_t * dsc, int32_t max_value)
{
    LV_UNUSED(obj);
    LV_CHECK_ARG(dsc != NULL, return);

    dsc->max_value = max_value;
    if(dsc->subject->type == LV_SUBJECT_TYPE_INT) {
        if(dsc->subject->value.num > max_value) {
            lv_subject_set_int(dsc->subject, max_value);
        }
    }
#if LV_USE_FLOAT
    else if(dsc->subject->type == LV_SUBJECT_TYPE_FLOAT) {
        if(dsc->subject->value.float_v > (float)max_value) {
            lv_subject_set_float(dsc->subject, (float)max_value);
        }
    }
#endif
}

void lv_obj_set_subject_increment_event_rollover(lv_obj_t * obj, lv_subject_increment_dsc_t * dsc, bool rollover)
{
    LV_UNUSED(obj);
    LV_CHECK_ARG(dsc != NULL, return);

    dsc->rollover = rollover;
}

void lv_obj_add_subject_toggle_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return);

    lv_obj_add_event_cb(obj, subject_toggle_cb, trigger, subject);
}

void lv_obj_add_subject_set_int_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, int32_t value)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return);

    subject_set_int_user_data_t * user_data = lv_malloc(sizeof(subject_set_int_user_data_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data");
        return;
    }

    user_data->subject = subject;
    user_data->value = value;

    lv_obj_add_event_cb(obj, subject_set_int_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, lv_event_free_user_data_cb, LV_EVENT_DELETE, user_data);
}

#if LV_USE_FLOAT
void lv_obj_add_subject_set_float_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, float value)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return);

    subject_set_float_user_data_t * user_data = lv_malloc(sizeof(subject_set_float_user_data_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data");
        return;
    }

    user_data->subject = subject;
    user_data->value = value;

    lv_obj_add_event_cb(obj, subject_set_float_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, lv_event_free_user_data_cb, LV_EVENT_DELETE, user_data);
}
#endif /*LV_USE_FLOAT*/

void lv_obj_add_subject_set_string_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger,
                                         const char * value)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(subject != NULL, return);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return);
    LV_CHECK_ARG(value != NULL, return);

    subject_set_string_user_data_t * user_data = lv_malloc(sizeof(subject_set_string_user_data_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data");
        return;
    }

    user_data->subject = subject;
    user_data->value = lv_strdup(value);
    if(user_data->value == NULL) {
        LV_ASSERT_MALLOC(user_data->value);
        LV_LOG_WARN("Couldn't allocate string value");
        lv_free(user_data);
        return;
    }

    lv_obj_add_event_cb(obj, subject_set_string_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, subject_set_string_free_user_data_event_cb, LV_EVENT_DELETE, user_data);
}

lv_observer_t * lv_obj_bind_bool(lv_obj_t * obj, lv_subject_t * subject, lv_obj_set_bool_t set_bool_cb)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(set_bool_cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return NULL);

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, set_bool_observer, obj, NULL);
    if(observable == NULL) {
        LV_LOG_WARN("Couldn't add observer to subject");
        return NULL;
    }

    /* Passing a function pointer as void * user_data generates warning so set it here, and call the callback manually */
    observable->user_cb = (void (*)(void))set_bool_cb;
    set_bool_observer(observable, subject);
    return observable;
}

lv_observer_t * lv_obj_bind_int(lv_obj_t * obj, lv_subject_t * subject, lv_obj_set_int_t set_int_cb)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(set_int_cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_INT, return NULL);
    lv_observer_t * observable = lv_subject_add_observer_obj(subject, set_int_observer, obj, NULL);
    if(observable == NULL) {
        LV_LOG_WARN("Couldn't add observer to subject");
        return NULL;
    }

    /* Passing a function pointer as void * user_data generates warning so set it here, and call the callback manually */
    observable->user_cb = (void (*)(void))set_int_cb;
    set_int_observer(observable, subject);
    return observable;
}

#if LV_USE_FLOAT
lv_observer_t * lv_obj_bind_float(lv_obj_t * obj, lv_subject_t * subject, lv_obj_set_float_t set_float_cb)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(set_float_cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_FLOAT, return NULL);

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, set_float_observer, obj, NULL);
    if(observable == NULL) {
        LV_LOG_WARN("Couldn't add observer to subject");
        return NULL;
    }

    /* Passing a function pointer as void * user_data generates warning so set it here, and call the callback manually */
    observable->user_cb = (void (*)(void))set_float_cb;
    set_float_observer(observable, subject);
    return observable;
}
#endif

lv_observer_t * lv_obj_bind_string(lv_obj_t * obj, lv_subject_t * subject, lv_obj_set_string_t set_string_cb)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(set_string_cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_STRING, return NULL);

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, set_string_observer, obj, NULL);
    if(observable == NULL) {
        LV_LOG_WARN("Couldn't add observer to subject");
        return NULL;
    }

    /* Passing a function pointer as void * user_data generates warning so set it here, and call the callback manually */
    observable->user_cb = (void (*)(void))set_string_cb;
    set_string_observer(observable, subject);
    return observable;
}

lv_observer_t * lv_obj_bind_color(lv_obj_t * obj, lv_subject_t * subject, lv_obj_set_color_t set_color_cb)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(set_color_cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_COLOR, return NULL);

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, set_color_observer, obj, NULL);
    if(observable == NULL) {
        LV_LOG_WARN("Couldn't add observer to subject");
        return NULL;
    }

    /* Passing a function pointer as void * user_data generates warning so set it here, and call the callback manually */
    observable->user_cb = (void (*)(void))set_color_cb;
    set_color_observer(observable, subject);
    return observable;
}

lv_observer_t * lv_obj_bind_pointer(lv_obj_t * obj, lv_subject_t * subject, lv_obj_set_pointer_t set_pointer_cb)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);
    LV_CHECK_ARG(set_pointer_cb != NULL, return NULL);
    LV_CHECK_ARG(subject->type == LV_SUBJECT_TYPE_POINTER, return NULL);

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, set_pointer_observer, obj, NULL);
    if(observable == NULL) {
        LV_LOG_WARN("Couldn't add observer to subject");
        return NULL;
    }

    /* Passing a function pointer as void * user_data generates warning so set it here, and call the callback manually */
    observable->user_cb = (void (*)(void))set_pointer_cb;
    set_pointer_observer(observable, subject);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, false, FLAG_COND_EQ);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag,
                                           int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, true, FLAG_COND_EQ);
    return observable;
}
lv_observer_t * lv_obj_bind_flag_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, false, FLAG_COND_GT);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, false, FLAG_COND_GE);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    /* a < b == !(a >= b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, true, FLAG_COND_GE);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    /* a <= b == !(a > b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, true, FLAG_COND_GT);
    return observable;

}

lv_observer_t * lv_obj_bind_state_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, false,
                                                  FLAG_COND_EQ);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, true,
                                                  FLAG_COND_EQ);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, false,
                                                  FLAG_COND_GT);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, false,
                                                  FLAG_COND_GE);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    /* a < b == !(a >= b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, true,
                                                  FLAG_COND_GE);
    return observable;

}

lv_observer_t * lv_obj_bind_state_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    /* a <= b == !(a > b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, true,
                                                  FLAG_COND_GT);
    return observable;
}


lv_observer_t * lv_obj_bind_checked(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, LV_STATE_CHECKED, 0, true,
                                                  FLAG_COND_EQ);

    lv_obj_add_event_cb(obj, obj_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);

    return observable;
}


lv_obj_t * lv_observer_get_target_obj(lv_observer_t * observer)
{
    LV_CHECK_ARG(observer != NULL, return NULL);

    return (lv_obj_t *)lv_observer_get_target(observer);
}

void * lv_observer_get_user_data(const lv_observer_t * observer)
{
    LV_CHECK_ARG(observer != NULL, return NULL);

    return observer->user_data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void subject_toggle_cb(lv_event_t * e)
{
    lv_subject_t * subject = lv_event_get_user_data(e);
    int32_t v = lv_subject_get_int(subject);
    v = !v;

    lv_subject_set_int(subject, v);
}

static void subject_set_int_cb(lv_event_t * e)
{
    subject_set_int_user_data_t * user_data = lv_event_get_user_data(e);
    lv_subject_set_int(user_data->subject, user_data->value);
}


#if LV_USE_FLOAT
static void subject_set_float_cb(lv_event_t * e)
{
    subject_set_float_user_data_t * user_data = lv_event_get_user_data(e);
    lv_subject_set_float(user_data->subject, user_data->value);
}
#endif

static void subject_set_string_cb(lv_event_t * e)
{
    subject_set_string_user_data_t * user_data = lv_event_get_user_data(e);
    lv_subject_copy_string(user_data->subject, user_data->value);
}

static void subject_increment_cb(lv_event_t * e)
{
    lv_subject_increment_dsc_t * user_data = lv_event_get_user_data(e);

    if(user_data->subject->type == LV_SUBJECT_TYPE_INT) {
        /*Use the smaller range*/
        int32_t max_value = LV_MIN(user_data->max_value, user_data->subject->max_value.num);
        int32_t min_value = LV_MAX(user_data->min_value, user_data->subject->min_value.num);

        int32_t value = lv_subject_get_int(user_data->subject);
        value += user_data->step;

        if(user_data->rollover) {
            if(value > max_value) {
                value = min_value;
            }
            else if(value < min_value) {
                value = max_value;
            }
        }
        else {
            value = LV_CLAMP(min_value, value, max_value);
        }

        lv_subject_set_int(user_data->subject, value);
    }
#if LV_USE_FLOAT
    else if(user_data->subject->type == LV_SUBJECT_TYPE_FLOAT) {
        /*Use the smaller range*/
        float max_value = LV_MIN((float)user_data->max_value, user_data->subject->max_value.float_v);
        float min_value = LV_MAX((float)user_data->min_value, user_data->subject->min_value.float_v);


        float value = lv_subject_get_float(user_data->subject);
        value += (float)user_data->step;

        if(user_data->rollover) {
            if(value > max_value) {
                value = min_value;
            }
            else if(value < min_value) {
                value = max_value;
            }
        }
        else {
            value = LV_CLAMP(min_value, value, max_value);
        }

        lv_subject_set_float(user_data->subject, value);
    }
#endif
}

static void group_notify_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_subject_t * subject_group = observer->user_data;
    lv_subject_notify(subject_group);
}

static void unsubscribe_on_delete_cb(lv_event_t * e)
{
    lv_observer_t * observer = lv_event_get_user_data(e);
    lv_observer_remove(observer);
}

static lv_observer_t * bind_to_bitfield(lv_subject_t * subject, lv_obj_t * obj, lv_observer_cb_t cb, uint32_t flag,
                                        int32_t ref_value, bool inv, flag_cond_t cond)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    flag_and_cond_t * p = lv_malloc(sizeof(flag_and_cond_t));
    if(p == NULL) {
        LV_LOG_WARN("Out of memory");
        return NULL;
    }

    p->flag = flag;
    p->value.num = ref_value;
    p->inv = inv;
    p->cond = cond;

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, cb, obj, p);
    observable->auto_free_user_data = 1;

    return observable;
}

static void obj_flag_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    flag_and_cond_t * p = observer->user_data;

    /* Initializing this keeps some compilers happy */
    bool res = false;
    switch(p->cond) {
        case FLAG_COND_EQ:
            res = subject->value.num == p->value.num;
            break;
        case FLAG_COND_GT:
            res = subject->value.num > p->value.num;
            break;
        case FLAG_COND_GE:
            res = subject->value.num >= p->value.num;
            break;
    }
    if(p->inv) res = !res;

    /*TODO: the flag binding API is deprecated separately; suppress the warning until then*/
    LV_DEPRECATIONS_IGNORE_BEGIN
    if(res) {
        lv_obj_add_flag(observer->target, p->flag);
    }
    else {
        lv_obj_remove_flag(observer->target, p->flag);
    }
    LV_DEPRECATIONS_IGNORE_END
}

static void obj_state_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    flag_and_cond_t * p = observer->user_data;

    /* Initializing this keeps some compilers happy */
    bool res = false;
    switch(p->cond) {
        case FLAG_COND_EQ:
            res = subject->value.num == p->value.num;
            break;
        case FLAG_COND_GT:
            res = subject->value.num > p->value.num;
            break;
        case FLAG_COND_GE:
            res = subject->value.num >= p->value.num;
            break;
    }
    if(p->inv) res = !res;

    if(res) {
        lv_obj_add_state(observer->target, p->flag);
    }
    else {
        lv_obj_remove_state(observer->target, p->flag);
    }
}

static void obj_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_subject_t * subject = lv_event_get_user_data(e);

    lv_subject_set_int(subject, lv_obj_has_state(obj, LV_STATE_CHECKED));
}

static void lv_subject_notify_if_changed(lv_subject_t * subject)
{

    switch(subject->type) {
        case LV_SUBJECT_TYPE_INVALID :
        case LV_SUBJECT_TYPE_NONE :
            return;
        case LV_SUBJECT_TYPE_INT :
            if(subject->value.num != subject->prev_value.num) {
                lv_subject_notify(subject);
            }
            break;
#if LV_USE_FLOAT
        case LV_SUBJECT_TYPE_FLOAT :
            if(subject->value.float_v != subject->prev_value.float_v) {
                lv_subject_notify(subject);
            }
            break;
#endif
        case LV_SUBJECT_TYPE_GROUP :
        case LV_SUBJECT_TYPE_POINTER :
            /* Always notify as we don't know how to compare this */
            lv_subject_notify(subject);
            break;
        case LV_SUBJECT_TYPE_COLOR  :
            if(!lv_color_eq(subject->value.color, subject->prev_value.color)) {
                lv_subject_notify(subject);
            }
            break;
        case LV_SUBJECT_TYPE_STRING:
            if(!subject->prev_value.pointer ||
               lv_strcmp(subject->value.pointer, subject->prev_value.pointer)) {
                lv_subject_notify(subject);
            }
            break;
    }
}

static void subject_set_string_free_user_data_event_cb(lv_event_t * e)
{
    subject_set_string_user_data_t * user_data = lv_event_get_user_data(e);
    lv_free((void *)user_data->value);
    lv_free(user_data);
}

static void set_bool_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = (lv_obj_t *)observer->target;
    lv_obj_set_bool_t set_bool_cb = (lv_obj_set_bool_t)observer->user_cb;
    if(set_bool_cb) set_bool_cb(obj, subject->value.num);
}

static void set_int_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = (lv_obj_t *)observer->target;
    lv_obj_set_int_t set_int_cb = (lv_obj_set_int_t)observer->user_cb;
    if(set_int_cb) set_int_cb(obj, subject->value.num);
}

#if LV_USE_FLOAT
static void set_float_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = (lv_obj_t *)observer->target;
    lv_obj_set_float_t set_float_cb = (lv_obj_set_float_t)observer->user_cb;
    if(set_float_cb) set_float_cb(obj, subject->value.float_v);
}
#endif /*LV_USE_FLOAT*/

static void set_string_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = (lv_obj_t *)observer->target;
    lv_obj_set_string_t set_string_cb = (lv_obj_set_string_t)observer->user_cb;
    if(set_string_cb) set_string_cb(obj, subject->value.pointer);
}


static void set_color_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = (lv_obj_t *)observer->target;
    lv_obj_set_color_t set_color_cb = (lv_obj_set_color_t)observer->user_cb;
    if(set_color_cb) set_color_cb(obj, subject->value.color);
}

static void set_pointer_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = (lv_obj_t *)observer->target;
    lv_obj_set_pointer_t set_pointer_cb = (lv_obj_set_pointer_t)observer->user_cb;
    if(set_pointer_cb) set_pointer_cb(obj, subject->value.pointer);
}

#endif /*LV_USE_OBSERVER*/
