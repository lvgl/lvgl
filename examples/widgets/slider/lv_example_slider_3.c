#include "../../lv_examples.h"
//#if LV_USE_SLIDER && LV_BUILD_EXAMPLES
//
//static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
//
///**
// * Show the current value when the slider if pressed (using only styles).
// *
// */
void lv_example_slider_3(void)
{
//    /*Create a slider in the center of the display*/
//    lv_obj_t * slider;
//    slider = lv_slider_create(lv_scr_act());
//    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_add_event_cb(slider, slider_event_cb, NULL);
//    lv_slider_set_mode(slider, LV_SLIDER_MODE_RANGE);
//
//    lv_slider_set_value(slider, 70, LV_ANIM_OFF);
//    lv_slider_set_left_value(slider, 20, LV_ANIM_OFF);
//
//    /*Now use only a local style.*/
//    lv_obj_set_style_content_ofs_y(slider, LV_PART_INDICATOR, LV_STATE_DEFAULT, -20);
//    lv_obj_set_style_content_color(slider, LV_PART_INDICATOR, LV_STATE_DEFAULT, lv_color_grey_darken_3());
//
//    /*To update the value text*/
//    lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
}
//
//static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
//{
//    if(code == LV_EVENT_VALUE_CHANGED) {
//        static char buf[8];
//        lv_snprintf(buf, sizeof(buf), "%d - %d", lv_slider_get_value(slider), lv_slider_get_left_value(slider));
//        lv_obj_set_style_content_text(slider, LV_PART_INDICATOR, LV_STATE_DEFAULT, buf);
//    }
//}
//
//#endif
