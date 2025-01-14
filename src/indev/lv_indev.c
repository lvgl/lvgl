#include "lv_indev_private.h"
#include "../misc/lv_event_private.h"
#include "../misc/lv_area_private.h"
#include "../misc/lv_anim_private.h"
#include "../core/lv_obj_draw_private.h"
/**
 * @file lv_indev.c
 *
 */

/*********************
 *      INCLUDES
 ********************/
#include "lv_indev_scroll.h"
#include "lv_indev_gesture.h"
#include "../display/lv_display_private.h"
#include "../core/lv_global.h"
#include "../core/lv_obj_private.h"
#include "../core/lv_group.h"
#include "../core/lv_refr.h"

#include "../tick/lv_tick.h"
#include "../misc/lv_timer_private.h"
#include "../misc/lv_math.h"
#include "../misc/lv_profiler.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
/*Drag threshold in pixels*/
#define LV_INDEV_DEF_SCROLL_LIMIT         10

/*Drag throw slow-down in [%]. Greater value -> faster slow-down*/
#define LV_INDEV_DEF_SCROLL_THROW         10

/*Long press time in milliseconds.
 *Time to send `LV_EVENT_LONG_PRESSED`)*/
#define LV_INDEV_DEF_LONG_PRESS_TIME      400

/*Repeated trigger period in long press [ms]
 *Time between `LV_EVENT_LONG_PRESSED_REPEAT*/
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100

/*Gesture threshold in pixels*/
#define LV_INDEV_DEF_GESTURE_LIMIT        50

/*Gesture min velocity at release before swipe (pixels)*/
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3

/**< Rotary diff count will be multiplied by this and divided by 256 */
#define LV_INDEV_DEF_ROTARY_SENSITIVITY         256

#if LV_INDEV_DEF_SCROLL_THROW <= 0
    #warning "LV_INDEV_DEF_SCROLL_THROW must be greater than 0"
#endif

#define indev_act LV_GLOBAL_DEFAULT()->indev_active
#define indev_obj_act LV_GLOBAL_DEFAULT()->indev_obj_active
#define indev_ll_head &(LV_GLOBAL_DEFAULT()->indev_ll)

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
static lv_result_t indev_proc_short_click(lv_indev_t * indev);
static void indev_proc_pointer_diff(lv_indev_t * indev);
static lv_obj_t * pointer_search_obj(lv_display_t * disp, lv_point_t * p);
static void indev_proc_reset_query_handler(lv_indev_t * indev);
static void indev_click_focus(lv_indev_t * indev);
static void indev_gesture(lv_indev_t * indev);
static bool indev_reset_check(lv_indev_t * indev);
static void indev_read_core(lv_indev_t * indev, lv_indev_data_t * data);
static void indev_reset_core(lv_indev_t * indev, lv_obj_t * obj);
static lv_result_t send_event(lv_event_code_t code, void * param);

static void indev_scroll_throw_anim_start(lv_indev_t * indev);
static void indev_scroll_throw_anim_cb(void * var, int32_t v);
static void indev_scroll_throw_anim_completed_cb(lv_anim_t * anim);
static inline void indev_scroll_throw_anim_reset(lv_indev_t * indev)
{
    if(indev) {
        indev->pointer.scroll_throw_vect.x = 0;
        indev->pointer.scroll_throw_vect.y = 0;
        indev->scroll_throw_anim = NULL;
    }
}

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_INDEV
    #define LV_TRACE_INDEV(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_INDEV(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_indev_t * lv_indev_create(void)
{
    lv_display_t * disp = lv_display_get_default();
    if(disp == NULL) {
        LV_LOG_WARN("no display was created so far");
    }

    lv_indev_t * indev = lv_ll_ins_head(indev_ll_head);
    LV_ASSERT_MALLOC(indev);
    if(indev == NULL) {
        return NULL;
    }

    lv_memzero(indev, sizeof(lv_indev_t));
    indev->reset_query  = 1;
    indev->enabled = 1;

    indev->read_timer = lv_timer_create(lv_indev_read_timer_cb, LV_DEF_REFR_PERIOD, indev);

    indev->disp                 = lv_display_get_default();
    indev->type                 = LV_INDEV_TYPE_NONE;
    indev->mode                 = LV_INDEV_MODE_TIMER;
    indev->scroll_limit         = LV_INDEV_DEF_SCROLL_LIMIT;
    indev->scroll_throw         = LV_INDEV_DEF_SCROLL_THROW;
    indev->long_press_time      = LV_INDEV_DEF_LONG_PRESS_TIME;
    indev->long_press_repeat_time  = LV_INDEV_DEF_LONG_PRESS_REP_TIME;
    indev->gesture_limit        = LV_INDEV_DEF_GESTURE_LIMIT;
    indev->gesture_min_velocity = LV_INDEV_DEF_GESTURE_MIN_VELOCITY;
    indev->rotary_sensitivity  = LV_INDEV_DEF_ROTARY_SENSITIVITY;
    return indev;
}

void lv_indev_delete(lv_indev_t * indev)
{
    LV_ASSERT_NULL(indev);

    lv_indev_send_event(indev, LV_EVENT_DELETE, NULL);
    lv_event_remove_all(&(indev->event_list));

    /*Clean up the read timer first*/
    if(indev->read_timer) lv_timer_delete(indev->read_timer);

    /*Remove the input device from the list*/
    lv_ll_remove(indev_ll_head, indev);
    /*Free the memory of the input device*/
    lv_free(indev);
}

lv_indev_t * lv_indev_get_next(lv_indev_t * indev)
{
    if(indev == NULL)
        return lv_ll_get_head(indev_ll_head);
    else
        return lv_ll_get_next(indev_ll_head, indev);
}

void indev_read_core(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_PROFILER_INDEV_BEGIN;
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
        LV_TRACE_INDEV("calling indev_read_cb");
        indev->read_cb(indev, data);
    }
    else {
        LV_LOG_WARN("indev_read_cb is not registered");
    }

    LV_PROFILER_INDEV_END;
}

void lv_indev_read_timer_cb(lv_timer_t * timer)
{
    lv_indev_read(timer->user_data);
}

void lv_indev_read(lv_indev_t * indev)
{
    if(indev == NULL) return;

    LV_TRACE_INDEV("begin");

    indev_act = indev;

    /*Read and process all indevs*/
    if(indev->disp == NULL) return; /*Not assigned to any displays*/

    /*Handle reset query before processing the point*/
    indev_proc_reset_query_handler(indev);

    if(indev->enabled == 0) return;
    if(indev->disp->prev_scr != NULL) {
        LV_TRACE_INDEV("input blocked while screen animation active");
        return;
    }

    LV_PROFILER_INDEV_BEGIN;

    bool continue_reading;
    lv_indev_data_t data;

    do {
        /*Read the data*/
        indev_read_core(indev, &data);
        continue_reading = indev->mode != LV_INDEV_MODE_EVENT && data.continue_reading;

        /*The active object might be deleted even in the read function*/
        indev_proc_reset_query_handler(indev);
        indev_obj_act = NULL;

        indev->state = data.state;

        /*Save the last activity time*/
        if(indev->state == LV_INDEV_STATE_PRESSED) {
            indev->disp->last_activity_time = lv_tick_get();
        }
        else if(indev->type == LV_INDEV_TYPE_ENCODER && data.enc_diff) {
            indev->disp->last_activity_time = lv_tick_get();
        }

        if(indev->type == LV_INDEV_TYPE_POINTER) {
            indev_pointer_proc(indev, &data);
        }
        else if(indev->type == LV_INDEV_TYPE_KEYPAD) {
            indev_keypad_proc(indev, &data);
        }
        else if(indev->type == LV_INDEV_TYPE_ENCODER) {
            indev_encoder_proc(indev, &data);
        }
        else if(indev->type == LV_INDEV_TYPE_BUTTON) {
            indev_button_proc(indev, &data);
        }
        /*Handle reset query if it happened in during processing*/
        indev_proc_reset_query_handler(indev);
    } while(continue_reading);

    /*End of indev processing, so no act indev*/
    indev_act     = NULL;
    indev_obj_act = NULL;

    LV_TRACE_INDEV("finished");
    LV_PROFILER_INDEV_END;
}

void lv_indev_enable(lv_indev_t * indev, bool enable)
{
    if(indev) {
        indev->enabled = (uint8_t) enable;
    }
    else {
        lv_indev_t * i = lv_indev_get_next(NULL);
        while(i) {
            i->enabled = (uint8_t) enable;
            i = lv_indev_get_next(i);
        }
    }
}

lv_indev_t * lv_indev_active(void)
{
    return indev_act;
}

void lv_indev_set_type(lv_indev_t * indev, lv_indev_type_t indev_type)
{
    if(indev == NULL) return;

    indev->type = indev_type;
    indev->reset_query = 1;
}

void lv_indev_set_read_cb(lv_indev_t * indev, lv_indev_read_cb_t read_cb)
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

lv_indev_read_cb_t lv_indev_get_read_cb(lv_indev_t * indev)
{
    if(indev == NULL) {
        LV_LOG_WARN("lv_indev_get_read_cb: indev was NULL");
        return NULL;
    }

    return indev->read_cb;
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

lv_display_t * lv_indev_get_display(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;

    return indev->disp;
}

void lv_indev_set_display(lv_indev_t * indev, lv_display_t * disp)
{
    if(indev == NULL) return;

    indev->disp = disp;
}

void lv_indev_set_long_press_time(lv_indev_t * indev, uint16_t long_press_time)
{
    if(indev == NULL) return;

    indev->long_press_time = long_press_time;
}

void lv_indev_set_long_press_repeat_time(lv_indev_t * indev, uint16_t long_press_repeat_time)
{
    if(indev == NULL) return;

    indev->long_press_repeat_time = long_press_repeat_time;
}

void lv_indev_set_scroll_limit(lv_indev_t * indev, uint8_t scroll_limit)
{
    if(indev == NULL) return;

    indev->scroll_limit = scroll_limit;
}

void lv_indev_set_scroll_throw(lv_indev_t * indev, uint8_t scroll_throw)
{
    if(indev == NULL) return;

    indev->scroll_throw = scroll_throw;
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

bool lv_indev_get_press_moved(const lv_indev_t * indev)
{
    if(indev == NULL) return false;

    return indev->pointer.press_moved;
}

void lv_indev_reset(lv_indev_t * indev, lv_obj_t * obj)
{
    if(indev) {
        indev_reset_core(indev, obj);
    }
    else {
        lv_indev_t * i = lv_indev_get_next(NULL);
        while(i) {
            indev_reset_core(i, obj);
            i = lv_indev_get_next(i);
        }
        indev_obj_act = NULL;
    }
}

void lv_indev_stop_processing(lv_indev_t * indev)
{
    if(indev == NULL) return;
    indev->stop_processing_query = 1;
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
    lv_obj_set_parent(indev->cursor, lv_display_get_layer_sys(indev->disp));
    lv_obj_set_pos(indev->cursor, indev->pointer.act_point.x, indev->pointer.act_point.y);
    lv_obj_remove_flag(indev->cursor, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(indev->cursor, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING);
}

void lv_indev_set_group(lv_indev_t * indev, lv_group_t * group)
{
    if(indev && (indev->type == LV_INDEV_TYPE_KEYPAD || indev->type == LV_INDEV_TYPE_ENCODER)) {
        indev->group = group;
    }
}

void lv_indev_set_button_points(lv_indev_t * indev, const lv_point_t points[])
{
    if(indev && indev->type == LV_INDEV_TYPE_BUTTON) {
        indev->btn_points = points;
    }
}

void lv_indev_get_point(const lv_indev_t * indev, lv_point_t * point)
{
    if(indev == NULL) {
        point->x = 0;
        point->y = 0;
    }
    else if(indev->type != LV_INDEV_TYPE_POINTER && indev->type != LV_INDEV_TYPE_BUTTON) {
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
    uint32_t key = 0;

    if(indev && indev->type == LV_INDEV_TYPE_KEYPAD)
        key = indev->keypad.last_key;

    return key;
}

uint8_t lv_indev_get_short_click_streak(const lv_indev_t * indev)
{
    return indev->pointer.short_click_streak;
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

lv_obj_t * lv_indev_get_cursor(lv_indev_t * indev)
{
    if(indev == NULL) return NULL;
    return indev->cursor;
}

void lv_indev_wait_release(lv_indev_t * indev)
{
    if(indev == NULL)return;
    indev->wait_until_release = 1;
}

lv_obj_t * lv_indev_get_active_obj(void)
{
    return indev_obj_act;
}

lv_timer_t * lv_indev_get_read_timer(lv_indev_t * indev)
{
    if(indev == NULL) {
        LV_LOG_WARN("lv_indev_get_read_timer: indev was NULL");
        return NULL;
    }

    return indev->read_timer;
}

lv_indev_mode_t lv_indev_get_mode(lv_indev_t * indev)
{
    if(indev) return indev->mode;
    return LV_INDEV_MODE_NONE;
}

void lv_indev_set_mode(lv_indev_t * indev, lv_indev_mode_t mode)
{
    if(indev == NULL || indev->mode == mode)
        return;

    indev->mode = mode;
    if(indev->read_timer) {
        if(mode == LV_INDEV_MODE_EVENT) {
            lv_timer_pause(indev->read_timer);
        }
        else if(mode == LV_INDEV_MODE_TIMER) {
            /* use default timer mode*/
            lv_timer_set_cb(indev->read_timer, lv_indev_read_timer_cb);
            lv_timer_resume(indev->read_timer);
        }
    }
}

lv_obj_t * lv_indev_search_obj(lv_obj_t * obj, lv_point_t * point)
{
    lv_obj_t * found_p = NULL;

    /*If this obj is hidden the children are hidden too so return immediately*/
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return NULL;

    lv_point_t p_trans = *point;
    lv_obj_transform_point(obj, &p_trans, LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE);

    bool hit_test_ok = lv_obj_hit_test(obj, &p_trans);

    /*If the point is on this object check its children too*/
    lv_area_t obj_coords = obj->coords;
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
        int32_t ext_draw_size = lv_obj_get_ext_draw_size(obj);
        lv_area_increase(&obj_coords, ext_draw_size, ext_draw_size);
    }
    if(lv_area_is_point_on(&obj_coords, &p_trans, 0)) {
        int32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(obj);

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

void lv_indev_add_event_cb(lv_indev_t * indev, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data)
{
    LV_ASSERT_NULL(indev);

    lv_event_add(&indev->event_list, event_cb, filter, user_data);
}

uint32_t lv_indev_get_event_count(lv_indev_t * indev)
{
    LV_ASSERT_NULL(indev);
    return lv_event_get_count(&indev->event_list);
}

lv_event_dsc_t * lv_indev_get_event_dsc(lv_indev_t * indev, uint32_t index)
{
    LV_ASSERT_NULL(indev);
    return lv_event_get_dsc(&indev->event_list, index);

}

bool lv_indev_remove_event(lv_indev_t * indev, uint32_t index)
{
    LV_ASSERT_NULL(indev);

    return lv_event_remove(&indev->event_list, index);
}

uint32_t lv_indev_remove_event_cb_with_user_data(lv_indev_t * indev, lv_event_cb_t event_cb, void * user_data)
{
    LV_ASSERT_NULL(indev);

    uint32_t event_cnt = lv_indev_get_event_count(indev);
    uint32_t removed_count = 0;
    int32_t i;

    for(i = event_cnt - 1; i >= 0; i--) {
        lv_event_dsc_t * dsc = lv_indev_get_event_dsc(indev, i);
        if(dsc && dsc->cb == event_cb && dsc->user_data == user_data) {
            lv_indev_remove_event(indev, i);
            removed_count ++;
        }
    }

    return removed_count;
}

lv_result_t lv_indev_send_event(lv_indev_t * indev, lv_event_code_t code, void * param)
{

    lv_event_t e;
    lv_memzero(&e, sizeof(e));
    e.code = code;
    e.current_target = indev;
    e.original_target = indev;
    e.param = param;
    lv_result_t res;
    res = lv_event_send(&indev->event_list, &e, true);
    if(res != LV_RESULT_OK) return res;

    res = lv_event_send(&indev->event_list, &e, false);
    if(res != LV_RESULT_OK) return res;

    return res;
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
    lv_display_t * disp = i->disp;
    /*Save the raw points so they can be used again in indev_read_core*/
    i->pointer.last_raw_point.x = data->point.x;
    i->pointer.last_raw_point.y = data->point.y;

    if(disp->rotation == LV_DISPLAY_ROTATION_180 || disp->rotation == LV_DISPLAY_ROTATION_270) {
        data->point.x = disp->hor_res - data->point.x - 1;
        data->point.y = disp->ver_res - data->point.y - 1;
    }
    if(disp->rotation == LV_DISPLAY_ROTATION_90 || disp->rotation == LV_DISPLAY_ROTATION_270) {
        int32_t tmp = data->point.y;
        data->point.y = data->point.x;
        data->point.x = disp->ver_res - tmp - 1;
    }

    /*Simple sanity check*/
    if(data->point.x < 0) {
        LV_LOG_WARN("X is %d which is smaller than zero", (int)data->point.x);
    }
    if(data->point.x >= lv_display_get_horizontal_resolution(i->disp)) {
        LV_LOG_WARN("X is %d which is greater than hor. res", (int)data->point.x);
    }
    if(data->point.y < 0) {
        LV_LOG_WARN("Y is %d which is smaller than zero", (int)data->point.y);
    }
    if(data->point.y >= lv_display_get_vertical_resolution(i->disp)) {
        LV_LOG_WARN("Y is %d which is greater than ver. res", (int)data->point.y);
    }

    /*Move the cursor if set and moved*/
    if(i->cursor != NULL &&
       (i->pointer.last_point.x != data->point.x || i->pointer.last_point.y != data->point.y)) {
        lv_obj_set_pos(i->cursor, data->point.x, data->point.y);
    }

    i->pointer.act_point.x = data->point.x;
    i->pointer.act_point.y = data->point.y;
    i->pointer.diff = data->enc_diff;

    i->gesture_type = data->gesture_type;
    i->gesture_data = data->gesture_data;

    /*Process the diff first as scrolling will be processed in indev_proc_release*/
    indev_proc_pointer_diff(i);

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

    /*Save the last key. *It must be done here else `lv_indev_get_key` will return the last key in events*/
    uint32_t prev_key = i->keypad.last_key;
    i->keypad.last_key = data->key;

    lv_group_t * g = i->group;
    if(g == NULL) return;

    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    const bool is_enabled = !lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);

    /*Save the previous state so we can detect state changes below and also set the last state now
     *so if any event handler on the way returns `LV_RESULT_INVALID` the last state is remembered
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
        else if(is_enabled) {
            /*Simulate a press on the object if ENTER was pressed*/
            if(data->key == LV_KEY_ENTER) {
                /*Send the ENTER as a normal KEY*/
                lv_group_send_data(g, LV_KEY_ENTER);
                if(indev_reset_check(i)) return;

                if(send_event(LV_EVENT_PRESSED, indev_act) == LV_RESULT_INVALID) return;

            }
            else if(data->key == LV_KEY_ESC) {
                /*Send the ESC as a normal KEY*/
                lv_group_send_data(g, LV_KEY_ESC);
                if(indev_reset_check(i)) return;

                if(send_event(LV_EVENT_CANCEL, indev_act) == LV_RESULT_INVALID) return;
            }
            /*Just send other keys to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT`)*/
            else {
                lv_group_send_data(g, data->key);
                if(indev_reset_check(i)) return;
            }
        }
    }
    /*Pressing*/
    else if(is_enabled && data->state == LV_INDEV_STATE_PRESSED && prev_state == LV_INDEV_STATE_PRESSED) {

        if(data->key == LV_KEY_ENTER) {
            if(send_event(LV_EVENT_PRESSING, indev_act) == LV_RESULT_INVALID) return;
        }

        /*Long press time has elapsed?*/
        if(i->long_pr_sent == 0 && lv_tick_elaps(i->pr_timestamp) > i->long_press_time) {
            i->long_pr_sent = 1;
            if(data->key == LV_KEY_ENTER) {
                i->longpr_rep_timestamp = lv_tick_get();

                if(send_event(LV_EVENT_LONG_PRESSED, indev_act) == LV_RESULT_INVALID) return;
            }
        }
        /*Long press repeated time has elapsed?*/
        else if(i->long_pr_sent != 0 &&
                lv_tick_elaps(i->longpr_rep_timestamp) > i->long_press_repeat_time) {

            i->longpr_rep_timestamp = lv_tick_get();

            /*Send LONG_PRESS_REP on ENTER*/
            if(data->key == LV_KEY_ENTER) {
                if(send_event(LV_EVENT_LONG_PRESSED_REPEAT, indev_act) == LV_RESULT_INVALID) return;
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
    else if(is_enabled && data->state == LV_INDEV_STATE_RELEASED && prev_state == LV_INDEV_STATE_PRESSED) {
        LV_LOG_INFO("%" LV_PRIu32 " key is released", data->key);
        /*The user might clear the key when it was released. Always release the pressed key*/
        data->key = prev_key;
        if(data->key == LV_KEY_ENTER) {

            if(send_event(LV_EVENT_RELEASED, indev_act) == LV_RESULT_INVALID) return;

            if(i->long_pr_sent == 0) {
                if(indev_proc_short_click(i) == LV_RESULT_INVALID) return;
            }

            if(send_event(LV_EVENT_CLICKED, indev_act) == LV_RESULT_INVALID) return;

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

    const bool is_enabled = !lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);

    /*Button press happened*/
    if(data->state == LV_INDEV_STATE_PRESSED && last_state == LV_INDEV_STATE_RELEASED) {
        LV_LOG_INFO("pressed");

        i->pr_timestamp = lv_tick_get();

        if(data->key == LV_KEY_ENTER) {
            bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) ||
                                          lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);
            if(lv_group_get_editing(g) == true || editable_or_scrollable == false) {

                if(is_enabled) {
                    if(send_event(LV_EVENT_PRESSED, indev_act) == LV_RESULT_INVALID) return;
                }
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

            if(is_enabled) {
                if(send_event(LV_EVENT_CANCEL, indev_act) == LV_RESULT_INVALID) return;
            }
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
                /* Always send event to indev callbacks*/
                lv_indev_send_event(indev_act, LV_EVENT_LONG_PRESSED, indev_obj_act);
                if(indev_reset_check(indev_act)) return;

                bool editable_or_scrollable = lv_obj_is_editable(indev_obj_act) ||
                                              lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_SCROLLABLE);

                /*On enter long press toggle edit mode.*/
                if(editable_or_scrollable) {
                    /*Don't leave edit mode if there is only one object (nowhere to navigate)*/
                    if(lv_group_get_obj_count(g) > 1) {
                        LV_LOG_INFO("toggling edit mode");
                        lv_group_set_editing(g, lv_group_get_editing(g) ? false : true); /*Toggle edit mode on long press*/
                        lv_obj_remove_state(indev_obj_act, LV_STATE_PRESSED);    /*Remove the pressed state manually*/
                    }
                }
                /*If not editable then just send a long press event*/
                else {
                    if(is_enabled) {
                        lv_obj_send_event(indev_obj_act, LV_EVENT_LONG_PRESSED, indev_act);
                        if(indev_reset_check(indev_act)) return;
                    }
                }
            }

            i->long_pr_sent = 1;
        }
        /*Long press repeated time has elapsed?*/
        else if(i->long_pr_sent != 0 && lv_tick_elaps(i->longpr_rep_timestamp) > i->long_press_repeat_time) {

            i->longpr_rep_timestamp = lv_tick_get();

            if(data->key == LV_KEY_ENTER) {
                if(is_enabled) {
                    if(send_event(LV_EVENT_LONG_PRESSED_REPEAT, indev_act) == LV_RESULT_INVALID) return;
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
                if(is_enabled) {
                    if(send_event(LV_EVENT_RELEASED, indev_act) == LV_RESULT_INVALID) return;
                }

                if(i->long_pr_sent == 0 && is_enabled) {
                    if(indev_proc_short_click(i) == LV_RESULT_INVALID) return;
                }

                if(is_enabled) {
                    if(send_event(LV_EVENT_CLICKED, indev_act) == LV_RESULT_INVALID) return;
                }

            }
            /*An object is being edited and the button is released.*/
            else if(lv_group_get_editing(g)) {
                /*Ignore long pressed enter release because it comes from mode switch*/
                if(!i->long_pr_sent || lv_group_get_obj_count(g) <= 1) {
                    if(is_enabled) {
                        if(send_event(LV_EVENT_RELEASED, indev_act) == LV_RESULT_INVALID) return;
                        if(indev_proc_short_click(i) == LV_RESULT_INVALID) return;
                        if(send_event(LV_EVENT_CLICKED, indev_act) == LV_RESULT_INVALID) return;
                    }

                    lv_group_send_data(g, LV_KEY_ENTER);
                    if(indev_reset_check(i)) return;
                }
                else {
                    lv_obj_remove_state(indev_obj_act, LV_STATE_PRESSED);    /*Remove the pressed state manually*/
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

    int32_t x = i->btn_points[data->btn_id].x;
    int32_t y = i->btn_points[data->btn_id].y;

    if(LV_INDEV_STATE_RELEASED != data->state) {
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
 */
static void indev_proc_press(lv_indev_t * indev)
{
    LV_LOG_INFO("pressed at x:%d y:%d", (int)indev->pointer.act_point.x,
                (int)indev->pointer.act_point.y);
    indev_obj_act = indev->pointer.act_obj;

    if(indev->wait_until_release != 0) return;

    lv_display_t * disp = indev_act->disp;
    bool new_obj_searched = false;

    /*If there is no last object then search*/
    if(indev_obj_act == NULL) {
        indev_obj_act = pointer_search_obj(disp, &indev->pointer.act_point);
        new_obj_searched = true;
    }
    /*If there is an active object it's not scrolled and not press locked also search*/
    else if(indev->pointer.scroll_obj == NULL &&
            lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_PRESS_LOCK) == false) {
        indev_obj_act = pointer_search_obj(disp, &indev->pointer.act_point);
        new_obj_searched = true;
    }

    /*The scroll object might have scroll throw. Stop it manually*/
    if(new_obj_searched && indev->pointer.scroll_obj) {
        /*Attempt to stop scroll throw animation firstly*/
        if(indev->scroll_throw_anim) {
            lv_anim_delete(indev, indev_scroll_throw_anim_cb);
            indev->scroll_throw_anim = NULL;
        }

        lv_indev_scroll_throw_handler(indev);
        if(indev_reset_check(indev)) return;
    }

    /*If a new object was found reset some variables and send a pressed event handler*/
    if(indev_obj_act != indev->pointer.act_obj) {
        indev->pointer.last_point.x = indev->pointer.act_point.x;
        indev->pointer.last_point.y = indev->pointer.act_point.y;

        /*Without `LV_OBJ_FLAG_PRESS_LOCK` new widget can be found while pressing.*/
        if(indev->pointer.last_hovered && indev->pointer.last_hovered != indev_obj_act) {
            lv_obj_send_event(indev->pointer.last_hovered, LV_EVENT_HOVER_LEAVE, indev);
            if(indev_reset_check(indev)) return;

            lv_indev_send_event(indev, LV_EVENT_HOVER_LEAVE, indev->pointer.last_hovered);
            if(indev_reset_check(indev)) return;

            indev->pointer.last_hovered = indev_obj_act;
        }

        /*If a new object found the previous was lost, so send a PRESS_LOST event*/
        if(indev->pointer.act_obj != NULL) {
            /*Save the obj because in special cases `act_obj` can change in the event */
            lv_obj_t * last_obj = indev->pointer.act_obj;

            lv_obj_send_event(last_obj, LV_EVENT_PRESS_LOST, indev_act);
            if(indev_reset_check(indev)) return;
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
            indev->pointer.scroll_obj = NULL;
            indev->pointer.gesture_dir = LV_DIR_NONE;
            indev->pointer.gesture_sent   = 0;
            indev->pointer.gesture_sum.x  = 0;
            indev->pointer.gesture_sum.y  = 0;
            indev->pointer.press_moved    = 0;
            indev->pointer.vect.x         = 0;
            indev->pointer.vect.y         = 0;

            const bool is_enabled = !lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);
            if(is_enabled) {
                if(indev->pointer.last_hovered != indev_obj_act) {
                    if(send_event(LV_EVENT_HOVER_OVER, indev_act) == LV_RESULT_INVALID) return;
                }
                if(send_event(LV_EVENT_PRESSED, indev_act) == LV_RESULT_INVALID) return;
            }

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

    if(LV_ABS(indev->pointer.vect.x) > indev->scroll_limit || LV_ABS(indev->pointer.vect.y) > indev->scroll_limit) {
        indev->pointer.press_moved = 1;
    }

    /* Send a gesture event to a potential indev cb callback, even if no object was found */
    if(indev->gesture_type != LV_INDEV_GESTURE_NONE) {
        lv_indev_send_event(indev, LV_EVENT_GESTURE, indev_act);
    }

    if(indev_obj_act) {
        const bool is_enabled = !lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);

        if(indev->gesture_type != LV_INDEV_GESTURE_NONE) {
            /* NOTE: hardcoded to pinch for now */
            if(send_event(LV_EVENT_GESTURE, indev_act) == LV_RESULT_INVALID) return;
        }

        if(is_enabled) {
            if(send_event(LV_EVENT_PRESSING, indev_act) == LV_RESULT_INVALID) return;
        }


        if(indev_act->wait_until_release) return;

        if(indev->pointer.scroll_obj) {
            lv_obj_stop_scroll_anim(indev->pointer.scroll_obj);
        }

        lv_indev_scroll_handler(indev);
        if(indev_reset_check(indev)) return;
        indev_gesture(indev);
        if(indev_reset_check(indev)) return;

        if(indev->mode == LV_INDEV_MODE_EVENT && indev->read_timer && lv_timer_get_paused(indev->read_timer)) {
            lv_timer_resume(indev->read_timer);
        }

        /*If there is no scrolling then check for long press time*/
        if(indev->pointer.scroll_obj == NULL && indev->long_pr_sent == 0) {
            /*Send a long press event if enough time elapsed*/
            if(lv_tick_elaps(indev->pr_timestamp) > indev_act->long_press_time) {
                if(is_enabled) {
                    if(send_event(LV_EVENT_LONG_PRESSED, indev_act) == LV_RESULT_INVALID) return;
                }
                /*Mark it to do not send the event again*/
                indev->long_pr_sent = 1;

                /*Save the long press time stamp for the long press repeat handler*/
                indev->longpr_rep_timestamp = lv_tick_get();
            }
        }

        if(indev->pointer.scroll_obj == NULL && indev->long_pr_sent == 1) {
            if(lv_tick_elaps(indev->longpr_rep_timestamp) > indev_act->long_press_repeat_time) {
                if(is_enabled) {
                    if(send_event(LV_EVENT_LONG_PRESSED_REPEAT, indev_act) == LV_RESULT_INVALID) return;
                }
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
    if(indev->wait_until_release || /*Hover the new widget even if the coordinates didn't changed*/
       (indev->pointer.last_point.x != indev->pointer.act_point.x ||
        indev->pointer.last_point.y != indev->pointer.act_point.y)) {
        lv_obj_t ** last = &indev->pointer.last_hovered;
        lv_obj_t * hovered = pointer_search_obj(lv_display_get_default(), &indev->pointer.act_point);
        if(*last != hovered) {
            lv_obj_send_event(hovered, LV_EVENT_HOVER_OVER, indev);
            if(indev_reset_check(indev)) return;
            lv_indev_send_event(indev, LV_EVENT_HOVER_OVER, hovered);
            if(indev_reset_check(indev)) return;

            lv_obj_send_event(*last, LV_EVENT_HOVER_LEAVE, indev);
            if(indev_reset_check(indev)) return;
            lv_indev_send_event(indev, LV_EVENT_HOVER_LEAVE, *last);
            if(indev_reset_check(indev)) return;
            *last = hovered;
        }
    }

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

    if(indev->mode == LV_INDEV_MODE_EVENT && indev->read_timer && !lv_timer_get_paused(indev->read_timer)) {
        lv_timer_pause(indev->read_timer);
    }

    /* Send a gesture event to a potential indev cb callback, even if no object was found */
    if(indev->gesture_type != LV_INDEV_GESTURE_NONE) {
        lv_indev_send_event(indev, LV_EVENT_GESTURE, indev_act);
    }

    if(indev_obj_act) {
        LV_LOG_INFO("released");

        const bool is_enabled = !lv_obj_has_state(indev_obj_act, LV_STATE_DISABLED);

        if(is_enabled && indev->gesture_type != LV_INDEV_GESTURE_NONE) {
            if(send_event(LV_EVENT_GESTURE, indev_act) == LV_RESULT_INVALID) return;
        }

        if(is_enabled) {
            if(send_event(LV_EVENT_RELEASED, indev_act) == LV_RESULT_INVALID) return;
        }

        if(is_enabled) {
            if(scroll_obj == NULL) {
                if(indev->long_pr_sent == 0) {
                    if(indev_proc_short_click(indev) == LV_RESULT_INVALID) return;
                }
                if(send_event(LV_EVENT_CLICKED, indev_act) == LV_RESULT_INVALID) return;
            }
            else {
                lv_obj_send_event(scroll_obj, LV_EVENT_SCROLL_THROW_BEGIN, indev_act);
                if(indev_reset_check(indev)) return;
            }
        }
        indev->pointer.act_obj = NULL;
        indev->pr_timestamp          = 0;
        indev->longpr_rep_timestamp  = 0;

        /*Get the transformed vector with this object*/
        if(scroll_obj) {
            int16_t angle = 0;
            int16_t scale_x = 256;
            int16_t scale_y = 256;
            lv_point_t pivot = { 0, 0 };
            lv_obj_t * parent = scroll_obj;
            while(parent) {
                angle += lv_obj_get_style_transform_rotation(parent, 0);
                int32_t zoom_act_x = lv_obj_get_style_transform_scale_x_safe(parent, 0);
                int32_t zoom_act_y = lv_obj_get_style_transform_scale_y_safe(parent, 0);
                scale_x = (scale_x * zoom_act_x) >> 8;
                scale_y = (scale_x * zoom_act_y) >> 8;
                parent = lv_obj_get_parent(parent);
            }

            if(scale_x == 0) {
                scale_x = 1;
            }

            if(scale_y == 0) {
                scale_y = 1;
            }

            if(angle != 0 || scale_y != LV_SCALE_NONE || scale_x != LV_SCALE_NONE) {
                angle = -angle;
                scale_x = (256 * 256) / scale_x;
                scale_y = (256 * 256) / scale_y;
                lv_point_transform(&indev->pointer.scroll_throw_vect, angle, scale_x, scale_y, &pivot, false);
                lv_point_transform(&indev->pointer.scroll_throw_vect_ori, angle, scale_x, scale_y, &pivot, false);
            }
        }
    }

    if(scroll_obj) {
        if(!indev->scroll_throw_anim) {
            indev_scroll_throw_anim_start(indev);
        }

        if(indev_reset_check(indev)) return;
    }
}

static lv_result_t indev_proc_short_click(lv_indev_t * indev)
{
    /*Update streak for clicks within small distance and short time*/
    indev->pointer.short_click_streak++;
    if(lv_tick_elaps(indev->pointer.last_short_click_timestamp) > indev->long_press_time) {
        indev->pointer.short_click_streak = 1;
    }
    else if(indev->type == LV_INDEV_TYPE_POINTER || indev->type == LV_INDEV_TYPE_BUTTON) {
        int32_t dx = indev->pointer.last_short_click_point.x - indev->pointer.act_point.x;
        int32_t dy = indev->pointer.last_short_click_point.y - indev->pointer.act_point.y;
        if(dx * dx + dy * dy > indev->scroll_limit * indev->scroll_limit) indev->pointer.short_click_streak = 1;
    }

    indev->pointer.last_short_click_timestamp = lv_tick_get();
    lv_indev_get_point(indev, &indev->pointer.last_short_click_point);

    /*Simple short click*/
    lv_result_t res = send_event(LV_EVENT_SHORT_CLICKED, indev_act);
    if(res == LV_RESULT_INVALID) {
        return res;
    }

    /*Cycle through single/double/triple click*/
    switch((indev->pointer.short_click_streak - 1) % 3) {
        case 0:
            return send_event(LV_EVENT_SINGLE_CLICKED, indev_act);
        case 1:
            return send_event(LV_EVENT_DOUBLE_CLICKED, indev_act);
        case 2:
            return send_event(LV_EVENT_TRIPLE_CLICKED, indev_act);
    }
    return res;
}

static void indev_proc_pointer_diff(lv_indev_t * indev)
{
    lv_obj_t * obj = indev->pointer.last_pressed;
    if(obj == NULL) return;
    if(indev->pointer.diff == 0) return;

    indev_obj_act = obj;

    bool editable = lv_obj_is_editable(obj);

    if(editable) {
        uint32_t indev_sensitivity = indev->rotary_sensitivity;
        uint32_t obj_sensitivity = lv_obj_get_style_rotary_sensitivity(indev_obj_act, 0);
        int32_t diff = (int32_t)((int32_t)indev->pointer.diff * indev_sensitivity * obj_sensitivity + 32768) >> 16;
        send_event(LV_EVENT_ROTARY, &diff);
    }
    else {

        int32_t vect = indev->pointer.diff > 0 ? indev->scroll_limit : -indev->scroll_limit;
        indev->pointer.vect.y = vect;
        indev->pointer.act_obj = obj;
        lv_obj_t * scroll_obj = lv_indev_find_scroll_obj(indev);
        if(scroll_obj == NULL) return;
        uint32_t indev_sensitivity = indev->rotary_sensitivity;
        uint32_t obj_sensitivity = lv_obj_get_style_rotary_sensitivity(scroll_obj, 0);
        int32_t diff = (int32_t)((int32_t)indev->pointer.diff * indev_sensitivity * obj_sensitivity + 32768) >> 16;

        indev->pointer.scroll_throw_vect.y = diff;
        indev->pointer.scroll_throw_vect_ori.y = diff;
        lv_indev_scroll_handler(indev);
    }

}

static lv_obj_t * pointer_search_obj(lv_display_t * disp, lv_point_t * p)
{
    indev_obj_act = lv_indev_search_obj(lv_display_get_layer_sys(disp), p);
    if(indev_obj_act) return indev_obj_act;

    indev_obj_act = lv_indev_search_obj(lv_display_get_layer_top(disp), p);
    if(indev_obj_act) return indev_obj_act;

    /* Search the object in the active screen */
    indev_obj_act = lv_indev_search_obj(lv_display_get_screen_active(disp), p);
    if(indev_obj_act) return indev_obj_act;

    indev_obj_act = lv_indev_search_obj(lv_display_get_layer_bottom(disp), p);
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
        indev->pointer.scroll_obj        = NULL;
        indev->pointer.last_hovered      = NULL;
        indev->long_pr_sent                    = 0;
        indev->pr_timestamp                    = 0;
        indev->longpr_rep_timestamp            = 0;
        indev->pointer.scroll_sum.x        = 0;
        indev->pointer.scroll_sum.y        = 0;
        indev->pointer.scroll_dir = LV_DIR_NONE;
        indev->pointer.scroll_obj = NULL;
        indev->pointer.scroll_throw_vect.x = 0;
        indev->pointer.scroll_throw_vect.y = 0;
        indev->pointer.gesture_sum.x     = 0;
        indev->pointer.gesture_sum.y     = 0;
        indev->reset_query                     = 0;
        indev->stop_processing_query           = 0;
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
            if(indev->pointer.last_pressed != indev_obj_act) {
                lv_obj_send_event(indev->pointer.last_pressed, LV_EVENT_DEFOCUSED, indev_act);
                if(indev_reset_check(indev)) return;

                lv_obj_send_event(indev_obj_act, LV_EVENT_FOCUSED, indev_act);
                if(indev_reset_check(indev)) return;
            }
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

        lv_indev_send_event(indev_act, LV_EVENT_GESTURE, gesture_obj);
        if(indev_reset_check(indev_act)) return;
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

    return indev->reset_query;
}

/**
 * Checks if the stop_processing_query flag has been set. If so, do not send any events to the object
 * @param indev pointer to an input device
 * @return true if indev should stop processing the event.
 */
static bool indev_stop_processing_check(lv_indev_t * indev)
{
    return indev->stop_processing_query;
}

/**
 * Reset the indev and send event to active obj and scroll obj
 * @param indev pointer to an input device
 * @param obj pointer to obj
*/
static void indev_reset_core(lv_indev_t * indev, lv_obj_t * obj)
{
    lv_obj_t * act_obj = NULL;
    lv_obj_t * scroll_obj = NULL;

    indev->reset_query = 1;
    if(indev_act == indev) indev_obj_act = NULL;
    if(indev->type == LV_INDEV_TYPE_POINTER || indev->type == LV_INDEV_TYPE_KEYPAD) {
        if(obj == NULL || indev->pointer.last_pressed == obj) {
            indev->pointer.last_pressed = NULL;
        }
        if(obj == NULL || indev->pointer.act_obj == obj) {
            if(indev->pointer.act_obj) {
                /* Avoid recursive calls */
                act_obj = indev->pointer.act_obj;
                indev->pointer.act_obj = NULL;
                lv_obj_send_event(act_obj, LV_EVENT_INDEV_RESET, indev);
                lv_indev_send_event(indev, LV_EVENT_INDEV_RESET, act_obj);
                act_obj = NULL;
            }
        }
        if(obj == NULL || indev->pointer.last_obj == obj) {
            indev->pointer.last_obj = NULL;
        }
        if(obj == NULL || indev->pointer.scroll_obj == obj) {
            if(indev->pointer.scroll_obj) {
                /* Avoid recursive calls */
                scroll_obj = indev->pointer.scroll_obj;
                indev->pointer.scroll_obj = NULL;
                lv_obj_send_event(scroll_obj, LV_EVENT_INDEV_RESET, indev);
                lv_indev_send_event(indev, LV_EVENT_INDEV_RESET, act_obj);
                scroll_obj = NULL;
            }
        }
        if(obj == NULL || indev->pointer.last_hovered == obj) {
            indev->pointer.last_hovered = NULL;
        }
    }
}

static lv_result_t send_event(lv_event_code_t code, void * param)
{
    lv_indev_t * indev = indev_act;

    if(code == LV_EVENT_PRESSED ||
       code == LV_EVENT_SHORT_CLICKED ||
       code == LV_EVENT_CLICKED ||
       code == LV_EVENT_RELEASED ||
       code == LV_EVENT_LONG_PRESSED ||
       code == LV_EVENT_LONG_PRESSED_REPEAT ||
       code == LV_EVENT_ROTARY) {
        lv_indev_send_event(indev, code, indev_obj_act);
        if(indev_reset_check(indev)) return LV_RESULT_INVALID;

        if(indev_stop_processing_check(indev)) {
            /* Not send event to the object if stop processing query is set */
            indev->stop_processing_query = 0;
            return LV_RESULT_OK;
        }
    }

    lv_obj_send_event(indev_obj_act, code, param);
    if(indev_reset_check(indev)) return LV_RESULT_INVALID;

    return LV_RESULT_OK;
}

static void indev_scroll_throw_anim_cb(void * var, int32_t v)
{
    LV_ASSERT_NULL(var);
    LV_UNUSED(v);
    lv_indev_t * indev = (lv_indev_t *)var;

    lv_indev_scroll_throw_handler(indev);

    if(indev->pointer.scroll_dir == LV_DIR_NONE || indev->pointer.scroll_obj == NULL) {
        if(indev->scroll_throw_anim) {
            LV_LOG_INFO("stop animation");
            lv_anim_delete(indev, indev_scroll_throw_anim_cb);
        }
    }
}

static void indev_scroll_throw_anim_completed_cb(lv_anim_t * anim)
{
    if(anim) {
        indev_scroll_throw_anim_reset((lv_indev_t *)anim->var);
    }
}

static void indev_scroll_throw_anim_start(lv_indev_t * indev)
{
    LV_ASSERT_NULL(indev);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, indev);
    lv_anim_set_duration(&a, 1024);
    lv_anim_set_values(&a, 0, 1024);
    lv_anim_set_exec_cb(&a, indev_scroll_throw_anim_cb);
    lv_anim_set_completed_cb(&a, indev_scroll_throw_anim_completed_cb);
    lv_anim_set_deleted_cb(&a, indev_scroll_throw_anim_completed_cb);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    indev->scroll_throw_anim = lv_anim_start(&a);
}
