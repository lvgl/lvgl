#include "../../lv_examples.h"
#if LV_USE_ROLLER && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * roller = lv_event_get_target_obj(e);
    char buf[32];
    lv_roller_get_selected_str(roller, buf, sizeof(buf));
    LV_LOG_USER("roller: selected %u (\"%s\")", (unsigned)lv_roller_get_selected(roller), buf);
    lv_label_set_text_fmt(status_label, "Day: %s", buf);
}

/**
 * @title Roller value-changed event
 * @brief Mirror the settled selection into a sibling label.
 *
 * `LV_EVENT_VALUE_CHANGED` fires once the roller has settled on a new
 * option (not while it's still scrolling). Read the option text with
 * `lv_roller_get_selected_str` and push it into a label from the handler.
 */
void lv_example_roller_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * roller = lv_roller_create(scr);
    lv_roller_set_options(roller, "Mon\nTue\nWed\nThu\nFri\nSat\nSun", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_add_event_cb(roller, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "Day: Mon");
}

#endif
