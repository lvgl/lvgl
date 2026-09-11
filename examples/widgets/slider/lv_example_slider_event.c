#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * status_label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    lv_event_code_t code = lv_event_get_code(e);
    int32_t value = lv_slider_get_value(slider);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("slider value: %d", (int)value);
        lv_label_set_text_fmt(status_label, "Dragging: %d", (int)value);
    }
    else if(code == LV_EVENT_RELEASED) {
        LV_LOG_USER("slider released at %d", (int)value);
        lv_label_set_text_fmt(status_label, "Released at: %d", (int)value);
    }
}

/**
 * @title Slider value-changed and released events
 * @brief Show drag updates and the final value via a sibling label.
 *
 * `LV_EVENT_VALUE_CHANGED` fires continuously while the knob is being
 * dragged or stepped with keys. `LV_EVENT_RELEASED` fires once on
 * release — useful for committing a final value without spamming
 * intermediate positions. The shared handler reads `lv_event_get_code`
 * to branch and writes the value into a sibling label.
 */
void lv_example_slider_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * slider = lv_slider_create(scr);
    lv_obj_set_width(slider, 200);
    lv_obj_add_event_cb(slider, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, event_cb, LV_EVENT_RELEASED, NULL);

    status_label = lv_label_create(scr);
    lv_label_set_text(status_label, "Drag the slider");
}

#endif
