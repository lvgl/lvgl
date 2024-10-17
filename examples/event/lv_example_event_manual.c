#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SWITCH

static void event_clicked_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    static uint32_t cnt = 0;
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
    //Send a refresh if clicked enough
    if(++cnt > 10) {
        lv_obj_send_event(btn, LV_EVENT_REFRESH, NULL);
    }
}

static void event_refresh_cb(lv_event_t * e)
{
    LV_LOG_USER("Refresh: manually called");

    lv_label_set_text(lv_obj_get_child(lv_event_get_target(e), 0), "Refreshed. Click me again!");
}

/**
 * Add events to a button
 */
void lv_example_event_manual(void)
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, event_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn, event_refresh_cb, LV_EVENT_REFRESH, NULL);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click me!");
    lv_obj_center(label);
}

#endif
