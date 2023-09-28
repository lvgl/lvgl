#include "../../lv_examples.h"
#if LV_USE_OBSERVER && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

typedef enum {
    LOGGED_OUT,
    LOGGED_IN,
    AUTH_FAILED,
} auth_state_t;

static lv_subject_t auth_state_subject;

static void textarea_event_cb(lv_event_t * e);
static void log_out_click_event_cb(lv_event_t * e);

/**
 * Simple PIN login screen.
 * No global variables are used, all state changes are communicated via messages.
 */
void lv_example_observer_2(void)
{
    lv_observer_t * observer;

    lv_subject_init_int(&auth_state_subject, LOGGED_OUT);

    /*Create a slider in the center of the display*/
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());
    lv_obj_set_pos(ta, 10, 10);
    lv_obj_set_width(ta, 200);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, "The password is: hello");
    lv_obj_add_event(ta, textarea_event_cb, LV_EVENT_READY, NULL);
    lv_obj_bind_state_if_eq(ta, &auth_state_subject, LV_STATE_DISABLED, LOGGED_IN);

    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_textarea(kb, ta);

    lv_obj_t * btn;
    lv_obj_t * label;

    /*Create a log out button which will be active only when logged in*/
    btn = lv_button_create(lv_scr_act());
    lv_obj_set_pos(btn, 240, 10);
    lv_obj_add_event(btn, log_out_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_bind_state_if_not_eq(btn, &auth_state_subject, LV_STATE_DISABLED, LOGGED_OUT);

    label = lv_label_create(btn);
    lv_label_set_text(label, "LOG OUT");

    /*Create a label to show info*/
    label = lv_label_create(lv_scr_act());
    lv_obj_set_pos(label, 10, 60);
    lv_subject_add_observer_obj(&auth_state_subject, info_label_observer_cb, label, NULL);

    /*Create button which will be active only when logged in*/
    btn = lv_button_create(lv_scr_act());
    lv_obj_set_pos(btn, 10, 80);
    lv_obj_bind_state_if_not_eq(btn, &auth_state_subject, LV_STATE_DISABLED, LOGGED_IN);

    label = lv_label_create(btn);
    lv_label_set_text(label, "START ENGINE");
}

static void textarea_event_cb(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    if(strcmp(lv_textarea_get_text(ta), "hello") == 0) {
        lv_subject_set_int(&auth_state_subject, LOGGED_IN);
    }
}

static void info_label_observer_cb(lv_subject_t * subject, lv_observer_t * observer)
{
    lv_obj_t * label = lv_observer_get_target(observer);
    switch(lv_subject_get_int(subject)) {
        case LOGGED_IN:
            lv_label_set_text(label, "Login successful");
            break;
        case LOGGED_OUT:
            lv_label_set_text(label, "Logged out");
            break;
        case AUTH_FAILED:
            lv_label_set_text(label, "Login failed");
            break;
    }
}

static void log_out_click_event_cb(lv_event_t * e)
{
    lv_subject_set_int(&auth_state_subject, LOGGED_OUT);
}

//static void start_engine_msg_event_cb(lv_event_t * e)
//{
//    lv_msg_t * m = lv_event_get_msg(e);
//    lv_obj_t * btn = lv_event_get_target(e);
//    switch(lv_msg_get_id(m)) {
//        case MSG_LOGIN_OK:
//            lv_obj_clear_state(btn, LV_STATE_DISABLED);
//            break;
//        case MSG_LOG_OUT:
//            lv_obj_add_state(btn, LV_STATE_DISABLED);
//            break;
//    }
//}

#endif
