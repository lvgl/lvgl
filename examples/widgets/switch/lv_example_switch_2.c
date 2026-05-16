#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    LV_UNUSED(obj);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
    }
}

/**
 * @title Vertical switch orientation
 * @brief Two 30 x 60 switches, one horizontal and one forced to vertical orientation.
 *
 * The active screen is laid out as `LV_FLEX_FLOW_ROW` with centered flex
 * alignment. Both switches are sized 30 x 60 so one is taller than it is
 * wide; the first keeps the default orientation while the second calls
 * `lv_switch_set_orientation(..., LV_SWITCH_ORIENTATION_VERTICAL)` and starts
 * in `LV_STATE_CHECKED`. A shared callback logs `On` or `Off` on
 * `LV_EVENT_VALUE_CHANGED`.
 */
void lv_example_switch_2(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * sw;

    sw = lv_switch_create(lv_screen_active());
    lv_obj_set_size(sw, 30, 60);
    lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);

    sw = lv_switch_create(lv_screen_active());
    lv_obj_set_size(sw, 30, 60);
    lv_switch_set_orientation(sw, LV_SWITCH_ORIENTATION_VERTICAL);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);
}

#endif
