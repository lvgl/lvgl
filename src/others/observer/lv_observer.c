/**
 * @file lv_observer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_observer_private.h"
#if LV_USE_OBSERVER

#include "../../lvgl.h"
#include "../../core/lv_obj_private.h"
#include "../../misc/lv_event_private.h"

/*********************
 *      DEFINES
 *********************/

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
    const lv_style_t * style;
    lv_style_selector_t selector;
    int32_t value;
} bind_style_t;

typedef struct {
    lv_subject_t * subject;
    int32_t value;
} subject_set_int_user_data_t;

typedef struct {
    lv_subject_t * subject;
    const char * value;
} subject_set_string_user_data_t;


typedef struct {
    lv_subject_t * subject;
    int32_t step;
    int32_t min;
    int32_t max;
} subject_increment_user_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void subject_set_int_cb(lv_event_t * e);
static void subject_set_string_cb(lv_event_t * e);
static void subject_increment_cb(lv_event_t * e);

static void unsubscribe_on_delete_cb(lv_event_t * e);
static void group_notify_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_observer_t * bind_to_bitfield(lv_subject_t * subject, lv_obj_t * obj, lv_observer_cb_t cb, uint32_t flag,
                                        int32_t ref_value, bool inv, flag_cond_t cond);

static void bind_style_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void obj_flag_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void obj_state_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void obj_value_changed_event_cb(lv_event_t * e);

static void lv_subject_notify_if_changed(lv_subject_t * subject);

#if LV_USE_LABEL
    static void label_text_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

#if LV_USE_ARC
    static void arc_value_changed_event_cb(lv_event_t * e);
    static void arc_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

#if LV_USE_SLIDER
    static void slider_value_changed_event_cb(lv_event_t * e);
    static void slider_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

#if LV_USE_ROLLER
    static void roller_value_changed_event_cb(lv_event_t * e);
    static void roller_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

#if LV_USE_DROPDOWN
    static void dropdown_value_changed_event_cb(lv_event_t * e);
    static void dropdown_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

static void free_user_data_event_cb(lv_event_t * e);
static void subject_set_string_free_user_data_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_subject_init_int(lv_subject_t * subject, int32_t value)
{
    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_INT;
    subject->value.num = value;
    subject->prev_value.num = value;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_int(lv_subject_t * subject, int32_t value)
{
    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_INT");
        return;
    }

    subject->prev_value.num = subject->value.num;
    subject->value.num = value;
    lv_subject_notify_if_changed(subject);
}

int32_t lv_subject_get_int(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_INT");
        return 0;
    }

    return subject->value.num;
}

int32_t lv_subject_get_previous_int(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_INT");
        return 0;
    }

    return subject->prev_value.num;
}

#if LV_USE_FLOAT

void lv_subject_init_float(lv_subject_t * subject, float value)
{
    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_FLOAT;
    subject->value.float_v = value;
    subject->prev_value.float_v = value;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_float(lv_subject_t * subject, float value)
{
    if(subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_FLOAT");
        return;
    }

    subject->prev_value.float_v = subject->value.float_v;
    subject->value.float_v = value;
    lv_subject_notify_if_changed(subject);
}

float lv_subject_get_float(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_FLOAT");
        return 0;
    }

    return subject->value.float_v;
}

float lv_subject_get_previous_float(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_FLOAT");
        return 0;
    }

    return subject->prev_value.float_v;
}

#endif /*LV_USE_FLOAT*/

void lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value)
{
    lv_memzero(subject, sizeof(lv_subject_t));
    lv_strlcpy(buf, value, size);
    if(prev_buf) lv_strlcpy(prev_buf, value, size);

    subject->type = LV_SUBJECT_TYPE_STRING;
    subject->size = (uint32_t)size;
    subject->value.pointer = buf;
    subject->prev_value.pointer = prev_buf;

    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_copy_string(lv_subject_t * subject, const char * buf)
{
    if(subject->type != LV_SUBJECT_TYPE_STRING) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_STRING");
        return;
    }

    if(subject->size < 1) return;
    if(subject->prev_value.pointer) {
        lv_strlcpy((char *)subject->prev_value.pointer, subject->value.pointer, subject->size);
    }

    lv_strlcpy((char *)subject->value.pointer, buf, subject->size);

    lv_subject_notify_if_changed(subject);
}

void lv_subject_snprintf(lv_subject_t * subject, const char * format, ...)
{
    if(subject->type != LV_SUBJECT_TYPE_STRING) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_STRING");
        return;
    }

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
    if(subject->type != LV_SUBJECT_TYPE_STRING) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_STRING");
        return "";
    }

    return subject->value.pointer;
}

const char * lv_subject_get_previous_string(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_STRING) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_STRING");
        return NULL;
    }

    return subject->prev_value.pointer;
}

void lv_subject_init_pointer(lv_subject_t * subject, void * value)
{
    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_POINTER;
    subject->value.pointer = value;
    subject->prev_value.pointer = value;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_pointer(lv_subject_t * subject, void * ptr)
{
    if(subject->type != LV_SUBJECT_TYPE_POINTER) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_POINTER");
        return;
    }

    subject->prev_value.pointer = subject->value.pointer;
    subject->value.pointer = ptr;
    lv_subject_notify_if_changed(subject);
}

const void * lv_subject_get_pointer(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_POINTER) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_POINTER");
        return NULL;
    }

    return subject->value.pointer;
}

const void * lv_subject_get_previous_pointer(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_POINTER) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_POINTER");
        return NULL;
    }

    return subject->prev_value.pointer;
}

void lv_subject_init_color(lv_subject_t * subject, lv_color_t color)
{
    lv_memzero(subject, sizeof(lv_subject_t));
    subject->type = LV_SUBJECT_TYPE_COLOR;
    subject->value.color = color;
    subject->prev_value.color = color;
    lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
}

void lv_subject_set_color(lv_subject_t * subject, lv_color_t color)
{
    if(subject->type != LV_SUBJECT_TYPE_COLOR) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_COLOR");
        return;
    }

    subject->prev_value.color = subject->value.color;
    subject->value.color = color;
    lv_subject_notify_if_changed(subject);
}

lv_color_t lv_subject_get_color(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_COLOR) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_COLOR");
        return lv_color_black();
    }

    return subject->value.color;
}

lv_color_t lv_subject_get_previous_color(lv_subject_t * subject)
{
    if(subject->type != LV_SUBJECT_TYPE_COLOR) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_COLOR");
        return lv_color_black();
    }

    return subject->prev_value.color;
}

void lv_subject_init_group(lv_subject_t * group_subject, lv_subject_t * list[], uint32_t list_len)
{
    group_subject->type = LV_SUBJECT_TYPE_GROUP;
    group_subject->size = list_len;
    lv_ll_init(&(group_subject->subs_ll), sizeof(lv_observer_t));
    group_subject->value.pointer = list;

    /* Bind all list[] subjects to `group_subject`. */
    uint32_t i;
    for(i = 0; i < list_len; i++) {
        /* If a subject in `list[]` changes, notify `group_subject`. */
        lv_subject_add_observer(list[i], group_notify_cb, group_subject);
    }
}

void lv_subject_deinit(lv_subject_t * subject)
{
    lv_observer_t * observer = lv_ll_get_head(&subject->subs_ll);
    while(observer) {
        lv_observer_t * observer_next = lv_ll_get_next(&subject->subs_ll, observer);

        lv_observer_remove(observer);
        observer = observer_next;
    }

    lv_ll_clear(&subject->subs_ll);
}

lv_subject_t * lv_subject_get_group_element(lv_subject_t * subject, int32_t index)
{
    if(subject->type != LV_SUBJECT_TYPE_GROUP) {
        LV_LOG_WARN("Subject type is not LV_SUBJECT_TYPE_GROUP");
        return NULL;
    }

    if(index >= (int32_t)subject->size)  return NULL;
    if(index < 0)  return NULL;

    return ((lv_subject_t **)(subject->value.pointer))[index];
}

lv_observer_t * lv_subject_add_observer(lv_subject_t * subject, lv_observer_cb_t cb, void * user_data)
{
    lv_observer_t * observer = lv_subject_add_observer_obj(subject, cb, NULL, user_data);
    if(observer == NULL) return NULL;

    observer->for_obj = 0;
    return observer;
}

lv_observer_t * lv_subject_add_observer_obj(lv_subject_t * subject, lv_observer_cb_t cb, lv_obj_t * obj,
                                            void * user_data)
{
    LV_ASSERT_NULL(subject);
    if(subject->type == LV_SUBJECT_TYPE_INVALID) {
        LV_LOG_WARN("Subject not initialized yet");
        return NULL;
    }
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
    LV_ASSERT_NULL(subject);
    if(subject->type == LV_SUBJECT_TYPE_INVALID) {
        LV_LOG_WARN("Subject not initialized yet");
        return NULL;
    }
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
    LV_ASSERT_NULL(observer);

    if(observer->for_obj && observer->target) {
        lv_obj_remove_event_cb_with_user_data(observer->target, unsubscribe_on_delete_cb, observer);
        lv_obj_remove_event_cb_with_user_data(observer->target, NULL, observer->subject);
    }

    observer->subject->notify_restart_query = 1;

    lv_ll_remove(&(observer->subject->subs_ll), observer);

    if(observer->auto_free_user_data) {
        lv_free(observer->user_data);
    }
    lv_free(observer);
}

void lv_obj_remove_from_subject(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(obj);
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
    LV_ASSERT_NULL(observer);

    return observer->target;
}

void lv_subject_notify(lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);

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

void lv_obj_add_subject_increment_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, int32_t step,
                                        int32_t min, int32_t max)
{
    subject_increment_user_data_t * user_data = lv_malloc(sizeof(subject_increment_user_data_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data in in <lv_obj-subject_increment>");
        return;
    }

    user_data->step = step;
    user_data->min = min;
    user_data->max = max;
    user_data->subject = subject;
    lv_obj_add_event_cb(obj, subject_increment_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, free_user_data_event_cb, LV_EVENT_DELETE, user_data);
}

void lv_obj_add_subject_set_int_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger, int32_t value)
{
    subject_set_int_user_data_t * user_data = lv_malloc(sizeof(subject_set_int_user_data_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data");
        return;
    }

    user_data->subject = subject;
    user_data->value = value;

    lv_obj_add_event_cb(obj, subject_set_int_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, free_user_data_event_cb, LV_EVENT_DELETE, user_data);
}

void lv_obj_add_subject_set_string_event(lv_obj_t * obj, lv_subject_t * subject, lv_event_code_t trigger,
                                         const char * value)
{
    subject_set_string_user_data_t * user_data = lv_malloc(sizeof(subject_set_int_user_data_t));
    if(user_data == NULL) {
        LV_ASSERT_MALLOC(user_data);
        LV_LOG_WARN("Couldn't allocate user_data");
        return;
    }

    user_data->subject = subject;
    user_data->value = lv_strdup(value);
    LV_ASSERT_MALLOC(user_data->value);

    lv_obj_add_event_cb(obj, subject_set_string_cb, trigger, user_data);
    lv_obj_add_event_cb(obj, subject_set_string_free_user_data_event_cb, LV_EVENT_DELETE, user_data);
}

lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector,
                                  lv_subject_t * subject, int32_t ref_value)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_add_style(obj, style, selector);

    bind_style_t * p = lv_malloc(sizeof(bind_style_t));
    LV_ASSERT_MALLOC(p);
    if(p == NULL) {
        LV_LOG_WARN("Out of memory");
        return NULL;
    }

    p->style = style;
    p->selector = selector;
    p->value = ref_value;

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, bind_style_observer_cb, obj, p);
    observable->auto_free_user_data = 1;
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, false, FLAG_COND_EQ);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag,
                                           int32_t ref_value)
{
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, true, FLAG_COND_EQ);
    return observable;
}
lv_observer_t * lv_obj_bind_flag_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, false, FLAG_COND_GT);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, false, FLAG_COND_GE);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    /* a < b == !(a >= b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, true, FLAG_COND_GE);
    return observable;
}

lv_observer_t * lv_obj_bind_flag_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_obj_flag_t flag, int32_t ref_value)
{
    /* a <= b == !(a > b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_flag_observer_cb, flag, ref_value, true, FLAG_COND_GT);
    return observable;

}

lv_observer_t * lv_obj_bind_state_if_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, false,
                                                  FLAG_COND_EQ);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_not_eq(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, true,
                                                  FLAG_COND_EQ);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_gt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{

    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, false,
                                                  FLAG_COND_GT);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_ge(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, false,
                                                  FLAG_COND_GE);
    return observable;
}

lv_observer_t * lv_obj_bind_state_if_lt(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{
    /* a < b == !(a >= b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, true,
                                                  FLAG_COND_GE);
    return observable;

}

lv_observer_t * lv_obj_bind_state_if_le(lv_obj_t * obj, lv_subject_t * subject, lv_state_t state, int32_t ref_value)
{

    /* a <= b == !(a > b) */
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, state, ref_value, true,
                                                  FLAG_COND_GT);
    return observable;
}


lv_observer_t * lv_obj_bind_checked(lv_obj_t * obj, lv_subject_t * subject)
{
    lv_observer_t * observable = bind_to_bitfield(subject, obj, obj_state_observer_cb, LV_STATE_CHECKED, 0, true,
                                                  FLAG_COND_EQ);
    lv_obj_add_event_cb(obj, obj_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);
    return observable;
}

#if LV_USE_LABEL
lv_observer_t * lv_label_bind_text(lv_obj_t * obj, lv_subject_t * subject, const char * fmt)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(fmt == NULL) {
        if(subject->type == LV_SUBJECT_TYPE_INT) {
            fmt = "%d";
        }
#if LV_USE_FLOAT
        else if(subject->type == LV_SUBJECT_TYPE_FLOAT) {
            fmt = "%0.1f";
        }
#endif
        else if(subject->type != LV_SUBJECT_TYPE_STRING && subject->type != LV_SUBJECT_TYPE_POINTER) {
            LV_LOG_WARN("Incompatible subject type: %d", subject->type);
            return NULL;
        }
    }
    else {
        if(subject->type != LV_SUBJECT_TYPE_STRING && subject->type != LV_SUBJECT_TYPE_POINTER &&
           subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
            LV_LOG_WARN("Incompatible subject type: %d", subject->type);
            return NULL;
        }
    }

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, label_text_observer_cb, obj, (void *)fmt);
    return observer;
}
#endif /*LV_USE_LABEL*/

#if LV_USE_ARC
lv_observer_t * lv_arc_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_add_event_cb(obj, arc_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, arc_value_observer_cb, obj, NULL);
    return observer;
}
#endif /*LV_USE_ARC*/

#if LV_USE_SLIDER
lv_observer_t * lv_slider_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_add_event_cb(obj, slider_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, slider_value_observer_cb, obj, NULL);
    return observer;
}
#endif /*LV_USE_SLIDER*/

#if LV_USE_ROLLER
lv_observer_t * lv_roller_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_add_event_cb(obj, roller_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, roller_value_observer_cb, obj, NULL);
    return observer;
}
#endif /*LV_USE_ROLLER*/

#if LV_USE_DROPDOWN
lv_observer_t * lv_dropdown_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_add_event_cb(obj, dropdown_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, dropdown_value_observer_cb, obj, NULL);
    return observer;
}
#endif /*LV_USE_DROPDOWN*/

lv_obj_t * lv_observer_get_target_obj(lv_observer_t * observer)
{
    return (lv_obj_t *)lv_observer_get_target(observer);
}

void * lv_observer_get_user_data(const lv_observer_t * observer)
{
    LV_ASSERT_NULL(observer);

    return observer->user_data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void subject_set_int_cb(lv_event_t * e)
{
    subject_set_int_user_data_t * user_data = lv_event_get_user_data(e);
    lv_subject_set_int(user_data->subject, user_data->value);
}

static void subject_set_string_cb(lv_event_t * e)
{
    subject_set_string_user_data_t * user_data = lv_event_get_user_data(e);
    lv_subject_copy_string(user_data->subject, user_data->value);
}

static void subject_increment_cb(lv_event_t * e)
{
    subject_increment_user_data_t * user_data = lv_event_get_user_data(e);
    int32_t value = lv_subject_get_int(user_data->subject);

    value += user_data->step;
    value = LV_CLAMP(user_data->min, value, user_data->max);

    lv_subject_set_int(user_data->subject, value);
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

static void bind_style_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    bind_style_t * p = observer->user_data;

    int32_t current_v = lv_subject_get_int(subject);
    bool dis = current_v != p->value;
    lv_obj_style_set_disabled(observer->target, p->style, p->selector, dis);
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

    if(res) {
        lv_obj_add_flag(observer->target, p->flag);
    }
    else {
        lv_obj_remove_flag(observer->target, p->flag);
    }
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

#if LV_USE_LABEL

static void label_text_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const char * fmt = observer->user_data;

    if(fmt == NULL) {
        lv_label_set_text(observer->target, subject->value.pointer);
    }
    else {
        switch(subject->type) {
            case LV_SUBJECT_TYPE_INT:
                lv_label_set_text_fmt(observer->target, fmt, subject->value.num);
                break;
#if LV_USE_FLOAT
            case LV_SUBJECT_TYPE_FLOAT:
                lv_label_set_text_fmt(observer->target, fmt, subject->value.float_v);
                break;
#endif
            case LV_SUBJECT_TYPE_STRING:
            case LV_SUBJECT_TYPE_POINTER:
                lv_label_set_text_fmt(observer->target, fmt, subject->value.pointer);
                break;
            default:
                break;
        }
    }
}

#endif /*LV_USE_LABEL*/

#if LV_USE_ARC

static void arc_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_current_target(e);
    lv_subject_t * subject = lv_event_get_user_data(e);

    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_subject_set_int(subject, lv_arc_get_value(arc));
    }
#if LV_USE_FLOAT
    else {
        lv_subject_set_float(subject, (float)lv_arc_get_value(arc));
    }
#endif
}

static void arc_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_arc_set_value(observer->target, subject->value.num);
    }
#if LV_USE_FLOAT
    else {
        lv_arc_set_value(observer->target, (int32_t)subject->value.float_v);
    }
#endif
}

#endif /*LV_USE_ARC*/

#if LV_USE_SLIDER

static void slider_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_current_target(e);
    lv_subject_t * subject = lv_event_get_user_data(e);

    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_subject_set_int(subject, lv_slider_get_value(slider));
    }
#if LV_USE_FLOAT
    else {
        lv_subject_set_float(subject, (float)lv_slider_get_value(slider));
    }
#endif
}

static void slider_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_slider_set_value(observer->target, subject->value.num, LV_ANIM_OFF);
    }
#if LV_USE_FLOAT
    else {
        lv_slider_set_value(observer->target, (int32_t)subject->value.float_v, LV_ANIM_OFF);
    }
#endif
}

#endif /*LV_USE_SLIDER*/

#if LV_USE_ROLLER

static void roller_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * roller = lv_event_get_current_target(e);
    lv_subject_t * subject = lv_event_get_user_data(e);

    lv_subject_set_int(subject, lv_roller_get_selected(roller));
}

static void roller_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if((int32_t)lv_roller_get_selected(observer->target) != subject->value.num) {
        lv_roller_set_selected(observer->target, subject->value.num, LV_ANIM_OFF);
    }
}

#endif /*LV_USE_ROLLER*/

#if LV_USE_DROPDOWN

static void dropdown_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_current_target(e);
    lv_subject_t * subject = lv_event_get_user_data(e);

    lv_subject_set_int(subject, lv_dropdown_get_selected(dropdown));
}

static void dropdown_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_dropdown_set_selected(observer->target, subject->value.num);
}

#endif /*LV_USE_DROPDOWN*/

static void free_user_data_event_cb(lv_event_t * e)
{
    lv_free(lv_event_get_user_data(e));
}

static void subject_set_string_free_user_data_event_cb(lv_event_t * e)
{
    subject_set_string_user_data_t * user_data = lv_event_get_user_data(e);
    lv_free((void *)user_data->value);
    lv_free(user_data);
}


#endif /*LV_USE_OBSERVER*/
