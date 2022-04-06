/**
 * @file lv_msg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_msg.h"
#if LV_USE_MSG

#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t msg_id;
    lv_msg_subscribe_cb_t callback;
    void * user_data;
    void * _priv_data;      /*Internal: used only store 'obj' in lv_obj_subscribe*/
} sub_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void notify(lv_msg_t * m);
static void obj_notify_cb(void * s, lv_msg_t * m);
static void obj_delete_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_ll_t subs_ll;

/**********************
 *  GLOBAL VARIABLES
 **********************/
lv_event_code_t LV_EVENT_MSG_RECEIVED;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_msg_init(void)
{
    LV_EVENT_MSG_RECEIVED = lv_event_register_id();
    _lv_ll_init(&subs_ll, sizeof(sub_dsc_t));
}

void * lv_msg_subsribe(uint32_t msg_id, lv_msg_subscribe_cb_t cb, void * user_data)
{
    sub_dsc_t * s = _lv_ll_ins_tail(&subs_ll);
    LV_ASSERT_MALLOC(s);
    if(s == NULL) return NULL;

    lv_memset_00(s, sizeof(*s));

    s->msg_id = msg_id;
    s->callback = cb;
    s->user_data = user_data;
    return s;
}

void * lv_msg_subsribe_obj(uint32_t msg_id, lv_obj_t * obj, void * user_data)
{
    sub_dsc_t * s = lv_msg_subsribe(msg_id, obj_notify_cb, user_data);
    if(s == NULL) return NULL;
    s->_priv_data = obj;

    /*If not added yet, add a delete event cb which automatically unsubcribes the object*/
    sub_dsc_t * s_first = lv_obj_get_event_user_data(obj, obj_delete_event_cb);
    if(s_first == NULL) {
        lv_obj_add_event_cb(obj, obj_delete_event_cb, LV_EVENT_DELETE, s);
    }
    return s;
}

void lv_msg_unsubscribe(void * s)
{
    LV_ASSERT_NULL(s);
    _lv_ll_remove(&subs_ll, s);
    lv_mem_free(s);
}

void lv_msg_send(uint32_t msg_id, const void * payload)
{
    lv_msg_t m;
    lv_memset_00(&m, sizeof(m));
    m.id = msg_id;
    m.payload = payload;
    notify(&m);
}

uint32_t lv_msg_get_id(lv_msg_t * m)
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
    sub_dsc_t * s;
    _LV_LL_READ(&subs_ll, s) {
        if(s->msg_id == m->id && s->callback) {
            m->user_data = s->user_data;
            m->_priv_data = s->_priv_data;
            s->callback(s, m);
        }
    }
}

static void obj_notify_cb(void * s, lv_msg_t * m)
{
    LV_UNUSED(s);
    lv_event_send(m->_priv_data, LV_EVENT_MSG_RECEIVED, m);
}

static void obj_delete_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    sub_dsc_t * s = _lv_ll_get_head(&subs_ll);
    sub_dsc_t * s_next;
    while(s) {
        /*On unsubscribe the list changes s becomes invalid so get next item while it's surely valid*/
        s_next = _lv_ll_get_next(&subs_ll, s);
        if(s->_priv_data == obj) {
            lv_msg_unsubscribe(s);
        }
        s = s_next;
    }
}

#endif /*LV_USE_MSG*/
