#include "../../lv_examples.h"
#if LV_USE_DROPDOWN && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * dd = lv_event_get_target_obj(e);
    char buf[32];
    lv_dropdown_get_selected_str(dd, buf, sizeof(buf));
    LV_LOG_USER("dropdown: selected %u (\"%s\")", (unsigned)lv_dropdown_get_selected(dd), buf);
    lv_label_set_text_fmt(status_label, "Selected: %s", buf);
}

/**
 * @title Dropdown value-changed event
 * @brief Mirror the selected option into a sibling label.
 *
 * `LV_EVENT_VALUE_CHANGED` fires when a new option is chosen.
 * `lv_dropdown_get_selected` returns the index; `_get_selected_str` reads
 * the option text into a buffer. The handler writes that text into a
 * sibling label.
 */
void lv_example_dropdown_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * dd = lv_dropdown_create(scr);
    lv_dropdown_set_options(dd, "Berlin\nParis\nTokyo\nSydney");
    lv_obj_add_event_cb(dd, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "Selected: Berlin");
}

#endif
