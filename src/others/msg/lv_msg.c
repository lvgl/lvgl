/**
 * @file lv_msg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_msg.h"
#if LV_USE_MSG

#include "../../misc/lv_assert.h"
#include "../../misc/lv_ll.h"
#include "../../misc/lv_gc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_msg_id_t msg_id;
    lv_msg_subscribe_cb_t callback;
    void * user_data;
    void * _priv_data;      /*Internal: used only store 'obj' in lv_obj_subscribe*/
    uint8_t _checked : 1;   /*Internal: used to prevent multiple notifications*/
} sub_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void notify(lv_msg_t * m);
static void obj_notify_cb(lv_msg_t * m);
static void obj_delete_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool restart_notify;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_MSG
    #define MSG_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define MSG_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_msg_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_subs_ll), sizeof(sub_dsc_t));
}

void * lv_msg_subscribe(lv_msg_id_t msg_id, lv_msg_subscribe_cb_t cb, void * user_data)
{
    sub_dsc_t * s = _lv_ll_ins_tail(&LV_GC_ROOT(_subs_ll));
    LV_ASSERT_MALLOC(s);
    if(s == NULL) return NULL;

    lv_memzero(s, sizeof(*s));

    s->msg_id = msg_id;
    s->callback = cb;
    s->user_data = user_data;
    s->_checked = 0; /*if subsribed during `notify`, it should be notified immediately*/
    restart_notify = true;
    return s;
}

void * lv_msg_subscribe_obj(lv_msg_id_t msg_id, lv_obj_t * obj, void * user_data)
{
    sub_dsc_t * s = lv_msg_subscribe(msg_id, obj_notify_cb, user_data);
    if(s == NULL) return NULL;
    s->_priv_data = obj;

    /*If not added yet, add a delete_event_cb which automatically unsubcribes the object when its deleted*/
    uint32_t i;
    uint32_t event_cnt = lv_obj_get_event_count(obj);
    sub_dsc_t * s_first = NULL;
    for(i = 0; i < event_cnt; i++) {
        lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(obj, i);
        if(lv_event_dsc_get_cb(event_dsc) == obj_delete_event_cb) {
            s_first = lv_event_dsc_get_user_data(event_dsc);
            break;
        }
    }

    if(s_first == NULL) {
        lv_obj_add_event(obj, obj_delete_event_cb, LV_EVENT_DELETE, s);
    }

    return s;
}

void lv_msg_unsubscribe(void * s)
{
    LV_ASSERT_NULL(s);
    _lv_ll_remove(&LV_GC_ROOT(_subs_ll), s);
    restart_notify = true;
    lv_free(s);
}

void lv_msg_send(lv_msg_id_t msg_id, const void * payload)
{
    lv_msg_t m;
    lv_memzero(&m, sizeof(m));
    m.id = msg_id;
    m.payload = payload;
    notify(&m);
}

void lv_msg_update_value(void * v)
{
    lv_msg_send((lv_msg_id_t)v, v);
}

lv_msg_id_t lv_msg_get_id(lv_msg_t * m)
{
    return m->id;
}

const void * lv_msg_get_payload(lv_msg_t * m)
{
    return m->payload;
}

void * lv_msg_get_user_data(lv_msg_t * m)
{
    return m->user_data;
}

lv_msg_t * lv_event_get_msg(lv_event_t * e)
{
    if(e->code == LV_EVENT_MSG_RECEIVED) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void notify(lv_msg_t * m)
{
    static unsigned int _recursion_counter = 0;
    _recursion_counter++;

    /*First clear all _checked flags*/
    sub_dsc_t * s;
    if(_recursion_counter == 1) {
        _LV_LL_READ(&LV_GC_ROOT(_subs_ll), s) {
            s->_checked = 0;
        }
    }

    /*Run all sub_dsc_t from the list*/
    do {
        restart_notify = false;
        s = _lv_ll_get_head(&LV_GC_ROOT(_subs_ll));
        while(s) {
            /*get next element while current is surely valid*/
            sub_dsc_t * next = _lv_ll_get_next(&LV_GC_ROOT(_subs_ll), s);

            /*Notify only once*/
            if(!s->_checked) {
                /*Check if this sub_dsc_t is about this msg_id*/
                if(s->msg_id == m->id && s->callback) {
                    /* Set this flag and notify*/
                    s->_checked = 1;
                    m->user_data = s->user_data;
                    m->_priv_data = s->_priv_data;
                    s->callback(m);
                }
            }

            /*restart or load next*/
            if(restart_notify) {
                MSG_TRACE("Start from the first sub_dsc_t again because _subs_ll may have changed");
                break;
            }
            s = next;
        }
    } while(s);

    _recursion_counter--;
    restart_notify = (_recursion_counter > 0);
}

static void obj_notify_cb(lv_msg_t * m)
{
    lv_obj_send_event(m->_priv_data, LV_EVENT_MSG_RECEIVED, m);
}

static void obj_delete_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    sub_dsc_t * s = _lv_ll_get_head(&LV_GC_ROOT(_subs_ll));
    sub_dsc_t * s_next;
    while(s) {
        /*On unsubscribe the list changes s becomes invalid so get next item while it's surely valid*/
        s_next = _lv_ll_get_next(&LV_GC_ROOT(_subs_ll), s);
        if(s->_priv_data == obj) {
            lv_msg_unsubscribe(s);
        }
        s = s_next;
    }
}

#endif /*LV_USE_MSG*/
