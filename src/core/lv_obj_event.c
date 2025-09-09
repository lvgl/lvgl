/**
 * @file lv_obj_event.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_event_private.h"
#include "lv_obj_event_private.h"
#include "lv_obj_class_private.h"
#include "lv_obj_private.h"
#include "../indev/lv_indev.h"
#include "../indev/lv_indev_private.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t event_send_core(lv_event_t * e);
static bool event_is_bubbled(lv_event_t * e);
static bool event_is_trickled(lv_event_t * e);

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

lv_result_t lv_obj_send_event(lv_obj_t * obj, lv_event_code_t event_code, void * param)
{
    if(obj == NULL) return LV_RESULT_OK;

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
    e.stop_trickling = 0;

    lv_event_push(&e);

    /*Send the event*/
    lv_result_t res = event_send_core(&e);

    /*Remove this element from the list*/
    lv_event_pop(&e);

    return res;
}

lv_result_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e)
{
    const lv_obj_class_t * base;
    if(class_p == NULL) base = ((lv_obj_t *)e->current_target)->class_p;
    else base = class_p->base_class;

    /*Find a base in which call the ancestor's event handler_cb if set*/
    while(base && base->event_cb == NULL) base = base->base_class;

    if(base == NULL) return LV_RESULT_OK;
    if(base->event_cb == NULL) return LV_RESULT_OK;

    /*Call the actual event callback*/
    e->user_data = NULL;
    LV_PROFILER_EVENT_BEGIN_TAG(lv_event_code_get_name(e->code));
    base->event_cb(base, e);
    LV_PROFILER_EVENT_END_TAG(lv_event_code_get_name(e->code));

    lv_result_t res = LV_RESULT_OK;
    /*Stop if the object is deleted*/
    if(e->deleted) res = LV_RESULT_INVALID;

    return res;
}

lv_event_dsc_t * lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_allocate_spec_attr(obj);

    return lv_event_add(&obj->spec_attr->event_list, event_cb, filter, user_data);
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

bool lv_obj_remove_event_dsc(lv_obj_t * obj, lv_event_dsc_t * dsc)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(dsc);
    if(obj->spec_attr == NULL) return false;
    return lv_event_remove_dsc(&obj->spec_attr->event_list, dsc);
}

uint32_t lv_obj_remove_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb)
{
    LV_ASSERT_NULL(obj);

    uint32_t event_cnt = lv_obj_get_event_count(obj);
    uint32_t removed_count = 0;
    int32_t i;

    if(event_cnt == 0) return 0;

    for(i = event_cnt - 1; i >= 0; i--) {
        lv_event_dsc_t * dsc = lv_obj_get_event_dsc(obj, i);
        if(dsc && dsc->cb == event_cb) {
            lv_obj_remove_event(obj, i);
            removed_count++;
        }
    }

    return removed_count;
}

uint32_t lv_obj_remove_event_cb_with_user_data(lv_obj_t * obj, lv_event_cb_t event_cb, void * user_data)
{
    LV_ASSERT_NULL(obj);

    uint32_t event_cnt = lv_obj_get_event_count(obj);
    uint32_t removed_count = 0;
    int32_t i;

    if(event_cnt == 0) return 0;

    for(i = event_cnt - 1; i >= 0; i--) {
        lv_event_dsc_t * dsc = lv_obj_get_event_dsc(obj, i);
        if(dsc && (event_cb == NULL || dsc->cb == event_cb) && dsc->user_data == user_data) {
            lv_obj_remove_event(obj, i);
            removed_count ++;
        }
    }

    return removed_count;
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
       e->code == LV_EVENT_LEAVE ||
       e->code == LV_EVENT_HOVER_OVER ||
       e->code == LV_EVENT_HOVER_LEAVE) {
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

int32_t lv_event_get_rotary_diff(lv_event_t * e)
{
    if(e->code == LV_EVENT_ROTARY) {
        int32_t * r = lv_event_get_param(e);
        if(r) return *r;
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

void lv_event_set_ext_draw_size(lv_event_t * e, int32_t size)
{
    if(e->code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t * cur_size = lv_event_get_param(e);
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

static lv_result_t event_send_core(lv_event_t * e)
{
    LV_TRACE_EVENT("Sending event %d to %p with %p param", e->code, (void *)e->original_target, e->param);

    lv_indev_t * indev_act = lv_indev_active();
    if(indev_act) {
        if(e->stop_processing) return LV_RESULT_OK;
        if(e->deleted) return LV_RESULT_INVALID;
    }

    lv_obj_t * target = e->current_target;
    lv_result_t res = LV_RESULT_OK;
    lv_event_list_t * list = target->spec_attr ?  &target->spec_attr->event_list : NULL;

    res = lv_event_send(list, e, true);
    if(res != LV_RESULT_OK || e->stop_processing) return res;

    res = lv_obj_event_base(NULL, e);
    if(res != LV_RESULT_OK || e->stop_processing) return res;

    res = lv_event_send(list, e, false);
    if(res != LV_RESULT_OK || e->stop_processing) return res;

    lv_obj_t * parent = lv_obj_get_parent(e->current_target);
    if(parent && event_is_bubbled(e)) {
        e->current_target = parent;
        res = event_send_core(e);
    }
    if(res != LV_RESULT_OK) return res;

    /*Trickle down to children if enabled*/
    if(event_is_trickled(e)) {
        uint32_t child_count = lv_obj_get_child_count(target);

        /* we don't want the event to bubble up again when trickling down */
        e->stop_bubbling = 1;

        for(uint32_t i = 0; i < child_count && res == LV_RESULT_OK && !e->stop_processing; i++) {
            lv_obj_t * child = lv_obj_get_child(target, i);
            if(child) {
                e->current_target = child;
                res = event_send_core(e);
                if(res != LV_RESULT_OK) {
                    LV_LOG_WARN("Trickle down event %d to child %p failed", e->code, (void *)child);
                    break;
                }
            }
        }
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

static bool event_is_trickled(lv_event_t * e)
{
    if(e->stop_trickling) return false;

    /*Check other codes only if trickle is enabled*/
    if(lv_obj_has_flag(e->current_target, LV_OBJ_FLAG_EVENT_TRICKLE) == false) return false;

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
