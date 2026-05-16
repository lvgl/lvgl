#include "../../lv_examples.h"
#if LV_USE_ARC && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_obj_t * arc;
static lv_obj_t * follower;

static void value_changed_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    /* Keep `follower` aligned 20 px outside the arc's current value angle. */
    lv_arc_align_obj_to_angle(arc, follower, 20);
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
    arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, 180, 180);
    lv_obj_center(arc);

    /* The follower — a label that sits just outside the knob. */
    follower = lv_label_create(arc);
    lv_label_set_text(follower, "0");

    /* Position once at creation time so the label starts on the right angle. */
    lv_arc_align_obj_to_angle(arc, follower, 20);

    lv_obj_add_event_cb(arc, value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

#endif
