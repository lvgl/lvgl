#include "../../../lvgl.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
    }
}

void lv_example_switch_1(void)
{
    lv_obj_set_layout(lv_scr_act(), &lv_flex_column_center);

    lv_obj_t * sw;

    sw = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_align(sw, NULL, LV_ALIGN_CENTER, 0, -50);
    lv_obj_add_event_cb(sw, event_handler, NULL);

    sw = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_align(sw, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(sw, event_handler, NULL);

    sw = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_add_state(sw, LV_STATE_DISABLED);
    lv_obj_align(sw, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(sw, event_handler, NULL);

    sw = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_add_state(sw, LV_STATE_CHECKED | LV_STATE_DISABLED);
    lv_obj_align(sw, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(sw, event_handler, NULL);


}

#endif
