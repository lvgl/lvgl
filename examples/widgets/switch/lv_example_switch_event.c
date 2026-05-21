#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target_obj(e);
    bool on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    LV_LOG_USER("switch %s", on ? "on" : "off");
    lv_label_set_text(status_label, on ? "State: ON" : "State: OFF");
}

/**
 * @title Switch value-changed event
 * @brief Mirror the toggle state into a sibling label.
 *
 * `LV_EVENT_VALUE_CHANGED` fires on toggle. The new state isn't in the
 * payload — read it with `lv_obj_has_state(sw, LV_STATE_CHECKED)` and
 * push it into a label from the handler.
 */
void lv_example_switch_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * sw = lv_switch_create(scr);
    lv_obj_add_event_cb(sw, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "State: OFF");
}

#endif
