#include "../../lv_examples.h"
#if LV_USE_ARC && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * arc;
static lv_obj_t * label;

static void value_changed_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_label_set_text_fmt(label, "%" LV_PRId32 "%%", lv_arc_get_value(arc));

    /*Rotate the label to the current position of the arc*/
    lv_arc_rotate_obj_to_angle(arc, label, 25);
}

/**
 * @title Place a Widget on the arc's knob
 * @brief Position a label so it tracks the arc's current value angle.
 *
 * `lv_arc_align_obj_to_angle(arc, widget, radius_offset)` repositions
 * `widget` so its centre sits on the same angle as the arc's current
 * value, offset radially by `radius_offset` pixels. Called from a
 * `LV_EVENT_VALUE_CHANGED` handler, it makes any sibling widget follow
 * the knob — useful for a value label, a marker, or a secondary
 * indicator. `lv_arc_rotate_obj_to_angle` does the same but also rotates
 * the widget to point along the angle.
 */
void lv_example_arc_knob_widget(void)
{
    label = lv_label_create(lv_screen_active());

    /*Create an Arc*/
    arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, 150, 150);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 10);
    lv_obj_center(arc);
    lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

    /*Manually update the label for the first time*/
    lv_obj_send_event(arc, LV_EVENT_VALUE_CHANGED, NULL);
}

#endif
