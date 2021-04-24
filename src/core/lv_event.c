/**
 * @file lv_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_indev.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_event_temp_data {
    lv_obj_t * obj;
    bool deleted;
    struct _lv_event_temp_data * prev;
} lv_event_temp_data_t;

typedef struct _lv_event_dsc_t{
    lv_event_cb_t cb;
    void * user_data;
    lv_event_code_t filter :8;
}lv_event_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_event_dsc_t * lv_obj_get_event_dsc(const lv_obj_t * obj, uint32_t id);
static lv_res_t event_send_core(lv_obj_t * obj, lv_event_code_t event_code, void * param);
static bool event_is_bubbled(lv_event_code_t e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_event_temp_data_t * event_temp_data_head;
static void * event_act_param;
static void * event_act_user_data_cb;
static lv_obj_t * event_original_target;

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_EVENT
#  define EVENT_TRACE(...) LV_LOG_TRACE( __VA_ARGS__)
#else
#  define EVENT_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_event_send(lv_obj_t * obj, lv_event_code_t event, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Save the original target first in tmp variable because nested `lv_event_send` calls can overwrite it*/
    lv_obj_t * event_original_target_tmp = event_original_target;
    event_original_target = obj;

    /*Send the event*/
    lv_res_t res = event_send_core(obj, event, param);

    /*Restore the original target*/
    event_original_target = event_original_target_tmp;

    return res;
}


lv_res_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e)
{
    const lv_obj_class_t * base;
    if(class_p == NULL) base = e->target->class_p;
    else base = class_p->base_class;

    /*Find a base in which Call the ancestor's event handler_cb is set*/
    while(base && base->event_cb == NULL) base = base->base_class;

    if(base == NULL) return LV_RES_OK;
    if(base->event_cb == NULL) return LV_RES_OK;

    /* Build a simple linked list from the objects used in the events
     * It's important to know if an this object was deleted by a nested event
     * called from this `event_cb`. */
    lv_event_temp_data_t event_temp_data;
    event_temp_data.obj     = e->target;
    event_temp_data.deleted = false;
    event_temp_data.prev    = NULL;

    if(event_temp_data_head) {
        event_temp_data.prev = event_temp_data_head;
    }
    event_temp_data_head = &event_temp_data;

    /*Call the actual event callback*/
    e->user_data = NULL;
    base->event_cb(base, e);

    lv_res_t res = LV_RES_OK;
    /*Stop if the object is deleted*/
    if(event_temp_data.deleted) res = LV_RES_INV;

    /*Remove this element from the list*/
    event_temp_data_head = event_temp_data_head->prev;

    return res;
}


lv_obj_t * lv_event_get_target(lv_event_t * e)
{
    return e->target;
}

lv_event_code_t lv_event_get_code(lv_event_t * e)
{
    return e->code;
}

void * lv_event_get_param(lv_event_t * e)
{
    return e->param;
}

void * lv_event_get_user_data(lv_event_t * e)
{
    return e->user_data;
}

lv_obj_t * lv_event_get_original_target(void)
{
    return event_original_target;
}

uint32_t lv_event_register_id(void)
{
    static uint32_t last_id = _LV_EVENT_LAST;
    last_id ++;
    return last_id;
}

void _lv_event_mark_deleted(lv_obj_t * obj)
{
    lv_event_temp_data_t * t = event_temp_data_head;

    while(t) {
        if(t->obj == obj) t->deleted = true;
        t = t->prev;
    }
}


struct _lv_event_dsc_t * lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_allocate_spec_attr(obj);

    obj->spec_attr->event_dsc_cnt++;
    obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
    LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);

    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].cb = event_cb;
    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].filter = filter;
    obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1].user_data = user_data;

    return &obj->spec_attr->event_dsc[obj->spec_attr->event_dsc_cnt - 1];
}

bool lv_obj_remove_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(obj->spec_attr == NULL) return false;

    int32_t i = 0;
    for(i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if(obj->spec_attr->event_dsc[i].cb == event_cb) {
            /*Shift the remaining event handlers forward*/
            for(; i < (obj->spec_attr->event_dsc_cnt-1); i++) {
                obj->spec_attr->event_dsc[i].cb = obj->spec_attr->event_dsc[i+1].cb;
                obj->spec_attr->event_dsc[i].user_data = obj->spec_attr->event_dsc[i+1].user_data;
            }
            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);
            return true;
        }
    }

    /*No event handler found*/
    return false;
}

bool lv_obj_remove_event_dsc(lv_obj_t * obj, struct _lv_event_dsc_t * event_dsc)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(obj->spec_attr == NULL) return false;

    int32_t i = 0;
    for(i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if(&obj->spec_attr->event_dsc[i] == event_dsc) {
            /*Shift the remaining event handlers forward*/
            for(; i < (obj->spec_attr->event_dsc_cnt-1); i++) {
                obj->spec_attr->event_dsc[i].cb = obj->spec_attr->event_dsc[i+1].cb;
                obj->spec_attr->event_dsc[i].user_data = obj->spec_attr->event_dsc[i+1].user_data;
            }
            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc, obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);
            return true;
        }
    }

    /*No event handler found*/
    return false;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_event_dsc_t * lv_obj_get_event_dsc(const lv_obj_t * obj, uint32_t id)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(!obj->spec_attr) return NULL;
    if(id >= obj->spec_attr->event_dsc_cnt) return NULL;

    return &obj->spec_attr->event_dsc[id];
}

static lv_res_t event_send_core(lv_obj_t * obj, lv_event_code_t event_code, void * param)
{
    EVENT_TRACE("Sending event %d to %p with %p param", event_code, obj, param);

    /*Build a simple linked list from the objects used in the events
     *It's important to know if an this object was deleted by a nested event
     *called from this `event_cb`.*/
    lv_event_temp_data_t event_temp_data;
    event_temp_data.obj     = obj;
    event_temp_data.deleted = false;
    event_temp_data.prev    = NULL;

    if(event_temp_data_head) {
        event_temp_data.prev = event_temp_data_head;
    }
    event_temp_data_head = &event_temp_data;

    /*There could be nested event sending with different param.
     *It needs to be saved for the current event context because `lv_event_get_data` returns a global param.*/
    void * event_act_param_save = event_act_param;
    event_act_param = param;

    /*Call the input device's feedback callback if set*/
    lv_indev_t * indev_act = lv_indev_get_act();
    if(indev_act) {
        if(indev_act->driver->feedback_cb) indev_act->driver->feedback_cb(indev_act->driver, event_code);
    }

    lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(obj, 0);
    lv_res_t res = LV_RES_OK;
    lv_event_t e;
    e.code = event_code;
    e.target = obj;
    e.original_target = obj;
    e.param = param;
    res = lv_obj_event_base(NULL, &e);

    uint32_t i = 0;
    while(event_dsc && res == LV_RES_OK) {
        if(event_dsc->cb && (event_dsc->filter == LV_EVENT_ALL || event_dsc->filter == event_code)) {
            void * event_act_user_data_cb_save = event_act_user_data_cb;
            event_act_user_data_cb = event_dsc->user_data;

            e.user_data = event_dsc->user_data;
            event_dsc->cb(&e);

            event_act_user_data_cb = event_act_user_data_cb_save;

            /*Stop if the object is deleted*/
            if(event_temp_data.deleted) {
                res = LV_RES_INV;
                break;
            }
        }

        i++;
        event_dsc = lv_obj_get_event_dsc(obj, i);
    }

    /*Restore the event_code param*/
    event_act_param = event_act_param_save;

    /*Remove this element from the list*/
    event_temp_data_head = event_temp_data_head->prev;

    if(res == LV_RES_OK && event_is_bubbled(event_code)) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE) && obj->parent) {
            res = event_send_core(obj->parent, event_code, param);
            if(res != LV_RES_OK) return LV_RES_INV;
        }
    }

    return res;
}

static bool event_is_bubbled(lv_event_code_t e)
{
    switch(e) {
    case LV_EVENT_HIT_TEST:
    case LV_EVENT_COVER_CHECK:
    case LV_EVENT_REFR_EXT_DRAW_SIZE:
    case LV_EVENT_DRAW_MAIN_BEGIN:
    case LV_EVENT_DRAW_MAIN:
    case LV_EVENT_DRAW_MAIN_END:
    case LV_EVENT_DRAW_POST_BEGIN:
    case LV_EVENT_DRAW_POST:
    case LV_EVENT_DRAW_POST_END:
    case LV_EVENT_DRAW_PART_BEGIN:
    case LV_EVENT_DRAW_PART_END:
    case LV_EVENT_REFRESH:
    case LV_EVENT_DELETE:
    case LV_EVENT_CHILD_CHANGED:
    case LV_EVENT_SIZE_CHANGED:
    case LV_EVENT_STYLE_CHANGED:
    case LV_EVENT_GET_SELF_SIZE:
        return false;
    default:
        return true;
    }
}
