#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_UNUSED(obj);
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
    }
}

void lv_example_switch_3(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * sw0;
    lv_style_t style0;

    //Define styles
    lv_style_init(&style0);
    lv_style_set_bg_color(&style0, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_opa(&style0, LV_OPA_COVER);

    //Create switch
    sw0 = lv_switch_create(lv_screen_active());
    lv_obj_center(sw0);
    lv_obj_align(sw0, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_size(sw0, 30, 60);
    lv_obj_add_event_cb(sw0, event_handler, LV_EVENT_ALL, NULL);
    lv_switch_set_orientation(sw0, LV_SWITCH_ORIENTATION_VERTICAL);
    lv_obj_add_state(sw0, LV_STATE_CHECKED);

    //Apply styles to specific state only
    lv_obj_add_style(sw0, &style0, LV_STATE_CHECKED);
}

#endif
