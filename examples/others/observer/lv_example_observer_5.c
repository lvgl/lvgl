#include "../../lv_examples.h"
#if LV_USE_OBSERVER && LV_USE_ARC && LV_USE_LABEL && LV_USE_BUTTON && LV_USE_SPINNER && LV_USE_FLEX \
    && LV_BUILD_EXAMPLES

typedef enum {
    FW_UPDATE_STATE_IDLE,
    FW_UPDATE_STATE_CONNECTING,
    FW_UPDATE_STATE_CONNECTED,
    FW_UPDATE_STATE_DOWNLOADING,
    FW_UPDATE_STATE_CANCEL,
    FW_UPDATE_STATE_READY,
} lv_fw_update_state_t;

static void fw_upload_manager_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void fw_update_btn_clicked_event_cb(lv_event_t * e);
static void fw_update_close_event_cb(lv_event_t * e);
static void fw_update_win_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

static lv_subject_t * fw_download_percent_subject;
static lv_subject_t * fw_update_status_subject;

/**
 * Create a window: a flex column with a header bar on top and a content area
 * below. The header holds the title and a close button, the content area grows
 * to fill what is left.
 */
static lv_obj_t * win_create(lv_obj_t * parent)
{
    lv_obj_t * win = lv_obj_create(parent);
    lv_obj_set_flex_flow(win, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(win, 0, 0);
    lv_obj_set_style_pad_gap(win, 0, 0);

    lv_obj_t * header = lv_obj_create(win);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(header, 0, 0);

    lv_obj_t * content = lv_obj_create(win);
    lv_obj_set_width(content, lv_pct(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_style_radius(content, 0, 0);

    return win;
}

static lv_obj_t * win_get_header(lv_obj_t * win)
{
    return lv_obj_get_child(win, 0);
}

static lv_obj_t * win_get_content(lv_obj_t * win)
{
    return lv_obj_get_child(win, 1);
}

/**
 * @title Firmware update state machine
 * @brief Drive a window through its update states using two int subjects.
 *
 * `fw_update_status_subject` holds an `lv_fw_update_state_t` value and
 * `fw_download_percent_subject` tracks progress. A start button opens a window,
 * built from a flex column by the static `win_*` helpers, whose observer renders
 * the appropriate content: a spinner for connecting, an arc plus percentage label
 * bound with `lv_arc_bind_value` and `lv_label_bind_text` for downloading, and a
 * restart button for ready. A separate app-side observer spawns `lv_timer_t`
 * instances that simulate the 2-second connect and a 50 ms per-step download.
 * The window's close button pushes `FW_UPDATE_STATE_CANCEL`, which the observer
 * uses to delete the window.
 */
void lv_example_observer_5(void)
{
    fw_download_percent_subject = lv_subject_create(LV_SUBJECT_TYPE_INT);
    fw_update_status_subject = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_IDLE);

    lv_subject_add_observer(fw_update_status_subject, fw_upload_manager_observer_cb, NULL);

    /*Create start FW update button*/
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn, fw_update_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(btn);
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Firmware update");
}

static void fw_update_btn_clicked_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_obj_t * win = win_create(lv_screen_active());
    lv_obj_set_size(win, lv_pct(90), lv_pct(90));
    lv_obj_set_height(win_get_header(win), 40);
    lv_obj_set_style_radius(win, 8, 0);
    lv_obj_set_style_shadow_width(win, 24, 0);
    lv_obj_set_style_shadow_offset_x(win, 2, 0);
    lv_obj_set_style_shadow_offset_y(win, 3, 0);
    lv_obj_set_style_shadow_color(win, lv_color_hex3(0x888), 0);

    lv_obj_t * title = lv_label_create(win_get_header(win));
    lv_label_set_text(title, "Firmware update");
    lv_obj_set_flex_grow(title, 1);

    lv_obj_t * btn = lv_button_create(win_get_header(win));
    lv_obj_set_size(btn, 40, lv_pct(100));
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, LV_SYMBOL_CLOSE);
    lv_obj_center(btn_label);
    lv_obj_add_event_cb(btn, fw_update_close_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_center(win);

    lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_IDLE);
    lv_subject_add_observer_obj(fw_update_status_subject, fw_update_win_observer_cb, win, NULL);
}

static void fw_update_close_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_CANCEL);
}

static void restart_btn_click_event_cb(lv_event_t * e)
{
    lv_obj_t * win = (lv_obj_t *) lv_event_get_user_data(e);
    lv_obj_delete(win);
    lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_IDLE);
}

static void fw_update_win_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * win = (lv_obj_t *) lv_observer_get_target(observer);
    lv_obj_t * cont = win_get_content(win);
    lv_fw_update_state_t status = (lv_fw_update_state_t) lv_subject_get_int(fw_update_status_subject);
    if(status == FW_UPDATE_STATE_IDLE) {
        lv_obj_clean(cont);
        lv_obj_t * spinner = lv_spinner_create(cont);
        lv_obj_center(spinner);
        lv_obj_set_size(spinner, 130, 130);

        lv_obj_t * label = lv_label_create(cont);
        lv_label_set_text(label, "Connecting");
        lv_obj_center(label);

        lv_subject_set_int(subject, FW_UPDATE_STATE_CONNECTING);
    }
    else if(status == FW_UPDATE_STATE_DOWNLOADING) {
        lv_obj_clean(cont);
        lv_obj_t * arc = lv_arc_create(cont);
        lv_arc_bind_value(arc, fw_download_percent_subject);
        lv_obj_center(arc);
        lv_obj_set_size(arc, 130, 130);
        lv_obj_set_clickable(arc, false);

        lv_obj_t * label = lv_label_create(cont);
        lv_label_bind_text(label, fw_download_percent_subject, "%d %%");
        lv_obj_center(label);
    }
    else if(status == FW_UPDATE_STATE_READY) {
        lv_obj_clean(cont);
        lv_obj_t * label = lv_label_create(cont);
        lv_label_set_text(label, "Firmware update is ready");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -20);

        lv_obj_t * btn = lv_button_create(cont);
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 20);
        lv_obj_add_event_cb(btn, restart_btn_click_event_cb, LV_EVENT_CLICKED, win);

        label = lv_label_create(btn);
        lv_label_set_text(label, "Restart");
    }
    else if(status == FW_UPDATE_STATE_CANCEL) {
        lv_obj_delete(win);
    }
}

static void connect_timer_cb(lv_timer_t * t)
{
    if(lv_subject_get_int(fw_update_status_subject) != FW_UPDATE_STATE_CANCEL) {
        lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_CONNECTED);
    }
    lv_timer_delete(t);
}

static void download_timer_cb(lv_timer_t * t)
{
    if(lv_subject_get_int(fw_update_status_subject) == FW_UPDATE_STATE_CANCEL) {
        lv_timer_delete(t);
        return;
    }

    int32_t v = lv_subject_get_int(fw_download_percent_subject);
    if(v < 100) {
        lv_subject_set_int(fw_download_percent_subject, v + 1);
    }
    else {
        lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_READY);
        lv_timer_delete(t);
    }
}

/**
 * Emulate connection and FW downloading by timers
 */
static void fw_upload_manager_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    LV_UNUSED(observer);

    lv_fw_update_state_t state = (lv_fw_update_state_t) lv_subject_get_int(fw_update_status_subject);
    if(state == FW_UPDATE_STATE_CONNECTING) {
        lv_timer_create(connect_timer_cb, 2000, NULL);
    }
    else if(state == FW_UPDATE_STATE_CONNECTED) {
        lv_subject_set_int(fw_download_percent_subject, 0);
        lv_subject_set_int(fw_update_status_subject, FW_UPDATE_STATE_DOWNLOADING);
        lv_timer_create(download_timer_cb, 50, NULL);
    }
}

#endif
