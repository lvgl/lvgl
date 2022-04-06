#include "../../lv_examples.h"
#if LV_USE_MSG && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

/*Define a message ID*/
#define MSG_LOGIN_ATTEMPT   1
#define MSG_LOG_OUT         2
#define MSG_LOGIN_ERROR     3
#define MSG_LOGIN_OK        4

static void auth_manager(void * s, lv_msg_t * m);
static void textarea_event_cb(lv_event_t * e);
static void log_out_event_cb(lv_event_t * e);
static void start_engine_msg_event_cb(lv_event_t * e);
static void info_label_msg_event_cb(lv_event_t * e);

/**
 * Simple PIN login screen.
 * No global variables are used, all state changes are communicated via messages.
 */
void lv_example_msg_2(void)
{
    lv_msg_subsribe(MSG_LOGIN_ATTEMPT, auth_manager, "hello");

    /*Create a slider in the center of the display*/
    lv_obj_t * ta = lv_textarea_create(lv_scr_act());
    lv_obj_set_pos(ta, 10, 10);
    lv_obj_set_width(ta, 200);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_password_mode(ta, true);
    lv_textarea_set_placeholder_text(ta, "The password is: hello");
    lv_obj_add_event_cb(ta, textarea_event_cb, LV_EVENT_ALL, NULL);
    lv_msg_subsribe_obj(MSG_LOGIN_ERROR, ta, NULL);
    lv_msg_subsribe_obj(MSG_LOGIN_OK, ta, NULL);
    lv_msg_subsribe_obj(MSG_LOG_OUT, ta, NULL);

    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_textarea(kb, ta);

    lv_obj_t * btn;
    lv_obj_t * label;

    /*Create a log out button which will be active only when logged in*/
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn, 240, 10);
    lv_obj_add_event_cb(btn, log_out_event_cb, LV_EVENT_ALL, NULL);
    lv_msg_subsribe_obj(MSG_LOGIN_OK, btn, NULL);
    lv_msg_subsribe_obj(MSG_LOG_OUT, btn, NULL);

    label = lv_label_create(btn);
    lv_label_set_text(label, "LOG OUT");

    /*Create a label to show info*/
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "");
    lv_obj_add_event_cb(label, info_label_msg_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_obj_set_pos(label, 10, 60);
    lv_msg_subsribe_obj(MSG_LOGIN_ERROR, label, NULL);
    lv_msg_subsribe_obj(MSG_LOGIN_OK, label, NULL);
    lv_msg_subsribe_obj(MSG_LOG_OUT, label, NULL);

    /*Create button which will be active only when logged in*/
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn, 10, 80);
    lv_obj_add_event_cb(btn, start_engine_msg_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_msg_subsribe_obj(MSG_LOGIN_OK, btn, NULL);
    lv_msg_subsribe_obj(MSG_LOG_OUT, btn, NULL);

    label = lv_label_create(btn);
    lv_label_set_text(label, "START ENGINE");

    lv_msg_send(MSG_LOG_OUT, NULL);
}

static void auth_manager(void * s, lv_msg_t * m)
{
    LV_UNUSED(s);
    const char * pin_act = lv_msg_get_payload(m);
    const char * pin_expexted = lv_msg_get_user_data(m);
    if(strcmp(pin_act, pin_expexted) == 0) {
        lv_msg_send(MSG_LOGIN_OK, NULL);
    }
    else {
        lv_msg_send(MSG_LOGIN_ERROR, "Incorrect PIN");
    }

}

static void textarea_event_cb(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_READY) {
        lv_msg_send(MSG_LOGIN_ATTEMPT, lv_textarea_get_text(ta));
    }
    else if(code == LV_EVENT_MSG_RECEIVED) {
        lv_msg_t * m = lv_event_get_msg(e);
        switch(lv_msg_get_id(m)) {
            case MSG_LOGIN_ERROR:
                /*If there was an error, clean the text area*/
                if(strlen(lv_msg_get_payload(m))) lv_textarea_set_text(ta, "");
                break;
            case MSG_LOGIN_OK:
                lv_obj_add_state(ta, LV_STATE_DISABLED);
                lv_obj_clear_state(ta, LV_STATE_FOCUSED | LV_STATE_FOCUS_KEY);
                break;
            case MSG_LOG_OUT:
                lv_textarea_set_text(ta, "");
                lv_obj_clear_state(ta, LV_STATE_DISABLED);
                break;
        }
    }
}

static void log_out_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_msg_send(MSG_LOG_OUT, NULL);
    }
    else if(code == LV_EVENT_MSG_RECEIVED) {
        lv_msg_t * m = lv_event_get_msg(e);
        lv_obj_t * btn = lv_event_get_target(e);
        switch(lv_msg_get_id(m)) {
            case MSG_LOGIN_OK:
                lv_obj_clear_state(btn, LV_STATE_DISABLED);
                break;
            case MSG_LOG_OUT:
                lv_obj_add_state(btn, LV_STATE_DISABLED);
                break;
        }
    }
}

static void start_engine_msg_event_cb(lv_event_t * e)
{
    lv_msg_t * m = lv_event_get_msg(e);
    lv_obj_t * btn = lv_event_get_target(e);
    switch(lv_msg_get_id(m)) {
        case MSG_LOGIN_OK:
            lv_obj_clear_state(btn, LV_STATE_DISABLED);
            break;
        case MSG_LOG_OUT:
            lv_obj_add_state(btn, LV_STATE_DISABLED);
            break;
    }
}

static void info_label_msg_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_msg_t * m = lv_event_get_msg(e);
    switch(lv_msg_get_id(m)) {
        case MSG_LOGIN_ERROR:
            lv_label_set_text(label, lv_msg_get_payload(m));
            lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), 0);
            break;
        case MSG_LOGIN_OK:
            lv_label_set_text(label, "Login successful");
            lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_GREEN), 0);
            break;
        case MSG_LOG_OUT:
            lv_label_set_text(label, "Logged out");
            lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_GREY), 0);
            break;
        default:
            break;
    }
}

#endif
