#include "../../lv_examples.h"
#if LV_USE_CHECKBOX && LV_BUILD_EXAMPLES

static lv_style_t style_radio;
static lv_style_t style_radio_chk;

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(lv_obj_has_state(obj, LV_STATE_CHECKED)) {
        LV_LOG_USER("%s is selected.", lv_checkbox_get_text(obj));
    }
    else {
        LV_LOG_USER("%s is de-selected.", lv_checkbox_get_text(obj));
    }
}

/**
 * Checkboxes as radio buttons
 */
void lv_example_checkbox_2(void)
{
    lv_style_init(&style_radio);
    lv_style_set_radius(&style_radio, LV_RADIUS_CIRCLE);

    lv_style_init(&style_radio_chk);
    lv_style_set_bg_image_src(&style_radio_chk, NULL);


    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont);

    uint32_t i;
    char buf[32];
    for(i = 0; i < 5; i++) {
        lv_snprintf(buf, sizeof(buf), "Radio button %d", (int)i + 1);

        lv_obj_t * obj = lv_checkbox_create(cont);
        lv_checkbox_set_text(obj, buf);

        lv_obj_add_event_cb(obj, event_cb, LV_EVENT_VALUE_CHANGED, NULL);

        /*This makes the checkboxes act as radio buttons*/
        lv_obj_add_flag(obj, LV_OBJ_FLAG_RADIO_BUTTON);

        lv_obj_add_style(obj, &style_radio, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &style_radio_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);
    }

    /*Make the first checkbox checked*/
    //    lv_obj_add_state(lv_obj_get_child(cont, 0), LV_STATE_CHECKED);
}

#endif
