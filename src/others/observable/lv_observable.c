/**
 * @file lv_observable.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_observable.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void notify(lv_subject_t* subject);
static void unsubscribe_on_delete_cb(lv_event_t* e);
static void int_set_obj_local_style_prop_cb(void* s);
static void string_set_label_text_cb(void* s);
static void int_set_label_text_cb(void* s);
static void formatted_int_set_format_cb(void* s);

static void set_obj_flag_to(lv_obj_t* obj, lv_obj_flag_t flag, bool cond);
static void set_obj_state_to(lv_obj_t* obj, lv_state_t flag, bool cond);
static void bitmask_set_obj_flag_cb(void* s);
static void bitmask_set_obj_state_cb(void* s);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_subject_update(lv_subject_t* subject, lv_observer_t* obsvr)
{
    if (obsvr->cb) {
        obsvr->cb(obsvr);
    }
}

void lv_subject_init_int(lv_subject_t* subject, int32_t value)
{
    subject->type = LV_SUBJECT_TYPE_INT;
    _lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
    subject->value.num = value;
}

void lv_subject_init_string(lv_subject_t* subject, char* buf, size_t size)
{
    subject->type = LV_SUBJECT_TYPE_STRING + size;
    _lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
    subject->value.ptr = buf;
}

void lv_subject_init_ptr(lv_subject_t* subject, void* value)
{
    subject->type = LV_SUBJECT_TYPE_POINTER;
    _lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
    subject->value.ptr = value;
}

void lv_subject_init_color(lv_subject_t* subject, lv_color_t color)
{
    subject->type = LV_SUBJECT_TYPE_COLOR;
    _lv_ll_init(&(subject->subs_ll), sizeof(lv_observer_t));
    subject->value.color = color;
}

void lv_subject_unsubscribe(lv_observer_t* s)
{
    LV_ASSERT_NULL(s);
    _lv_ll_remove(&(s->subject->subs_ll), s);
    lv_mem_free(s);
}

void lv_subject_unsubscribe_all(lv_subject_t* subject)
{
    LV_ASSERT_NULL(subject);
    lv_observer_t* s;
    _LV_LL_READ(&(subject->subs_ll), s) {
        lv_subject_unsubscribe(s);
    }
}

void lv_subject_unsubscribe_obj(lv_subject_t* subject, lv_obj_t* obj)
{
    lv_observer_t* s = _lv_ll_get_head(&subject->subs_ll);
    while (s) {
        lv_observer_t* s_next = _lv_ll_get_next(&subject->subs_ll, s);
        if (s->obj == obj) {
            lv_subject_unsubscribe(s);
        }
        s = s_next;
    }
}

void lv_subject_set_int(lv_subject_t* subject, int32_t value)
{
    subject->value.num = value;
    notify(subject);
}

void lv_subject_copy_string(lv_subject_t* subject, char* buf)
{
    size_t maxlen = subject->type - LV_SUBJECT_TYPE_STRING;
    if (maxlen < 1)
        return;
    strncpy((char*)subject->value.ptr, buf, maxlen - 1);
    ((char*)subject->value.ptr)[maxlen - 1] = 0;    /* make sure it is terminated properly */
    notify(subject);
}

void lv_subject_set_ptr(lv_subject_t* subject, void* ptr)
{
    subject->value.ptr = ptr;
    notify(subject);
}

void lv_subject_set_color(lv_subject_t* subject, lv_color_t color)
{
    subject->value.color = color;
    notify(subject);
}

void* lv_subject_subscribe_obj(lv_subject_t* subject, lv_observer_cb_t cb, lv_obj_t* obj, void* data1, void* data2)
{
    lv_observer_t* s = _lv_ll_ins_tail(&(subject->subs_ll));
    LV_ASSERT_MALLOC(s);
    if (s == NULL) return NULL;

    lv_memset_00(s, sizeof(*s));

    s->subject = subject;
    s->obj = obj;
    s->cb = cb;
    s->data1 = data1;
    s->data2 = data2;

    /* subscribe to delete event of the object */
    if (obj != NULL) {
        lv_obj_add_event_cb(obj, unsubscribe_on_delete_cb, LV_EVENT_DELETE, s);
    }

    /* update object immediately */
    lv_subject_update(subject, s);

    return s;
}

void* lv_bind_int_to_callback(lv_subject_t* subject, lv_observer_cb_t cb, lv_obj_t* obj, void* data1, void* data2)
{
    return lv_subject_subscribe_obj(subject, cb, obj, data1, data2);
}

void* lv_bind_int_to_obj_local_style_prop(lv_subject_t* subject, lv_obj_t* obj, lv_style_prop_t prop, lv_style_selector_t selector)
{
    return lv_subject_subscribe_obj(subject, int_set_obj_local_style_prop_cb, obj, (void*)prop, (void*)selector);
}

void* lv_bind_string_to_label_text(lv_subject_t* subject, lv_obj_t* obj)
{
    return lv_subject_subscribe_obj(subject, string_set_label_text_cb, obj, NULL, NULL);
}

void lv_observer_int_set_label_text_format(lv_observer_t* obsvr, const char* fmt)
{
    obsvr->data1 = (void*)fmt;
    notify(obsvr->subject);
}

void* lv_bind_int_to_label_text_fmt(lv_subject_t* subject, lv_obj_t* obj, const char* fmt)
{
    return lv_subject_subscribe_obj(subject, int_set_label_text_cb, obj, fmt, NULL);
}

void* lv_bind_formatted_int_to_label_text(lv_subject_t* subject, lv_subject_t* fmt, lv_obj_t* obj)
{
    /* 'fmt' should be an observable string */
    lv_observer_t* int_obsvr = lv_bind_int_to_label_text_fmt(subject, obj, fmt->value.ptr);
    /* bind the observable format string to the same object, and pass the int observer to the binding, so that when the format changes, the int observer would be notified as well */
    lv_subject_subscribe_obj(fmt, formatted_int_set_format_cb, obj, int_obsvr, NULL);
    return int_obsvr;
}

void lv_observer_bitmask_set_mask(lv_observer_t* obsvr, lv_bitmask_pattern_t pattern)
{
    obsvr->data2 = (void*)pattern;
    notify(obsvr->subject);
}

void* lv_bind_bitmask_to_obj_flag(lv_subject_t* subject, lv_obj_t* obj, lv_obj_flag_t flag, lv_bitmask_pattern_t pattern)
{
    return lv_subject_subscribe_obj(subject, bitmask_set_obj_flag_cb, obj, (void*)flag, (void*)pattern);
}

void* lv_bind_bitmask_to_obj_state(lv_subject_t* subject, lv_obj_t* obj, lv_state_t flag, lv_bitmask_pattern_t pattern)
{
    return lv_subject_subscribe_obj(subject, bitmask_set_obj_state_cb, obj, (void*)flag, (void*)pattern);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void notify(lv_subject_t* subject)
{
    lv_observer_t* s;
    _LV_LL_READ(&(subject->subs_ll), s) {
        lv_subject_update(subject, s);
    }
}

static void unsubscribe_on_delete_cb(lv_event_t* e)
{
    lv_observer_t* s = lv_event_get_user_data(e);
    lv_subject_unsubscribe(s);
}

void int_set_obj_local_style_prop_cb(void* s)
{
    lv_observer_t* obsvr = (lv_observer_t*)s;
    lv_obj_set_local_style_prop(obsvr->obj, (lv_style_prop_t)obsvr->data1, *(lv_style_value_t*)&obsvr->subject->value, (lv_style_selector_t)obsvr->data2);
}

static void string_set_label_text_cb(void* s)
{
    lv_observer_t* obsvr = (lv_observer_t*)s;
    lv_label_set_text(obsvr->obj, obsvr->subject->value.ptr);
}

static void int_set_label_text_cb(void* s)
{
    lv_observer_t* obsvr = (lv_observer_t*)s;
    lv_label_set_text_fmt(obsvr->obj, (const char*)obsvr->data1, obsvr->subject->value.num);
}

static void formatted_int_set_format_cb(void* s)
{
    lv_observer_t* obsvr = (lv_observer_t*)s;
    lv_observer_int_set_label_text_format((lv_observer_t*)obsvr->data1, (const char*)obsvr->subject->value.ptr);
}

static void set_obj_flag_to(lv_obj_t* obj, lv_obj_flag_t flag, bool cond)
{
    if (cond)
        lv_obj_add_flag(obj, flag);
    else
        lv_obj_clear_flag(obj, flag);
}

static void set_obj_state_to(lv_obj_t* obj, lv_state_t flag, bool cond)
{
    if (cond)
        lv_obj_add_state(obj, flag);
    else
        lv_obj_clear_state(obj, flag);
}

static void bitmask_set_obj_flag_cb(void* s)
{
    lv_observer_t* obsvr = (lv_observer_t*)s;
    uint32_t mask = (uint32_t)obsvr->data2 & 0x3fffffff; /* mask: bits 0..29 */
    uint32_t value = (((uint32_t)obsvr->data2) & LV_OBSERVER_BITMASK_ALL_SET) ? mask : 0UL;
    bool cond = (obsvr->subject->value.num & mask) == value;
    set_obj_flag_to(obsvr->obj, (lv_obj_flag_t)obsvr->data1, (((uint32_t)obsvr->data2) & LV_OBSERVER_BITMASK_NEGATE) ? !cond : cond);
}

static void bitmask_set_obj_state_cb(void* s)
{
    lv_observer_t* obsvr = (lv_observer_t*)s;
    uint32_t mask = (uint32_t)obsvr->data2 & 0x3fffffff; /* mask: bits 0..29 */
    uint32_t value = (((uint32_t)obsvr->data2) & LV_OBSERVER_BITMASK_ALL_SET) ? mask : 0UL;
    bool cond = (obsvr->subject->value.num & mask) == value;
    set_obj_state_to(obsvr->obj, (lv_state_t)obsvr->data1, (((uint32_t)obsvr->data2) & LV_OBSERVER_BITMASK_NEGATE) ? !cond : cond);
}
