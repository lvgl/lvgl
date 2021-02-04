//#include "../../lv_examples.h"
//
///**
// * Using the scale style properties
// */
//void lv_ex_style_11(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    /*Set a background color and a radius*/
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//
//    /*Set some paddings*/
//    lv_style_set_pad_top(&style, LV_STATE_DEFAULT, 20);
//    lv_style_set_pad_left(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_pad_right(&style, LV_STATE_DEFAULT, 5);
//
//    lv_style_set_scale_end_color(&style, LV_STATE_DEFAULT, LV_COLOR_RED);
//    lv_style_set_line_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//    lv_style_set_scale_grad_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_line_width(&style, LV_STATE_DEFAULT, 2);
//    lv_style_set_scale_end_line_width(&style, LV_STATE_DEFAULT, 4);
//    lv_style_set_scale_end_border_width(&style, LV_STATE_DEFAULT, 4);
//
//    /*Gauge has a needle but for simplicity its style is not initialized here*/
//#if LV_USE_GAUGE
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_gauge_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_GAUGE_PART_MAIN, &style);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//#endif
//}
//
