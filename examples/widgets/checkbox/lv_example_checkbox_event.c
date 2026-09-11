#include "../../lv_examples.h"
#if LV_USE_CHECKBOX && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * cb = lv_event_get_target_obj(e);
    bool checked = lv_obj_has_state(cb, LV_STATE_CHECKED);
    LV_LOG_USER("checkbox %s", checked ? "checked" : "unchecked");
    lv_label_set_text(status_label, checked ? "Feature: ON" : "Feature: OFF");
}

/**
 * @title Checkbox value-changed event
 * @brief Mirror the toggle state into a sibling label.
 *
 * `LV_EVENT_VALUE_CHANGED` fires when the user toggles the checkbox. The
 * state isn't in the payload — read it with
 * `lv_obj_has_state(cb, LV_STATE_CHECKED)` and use any LVGL call inside
 * the handler (here, `lv_label_set_text` on a sibling).
 */
void lv_example_checkbox_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * cb = lv_checkbox_create(scr);
    lv_checkbox_set_text(cb, "Enable feature");
    lv_obj_add_event_cb(cb, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "Feature: OFF");
}

#endif
