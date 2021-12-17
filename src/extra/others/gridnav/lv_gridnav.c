

/**
 * @file lv_gridnav.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gridnav.h"
#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_math.h"
#include "../../../core/lv_indev.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_gridnav_ctrl_t ctrl;
    lv_obj_t * focused_obj;
} lv_gridnav_dsc_t;

typedef enum {
    FIND_LEFT,
    FIND_RIGHT,
    FIND_TOP,
    FIND_BOTTOM,
    FIND_NEXT_ROW_FIRST_ITEM,
    FIND_PREV_ROW_LAST_ITEM,
    FIND_FIRST_ROW,
    FIND_LAST_ROW,
} find_mode_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void gridnav_event_cb(lv_event_t * e);
static lv_obj_t * find_chid(lv_obj_t * obj, lv_obj_t * start_child, find_mode_t mode);
static lv_coord_t get_x_center(lv_obj_t * obj);
static lv_coord_t get_y_center(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gridnav_add(lv_obj_t * obj, lv_gridnav_ctrl_t ctrl)
{
    lv_gridnav_dsc_t * dsc = lv_mem_alloc(sizeof(lv_gridnav_dsc_t));
    LV_ASSERT_MALLOC(dsc);
    dsc->ctrl = ctrl;
    dsc->focused_obj = NULL;
    lv_obj_add_event_cb(obj, gridnav_event_cb, LV_EVENT_ALL, dsc);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
}

void lv_gridnav_remove(lv_obj_t * obj)
{
    lv_gridnav_dsc_t * dsc = lv_obj_get_event_user_data(obj, gridnav_event_cb);
    lv_mem_free(dsc);
    lv_obj_remove_event_cb(obj, gridnav_event_cb);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void gridnav_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_gridnav_dsc_t * dsc = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_KEY) {
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        if(child_cnt == 0) return;

        uint32_t key = lv_indev_get_key(lv_indev_get_act());
        lv_obj_t * guess = NULL;
        if(key == LV_KEY_RIGHT) {
            guess = find_chid(obj, dsc->focused_obj, FIND_RIGHT);
            if(guess == NULL) {
                if(dsc->ctrl & LV_GRIDNAV_CTRL_ROLLOVER) {
                    guess = find_chid(obj, dsc->focused_obj, FIND_NEXT_ROW_FIRST_ITEM);
                    if(guess == NULL) guess = lv_obj_get_child(obj, 0);
                }
                else {
                    lv_group_focus_next(lv_obj_get_group(obj));
                }
            }
        }
        else if(key == LV_KEY_LEFT) {
            guess = find_chid(obj, dsc->focused_obj, FIND_LEFT);
            if(guess == NULL) {
                if(dsc->ctrl & LV_GRIDNAV_CTRL_ROLLOVER) {
                    guess = find_chid(obj, dsc->focused_obj, FIND_PREV_ROW_LAST_ITEM);
                    /*Last item*/
                    if(guess == NULL) guess = lv_obj_get_child(obj, -1);
                }
                else {
                    lv_group_focus_prev(lv_obj_get_group(obj));
                }
            }
        }
        else if(key == LV_KEY_DOWN) {
            guess = find_chid(obj, dsc->focused_obj, FIND_BOTTOM);
            if(guess == NULL) {
                if(dsc->ctrl & LV_GRIDNAV_CTRL_ROLLOVER) {
                    guess = find_chid(obj, dsc->focused_obj, FIND_FIRST_ROW);
                }
                else {
                    lv_group_focus_next(lv_obj_get_group(obj));
                }
            }
        }
        else if(key == LV_KEY_UP) {
            guess = find_chid(obj, dsc->focused_obj, FIND_TOP);
            if(guess == NULL) {
                if(dsc->ctrl & LV_GRIDNAV_CTRL_ROLLOVER) {
                    guess = find_chid(obj, dsc->focused_obj, FIND_LAST_ROW);
                }
                else {
                    lv_group_focus_prev(lv_obj_get_group(obj));
                }
            }
        }
        else {
            lv_event_send(dsc->focused_obj, LV_EVENT_KEY, &key);
        }

        if(guess && guess != dsc->focused_obj) {
            lv_obj_clear_state(dsc->focused_obj, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
            lv_obj_add_state(guess, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
            lv_obj_scroll_to_view(guess, LV_ANIM_ON);
            dsc->focused_obj = guess;
        }
    }
    else if(code == LV_EVENT_FOCUSED) {
        if(dsc->focused_obj) {
            lv_obj_add_state(dsc->focused_obj, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
            lv_obj_scroll_to_view(dsc->focused_obj, LV_ANIM_OFF);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        if(dsc->focused_obj) {
            lv_obj_clear_state(dsc->focused_obj, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
        }
    }
    else if(code == LV_EVENT_CHILD_CREATED) {
        if(lv_obj_has_state(obj, LV_STATE_FOCUSED)) {
            lv_obj_t * child = lv_event_get_target(e);
            if(lv_obj_get_parent(child) == obj) {
                if(dsc->focused_obj == NULL) {
                    dsc->focused_obj = child;
                    lv_obj_add_state(child, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
                    lv_obj_scroll_to_view(child, LV_ANIM_OFF);
                }
            }
        }
    }
    else if(code == LV_EVENT_CHILD_DELETED) {
        /*This event bubble, so be sure this object's child was deleted.
         *As we don't know which object was deleted we can't make the next focused.
         *So make the first object focused*/
        lv_obj_t * target = lv_event_get_target(e);
        if(target == obj) {
            dsc->focused_obj = lv_obj_get_child(obj, 0);
        }
    }
    else if(code == LV_EVENT_DELETE) {
        lv_gridnav_remove(obj);
    }
    else if(code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING || code == LV_EVENT_PRESS_LOST ||
            code == LV_EVENT_LONG_PRESSED || code == LV_EVENT_LONG_PRESSED_REPEAT ||
            code == LV_EVENT_CLICKED || LV_EVENT_RELEASED) {
        /*Forward press/release related event too*/
        lv_indev_type_t t = lv_indev_get_type(lv_indev_get_act());
        if(t == LV_INDEV_TYPE_ENCODER || t == LV_INDEV_TYPE_KEYPAD) {
            lv_event_send(dsc->focused_obj, code, lv_indev_get_act());
        }
    }
}

static lv_obj_t * find_chid(lv_obj_t * obj, lv_obj_t * start_child, find_mode_t mode)
{
    lv_coord_t x_start = get_x_center(start_child);
    lv_coord_t y_start = get_y_center(start_child);
    uint32_t child_cnt = lv_obj_get_child_cnt(obj);
    lv_obj_t * guess = NULL;
    lv_coord_t x_err_guess = LV_COORD_MAX;
    lv_coord_t y_err_guess = LV_COORD_MAX;
    lv_coord_t h_half = lv_obj_get_height(start_child) / 2;
    lv_coord_t h_max = lv_obj_get_height(obj) + lv_obj_get_scroll_top(obj) + lv_obj_get_scroll_bottom(obj);
    uint32_t i;
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(child == start_child) continue;
        if(lv_obj_has_flag(child, LV_OBJ_FLAG_CLICK_FOCUSABLE) == false) continue;

        lv_coord_t x_err;
        lv_coord_t y_err;
        switch(mode) {
            case FIND_LEFT:
                x_err = get_x_center(child) - x_start;
                y_err = get_y_center(child) - y_start;
                if(x_err >= 0) continue;    /*It's on the right*/
                if(LV_ABS(y_err) > h_half) continue;    /*Too far*/
                break;
            case FIND_RIGHT:
                x_err = get_x_center(child) - x_start;
                y_err = get_y_center(child) - y_start;
                if(x_err <= 0) continue;    /*It's on the left*/
                if(LV_ABS(y_err) > h_half) continue;    /*Too far*/
                break;
            case FIND_TOP:
                x_err = get_x_center(child) - x_start;
                y_err = get_y_center(child) - y_start;
                if(y_err >= 0) continue;    /*It's on the bottom*/
                break;
            case FIND_BOTTOM:
                x_err = get_x_center(child) - x_start;
                y_err = get_y_center(child) - y_start;
                if(y_err <= 0) continue;    /*It's on the top*/
                break;
            case FIND_NEXT_ROW_FIRST_ITEM:
                y_err = get_y_center(child) - y_start;
                if(y_err <= 0) continue;    /*It's on the top*/
                x_err = lv_obj_get_x(child);
                break;
            case FIND_PREV_ROW_LAST_ITEM:
                y_err = get_y_center(child) - y_start;
                if(y_err >= 0) continue;    /*It's on the bottom*/
                x_err = obj->coords.x2 - child->coords.x2;
                break;
            case FIND_FIRST_ROW:
                x_err = get_x_center(child) - x_start;
                y_err = lv_obj_get_y(child);
                break;
            case FIND_LAST_ROW:
                x_err = get_x_center(child) - x_start;
                y_err = h_max - lv_obj_get_y(child);
        }

        if(guess == NULL ||
           (y_err * y_err + x_err * x_err < y_err_guess * y_err_guess + x_err_guess * x_err_guess)) {
            guess = child;
            x_err_guess  = x_err;
            y_err_guess  = y_err;
        }
    }
    return guess;
}

static lv_coord_t get_x_center(lv_obj_t * obj)
{
    return obj->coords.x1 + lv_area_get_width(&obj->coords) / 2;
}

static lv_coord_t get_y_center(lv_obj_t * obj)
{
    return obj->coords.y1 + lv_area_get_height(&obj->coords) / 2;
}
