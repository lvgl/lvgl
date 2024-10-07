#include "../../lv_examples.h"
#if LV_USE_CHECKBOX && LV_BUILD_EXAMPLES

static void event_handler0(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * other_obj = lv_obj_get_child(lv_obj_get_parent(obj), (1 - lv_obj_get_child_cnt(obj)));
    if(code == LV_EVENT_VALUE_CHANGED) {
        //Handle style changes
        if(lv_obj_get_state(obj) == LV_STATE_CHECKED) {
            //If other object is also checked, then trigger an event manually for the 3rd object
            if(lv_obj_get_state(other_obj) == LV_STATE_CHECKED) {
                lv_obj_send_event(lv_obj_get_child(lv_obj_get_parent(obj), 2), LV_EVENT_VALUE_CHANGED, NULL);
            }
        }
    }
}

void lv_example_checkbox_3(void)
{
    lv_obj_t * checkbox0;
    lv_obj_t * checkbox1;
    lv_obj_t * checkbox2;

    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    checkbox0 = lv_checkbox_create(lv_screen_active());
    lv_checkbox_set_text(checkbox0, "A");
    lv_obj_add_event_cb(checkbox0, event_handler0, LV_EVENT_ALL, NULL);

    checkbox1 = lv_checkbox_create(lv_screen_active());
    lv_checkbox_set_text(checkbox1, "B");
    lv_obj_add_event_cb(checkbox1, event_handler0, LV_EVENT_ALL, NULL);

    //Make the "ouput" not clickable by user
    checkbox2 = lv_checkbox_create(lv_screen_active());
    lv_checkbox_set_text(checkbox2, "F = A & B");
    lv_obj_add_state(checkbox2, LV_STATE_DISABLED);

    //Create styles for the checkboxes
    static lv_style_t style_red;
    static lv_style_t style_green;

    lv_style_init(&style_red);
    lv_style_set_bg_color(&style_red, lv_palette_lighten(LV_PALETTE_RED, 1));

    lv_style_init(&style_green);
    lv_style_set_bg_color(&style_green, lv_palette_lighten(LV_PALETTE_GREEN, 1));

    //Add styles for the objects
    lv_obj_add_style(checkbox0, &style_green, LV_STATE_CHECKED);
    lv_obj_add_style(checkbox1, &style_green, LV_STATE_CHECKED);
    lv_obj_add_style(checkbox2, &style_green, LV_STATE_CHECKED);

    lv_obj_add_style(checkbox0, &style_red, 0);
    lv_obj_add_style(checkbox1, &style_red, 0);
    lv_obj_add_style(checkbox2, &style_red, LV_STATE_DISABLED);

}

#endif
