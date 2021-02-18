/**
 * @file lv_indev.c
 *
 */

/*********************
 *      INCLUDES
 ********************/
#include "lv_indev.h"
#include "lv_disp.h"
#include "lv_obj.h"
#include "lv_indev_scroll.h"
#include "lv_group.h"
#include "lv_refr.h"

#include "../lv_hal/lv_hal_tick.h"
#include "../lv_misc/lv_timer.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

#if LV_INDEV_DEF_DRAG_THROW <= 0
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
static void indev_proc_press(lv_indev_proc_t * proc);
static void indev_proc_release(lv_indev_proc_t * proc);
static void indev_proc_reset_query_handler(lv_indev_t * indev);
static void indev_click_focus(lv_indev_proc_t * proc);
static void indev_gesture(lv_indev_proc_t * proc);
static bool indev_reset_check(lv_indev_proc_t * proc);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_indev_t * indev_act;
static lv_obj_t * indev_obj_act = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_indev_read_task_cb(lv_timer_t * task)
{
    LV_LOG_TRACE("indev read task started");

    lv_indev_data_t data;

    indev_act = task->user_data;

    /*Read and process all indevs*/
    if(indev_act->driver.disp == NULL) return; /*Not assigned to any displays*/

    /*Handle reset query before processing the point*/
    indev_proc_reset_query_handler(indev_act);

    if(indev_act->proc.disabled) return;
    bool more_to_read;
    do {
        /*Read the data*/
        more_to_read = _lv_indev_read(indev_act, &data);

        /*The active object might deleted even in the read function*/
        indev_proc_reset_query_handler(indev_act);
        indev_obj_act = NULL;

        indev_act->proc.state = data.state;

        /*Save the last activity time*/
        if(indev_act->proc.state == LV_INDEV_STATE_PR) {
            indev_act->driver.disp->last_activity_time = lv_tick_get();
        }
        else if(indev_act->driver.type == LV_INDEV_TYPE_ENCODER && data.enc_diff) {
            indev_act->driver.disp->last_activity_time = lv_tick_get();
        }

        if(indev_act->driver.type == LV_INDEV_TYPE_POINTER) {
            indev_pointer_proc(indev_act, &data);
        }
        else if(indev_act->driver.type == LV_INDEV_TYPE_KEYPAD) {
            indev_keypad_proc(indev_act, &data);
        }
        else if(indev_act->driver.type == LV_INDEV_TYPE_ENCODER) {
            indev_encoder_proc(indev_act, &data);
        }
        else if(indev_act->driver.type == LV_INDEV_TYPE_BUTTON) {
            indev_button_proc(indev_act, &data);
        }
        /*Handle reset query if it happened in during processing*/
        indev_proc_reset_query_handler(indev_act);
    } while(more_to_read);

    /*End of indev processing, so no act indev*/
    indev_act     = NULL;
    indev_obj_act = NULL;

    LV_LOG_TRACE("indev read task finished");
}

void lv_indev_enable(lv_indev_t * indev, bool en)
{
    if(!indev) return;

    indev->proc.disabled = en ? 0 : 1;
}

lv_indev_t * lv_indev_get_act(void)
{
    return indev_act;
}

lv_indev_type_t lv_indev_get_type(const lv_indev_t * indev)
{
    if(indev == NULL) return LV_INDEV_TYPE_NONE;

    return indev->driver.type;
}

void lv_indev_reset(lv_indev_t * indev, lv_obj_t * obj)
{
    if(indev) {
        indev->proc.reset_query = 1;
        if(indev_act == indev) indev_obj_act = NULL;
        if(obj == NULL || indev->proc.types.pointer.last_pressed == obj) {
            indev->proc.types.pointer.last_pressed = NULL;
        }
    }
    else {
        lv_indev_t * i = lv_indev_get_next(NULL);
        while(i) {
            i->proc.reset_query = 1;
            if((i->driver.type == LV_INDEV_TYPE_POINTER || i->driver.type == LV_INDEV_TYPE_KEYPAD) &&
               (obj == NULL || i->proc.types.pointer.last_pressed == obj)) {
                i->proc.types.pointer.last_pressed = NULL;
            }
            i = lv_indev_get_next(i);
        }
        indev_obj_act = NULL;
    }
}

void lv_indev_reset_long_press(lv_indev_t * indev)
{
    indev->proc.long_pr_sent         = 0;
    indev->proc.longpr_rep_timestamp = lv_tick_get();
    indev->proc.pr_timestamp         = lv_tick_get();
}

void lv_indev_set_cursor(lv_indev_t * indev, lv_obj_t * cur_obj)
{
    if(indev->driver.type != LV_INDEV_TYPE_POINTER) return;

    indev->cursor = cur_obj;
    lv_obj_set_parent(indev->cursor, lv_disp_get_layer_sys(indev->driver.disp));
    lv_obj_set_pos(indev->cursor, indev->proc.types.pointer.act_point.x, indev->proc.types.pointer.act_point.y);
    lv_obj_clear_flag(indev->cursor, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(indev->cursor, LV_OBJ_FLAG_LAYOUTABLE);
}

void lv_indev_set_group(lv_indev_t * indev, lv_group_t * group)
{
    if(indev->driver.type == LV_INDEV_TYPE_KEYPAD || indev->driver.type == LV_INDEV_TYPE_ENCODER) {
        indev->group = group;
    }
}

void lv_indev_set_button_points(lv_indev_t * indev, const lv_point_t points[])
{
    if(indev->driver.type == LV_INDEV_TYPE_BUTTON) {
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
    if(indev->driver.type != LV_INDEV_TYPE_POINTER && indev->driver.type != LV_INDEV_TYPE_BUTTON) {
        point->x = -1;
        point->y = -1;
    }
    else {
        point->x = indev->proc.types.pointer.act_point.x;
        point->y = indev->proc.types.pointer.act_point.y;
    }
}

lv_gesture_dir_t lv_indev_get_gesture_dir(const lv_indev_t * indev)
{
    return indev->proc.types.pointer.gesture_dir;
}

uint32_t lv_indev_get_key(const lv_indev_t * indev)
{
    if(indev->driver.type != LV_INDEV_TYPE_KEYPAD)
        return 0;
    else
        return indev->proc.types.keypad.last_key;
}

lv_indev_scroll_dir_t lv_indev_get_scroll_dir(const lv_indev_t * indev)
{
    if(indev == NULL) return false;
    if(indev->driver.type != LV_INDEV_TYPE_POINTER && indev->driver.type != LV_INDEV_TYPE_BUTTON) return false;
    return indev->proc.types.pointer.scroll_dir;
}

lv_obj_t * lv_indev_get_scroll_obj(const lv_indev_t * indev)
{
    if(indev == NULL) return NULL;
    if(indev->driver.type != LV_INDEV_TYPE_POINTER && indev->driver.type != LV_INDEV_TYPE_BUTTON) return NULL;
    return indev->proc.types.pointer.scroll_obj;
}

void lv_indev_get_vect(const lv_indev_t * indev, lv_point_t * point)
{
    point->x = 0;
    point->y = 0;

    if(indev == NULL) return;

    if(indev->driver.type == LV_INDEV_TYPE_POINTER || indev->driver.type == LV_INDEV_TYPE_BUTTON) {
        point->x = indev->proc.types.pointer.vect.x;
        point->y = indev->proc.types.pointer.vect.y;
    }
}

void lv_indev_wait_release(lv_indev_t * indev)
{
    if(indev == NULL)return;
    indev->proc.wait_until_release = 1;
}

lv_obj_t * lv_indev_get_obj_act(void)
{
    return indev_obj_act;
}

lv_timer_t * lv_indev_get_read_task(lv_disp_t * indev)
{
    if(!indev) {
        LV_LOG_WARN("lv_indev_get_read_task: indev was NULL");
        return NULL;
    }

    return indev->read_task;
}

lv_obj_t * lv_indev_search_obj(lv_obj_t * obj, lv_point_t * point)
{
    lv_obj_t * found_p = NULL;

    /*If the point is on this object check its children too*/
    if(lv_obj_hit_test(obj, point)) {
        int32_t i;
        for(i = lv_obj_get_child_cnt(obj) - 1; i >= 0; i--) {
            lv_obj_t * child = lv_obj_get_child(obj, i);
            found_p = lv_indev_search_obj(child, point);

            /*If a child was found then break*/
            if(found_p != NULL) break;
        }

        /*If then the children was not ok, and this obj is clickable
         * and it or its parent is not hidden then save this object*/
        if(found_p == NULL && lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE)) {
            lv_obj_t * hidden_i = obj;
            while(hidden_i != NULL) {
                if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN) == true) break;
                hidden_i = lv_obj_get_parent(hidden_i);
            }
            /*No parent found with hidden == true*/
            if(hidden_i == NULL && (lv_obj_get_state(obj) & LV_STATE_DISABLED) == false) found_p = obj;
        }
    }

    return found_p;
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
    /*Move the cursor if set and moved*/
    if(i->cursor != NULL &&
       (i->proc.types.pointer.last_point.x != data->point.x || i->proc.types.pointer.last_point.y != data->point.y)) {
        lv_obj_set_pos(i->cursor, data->point.x, data->point.y);
    }

    i->proc.types.pointer.act_point.x = data->point.x;
    i->proc.types.pointer.act_point.y = data->point.y;

    if(i->proc.state == LV_INDEV_STATE_PR) {
        indev_proc_press(&i->proc);
    }
    else {
        indev_proc_release(&i->proc);
    }

    i->proc.types.pointer.last_point.x = i->proc.types.pointer.act_point.x;
    i->proc.types.pointer.last_point.y = i->proc.types.pointer.act_point.y;
}

/**
 * Process a new point from LV_INDEV_TYPE_KEYPAD input device
 * @param i pointer to an input device
 * @param data pointer to the data read from the input device
 */
static void indev_keypad_proc(lv_indev_t * i, lv_indev_data_t * data)
{
    if(data->state == LV_INDEV_STATE_PR && i->proc.wait_until_release) return;

    if(i->proc.wait_until_release) {
        i->proc.wait_until_release      = 0;
        i->proc.pr_timestamp            = 0;
        i->proc.long_pr_sent            = 0;
        i->proc.types.keypad.last_state = LV_INDEV_STATE_REL; /*To skip the processing of release*/
    }

    lv_group_t * g = i->group;
    if(g == NULL) return;

    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    /*Save the last key to compare it with the current latter on RELEASE*/
    uint32_t prev_key = i->proc.types.keypad.last_key;

    /* Save the last key.
     * It must be done here else `lv_indev_get_key` will return the last key in events and signals*/
    i->proc.types.keypad.last_key = data->key;

    /* Save the previous state so we can detect state changes below and also set the last state now
     * so if any signal/event handler on the way returns `LV_RES_INV` the last state is remembered
     * for the next time*/
    uint32_t prev_state             = i->proc.types.keypad.last_state;
    i->proc.types.keypad.last_state = data->state;

    /*Key press happened*/
    if(data->state == LV_INDEV_STATE_PR && prev_state == LV_INDEV_STATE_REL) {
        i->proc.pr_timestamp = lv_tick_get();

        /*Simulate a press on the object if ENTER was pressed*/
        if(data->key == LV_KEY_ENTER) {
            /*Send the ENTER as a normal KEY*/
            lv_group_send_data(g, LV_KEY_ENTER);

            lv_signal_send(indev_obj_act, LV_SIGNAL_PRESSED, NULL);
            if(indev_reset_check(&i->proc)) return;
            lv_event_send(indev_obj_act, LV_EVENT_PRESSED, NULL);
            if(indev_reset_check(&i->proc)) return;
        }
        else if(data->key == LV_KEY_ESC) {
            /*Send the ESC as a normal KEY*/
            lv_group_send_data(g, LV_KEY_ESC);

            lv_event_send(indev_obj_act, LV_EVENT_CANCEL, NULL);
            if(indev_reset_check(&i->proc)) return;
        }
        /*Move the focus on NEXT*/
        else if(data->key == LV_KEY_NEXT) {
            lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
            lv_group_focus_next(g);
            if(indev_reset_check(&i->proc)) return;
        }
        /*Move the focus on PREV*/
        else if(data->key == LV_KEY_PREV) {
            lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
            lv_group_focus_prev(g);
            if(indev_reset_check(&i->proc)) return;
        }
        /*Just send other keys to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT`)*/
        else {
            lv_group_send_data(g, data->key);
        }
    }
    /*Pressing*/
    else if(data->state == LV_INDEV_STATE_PR && prev_state == LV_INDEV_STATE_PR) {

        if(data->key == LV_KEY_ENTER) {
            lv_signal_send(indev_obj_act, LV_SIGNAL_PRESSING, NULL);
            if(indev_reset_check(&i->proc)) return;
            lv_event_send(indev_obj_act, LV_EVENT_PRESSING, NULL);
            if(indev_reset_check(&i->proc)) return;
        }

        /*Long press time has elapsed?*/
        if(i->proc.long_pr_sent == 0 && lv_tick_elaps(i->proc.pr_timestamp) > i->driver.long_press_time) {
            i->proc.long_pr_sent = 1;
            if(data->key == LV_KEY_ENTER) {
                i->proc.longpr_rep_timestamp = lv_tick_get();
                lv_signal_send(indev_obj_act, LV_SIGNAL_LONG_PRESS, NULL);
                if(indev_reset_check(&i->proc)) return;
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED, NULL);
                if(indev_reset_check(&i->proc)) return;
            }
        }
        /*Long press repeated time has elapsed?*/
        else if(i->proc.long_pr_sent != 0 &&
                lv_tick_elaps(i->proc.longpr_rep_timestamp) > i->driver.long_press_rep_time) {

            i->proc.longpr_rep_timestamp = lv_tick_get();

            /*Send LONG_PRESS_REP on ENTER*/
            if(data->key == LV_KEY_ENTER) {
                lv_signal_send(indev_obj_act, LV_SIGNAL_LONG_PRESS_REP, NULL);
                if(indev_reset_check(&i->proc)) return;
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
                if(indev_reset_check(&i->proc)) return;
            }
            /*Move the focus on NEXT again*/
            else if(data->key == LV_KEY_NEXT) {
                lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
                lv_group_focus_next(g);
                if(indev_reset_check(&i->proc)) return;
            }
            /*Move the focus on PREV again*/
            else if(data->key == LV_KEY_PREV) {
                lv_group_set_editing(g, false); /*Editing is not used by KEYPAD is be sure it is disabled*/
                lv_group_focus_prev(g);
                if(indev_reset_check(&i->proc)) return;
            }
            /*Just send other keys again to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT)*/
            else {
                lv_group_send_data(g, data->key);
                if(indev_reset_check(&i->proc)) return;
            }
        }
    }
    /*Release happened*/
    else if(data->state == LV_INDEV_STATE_REL && prev_state == LV_INDEV_STATE_PR) {
        /*The user might clear the key when it was released. Always release the pressed key*/
        data->key = prev_key;
        if(data->key == LV_KEY_ENTER) {

            lv_signal_send(indev_obj_act, LV_SIGNAL_RELEASED, NULL);
            if(indev_reset_check(&i->proc)) return;

            if(i->proc.long_pr_sent == 0) {
                lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, NULL);
                if(indev_reset_check(&i->proc)) return;
            }

            lv_event_send(indev_obj_act, LV_EVENT_CLICKED, NULL);
            if(indev_reset_check(&i->proc)) return;

            lv_event_send(indev_obj_act, LV_EVENT_RELEASED, NULL);
            if(indev_reset_check(&i->proc)) return;
        }
        i->proc.pr_timestamp = 0;
        i->proc.long_pr_sent = 0;
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
    if(data->state == LV_INDEV_STATE_PR && i->proc.wait_until_release) return;

    if(i->proc.wait_until_release) {
        i->proc.wait_until_release      = 0;
        i->proc.pr_timestamp            = 0;
        i->proc.long_pr_sent            = 0;
        i->proc.types.keypad.last_state = LV_INDEV_STATE_REL; /*To skip the processing of release*/
    }

    /* Save the last keys before anything else.
     * They need to be already saved if the function returns for any reason*/
    lv_indev_state_t last_state     = i->proc.types.keypad.last_state;
    i->proc.types.keypad.last_state = data->state;
    i->proc.types.keypad.last_key   = data->key;

    lv_group_t * g = i->group;
    if(g == NULL) return;

    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    /*Process the steps they are valid only with released button*/
    if(data->state != LV_INDEV_STATE_REL) {
        data->enc_diff = 0;
    }

    /*Refresh the focused object. It might change due to lv_group_focus_prev/next*/
    indev_obj_act = lv_group_get_focused(g);
    if(indev_obj_act == NULL) return;

    /*Button press happened*/
    if(data->state == LV_INDEV_STATE_PR && last_state == LV_INDEV_STATE_REL) {

        i->proc.pr_timestamp = lv_tick_get();

        if(data->key == LV_KEY_ENTER) {
            bool editable = lv_obj_is_editable(indev_obj_act);

            if(lv_group_get_editing(g) == true || editable == false) {
                lv_signal_send(indev_obj_act, LV_SIGNAL_PRESSED, NULL);
                if(indev_reset_check(&i->proc)) return;

                lv_event_send(indev_obj_act, LV_EVENT_PRESSED, NULL);
                if(indev_reset_check(&i->proc)) return;
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

            lv_event_send(indev_obj_act, LV_EVENT_CANCEL, NULL);
            if(indev_reset_check(&i->proc)) return;
        }
        /*Just send other keys to the object (e.g. 'A' or `LV_GROUP_KEY_RIGHT`)*/
        else {
            lv_group_send_data(g, data->key);
        }
    }
    /*Pressing*/
    else if(data->state == LV_INDEV_STATE_PR && last_state == LV_INDEV_STATE_PR) {
        /* Long press*/
        if(i->proc.long_pr_sent == 0 && lv_tick_elaps(i->proc.pr_timestamp) > i->driver.long_press_time) {

            i->proc.long_pr_sent = 1;
            i->proc.longpr_rep_timestamp = lv_tick_get();

            if(data->key == LV_KEY_ENTER) {
                bool editable = lv_obj_is_editable(indev_obj_act);

                /*On enter long press toggle edit mode.*/
                if(editable) {
                    /*Don't leave edit mode if there is only one object (nowhere to navigate)*/
                    if(_lv_ll_get_len(&g->obj_ll) > 1) {
                        lv_group_set_editing(g, lv_group_get_editing(g) ? false : true); /*Toggle edit mode on long press*/
                    }
                }
                /*If not editable then just send a long press signal*/
                else {
                    lv_signal_send(indev_obj_act, LV_SIGNAL_LONG_PRESS, NULL);
                    if(indev_reset_check(&i->proc)) return;
                    lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED, NULL);
                    if(indev_reset_check(&i->proc)) return;
                }
            }

            i->proc.long_pr_sent = 1;
        }
        /*Long press repeated time has elapsed?*/
        else if(i->proc.long_pr_sent != 0 && lv_tick_elaps(i->proc.longpr_rep_timestamp) > i->driver.long_press_rep_time) {

            i->proc.longpr_rep_timestamp = lv_tick_get();

            if(data->key == LV_KEY_ENTER) {
                lv_signal_send(indev_obj_act, LV_SIGNAL_LONG_PRESS_REP, NULL);
                if(indev_reset_check(&i->proc)) return;
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
                if(indev_reset_check(&i->proc)) return;
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
                if(indev_reset_check(&i->proc)) return;
            }

        }

    }
    /*Release happened*/
    else if(data->state == LV_INDEV_STATE_REL && last_state == LV_INDEV_STATE_PR) {

        if(data->key == LV_KEY_ENTER) {
            bool editable = lv_obj_is_editable(indev_obj_act);

            /*The button was released on a non-editable object. Just send enter*/
            if(editable == false) {
                lv_signal_send(indev_obj_act, LV_SIGNAL_RELEASED, NULL);
                if(indev_reset_check(&i->proc)) return;

                if(i->proc.long_pr_sent == 0) lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, NULL);
                if(indev_reset_check(&i->proc)) return;

                lv_event_send(indev_obj_act, LV_EVENT_CLICKED, NULL);
                if(indev_reset_check(&i->proc)) return;

                lv_event_send(indev_obj_act, LV_EVENT_RELEASED, NULL);
                if(indev_reset_check(&i->proc)) return;
            }
            /*An object is being edited and the button is released. */
            else if(g->editing) {
                /*Ignore long pressed enter release because it comes from mode switch*/
                if(!i->proc.long_pr_sent || _lv_ll_get_len(&g->obj_ll) <= 1) {
                    lv_signal_send(indev_obj_act, LV_SIGNAL_RELEASED, NULL);
                    if(indev_reset_check(&i->proc)) return;

                    lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, NULL);
                    if(indev_reset_check(&i->proc)) return;

                    lv_event_send(indev_obj_act, LV_EVENT_CLICKED, NULL);
                    if(indev_reset_check(&i->proc)) return;

                    lv_event_send(indev_obj_act, LV_EVENT_RELEASED, NULL);
                    if(indev_reset_check(&i->proc)) return;

                    lv_group_send_data(g, LV_KEY_ENTER);
                }
            }
            /*If the focused object is editable and now in navigate mode then on enter switch edit
               mode*/
            else if(editable && !g->editing && !i->proc.long_pr_sent) {
                lv_group_set_editing(g, true); /*Set edit mode*/
            }
        }

        i->proc.pr_timestamp = 0;
        i->proc.long_pr_sent = 0;
    }
    indev_obj_act = NULL;

    /*if encoder steps or simulated steps via left/right keys*/
    if(data->enc_diff != 0) {
        /*In edit mode send LEFT/RIGHT keys*/
        if(lv_group_get_editing(g)) {
            int32_t s;
            if(data->enc_diff < 0) {
                for(s = 0; s < -data->enc_diff; s++) lv_group_send_data(g, LV_KEY_LEFT);
            }
            else if(data->enc_diff > 0) {
                for(s = 0; s < data->enc_diff; s++) lv_group_send_data(g, LV_KEY_RIGHT);
            }
        }
        /*In navigate mode focus on the next/prev objects*/
        else {
            int32_t s;
            if(data->enc_diff < 0) {
                for(s = 0; s < -data->enc_diff; s++) lv_group_focus_prev(g);
            }
            else if(data->enc_diff > 0) {
                for(s = 0; s < data->enc_diff; s++) lv_group_focus_next(g);
            }
        }
    }
}

/**
 * Process new points from a input device. indev->state.pressed has to be set
 * @param indev pointer to an input device state
 * @param x x coordinate of the next point
 * @param y y coordinate of the next point
 */
static void indev_button_proc(lv_indev_t * i, lv_indev_data_t * data)
{
    /* Die gracefully if i->btn_points is NULL */
    if(i->btn_points == NULL) {
        LV_LOG_WARN("indev_button_proc: btn_points was  NULL");
        return;
    }

    lv_coord_t x = i->btn_points[data->btn_id].x;
    lv_coord_t y = i->btn_points[data->btn_id].y;

    /*If a new point comes always make a release*/
    if(data->state == LV_INDEV_STATE_PR) {
        if(i->proc.types.pointer.last_point.x != x ||
           i->proc.types.pointer.last_point.y != y) {
            indev_proc_release(&i->proc);
        }
    }

    if(indev_reset_check(&i->proc)) return;

    /*Save the new points*/
    i->proc.types.pointer.act_point.x = x;
    i->proc.types.pointer.act_point.y = y;

    if(data->state == LV_INDEV_STATE_PR) indev_proc_press(&i->proc);
    else indev_proc_release(&i->proc);

    if(indev_reset_check(&i->proc)) return;

    i->proc.types.pointer.last_point.x = i->proc.types.pointer.act_point.x;
    i->proc.types.pointer.last_point.y = i->proc.types.pointer.act_point.y;
}

/**
 * Process the pressed state of LV_INDEV_TYPE_POINTER input devices
 * @param indev pointer to an input device 'proc'
 * @return LV_RES_OK: no indev reset required; LV_RES_INV: indev reset is required
 */
static void indev_proc_press(lv_indev_proc_t * proc)
{
    indev_obj_act = proc->types.pointer.act_obj;

    if(proc->wait_until_release != 0) return;

    lv_disp_t * disp = indev_act->driver.disp;
    bool new_obj_searched = false;

    /*If there is no last object then search*/
    if(indev_obj_act == NULL) {
        indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_sys(disp), &proc->types.pointer.act_point);
        if(indev_obj_act == NULL) indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_top(disp),
                                                                          &proc->types.pointer.act_point);
        if(indev_obj_act == NULL) indev_obj_act = lv_indev_search_obj(lv_disp_get_scr_act(disp),
                                                                          &proc->types.pointer.act_point);
        new_obj_searched = true;
    }
    /*If there is last object but it is not scrolled and not protected also search*/
    else if(proc->types.pointer.scroll_obj == NULL &&
            lv_obj_has_flag(indev_obj_act, LV_OBJ_FLAG_PRESS_LOCK) == false) {
        indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_sys(disp), &proc->types.pointer.act_point);
        if(indev_obj_act == NULL) indev_obj_act = lv_indev_search_obj(lv_disp_get_layer_top(disp),
                                                                          &proc->types.pointer.act_point);
        if(indev_obj_act == NULL) indev_obj_act = lv_indev_search_obj(lv_disp_get_scr_act(disp),
                                                                          &proc->types.pointer.act_point);
        new_obj_searched = true;
    }

    /*The last object might have scroll throw. Stop it manually*/
    if(new_obj_searched && proc->types.pointer.last_obj) {
        proc->types.pointer.scroll_throw_vect.x = 0;
        proc->types.pointer.scroll_throw_vect.y = 0;
        _lv_indev_scroll_throw_handler(proc);
        if(indev_reset_check(proc)) return;
    }

    /*If a new object was found reset some variables and send a pressed signal*/
    if(indev_obj_act != proc->types.pointer.act_obj) {
        proc->types.pointer.last_point.x = proc->types.pointer.act_point.x;
        proc->types.pointer.last_point.y = proc->types.pointer.act_point.y;

        /*If a new object found the previous was lost, so send a signal*/
        if(proc->types.pointer.act_obj != NULL) {
            /*Save the obj because in special cases `act_obj` can change in the signal function*/
            lv_obj_t * last_obj = proc->types.pointer.act_obj;

            lv_signal_send(last_obj, LV_SIGNAL_PRESS_LOST, indev_act);
            if(indev_reset_check(proc)) return;
            lv_event_send(last_obj, LV_EVENT_PRESS_LOST, NULL);
            if(indev_reset_check(proc)) return;
        }

        proc->types.pointer.act_obj  = indev_obj_act; /*Save the pressed object*/
        proc->types.pointer.last_obj = indev_obj_act;

        if(indev_obj_act != NULL) {
            /* Save the time when the obj pressed to count long press time.*/
            proc->pr_timestamp                 = lv_tick_get();
            proc->long_pr_sent                 = 0;
            proc->types.pointer.scroll_sum.x     = 0;
            proc->types.pointer.scroll_sum.y     = 0;
            proc->types.pointer.scroll_dir = LV_INDEV_SCROLL_DIR_NONE;
            proc->types.pointer.gesture_sent   = 0;
            proc->types.pointer.gesture_sum.x  = 0;
            proc->types.pointer.gesture_sum.y  = 0;
            proc->types.pointer.vect.x         = 0;
            proc->types.pointer.vect.y         = 0;

            /*Send a signal about the press*/
            lv_signal_send(indev_obj_act, LV_SIGNAL_PRESSED, indev_act);
            if(indev_reset_check(proc)) return;

            lv_event_send(indev_obj_act, LV_EVENT_PRESSED, NULL);
            if(indev_reset_check(proc)) return;

            if(indev_act->proc.wait_until_release) return;

            /*Handle focus*/
            indev_click_focus(&indev_act->proc);
            if(indev_reset_check(proc)) return;

        }
    }

    /*Calculate the vector and apply a low pass filter: new value = 0.5 * old_value + 0.5 * new_value*/
    proc->types.pointer.vect.x = proc->types.pointer.act_point.x - proc->types.pointer.last_point.x;
    proc->types.pointer.vect.y = proc->types.pointer.act_point.y - proc->types.pointer.last_point.y;

    proc->types.pointer.scroll_throw_vect.x = (proc->types.pointer.scroll_throw_vect.x * 4) >> 3;
    proc->types.pointer.scroll_throw_vect.y = (proc->types.pointer.scroll_throw_vect.y * 4) >> 3;

    proc->types.pointer.scroll_throw_vect.x += (proc->types.pointer.vect.x * 4) >> 3;
    proc->types.pointer.scroll_throw_vect.y += (proc->types.pointer.vect.y * 4) >> 3;

    proc->types.pointer.scroll_throw_vect_ori = proc->types.pointer.scroll_throw_vect;

    if(indev_obj_act) {
        lv_signal_send(indev_obj_act, LV_SIGNAL_PRESSING, indev_act);
        if(indev_reset_check(proc)) return;
        lv_event_send(indev_obj_act, LV_EVENT_PRESSING, NULL);
        if(indev_reset_check(proc)) return;

        if(indev_act->proc.wait_until_release) return;

        _lv_indev_scroll_handler(proc);
        if(indev_reset_check(proc)) return;
        indev_gesture(proc);
        if(indev_reset_check(proc)) return;

        /*If there is no scrolling then check for long press time*/
        if(proc->types.pointer.scroll_obj == NULL && proc->long_pr_sent == 0) {
            /*Send a signal about the long press if enough time elapsed*/
            if(lv_tick_elaps(proc->pr_timestamp) > indev_act->driver.long_press_time) {
                lv_signal_send(indev_obj_act, LV_SIGNAL_LONG_PRESS, indev_act);
                if(indev_reset_check(proc)) return;
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED, NULL);
                if(indev_reset_check(proc)) return;

                /*Mark the signal sending to do not send it again*/
                proc->long_pr_sent = 1;

                /*Save the long press time stamp for the long press repeat handler*/
                proc->longpr_rep_timestamp = lv_tick_get();
            }
        }

        /*Send long press repeated signal*/
        if(proc->types.pointer.scroll_obj == NULL && proc->long_pr_sent == 1) {
            /*Send a signal about the long press repeat if enough time elapsed*/
            if(lv_tick_elaps(proc->longpr_rep_timestamp) > indev_act->driver.long_press_rep_time) {
                lv_signal_send(indev_obj_act, LV_SIGNAL_LONG_PRESS_REP, indev_act);
                if(indev_reset_check(proc)) return;
                lv_event_send(indev_obj_act, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
                if(indev_reset_check(proc)) return;
                proc->longpr_rep_timestamp = lv_tick_get();
            }
        }
    }
}

/**
 * Process the released state of LV_INDEV_TYPE_POINTER input devices
 * @param proc pointer to an input device 'proc'
 */
static void indev_proc_release(lv_indev_proc_t * proc)
{
    if(proc->wait_until_release != 0) {
        proc->types.pointer.act_obj  = NULL;
        proc->types.pointer.last_obj = NULL;
        proc->pr_timestamp           = 0;
        proc->longpr_rep_timestamp   = 0;
        proc->wait_until_release     = 0;
    }
    indev_obj_act = proc->types.pointer.act_obj;
    lv_obj_t * scroll_obj = proc->types.pointer.scroll_obj;

    /*Forget the act obj and send a released signal */
    if(indev_obj_act) {

        /*Send RELEASE signal and event*/
        lv_signal_send(indev_obj_act, LV_SIGNAL_RELEASED, indev_act);
        if(indev_reset_check(proc)) return;

        lv_event_send(indev_obj_act, LV_EVENT_RELEASED, NULL);
        if(indev_reset_check(proc)) return;

        /*Send CLICK if no scrolling*/
        if(scroll_obj == NULL) {
            if(proc->long_pr_sent == 0) {
                lv_event_send(indev_obj_act, LV_EVENT_SHORT_CLICKED, NULL);
                if(indev_reset_check(proc)) return;
            }

            lv_event_send(indev_obj_act, LV_EVENT_CLICKED, NULL);
            if(indev_reset_check(proc)) return;
        }

        proc->types.pointer.act_obj = NULL;
        proc->pr_timestamp          = 0;
        proc->longpr_rep_timestamp  = 0;

    }

    /*The reset can be set in the signal function.
     * In case of reset query ignore the remaining parts.*/
    if(scroll_obj) {
        _lv_indev_scroll_throw_handler(proc);
        if(indev_reset_check(proc)) return;
    }
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
    if(indev->proc.reset_query) {
        indev->proc.types.pointer.act_obj           = NULL;
        indev->proc.types.pointer.last_obj          = NULL;
        indev->proc.types.pointer.scroll_obj          = NULL;
        indev->proc.long_pr_sent                    = 0;
        indev->proc.pr_timestamp                    = 0;
        indev->proc.longpr_rep_timestamp            = 0;
        indev->proc.types.pointer.scroll_sum.x        = 0;
        indev->proc.types.pointer.scroll_sum.y        = 0;
        indev->proc.types.pointer.scroll_dir = LV_INDEV_SCROLL_DIR_NONE;
        indev->proc.types.pointer.scroll_throw_vect.x = 0;
        indev->proc.types.pointer.scroll_throw_vect.y = 0;
        indev->proc.types.pointer.gesture_sum.x     = 0;
        indev->proc.types.pointer.gesture_sum.y     = 0;
        indev->proc.reset_query                     = 0;
        indev_obj_act                               = NULL;
    }
}

/**
 * Handle focus/defocus on click for POINTER input devices
 * @param proc pointer to the state of the indev
 */
static void indev_click_focus(lv_indev_proc_t * proc)
{
    /*Handle click focus*/
    lv_obj_t * obj_to_focus = lv_obj_get_focused_obj(indev_obj_act);
    if(lv_obj_has_flag(obj_to_focus, LV_OBJ_FLAG_CLICK_FOCUSABLE) &&
       proc->types.pointer.last_pressed != obj_to_focus) {
        lv_group_t * g_act = lv_obj_get_group(obj_to_focus);
        lv_group_t * g_prev = proc->types.pointer.last_pressed ? lv_obj_get_group(proc->types.pointer.last_pressed) : NULL;

        /*If both the last and act. obj. are in the same group (or no group but it's also the same) */
        if(g_act == g_prev) {
            /*The objects are in a group*/
            if(g_act) {
                lv_group_focus_obj(obj_to_focus);
                if(indev_reset_check(proc)) return;
            }
            /*The object are not in group*/
            else {
                if(proc->types.pointer.last_pressed) {
                    lv_signal_send(proc->types.pointer.last_pressed, LV_SIGNAL_DEFOCUS, NULL);
                    if(indev_reset_check(proc)) return;
                    lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_DEFOCUSED, NULL);
                    if(indev_reset_check(proc)) return;
                }

                lv_signal_send(obj_to_focus, LV_SIGNAL_FOCUS, NULL);
                if(indev_reset_check(proc)) return;
                lv_event_send(obj_to_focus, LV_EVENT_FOCUSED, NULL);
                if(indev_reset_check(proc)) return;
            }
        }
        /*The object are not in the same group (in different group or one in not a group)*/
        else {
            /*If the prev. obj. is not in a group then defocus it.*/
            if(g_prev == NULL && proc->types.pointer.last_pressed) {
                lv_signal_send(proc->types.pointer.last_pressed, LV_SIGNAL_DEFOCUS, NULL);
                if(indev_reset_check(proc)) return;
                lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_DEFOCUSED, NULL);
                if(indev_reset_check(proc)) return;
            }
            /*Focus on a non-group object*/
            else {
                if(proc->types.pointer.last_pressed) {
                    /*If the prev. object also wasn't in a group defocus it*/
                    if(g_prev == NULL) {
                        lv_signal_send(proc->types.pointer.last_pressed, LV_SIGNAL_DEFOCUS, NULL);
                        if(indev_reset_check(proc)) return;
                        lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_DEFOCUSED, NULL);
                        if(indev_reset_check(proc)) return;
                    }
                    /*If the prev. object also was in a group at least "LEAVE" it instead of defocus*/
                    else {
                        lv_signal_send(proc->types.pointer.last_pressed, LV_SIGNAL_LEAVE, NULL);
                        if(indev_reset_check(proc)) return;
                        lv_event_send(proc->types.pointer.last_pressed, LV_EVENT_LEAVE, NULL);
                        if(indev_reset_check(proc)) return;
                    }
                }
            }

            /*Focus to the act. in its group*/
            if(g_act) {
                lv_group_focus_obj(obj_to_focus);
                if(indev_reset_check(proc)) return;
            }
            else {
                lv_signal_send(obj_to_focus, LV_SIGNAL_FOCUS, NULL);
                if(indev_reset_check(proc)) return;
                lv_event_send(obj_to_focus, LV_EVENT_FOCUSED, NULL);
                if(indev_reset_check(proc)) return;
            }
        }
        proc->types.pointer.last_pressed = obj_to_focus;
    }

}

/**
* Handle the gesture of indev_proc_p->types.pointer.act_obj
* @param indev pointer to a input device state
*/
void indev_gesture(lv_indev_proc_t * proc)
{

    if(proc->types.pointer.scroll_obj) return;
    if(proc->types.pointer.gesture_sent) return;

    lv_obj_t * gesture_obj = proc->types.pointer.act_obj;

    /*If gesture parent is active check recursively the gesture attribute*/
    while(gesture_obj && lv_obj_has_flag(gesture_obj, LV_OBJ_FLAG_GESTURE_BUBBLE)) {
        gesture_obj = lv_obj_get_parent(gesture_obj);
    }

    if(gesture_obj == NULL) return;

    if((LV_ABS(proc->types.pointer.vect.x) < indev_act->driver.gesture_min_velocity) &&
       (LV_ABS(proc->types.pointer.vect.y) < indev_act->driver.gesture_min_velocity)) {
        proc->types.pointer.gesture_sum.x = 0;
        proc->types.pointer.gesture_sum.y = 0;
    }

    /*Count the movement by gesture*/
    proc->types.pointer.gesture_sum.x += proc->types.pointer.vect.x;
    proc->types.pointer.gesture_sum.y += proc->types.pointer.vect.y;

    if((LV_ABS(proc->types.pointer.gesture_sum.x) > indev_act->driver.gesture_limit) ||
       (LV_ABS(proc->types.pointer.gesture_sum.y) > indev_act->driver.gesture_limit)) {

        proc->types.pointer.gesture_sent = 1;

        if(LV_ABS(proc->types.pointer.gesture_sum.x) > LV_ABS(proc->types.pointer.gesture_sum.y)) {
            if(proc->types.pointer.gesture_sum.x > 0)
                proc->types.pointer.gesture_dir = LV_GESTURE_DIR_RIGHT;
            else
                proc->types.pointer.gesture_dir = LV_GESTURE_DIR_LEFT;
        }
        else {
            if(proc->types.pointer.gesture_sum.y > 0)
                proc->types.pointer.gesture_dir = LV_GESTURE_DIR_BOTTOM;
            else
                proc->types.pointer.gesture_dir = LV_GESTURE_DIR_TOP;
        }

        lv_signal_send(gesture_obj, LV_SIGNAL_GESTURE, indev_act);
        if(indev_reset_check(proc)) return;
        lv_event_send(gesture_obj, LV_EVENT_GESTURE, NULL);
        if(indev_reset_check(proc)) return;
    }
}

/**
 * Checks if the reset_query flag has been set. If so, perform necessary global indev cleanup actions
 * @param proc pointer to an input device 'proc'
 * @return true if indev query should be immediately truncated.
 */
static bool indev_reset_check(lv_indev_proc_t * proc)
{
    if(proc->reset_query) {
        indev_obj_act = NULL;
    }

    return proc->reset_query ? true : false;
}
