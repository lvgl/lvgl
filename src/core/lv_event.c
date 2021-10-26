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
typedef struct _lv_event_dsc_t {
    lv_event_cb_t cb;
    void * user_data;
    lv_event_code_t filter : 8;
} lv_event_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_event_dsc_t * lv_obj_get_event_dsc(const lv_obj_t * obj, uint32_t id);
static lv_res_t event_send_core(lv_event_t * e);
static bool event_is_bubbled(lv_event_t * e);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_event_t * event_head;

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

lv_res_t lv_event_send(lv_obj_t * obj, lv_event_code_t event_code, void * param)
{
    if(obj == NULL) return LV_RES_OK;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_event_t e;
    e.target = obj;
    e.current_target = obj;
    e.code = event_code;
    e.user_data = NULL;
    e.param = param;
    e.deleted = 0;


    /*Build a simple linked list from the objects used in the events
     *It's important to know if an this object was deleted by a nested event
     *called from this `event_cb`.*/
    e.prev = event_head;
    event_head = &e;

    /*Send the event*/
    lv_res_t res = event_send_core(&e);

    /*Remove this element from the list*/
    event_head = e.prev;

    return res;
}


lv_res_t lv_obj_event_base(const lv_obj_class_t * class_p, lv_event_t * e)
{
    const lv_obj_class_t * base;
    if(class_p == NULL) base = e->current_target->class_p;
    else base = class_p->base_class;

    /*Find a base in which Call the ancestor's event handler_cb is set*/
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


lv_obj_t * lv_event_get_target(lv_event_t * e)
{
    return e->target;
}

lv_obj_t * lv_event_get_current_target(lv_event_t * e)
{
    return e->current_target;
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


uint32_t lv_event_register_id(void)
{
    static uint32_t last_id = _LV_EVENT_LAST;
    last_id ++;
    return last_id;
}

void _lv_event_mark_deleted(lv_obj_t * obj)
{
    lv_event_t * e = event_head;

    while(e) {
        if(e->current_target == obj || e->target == obj) e->deleted = 1;
        e = e->prev;
    }
}


struct _lv_event_dsc_t * lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter,
                                             void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_obj_allocate_spec_attr(obj);

    obj->spec_attr->event_dsc_cnt++;
    obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc,
                                               obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
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
        if(event_cb == NULL || obj->spec_attr->event_dsc[i].cb == event_cb) {
            /*Shift the remaining event handlers forward*/
            for(; i < (obj->spec_attr->event_dsc_cnt - 1); i++) {
                obj->spec_attr->event_dsc[i] = obj->spec_attr->event_dsc[i + 1];
            }
            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc,
                                                       obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);
            return true;
        }
    }

    /*No event handler found*/
    return false;
}

bool lv_obj_remove_event_cb_with_user_data(lv_obj_t * obj, lv_event_cb_t event_cb, const void * user_data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(obj->spec_attr == NULL) return false;

    int32_t i = 0;
    for(i = 0; i < obj->spec_attr->event_dsc_cnt; i++) {
        if((event_cb == NULL || obj->spec_attr->event_dsc[i].cb) &&
           obj->spec_attr->event_dsc[i].user_data == user_data) {
            /*Shift the remaining event handlers forward*/
            for(; i < (obj->spec_attr->event_dsc_cnt - 1); i++) {
                obj->spec_attr->event_dsc[i] = obj->spec_attr->event_dsc[i + 1];
            }
            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc,
                                                       obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
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
            for(; i < (obj->spec_attr->event_dsc_cnt - 1); i++) {
                obj->spec_attr->event_dsc[i] = obj->spec_attr->event_dsc[i + 1];
            }
            obj->spec_attr->event_dsc_cnt--;
            obj->spec_attr->event_dsc = lv_mem_realloc(obj->spec_attr->event_dsc,
                                                       obj->spec_attr->event_dsc_cnt * sizeof(lv_event_dsc_t));
            LV_ASSERT_MALLOC(obj->spec_attr->event_dsc);
            return true;
        }
    }

    /*No event handler found*/
    return false;
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

lv_obj_draw_part_dsc_t * lv_event_get_draw_part_dsc(lv_event_t * e)
{
    if(e->code == LV_EVENT_DRAW_PART_BEGIN ||
       e->code == LV_EVENT_DRAW_PART_END) {
        return lv_event_get_param(e);
    }
    else {
        LV_LOG_WARN("Not interpreted with this event code");
        return NULL;
    }
}

const lv_area_t * lv_event_get_clip_area(lv_event_t * e)
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
        return 0;
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

static lv_res_t event_send_core(lv_event_t * e)
{
    EVENT_TRACE("Sending event %d to %p with %p param", e->code, (void *)e->current_target, e->param);

    /*Call the input device's feedback callback if set*/
    lv_indev_t * indev_act = lv_indev_get_act();
    if(indev_act) {
        if(indev_act->driver->feedback_cb) indev_act->driver->feedback_cb(indev_act->driver, e->code);
    }

    lv_event_dsc_t * event_dsc = lv_obj_get_event_dsc(e->current_target, 0);
    lv_res_t res = LV_RES_OK;
    res = lv_obj_event_base(NULL, e);

    uint32_t i = 0;
    while(event_dsc && res == LV_RES_OK) {
        if(event_dsc->cb && (event_dsc->filter == LV_EVENT_ALL || event_dsc->filter == e->code)) {
            e->user_data = event_dsc->user_data;
            event_dsc->cb(e);

            /*Stop if the object is deleted*/
            if(e->deleted) return LV_RES_INV;
        }

        i++;
        event_dsc = lv_obj_get_event_dsc(e->current_target, i);
    }

    if(res == LV_RES_OK && e->current_target->parent && event_is_bubbled(e))
    {
            e->current_target = e->current_target->parent;
            res = event_send_core(e);
            if(res != LV_RES_OK) return LV_RES_INV;

    }

    return res;
}

static bool event_is_bubbled(lv_event_t * e)
{
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
        case LV_EVENT_DRAW_PART_BEGIN:
        case LV_EVENT_DRAW_PART_END:
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
