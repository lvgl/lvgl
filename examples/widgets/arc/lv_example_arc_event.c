#include "../../lv_examples.h"
#if LV_USE_ARC && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * label;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target_obj(e);
    int32_t value = lv_arc_get_value(arc);
    LV_LOG_USER("arc value: %d", (int)value);
    /* Any LVGL API can be called from an event handler — here we mirror the
     * value into a sibling label so the change is visible on screen. */
    lv_label_set_text_fmt(label, "Value: %d", (int)value);
}

/**
 * @title Arc value-changed event
 * @brief React to drag updates by logging and refreshing a sibling label.
 *
 * The arc emits `LV_EVENT_VALUE_CHANGED` continuously while it's being
 * dragged. The handler reads the current value via `lv_arc_get_value`,
 * logs it, and updates a label with `lv_label_set_text_fmt` — any LVGL
 * function may be called from the event callback.
 */
void lv_example_arc_event(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(scr, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(scr, 12, 0);

    lv_obj_t * arc = lv_arc_create(scr);
    lv_obj_set_size(arc, 150, 150);
    lv_obj_add_event_cb(arc, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    label = lv_label_create(scr);
    lv_label_set_text_fmt(label, "Value: %d", (int)lv_arc_get_value(arc));
}

#endif
