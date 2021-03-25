//#include "../../../lvgl.h"
//#if LV_USE_SLIDER && LV_BUILD_EXAMPLES
//
//static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
//
//static lv_style_t style_pr;
//static lv_style_t style_def;
//
///**
// * Show the current value when the slider if pressed using a fancy style transition.
// */
//void lv_example_slider_2(void)
//{
//    lv_style_init(&style_def);
//    lv_style_set_content_opa(&style_def, LV_OPA_TRANSP);
//    lv_style_set_content_align(&style_def, LV_ALIGN_OUT_TOP_MID);
//
//    lv_style_init(&style_pr);
//    lv_style_set_content_opa(&style_pr, LV_OPA_COVER);
//    lv_style_set_content_ofs_y(&style_pr, -15);
//
//    /*Create a slider in the center of the display*/
//    lv_obj_t * slider;
//    slider = lv_slider_create(lv_scr_act());
//    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_add_event_cb(slider, slider_event_cb, NULL);
//
//    lv_obj_add_style(slider, LV_PART_KNOB, LV_STATE_DEFAULT, &style_def);
//    lv_obj_add_style(slider, LV_PART_KNOB, LV_STATE_PRESSED, &style_pr);
//}
//
//static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
//{
//    if(event == LV_EVENT_VALUE_CHANGED) {
//        static char buf[8];
//        lv_snprintf(buf, sizeof(buf), "%u", lv_slider_get_value(slider));
//        lv_obj_set_style_content_text(slider, LV_PART_KNOB, LV_STATE_DEFAULT, buf);
//    }
//}
//
//#endif
