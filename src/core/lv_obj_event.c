/**
 * @file lv_obj_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../indev/lv_indev.h"
#include "../indev/lv_indev_private.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t event_send_core(lv_event_t * e);
static bool event_is_bubbled(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

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

lv_res_t lv_obj_send_event(lv_obj_t * obj, lv_event_code_t event_code, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_event_t e;
    e.current_target = obj;
    e.original_target = obj;
    e.code = event_code;
    e.user_data = NULL;
    e.param = param;
    e.deleted = 0;
    e.stop_bubbling = 0;
    e.stop_processing = 0;

    _lv_event_push(&e);

    /*Send the event*/
    lv_res_t res = event_send_core(&e);

    /*Remove this element from the list*/
    _lv_event_pop(&e);

    return res;
}


lv_res_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e)
{
    const lv_obj_class_t * base;
    if(class_p == NULL) base = ((lv_obj_t *)e->current_target)->class_p;
    else base = class_p->base_class;

    /*Find a base in which call the ancestor's event handler_cb if set*/
    while(base && base->event_cb == NULL) base = base->base_class;

    if(base == NULL) return LV_RES_OK;
    if(base->event_cb == NULL) return LV_RES_OK;

    /*Call the actual event callback*/
    e->user_data = NULL;
    base->event_cb(base, e);

    lv_res_t res = LV_RES_OK;
    /*Stop if the object is deleted*/
    if(e->deleted) res = LV_RES_INV;

    return res;
}

void lv_obj_add_event(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter,
                      void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_allocate_spec_attr(obj);

    lv_event_add(&obj->spec_attr->event_list, event_cb, filter, user_data);
}

uint32_t lv_obj_get_event_count(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(obj->spec_attr == NULL) return 0;
    return lv_event_get_count(&obj->spec_attr->event_list);
}


lv_event_dsc_t * lv_obj_get_event_dsc(lv_obj_t * obj, uint32_t index)
{
    LV_ASSERT_NULL(obj);
    if(obj->spec_attr == NULL) return NULL;
    return lv_event_get_dsc(&obj->spec_attr->event_list, index);
}

bool lv_obj_remove_event(lv_obj_t * obj, uint32_t index)
{
    LV_ASSERT_NULL(obj);
    if(obj->spec_attr == NULL) return false;
    return lv_event_remove(&obj->spec_attr->event_list, index);
}

lv_obj_t * lv_event_get_current_target_obj(lv_event_t * e)
{
    return lv_event_get_current_target(e);
}

lv_obj_t * lv_event_get_target_obj(lv_event_t * e)
{
    return lv_event_get_target(e);
}


lv_indev_t * lv_event_get_indev(lv_event_t * e)
{

    if(e->code == LV_EVENT_PRESSED ||
       e->code == LV_EVENT_PRESSING ||
       e->code == LV_EVENT_PRESS_LOST ||
       e->code == LV_EVENT_SHORT_CLICKED ||
       e->code == LV_EVENT_LONG_PRESSED ||
       e->code == LV_EVENT_LONG_PRESSED_REPEAT ||
       e->code == LV_EVENT_CLICKED ||
       e->code == LV_EVENT_RELEASED ||
       e->code == LV_EVENT_SCROLL_BEGIN ||
       e->code == LV_EVENT_SCROLL_END ||
       e->code == LV_EVENT_SCROLL ||
       e->code == LV_EVENT_GESTURE ||
       e->code == LV_EVENT_KEY ||
       e->code == LV_EVENT_FOCUSED ||
       e->code == LV_EVENT_DEFOCUSED ||
       e->code == LV_EVENT_LEAVE) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

lv_layer_t * lv_event_get_layer(lv_event_t * e)
{
    if(e->code == LV_EVENT_DRAW_MAIN ||
       e->code == LV_EVENT_DRAW_MAIN_BEGIN ||
       e->code == LV_EVENT_DRAW_MAIN_END ||
       e->code == LV_EVENT_DRAW_POST ||
       e->code == LV_EVENT_DRAW_POST_BEGIN ||
       e->code == LV_EVENT_DRAW_POST_END) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

const lv_area_t * lv_event_get_old_size(lv_event_t * e)
{
    if(e->code == LV_EVENT_SIZE_CHANGED) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

uint32_t lv_event_get_key(lv_event_t * e)
{
    if(e->code == LV_EVENT_KEY) {
        uint32_t * k = lv_event_get_param(e);
        if(k) return *k;
        else return 0;
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return 0;
    }
}

lv_anim_t * lv_event_get_scroll_anim(lv_event_t * e)
{
    if(e->code == LV_EVENT_SCROLL_BEGIN) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

void lv_event_set_ext_draw_size(lv_event_t * e, lv_coord_t size)
{
    if(e->code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * cur_size = lv_event_get_param(e);
        *cur_size = LV_MAX(*cur_size, size);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
    }
}

lv_point_t * lv_event_get_self_size_info(lv_event_t * e)
{
    if(e->code == LV_EVENT_GET_SELF_SIZE) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return 0;
    }
}

lv_hit_test_info_t * lv_event_get_hit_test_info(lv_event_t * e)
{
    if(e->code == LV_EVENT_HIT_TEST) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return 0;
    }
}

const lv_area_t * lv_event_get_cover_area(lv_event_t * e)
{
    if(e->code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * p = lv_event_get_param(e);
        return p->area;
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

void lv_event_set_cover_res(lv_event_t * e, lv_cover_res_t res)
{
    if(e->code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * p = lv_event_get_param(e);
        if(res > p->res) p->res = res;  /*Save only "stronger" results*/
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
    }
}

lv_draw_task_t * lv_event_get_draw_task(lv_event_t * e)
{
    if(e->code == LV_EVENT_DRAW_TASK_ADDED) {
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

static lv_res_t event_send_core(lv_event_t * e)
{
    EVENT_TRACE("Sending event %d to %p with %p param", e->code, (void *)e->original_target, e->param);

    /*Call the input device's feedback callback if set*/
    lv_indev_t * indev_act = lv_indev_get_act();
    if(indev_act) {
        if(indev_act->feedback_cb) indev_act->feedback_cb(indev_act, e->code);
        if(e->stop_processing) return LV_RES_OK;
        if(e->deleted) return LV_RES_INV;
    }

    lv_obj_t * target = e->current_target;
    lv_res_t res = LV_RES_OK;
    lv_event_list_t * list = target->spec_attr ?  &target->spec_attr->event_list : NULL;

    res = lv_event_send(list, e, true);
    if(res != LV_RES_OK) return res;

    res = lv_obj_event_base(NULL, e);
    if(res != LV_RES_OK) return res;

    res = lv_event_send(list, e, false);
    if(res != LV_RES_OK) return res;

    lv_obj_t * parent = lv_obj_get_parent(e->current_target);
    if(parent && event_is_bubbled(e)) {
        e->current_target = parent;
        res = event_send_core(e);
        if(res != LV_RES_OK) return res;
    }

    return res;
}


static bool event_is_bubbled(lv_event_t * e)
{
    if(e->stop_bubbling) return false;

    /*Event codes that always bubble*/
    switch(e->code) {
        case LV_EVENT_CHILD_CREATED:
        case LV_EVENT_CHILD_DELETED:
            return true;
        default:
            break;
    }

    /*Check other codes only if bubbling is enabled*/
    if(lv_obj_has_flag(e->current_target, LV_OBJ_FLAG_EVENT_BUBBLE) == false) return false;

    switch(e->code) {
        case LV_EVENT_HIT_TEST:
        case LV_EVENT_COVER_CHECK:
        case LV_EVENT_REFR_EXT_DRAW_SIZE:
        case LV_EVENT_DRAW_MAIN_BEGIN:
        case LV_EVENT_DRAW_MAIN:
        case LV_EVENT_DRAW_MAIN_END:
        case LV_EVENT_DRAW_POST_BEGIN:
        case LV_EVENT_DRAW_POST:
        case LV_EVENT_DRAW_POST_END:
        case LV_EVENT_DRAW_TASK_ADDED:
        case LV_EVENT_REFRESH:
        case LV_EVENT_DELETE:
        case LV_EVENT_CHILD_CREATED:
        case LV_EVENT_CHILD_DELETED:
        case LV_EVENT_CHILD_CHANGED:
        case LV_EVENT_SIZE_CHANGED:
        case LV_EVENT_STYLE_CHANGED:
        case LV_EVENT_GET_SELF_SIZE:
            return false;
        default:
            return true;
    }
}
