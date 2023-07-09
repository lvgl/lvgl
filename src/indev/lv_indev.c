/**
 * @file lv_indev.c
 *
 */

/*********************
 *      INCLUDES
 ********************/
#include "lv_indev_private.h"
#include "lv_indev_scroll.h"
#include "../disp/lv_disp_private.h"
#include "../core/lv_obj.h"
#include "../core/lv_group.h"
#include "../core/lv_refr.h"

#include "../tick/lv_tick.h"
#include "../misc/lv_timer.h"
#include "../misc/lv_math.h"
#include "../misc/lv_gc.h"
#include "../misc/lv_profiler.h"

/*********************
 *      DEFINES
 *********************/
/*Drag threshold in pixels*/
#define LV_INDEV_DEF_SCROLL_LIMIT         10

/*Drag throw slow-down in [%]. Greater value -> faster slow-down*/
#define LV_INDEV_DEF_SCROLL_THROW         10

/*Long press time in milliseconds.
 *Time to send `LV_EVENT_LONG_PRESSSED`)*/
#define LV_INDEV_DEF_LONG_PRESS_TIME      400

/*Repeated trigger period in long press [ms]
 *Time between `LV_EVENT_LONG_PRESSED_REPEAT*/
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100


/*Gesture threshold in pixels*/
#define LV_INDEV_DEF_GESTURE_LIMIT        50

/*Gesture min velocity at release before swipe (pixels)*/
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3

#if LV_INDEV_DEF_SCROLL_THROW <= 0
    #warning "LV_INDEV_DRAG_THROW must be greater than 0"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void indev_pointer_proc(lv_indev_t * i, lv_indev_data_t * data);
static void indev_keypad_proc(lv_indev_t * i, lv_indev_data_t * data);
static void indev_encoder_proc(lv_indev_t * i, lv_indev_data_t * data);
static void indev_button_proc(lv_indev_t * i, lv_indev_data_t * data);
static void indev_proc_press(lv_indev_t * indev);
static void indev_proc_release(lv_indev_t * indev);
static lv_obj_t * pointer_search_obj(lv_disp_t * disp, lv_point_t * p);
static void indev_proc_reset_query_handler(lv_indev_t * indev);
static void indev_click_focus(lv_indev_t * indev);
static void indev_gesture(lv_indev_t * indev);
static bool indev_reset_check(lv_indev_t * indev);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t * indev_act;
static lv_obj_t * indev_obj_act = NULL;

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_INDEV
    #define INDEV_TRACE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define INDEV_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_indev_create(void)
{
    lv_disp_t * disp = lv_disp_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("no display was created so far");
    }

    lv_indev_t * indev = _lv_ll_ins_head(&LV_GC_ROOT(_lv_indev_ll));
    LV_ASSERT_MALLOC(indev);
    if(!indev) {
        return NULL;
    }

    lv_memzero(indev, sizeof(lv_indev_t));
    indev->reset_query  = 1;
    indev->read_timer = lv_timer_create(lv_indev_read_timer_cb, LV_DEF_REFR_PERIOD, indev);

    indev->disp                 = lv_disp_get_default();
    indev->type                 = LV_INDEV_TYPE_NONE;
    indev->scroll_limit         = LV_INDEV_DEF_SCROLL_LIMIT;
    indev->scroll_throw         = LV_INDEV_DEF_SCROLL_THROW;
    indev->long_press_time      = LV_INDEV_DEF_LONG_PRESS_TIME;
    indev->long_press_repeat_time  = LV_INDEV_DEF_LONG_PRESS_REP_TIME;
    indev->gesture_limit        = LV_INDEV_DEF_GESTURE_LIMIT;
    indev->gesture_min_velocity = LV_INDEV_DEF_GESTURE_MIN_VELOCITY;

    return indev;
}

void lv_indev_delete(lv_indev_t * indev)
{
    LV_ASSERT_NULL(indev);
    /*Clean up the read timer first*/
    lv_timer_del(indev->read_timer);
    /*Remove the input device from the list*/
    _lv_ll_remove(&LV_GC_ROOT(_lv_indev_ll), indev);
    /*Free the memory of the input device*/
    lv_free(indev);
}

lv_indev_t * lv_indev_get_next(lv_indev_t * indev)
{
    if(indev == NULL)
        return _lv_ll_get_head(&LV_GC_ROOT(_lv_indev_ll));
    else
        return _lv_ll_get_next(&LV_GC_ROOT(_lv_indev_ll), indev);
}

void _lv_indev_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_PROFILER_BEGIN;
    lv_memzero(data, sizeof(lv_indev_data_t));

    /* For touchpad sometimes users don't set the last pressed coordinate on release.
     * So be sure a coordinates are initialized to the last point */
    if(indev->type == LV_INDEV_TYPE_POINTER) {
        data->point.x = indev->pointer.last_raw_point.x;
        data->point.y = indev->pointer.last_raw_point.y;
    }
    /*Similarly set at least the last key in case of the user doesn't set it on release*/
    else if(indev->type == LV_INDEV_TYPE_KEYPAD) {
        data->key = indev->keypad.last_key;
    }
    /*For compatibility assume that used button was enter (encoder push)*/
    else if(indev->type == LV_INDEV_TYPE_ENCODER) {
        data->key = LV_KEY_ENTER;
    }

    if(indev->read_cb) {
        INDEV_TRACE("calling indev_read_cb");
        indev->read_cb(indev, data);
    }
    else {
        LV_LOG_WARN("indev_read_cb is not registered");
    }
    LV_PROFILER_END;
}

void lv_indev_read_timer_cb(lv_timer_t * timer)
{
    INDEV_TRACE("begin");

    lv_indev_data_t data;

    indev_act = timer->user_data;

    /*Read and process all indevs*/
    if(indev_act->disp == NULL) return; /*Not assigned to any displays*/

    /*Handle reset query before processing the point*/
    indev_proc_reset_query_handler(indev_act);

    if(indev_act->disabled ||
       indev_act->disp->prev_scr != NULL) return; /*Input disabled or screen animation active*/

    LV_PROFILER_BEGIN;

    bool continue_reading;
    do {
        /*Read the data*/
        _lv_indev_read(indev_act, &data);
        continue_reading = data.continue_reading;

        /*The active object might be deleted even in the read function*/
        indev_proc_reset_query_handler(indev_act);
        indev_obj_act = NULL;

        indev_act->state = data.state;

        /*Save the last activity time*/
        if(indev_act->state == LV_INDEV_STATE_PRESSED) {
            indev_act->disp->last_activity_time = lv_tick_get();
        }
        else if(indev_act->type == LV_INDEV_TYPE_ENCODER && data.enc_diff) {
            indev_act->disp->last_activity_time = lv_tick_get();
        }

        if(indev_act->type == LV_INDEV_TYPE_POINTER) {
            indev_pointer_proc(indev_act, &data);
        }
        else if(indev_act->type == LV_INDEV_TYPE_KEYPAD) {
            indev_keypad_proc(indev_act, &data);
        }
        else if(indev_act->type == LV_INDEV_TYPE_ENCODER) {
            indev_encoder_proc(indev_act, &data);
        }
        else if(indev_act->type == LV_INDEV_TYPE_BUTTON) {
            indev_button_proc(indev_act, &data);
        }
        /*Handle reset query if it happened in during processing*/
        indev_proc_reset_query_handler(indev_act);
    } while(continue_reading);

    /*End of indev processing, so no act indev*/
    indev_act     = NULL;
    indev_obj_act = NULL;

    INDEV_TRACE("finished");
    LV_PROFILER_END;
}

void lv_indev_enable(lv_indev_t * indev, bool en)
{
    uint8_t enable = en ? 0 : 1;

    if(indev) {
        indev->disabled = enable;
    }
    else {
        lv_indev_t * i = lv_indev_get_next(NULL);
        while(i) {
            i->disabled = enable;
            i = lv_indev_get_next(i);
        }
    }
}

lv_indev_t * lv_indev_get_act(void)
{
    return indev_act;
}

void lv_indev_set_type(lv_indev_t * indev, lv_indev_type_t indev_type)
{
    if(indev == NULL) return;

    indev->type = indev_type;
    indev->reset_query = 1;
}

void lv_indev_set_read_cb(lv_indev_t * indev,  lv_indev_read_cb_t read_cb)
{
    if(indev == NULL) return;

    indev->read_cb = read_cb;
}

void lv_indev_set_user_data(lv_indev_t * indev, void * user_data)
{
    if(indev == NULL) return;
    indev->user_data = user_data;
}

void lv_indev_set_driver_data(lv_indev_t * indev, void * driver_data)
{
    if(indev == NULL) return;
    indev->driver_data = driver_data;
}


lv_indev_type_t lv_indev_get_type(const lv_indev_t * indev)
{
    if(indev == NULL) return LV_INDEV_TYPE_NONE;

    return indev->type;
}

lv_indev_state_t lv_indev_get_state(const lv_indev_t * indev)
{
    if(indev == NULL) return LV_INDEV_STATE_RELEASED;

    return indev->state;
}

lv_group_t * lv_indev_get_group(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;

    return indev->group;
}

lv_disp_t * lv_indev_get_disp(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;

    return indev->disp;
}

void lv_indev_set_disp(lv_indev_t * indev, lv_disp_t * disp)
{
    if(indev == NULL) return;

    indev->disp = disp;
}

void * lv_indev_get_user_data(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;
    return indev->user_data;
}

void * lv_indev_get_driver_data(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;

    return indev->driver_data;
}

void lv_indev_reset(lv_indev_t * indev, lv_obj_t * obj)
{
    if(indev) {
        indev->reset_query = 1;
        if(indev_act == indev) indev_obj_act = NULL;
        if(indev->type == LV_INDEV_TYPE_POINTER || indev->type == LV_INDEV_TYPE_KEYPAD) {
            if(obj == NULL || indev->pointer.last_pressed == obj) {
                indev->pointer.last_pressed = NULL;
            }
            if(obj == NULL || indev->pointer.act_obj == obj) {
                indev->pointer.act_obj = NULL;
            }
            if(obj == NULL || indev->pointer.last_obj == obj) {
                indev->pointer.last_obj = NULL;
            }
        }
    }
    else {
        lv_indev_t * i = lv_indev_get_next(NULL);
        while(i) {
            i->reset_query = 1;
            if(i->type == LV_INDEV_TYPE_POINTER || i->type == LV_INDEV_TYPE_KEYPAD) {
                if(obj == NULL || i->pointer.last_pressed == obj) {
                    i->pointer.last_pressed = NULL;
                }
                if(obj == NULL || i->pointer.act_obj == obj) {
                    i->pointer.act_obj = NULL;
                }
                if(obj == NULL || i->pointer.last_obj == obj) {
                    i->pointer.last_obj = NULL;
                }
            }
            i = lv_indev_get_next(i);
        }
        indev_obj_act = NULL;
    }
}

void lv_indev_reset_long_press(lv_indev_t * indev)
{
    indev->long_pr_sent         = 0;
    indev->longpr_rep_timestamp = lv_tick_get();
    indev->pr_timestamp         = lv_tick_get();
}

void lv_indev_set_cursor(lv_indev_t * indev, lv_obj_t * cur_obj)
{
    if(indev->type != LV_INDEV_TYPE_POINTER) return;

    indev->cursor = cur_obj;
    lv_obj_set_parent(indev->cursor, lv_disp_get_layer_sys(indev->disp));
    lv_obj_set_pos(indev->cursor, indev->pointer.act_point.x, indev->pointer.act_point.y);
    lv_obj_clear_flag(indev->cursor, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(indev->cursor, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING);
}

void lv_indev_set_group(lv_indev_t * indev, lv_group_t * group)
{
    if(indev->type == LV_INDEV_TYPE_KEYPAD || indev->type == LV_INDEV_TYPE_ENCODER) {
        indev->group = group;
    }
}

void lv_indev_set_button_points(lv_indev_t * indev, const lv_point_t points[])
{
    if(indev->type == LV_INDEV_TYPE_BUTTON) {
        indev->btn_points = points;
    }
}

void lv_indev_get_point(const lv_indev_t * indev, lv_point_t * point)
{
    if(indev == NULL) {
        point->x = 0;
        point->y = 0;
        return;
    }
    if(indev->type != LV_INDEV_TYPE_POINTER && indev->type != LV_INDEV_TYPE_BUTTON) {
        point->x = -1;
        point->y = -1;
    }
    else {
        point->x = indev->pointer.act_point.x;
        point->y = indev->pointer.act_point.y;
    }
}

lv_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev)
{
    return indev->pointer.gesture_dir;
}

uint32_t lv_indev_get_key(const lv_indev_t * indev)
{
    if(indev->type != LV_INDEV_TYPE_KEYPAD)
        return 0;
    else
        return indev->keypad.last_key;
}

lv_dir_t lv_indev_get_scroll_dir(const lv_indev_t * indev)
{
    if(indev == NULL) return false;
    if(indev->type != LV_INDEV_TYPE_POINTER && indev->type != LV_INDEV_TYPE_BUTTON) return false;
    return indev->pointer.scroll_dir;
}

lv_obj_t * lv_indev_get_scroll_obj(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;
    if(indev->type != LV_INDEV_TYPE_POINTER && indev->type != LV_INDEV_TYPE_BUTTON) return NULL;
    return indev->pointer.scroll_obj;
}

void lv_indev_get_vect(const lv_indev_t * indev, lv_point_t * point)
{
    point->x = 0;
    point->y = 0;

    if(indev == NULL) return;

    if(indev->type == LV_INDEV_TYPE_POINTER || indev->type == LV_INDEV_TYPE_BUTTON) {
        point->x = indev->pointer.vect.x;
        point->y = indev->pointer.vect.y;
    }
}

void lv_indev_wait_release(lv_indev_t * indev)
{
    if(indev == NULL)return;
    indev->wait_until_release = 1;
}

lv_obj_t * lv_indev_get_obj_act(void)
{
    return indev_obj_act;
}

lv_timer_t * lv_indev_get_read_timer(lv_indev_t * indev)
{
    if(!indev) {
        LV_LOG_WARN("lv_indev_get_read_timer: indev was NULL");
        return NULL;
    }

    return indev->read_timer;
}


lv_obj_t * lv_indev_search_obj(lv_obj_t * obj, lv_point_t * point)
{
    lv_obj_t * found_p = NULL;

    /*If this obj is hidden the children are hidden too so return immediately*/
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return NULL;

    lv_point_t p_trans = *point;
    lv_obj_transform_point(obj, &p_trans, false, true);

    bool hit_test_ok = lv_obj_hit_test(obj, &p_trans);

    /*If the point is on this object check its children too*/
    if(_lv_area_is_point_on(&obj->coords, &p_trans, 0)) {
        int32_t i;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);

        /*If a child matches use it*/
        for(i = child_cnt - 1; i >= 0; i--) {
            lv_obj_t * child = obj->spec_attr->children[i];
            found_p = lv_indev_search_obj(child, &p_trans);
            if(found_p) return found_p;
        }
    }

    /*If not return earlier for a clicked child and this obj's hittest was ok use it
     *else return NULL*/
    if(hit_test_ok) return obj;
    else return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Process a new point from LV_INDEV_TYPE_POINTER input device
 * @param i pointer to an input device
 * @param data pointer to the data read from the input device
 */
static void indev_pointer_proc(lv_indev_t * i, lv_indev_data_t * data)
{
    lv_disp_t * disp = i->disp;
    /*Save the raw points so they can be used again in _lv_indev_read*/
    i->pointer.last_raw_point.x = data->point.x;
    i->pointer.last_raw_point.y = data->point.y;

    if(disp->rotation == LV_DISP_ROTATION_180 || disp->rotation == LV_DISP_ROTATION_270) {
        data->point.x = disp->hor_res - data->point.x - 1;
        data->point.y = disp->ver_res - data->point.y - 1;
    }
    if(disp->rotation == LV_DISP_ROTATION_90 || disp->rotation == LV_DISP_ROTATION_270) {
        lv_coord_t tmp = data->point.y;
        data->point.y = data->point.x;
        data->point.x = disp->ver_res - tmp - 1;
    }

    /*Simple sanity check*/
    if(data->point.x < 0) {
        LV_LOG_WARN("X is %d which is smaller than zero", (int)data->point.x);
    }
    if(data->point.x >= lv_disp_get_hor_res(i->disp)) {
        LV_LOG_WARN("X is %d which is greater than hor. res", (int)data->point.x);
    }
    if(data->point.y < 0) {
        LV_LOG_WARN("Y is %d which is smaller than zero", (int)data->point.y);
    }
    if(data->point.y >= lv_disp_get_ver_res(i->disp)) {
        LV_LOG_WARN("Y is %d which is greater than ver. res", (int)data->point.y);
    }

    /*Move the cursor if set and moved*/
    if(i->cursor != NULL &&
       (i->pointer.last_point.x != data->point.x || i->pointer.last_point.y != data->point.y)) {
        lv_obj_set_pos(i->cursor, data->point.x, data->point.y);
    }

    i->pointer.act_point.x = data->point.x;
    i->pointer.act_point.y = data->point.y;

    if(i->state == LV_INDEV_STATE_PRESSED) {
        indev_proc_press(i);
    }
    else {
        indev_proc_release(i);
    }

    i->pointer.last_point.x = i->pointer.act_point.x;
    i->pointer.last_point.y = i->pointer.act_point.y;
}

/**
 * Process a new point from LV_INDEV_TYPE_KEYPAD input device
 * @param i pointer to an input device
 * @param data pointer to the data read from the input device
 */
static void indev_keypad_proc(lv_indev_t * i, lv_indev_data_t * data)
{
    if(data->state == LV_INDEV_STATE_PRESSED && i->wait_until_release) return;

    if(i->wait_until_release) {
        i->wait_until_release      = 0;
        i->pr_timestamp            = 0;
        i->long_pr_sent            = 0;
        i->keypad.last_state = LV_INDEV_STATE_RELEASED; /*To skip the processing of release*/
    }

    lv_group_t * g = i->group;
    if(g == NULL) return;

    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    bool dis = lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);

    /*Save the last key to compare it with the current latter on RELEASE*/
    uint32_t prev_key = i->keypad.last_key;

    /*Save the last key.
     *It must be done here else `lv_indev_get_key` will return the last key in events*/
    i->keypad.last_key = data->key;

    /*Save the previous state so we can detect state changes below and also set the last state now
     *so if any event handler on the way returns `LV_RES_INV` the last state is remembered
     *for the next time*/
    uint32_t prev_state             = i->keypad.last_state;
    i->keypad.last_state = data->state;

    /*Key press happened*/
    if(data->state == LV_INDEV_STATE_PRESSED && prev_state == LV_INDEV_STATE_RELEASED) {
        LV_LOG_INFO("%" LV_PRIu32 " key is pressed", data->key);
        i->pr_timestamp = lv_tick_get();

        /*Move the focus on NEXT*/
        if(data->key == LV_KEY_NEXT) {
            lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
            lv_group_focus_next(g);
            if(indev_reset_check(i)) return;
        }
        /*Move the focus on PREV*/
        else if(data->key == LV_KEY_PREV) {
            lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
            lv_group_focus_prev(g);
            if(indev_reset_check(i)) return;
        }
        else if(!dis) {
            /*Simulate a press on the object if ENTER was pressed*/
            if(data->key == LV_KEY_ENTER) {
                /*Send the ENTER as a normal KEY*/
                lv_group_send_data(g, LV_KEY_ENTER);
                if(indev_reset_check(i)) return;

                if(!dis) lv_obj_send_event(indev_obj_act, LV_EVENT_PRESSED, indev_act);
                if(indev_reset_check(i)) return;
            }
            else if(data->key == LV_KEY_ESC) {
                /*Send the ESC as a normal KEY*/
                lv_group_send_data(g, LV_KEY_ESC);
                if(indev_reset_check(i)) return;

                lv_obj_send_event(indev_obj_act, LV_EVENT_CANCEL, indev_act);
                if(indev_reset_check(i)) return;
            }
            /*Just send other keys to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT`)*/
            else {
                lv_group_send_data(g, data->key);
                if(indev_reset_check(i)) return;
            }
        }
    }
    /*Pressing*/
    else if(!dis && data->state == LV_INDEV_STATE_PRESSED && prev_state == LV_INDEV_STATE_PRESSED) {

        if(data->key == LV_KEY_ENTER) {
            lv_obj_send_event(indev_obj_act, LV_EVENT_PRESSING, indev_act);
            if(indev_reset_check(i)) return;
        }

        /*Long press time has elapsed?*/
        if(i->long_pr_sent == 0 && lv_tick_elaps(i->pr_timestamp) > i->long_press_time) {
            i->long_pr_sent = 1;
            if(data->key == LV_KEY_ENTER) {
                i->longpr_rep_timestamp = lv_tick_get();
                lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                if(indev_reset_check(i)) return;
            }
        }
        /*Long press repeated time has elapsed?*/
        else if(i->long_pr_sent != 0 &&
                lv_tick_elaps(i->longpr_rep_timestamp) > i->long_press_repeat_time) {

            i->longpr_rep_timestamp = lv_tick_get();

            /*Send LONG_PRESS_REP on ENTER*/
            if(data->key == LV_KEY_ENTER) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, indev_act);
                if(indev_reset_check(i)) return;
            }
            /*Move the focus on NEXT again*/
            else if(data->key == LV_KEY_NEXT) {
                lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
                lv_group_focus_next(g);
                if(indev_reset_check(i)) return;
            }
            /*Move the focus on PREV again*/
            else if(data->key == LV_KEY_PREV) {
                lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
                lv_group_focus_prev(g);
                if(indev_reset_check(i)) return;
            }
            /*Just send other keys again to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT)*/
            else {
                lv_group_send_data(g, data->key);
                if(indev_reset_check(i)) return;
            }
        }
    }
    /*Release happened*/
    else if(!dis && data->state == LV_INDEV_STATE_RELEASED && prev_state == LV_INDEV_STATE_PRESSED) {
        LV_LOG_INFO("%" LV_PRIu32 " key is released", data->key);
        /*The user might clear the key when it was released. Always release the pressed key*/
        data->key = prev_key;
        if(data->key == LV_KEY_ENTER) {

            lv_obj_send_event(indev_obj_act, LV_EVENT_RELEASED, indev_act);
            if(indev_reset_check(i)) return;

            if(i->long_pr_sent == 0) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                if(indev_reset_check(i)) return;
            }

            lv_obj_send_event(indev_obj_act, LV_EVENT_CLICKED, indev_act);
            if(indev_reset_check(i)) return;

        }
        i->pr_timestamp = 0;
        i->long_pr_sent = 0;
    }
    indev_obj_act = NULL;
}

/**
 * Process a new point from LV_INDEV_TYPE_ENCODER input device
 * @param i pointer to an input device
 * @param data pointer to the data read from the input device
 */
static void indev_encoder_proc(lv_indev_t * i, lv_indev_data_t * data)
{
    if(data->state == LV_INDEV_STATE_PRESSED && i->wait_until_release) return;

    if(i->wait_until_release) {
        i->wait_until_release      = 0;
        i->pr_timestamp            = 0;
        i->long_pr_sent            = 0;
        i->keypad.last_state = LV_INDEV_STATE_RELEASED; /*To skip the processing of release*/
    }

    /*Save the last keys before anything else.
     *They need to be already saved if the function returns for any reason*/
    lv_indev_state_t last_state     = i->keypad.last_state;
    i->keypad.last_state = data->state;
    i->keypad.last_key   = data->key;

    lv_group_t * g = i->group;
    if(g == NULL) return;

    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    /*Process the steps they are valid only with released button*/
    if(data->state != LV_INDEV_STATE_RELEASED) {
        data->enc_diff = 0;
    }

    /*Refresh the focused object. It might change due to lv_group_focus_prev/next*/
    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    /*Button press happened*/
    if(data->state == LV_INDEV_STATE_PRESSED && last_state == LV_INDEV_STATE_RELEASED) {
        LV_LOG_INFO("pressed");

        i->pr_timestamp = lv_tick_get();

        if(data->key == LV_KEY_ENTER) {
            bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) ||
                                          lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);
            if(lv_group_get_editing(g) == true || editable_or_scrollable == false) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_PRESSED, indev_act);
                if(indev_reset_check(i)) return;
            }
        }
        else if(data->key == LV_KEY_LEFT) {
            /*emulate encoder left*/
            data->enc_diff--;
        }
        else if(data->key == LV_KEY_RIGHT) {
            /*emulate encoder right*/
            data->enc_diff++;
        }
        else if(data->key == LV_KEY_ESC) {
            /*Send the ESC as a normal KEY*/
            lv_group_send_data(g, LV_KEY_ESC);
            if(indev_reset_check(i)) return;

            lv_obj_send_event(indev_obj_act, LV_EVENT_CANCEL, indev_act);
            if(indev_reset_check(i)) return;
        }
        /*Just send other keys to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT`)*/
        else {
            lv_group_send_data(g, data->key);
            if(indev_reset_check(i)) return;
        }
    }
    /*Pressing*/
    else if(data->state == LV_INDEV_STATE_PRESSED && last_state == LV_INDEV_STATE_PRESSED) {
        /*Long press*/
        if(i->long_pr_sent == 0 && lv_tick_elaps(i->pr_timestamp) > i->long_press_time) {

            i->long_pr_sent = 1;
            i->longpr_rep_timestamp = lv_tick_get();

            if(data->key == LV_KEY_ENTER) {
                bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) ||
                                              lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);

                /*On enter long press toggle edit mode.*/
                if(editable_or_scrollable) {
                    /*Don't leave edit mode if there is only one object (nowhere to navigate)*/
                    if(lv_group_get_obj_count(g) > 1) {
                        LV_LOG_INFO("toggling edit mode");
                        lv_group_set_editing(g, lv_group_get_editing(g) ? false : true); /*Toggle edit mode on long press*/
                        lv_obj_clear_state(indev_obj_act, LV_STATE_PRESSED);    /*Remove the pressed state manually*/
                    }
                }
                /*If not editable then just send a long press Call the ancestor's event handler*/
                else {
                    lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                    if(indev_reset_check(i)) return;
                }
            }

            i->long_pr_sent = 1;
        }
        /*Long press repeated time has elapsed?*/
        else if(i->long_pr_sent != 0 && lv_tick_elaps(i->longpr_rep_timestamp) > i->long_press_repeat_time) {

            i->longpr_rep_timestamp = lv_tick_get();

            if(data->key == LV_KEY_ENTER) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, indev_act);
                if(indev_reset_check(i)) return;
            }
            else if(data->key == LV_KEY_LEFT) {
                /*emulate encoder left*/
                data->enc_diff--;
            }
            else if(data->key == LV_KEY_RIGHT) {
                /*emulate encoder right*/
                data->enc_diff++;
            }
            else {
                lv_group_send_data(g, data->key);
                if(indev_reset_check(i)) return;
            }

        }

    }
    /*Release happened*/
    else if(data->state == LV_INDEV_STATE_RELEASED && last_state == LV_INDEV_STATE_PRESSED) {
        LV_LOG_INFO("released");

        if(data->key == LV_KEY_ENTER) {
            bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) ||
                                          lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);

            /*The button was released on a non-editable object. Just send enter*/
            if(editable_or_scrollable == false) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_RELEASED, indev_act);
                if(indev_reset_check(i)) return;

                if(i->long_pr_sent == 0) lv_obj_send_event(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                if(indev_reset_check(i)) return;

                lv_obj_send_event(indev_obj_act, LV_EVENT_CLICKED, indev_act);
                if(indev_reset_check(i)) return;

            }
            /*An object is being edited and the button is released.*/
            else if(lv_group_get_editing(g)) {
                /*Ignore long pressed enter release because it comes from mode switch*/
                if(!i->long_pr_sent || lv_group_get_obj_count(g) <= 1) {
                    lv_obj_send_event(indev_obj_act, LV_EVENT_RELEASED, indev_act);
                    if(indev_reset_check(i)) return;

                    lv_obj_send_event(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                    if(indev_reset_check(i)) return;

                    lv_obj_send_event(indev_obj_act, LV_EVENT_CLICKED, indev_act);
                    if(indev_reset_check(i)) return;

                    lv_group_send_data(g, LV_KEY_ENTER);
                    if(indev_reset_check(i)) return;
                }
                else {
                    lv_obj_clear_state(indev_obj_act, LV_STATE_PRESSED);    /*Remove the pressed state manually*/
                }
            }
            /*If the focused object is editable and now in navigate mode then on enter switch edit
               mode*/
            else if(!i->long_pr_sent) {
                LV_LOG_INFO("entering edit mode");
                lv_group_set_editing(g, true); /*Set edit mode*/
            }
        }

        i->pr_timestamp = 0;
        i->long_pr_sent = 0;
    }
    indev_obj_act = NULL;

    /*if encoder steps or simulated steps via left/right keys*/
    if(data->enc_diff != 0) {
        /*In edit mode send LEFT/RIGHT keys*/
        if(lv_group_get_editing(g)) {
            LV_LOG_INFO("rotated by %+d (edit)", data->enc_diff);
            int32_t s;
            if(data->enc_diff < 0) {
                for(s = 0; s < -data->enc_diff; s++) {
                    lv_group_send_data(g, LV_KEY_LEFT);
                    if(indev_reset_check(i)) return;
                }
            }
            else if(data->enc_diff > 0) {
                for(s = 0; s < data->enc_diff; s++) {
                    lv_group_send_data(g, LV_KEY_RIGHT);
                    if(indev_reset_check(i)) return;
                }
            }
        }
        /*In navigate mode focus on the next/prev objects*/
        else {
            LV_LOG_INFO("rotated by %+d (nav)", data->enc_diff);
            int32_t s;
            if(data->enc_diff < 0) {
                for(s = 0; s < -data->enc_diff; s++) {
                    lv_group_focus_prev(g);
                    if(indev_reset_check(i)) return;
                }
            }
            else if(data->enc_diff > 0) {
                for(s = 0; s < data->enc_diff; s++) {
                    lv_group_focus_next(g);
                    if(indev_reset_check(i)) return;
                }
            }
        }
    }
}

/**
 * Process new points from an input device. indev->state.pressed has to be set
 * @param indev pointer to an input device state
 * @param x x coordinate of the next point
 * @param y y coordinate of the next point
 */
static void indev_button_proc(lv_indev_t * i, lv_indev_data_t * data)
{
    /*Die gracefully if i->btn_points is NULL*/
    if(i->btn_points == NULL) {
        LV_LOG_WARN("btn_points is NULL");
        return;
    }

    lv_coord_t x = i->btn_points[data->btn_id].x;
    lv_coord_t y = i->btn_points[data->btn_id].y;

    static lv_indev_state_t prev_state = LV_INDEV_STATE_RELEASED;
    if(prev_state != data->state) {
        if(data->state == LV_INDEV_STATE_PRESSED) {
            LV_LOG_INFO("button %" LV_PRIu32 " is pressed (x:%d y:%d)", data->btn_id, (int)x, (int)y);
        }
        else {
            LV_LOG_INFO("button %" LV_PRIu32 " is released (x:%d y:%d)", data->btn_id, (int)x, (int)y);
        }
    }

    /*If a new point comes always make a release*/
    if(data->state == LV_INDEV_STATE_PRESSED) {
        if(i->pointer.last_point.x != x ||
           i->pointer.last_point.y != y) {
            indev_proc_release(i);
        }
    }

    if(indev_reset_check(i)) return;

    /*Save the new points*/
    i->pointer.act_point.x = x;
    i->pointer.act_point.y = y;

    if(data->state == LV_INDEV_STATE_PRESSED) indev_proc_press(i);
    else indev_proc_release(i);

    if(indev_reset_check(i)) return;

    i->pointer.last_point.x = i->pointer.act_point.x;
    i->pointer.last_point.y = i->pointer.act_point.y;
}

/**
 * Process the pressed state of LV_INDEV_TYPE_POINTER input devices
 * @param indev pointer to an input device 'proc'
 * @return LV_RES_OK: no indev reset required; LV_RES_INV: indev reset is required
 */
static void indev_proc_press(lv_indev_t * indev)
{
    LV_LOG_INFO("pressed at x:%d y:%d", (int)indev->pointer.act_point.x,
                (int)indev->pointer.act_point.y);
    indev_obj_act = indev->pointer.act_obj;

    if(indev->wait_until_release != 0) return;

    lv_disp_t * disp = indev_act->disp;
    bool new_obj_searched = false;

    /*If there is no last object then search*/
    if(indev_obj_act == NULL) {
        indev_obj_act = pointer_search_obj(disp, &indev->pointer.act_point);
        new_obj_searched = true;
    }
    /*If there is an active object it's not scrolled and not protected also search*/
    else if(indev->pointer.scroll_obj == NULL &&
            lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_PRESS_LOCK) == false) {
        indev_obj_act = pointer_search_obj(disp, &indev->pointer.act_point);
        new_obj_searched = true;
    }

    /*The last object might have scroll throw. Stop it manually*/
    if(new_obj_searched && indev->pointer.last_obj) {
        indev->pointer.scroll_throw_vect.x = 0;
        indev->pointer.scroll_throw_vect.y = 0;
        _lv_indev_scroll_throw_handler(indev);
        if(indev_reset_check(indev)) return;
    }

    /*If a new object was found reset some variables and send a pressed event handler*/
    if(indev_obj_act != indev->pointer.act_obj) {
        indev->pointer.last_point.x = indev->pointer.act_point.x;
        indev->pointer.last_point.y = indev->pointer.act_point.y;

        /*If a new object found the previous was lost, so send a PRESS_LOST event*/
        if(indev->pointer.act_obj != NULL) {
            /*Save the obj because in special cases `act_obj` can change in the event */
            lv_obj_t * last_obj = indev->pointer.act_obj;

            lv_obj_send_event(last_obj, LV_EVENT_PRESS_LOST, indev_act);
            if(indev_reset_check(indev)) return;

            /*Do nothing until release and a new press*/
            lv_indev_reset(indev, NULL);
            lv_indev_wait_release(indev);
            return;
        }

        indev->pointer.act_obj  = indev_obj_act; /*Save the pressed object*/
        indev->pointer.last_obj = indev_obj_act;

        if(indev_obj_act != NULL) {
            /*Save the time when the obj pressed to count long press time.*/
            indev->pr_timestamp                 = lv_tick_get();
            indev->long_pr_sent                 = 0;
            indev->pointer.scroll_sum.x     = 0;
            indev->pointer.scroll_sum.y     = 0;
            indev->pointer.scroll_dir = LV_DIR_NONE;
            indev->pointer.gesture_dir = LV_DIR_NONE;
            indev->pointer.gesture_sent   = 0;
            indev->pointer.gesture_sum.x  = 0;
            indev->pointer.gesture_sum.y  = 0;
            indev->pointer.vect.x         = 0;
            indev->pointer.vect.y         = 0;

            /*Call the ancestor's event handler about the press*/
            lv_obj_send_event(indev_obj_act, LV_EVENT_PRESSED, indev_act);
            if(indev_reset_check(indev)) return;

            if(indev_act->wait_until_release) return;

            /*Handle focus*/
            indev_click_focus(indev_act);
            if(indev_reset_check(indev)) return;

        }
    }

    /*Calculate the vector and apply a low pass filter: new value = 0.5 * old_value + 0.5 * new_value*/
    indev->pointer.vect.x = indev->pointer.act_point.x - indev->pointer.last_point.x;
    indev->pointer.vect.y = indev->pointer.act_point.y - indev->pointer.last_point.y;

    indev->pointer.scroll_throw_vect.x = (indev->pointer.scroll_throw_vect.x + indev->pointer.vect.x) / 2;
    indev->pointer.scroll_throw_vect.y = (indev->pointer.scroll_throw_vect.y + indev->pointer.vect.y) / 2;

    indev->pointer.scroll_throw_vect_ori = indev->pointer.scroll_throw_vect;

    if(indev_obj_act) {
        lv_obj_send_event(indev_obj_act, LV_EVENT_PRESSING, indev_act);
        if(indev_reset_check(indev)) return;

        if(indev_act->wait_until_release) return;

        _lv_indev_scroll_handler(indev);
        if(indev_reset_check(indev)) return;
        indev_gesture(indev);
        if(indev_reset_check(indev)) return;

        /*If there is no scrolling then check for long press time*/
        if(indev->pointer.scroll_obj == NULL && indev->long_pr_sent == 0) {
            /*Call the ancestor's event handler about the long press if enough time elapsed*/
            if(lv_tick_elaps(indev->pr_timestamp) > indev_act->long_press_time) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                if(indev_reset_check(indev)) return;

                /*Mark the Call the ancestor's event handler sending to do not send it again*/
                indev->long_pr_sent = 1;

                /*Save the long press time stamp for the long press repeat handler*/
                indev->longpr_rep_timestamp = lv_tick_get();
            }
        }

        /*Send long press repeated Call the ancestor's event handler*/
        if(indev->pointer.scroll_obj == NULL && indev->long_pr_sent == 1) {
            /*Call the ancestor's event handler about the long press repeat if enough time elapsed*/
            if(lv_tick_elaps(indev->longpr_rep_timestamp) > indev_act->long_press_repeat_time) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, indev_act);
                if(indev_reset_check(indev)) return;
                indev->longpr_rep_timestamp = lv_tick_get();
            }
        }
    }
}

/**
 * Process the released state of LV_INDEV_TYPE_POINTER input devices
 * @param proc pointer to an input device 'proc'
 */
static void indev_proc_release(lv_indev_t * indev)
{
    if(indev->wait_until_release) {
        lv_obj_send_event(indev->pointer.act_obj, LV_EVENT_PRESS_LOST, indev_act);
        if(indev_reset_check(indev)) return;

        indev->pointer.act_obj  = NULL;
        indev->pointer.last_obj = NULL;
        indev->pr_timestamp           = 0;
        indev->longpr_rep_timestamp   = 0;
        indev->wait_until_release     = 0;
    }
    indev_obj_act = indev->pointer.act_obj;
    lv_obj_t * scroll_obj = indev->pointer.scroll_obj;

    /*Forget the act obj and send a released Call the ancestor's event handler*/
    if(indev_obj_act) {
        LV_LOG_INFO("released");

        /*Send RELEASE Call the ancestor's event handler and event*/
        lv_obj_send_event(indev_obj_act, LV_EVENT_RELEASED, indev_act);
        if(indev_reset_check(indev)) return;

        /*Send CLICK if no scrolling*/
        if(scroll_obj == NULL) {
            if(indev->long_pr_sent == 0) {
                lv_obj_send_event(indev_obj_act, LV_EVENT_SHORT_CLICKED, indev_act);
                if(indev_reset_check(indev)) return;
            }

            lv_obj_send_event(indev_obj_act, LV_EVENT_CLICKED, indev_act);
            if(indev_reset_check(indev)) return;
        }
        else {
            lv_obj_send_event(scroll_obj, LV_EVENT_SCROLL_THROW_BEGIN, indev_act);
            if(indev_reset_check(indev)) return;
        }

        indev->pointer.act_obj = NULL;
        indev->pr_timestamp          = 0;
        indev->longpr_rep_timestamp  = 0;


        /*Get the transformed vector with this object*/
        if(scroll_obj) {
            int16_t angle = 0;
            int16_t zoom = 256;
            lv_point_t pivot = { 0, 0 };
            lv_obj_t * parent = scroll_obj;
            while(parent) {
                angle += lv_obj_get_style_transform_angle(parent, 0);
                int32_t zoom_act = lv_obj_get_style_transform_zoom_safe(parent, 0);
                zoom = (zoom * zoom_act) >> 8;
                parent = lv_obj_get_parent(parent);
            }

            if(angle != 0 || zoom != LV_ZOOM_NONE) {
                angle = -angle;
                zoom = (256 * 256) / zoom;
                lv_point_transform(&indev->pointer.scroll_throw_vect, angle, zoom, &pivot);
                lv_point_transform(&indev->pointer.scroll_throw_vect_ori, angle, zoom, &pivot);
            }
        }

    }

    if(scroll_obj) {
        _lv_indev_scroll_throw_handler(indev);
        if(indev_reset_check(indev)) return;
    }
}

static lv_obj_t * pointer_search_obj(lv_disp_t * disp, lv_point_t * p)
{
    indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_sys(disp), p);
    if(indev_obj_act) return indev_obj_act;

    indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_top(disp), p);
    if(indev_obj_act) return indev_obj_act;

    indev_obj_act = lv_indev_search_obj(lv_disp_get_scr_act(disp), p);
    if(indev_obj_act) return indev_obj_act;

    indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_bottom(disp), p);
    return indev_obj_act;
}

/**
 * Process a new point from LV_INDEV_TYPE_BUTTON input device
 * @param i pointer to an input device
 * @param data pointer to the data read from the input device
 * Reset input device if a reset query has been sent to it
 * @param indev pointer to an input device
 */
static void indev_proc_reset_query_handler(lv_indev_t * indev)
{
    if(indev->reset_query) {
        indev->pointer.act_obj           = NULL;
        indev->pointer.last_obj          = NULL;
        indev->pointer.scroll_obj          = NULL;
        indev->long_pr_sent                    = 0;
        indev->pr_timestamp                    = 0;
        indev->longpr_rep_timestamp            = 0;
        indev->pointer.scroll_sum.x        = 0;
        indev->pointer.scroll_sum.y        = 0;
        indev->pointer.scroll_dir = LV_DIR_NONE;
        indev->pointer.scroll_throw_vect.x = 0;
        indev->pointer.scroll_throw_vect.y = 0;
        indev->pointer.gesture_sum.x     = 0;
        indev->pointer.gesture_sum.y     = 0;
        indev->reset_query                     = 0;
        indev_obj_act                               = NULL;
    }
}

/**
 * Handle focus/defocus on click for POINTER input devices
 * @param proc pointer to the state of the indev
 */
static void indev_click_focus(lv_indev_t * indev)
{
    /*Handle click focus*/
    if(lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_CLICK_FOCUSABLE) == false) {
        return;
    }

    lv_group_t * g_act = lv_obj_get_group(indev_obj_act);
    lv_group_t * g_prev = indev->pointer.last_pressed ? lv_obj_get_group(indev->pointer.last_pressed) : NULL;

    /*If both the last and act. obj. are in the same group (or have no group)*/
    if(g_act == g_prev) {
        /*The objects are in a group*/
        if(g_act) {
            lv_group_focus_obj(indev_obj_act);
            if(indev_reset_check(indev)) return;
        }
        /*The object are not in group*/
        else {
            if(indev->pointer.last_pressed) {
                lv_obj_send_event(indev->pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
                if(indev_reset_check(indev)) return;
            }

            lv_obj_send_event(indev_obj_act, LV_EVENT_FOCUSED, indev_act);
            if(indev_reset_check(indev)) return;
        }
    }
    /*The object are not in the same group (in different groups or one has no group)*/
    else {
        /*If the prev. obj. is not in a group then defocus it.*/
        if(g_prev == NULL && indev->pointer.last_pressed) {
            lv_obj_send_event(indev->pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
            if(indev_reset_check(indev)) return;
        }
        /*Focus on a non-group object*/
        else {
            if(indev->pointer.last_pressed) {
                /*If the prev. object also wasn't in a group defocus it*/
                if(g_prev == NULL) {
                    lv_obj_send_event(indev->pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
                    if(indev_reset_check(indev)) return;
                }
                /*If the prev. object also was in a group at least "LEAVE" it instead of defocus*/
                else {
                    lv_obj_send_event(indev->pointer.last_pressed, LV_EVENT_LEAVE, indev_act);
                    if(indev_reset_check(indev)) return;
                }
            }
        }

        /*Focus to the act. in its group*/
        if(g_act) {
            lv_group_focus_obj(indev_obj_act);
            if(indev_reset_check(indev)) return;
        }
        else {
            lv_obj_send_event(indev_obj_act, LV_EVENT_FOCUSED, indev_act);
            if(indev_reset_check(indev)) return;
        }
    }
    indev->pointer.last_pressed = indev_obj_act;
}

/**
* Handle the gesture of indev_proc_p->pointer.act_obj
* @param indev pointer to an input device state
*/
void indev_gesture(lv_indev_t * indev)
{

    if(indev->pointer.scroll_obj) return;
    if(indev->pointer.gesture_sent) return;

    lv_obj_t * gesture_obj = indev->pointer.act_obj;

    /*If gesture parent is active check recursively the gesture attribute*/
    while(gesture_obj && lv_obj_has_flag(gesture_obj, LV_OBJ_FLAG_GESTURE_BUBBLE)) {
        gesture_obj = lv_obj_get_parent(gesture_obj);
    }

    if(gesture_obj == NULL) return;

    if((LV_ABS(indev->pointer.vect.x) < indev_act->gesture_min_velocity) &&
       (LV_ABS(indev->pointer.vect.y) < indev_act->gesture_min_velocity)) {
        indev->pointer.gesture_sum.x = 0;
        indev->pointer.gesture_sum.y = 0;
    }

    /*Count the movement by gesture*/
    indev->pointer.gesture_sum.x += indev->pointer.vect.x;
    indev->pointer.gesture_sum.y += indev->pointer.vect.y;

    if((LV_ABS(indev->pointer.gesture_sum.x) > indev_act->gesture_limit) ||
       (LV_ABS(indev->pointer.gesture_sum.y) > indev_act->gesture_limit)) {

        indev->pointer.gesture_sent = 1;

        if(LV_ABS(indev->pointer.gesture_sum.x) > LV_ABS(indev->pointer.gesture_sum.y)) {
            if(indev->pointer.gesture_sum.x > 0)
                indev->pointer.gesture_dir = LV_DIR_RIGHT;
            else
                indev->pointer.gesture_dir = LV_DIR_LEFT;
        }
        else {
            if(indev->pointer.gesture_sum.y > 0)
                indev->pointer.gesture_dir = LV_DIR_BOTTOM;
            else
                indev->pointer.gesture_dir = LV_DIR_TOP;
        }

        lv_obj_send_event(gesture_obj, LV_EVENT_GESTURE, indev_act);
        if(indev_reset_check(indev)) return;
    }
}

/**
 * Checks if the reset_query flag has been set. If so, perform necessary global indev cleanup actions
 * @param proc pointer to an input device 'proc'
 * @return true if indev query should be immediately truncated.
 */
static bool indev_reset_check(lv_indev_t * indev)
{
    if(indev->reset_query) {
        indev_obj_act = NULL;
    }

    return indev->reset_query ? true : false;
}
