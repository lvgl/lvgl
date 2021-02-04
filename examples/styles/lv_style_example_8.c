//#include "../../lv_examples.h"
//
///**
// * Using the line style properties
// */
//void lv_ex_style_8(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    lv_style_set_line_color(&style, LV_STATE_DEFAULT, LV_COLOR_GRAY);
//    lv_style_set_line_width(&style, LV_STATE_DEFAULT, 6);
//    lv_style_set_line_rounded(&style, LV_STATE_DEFAULT, true);
//#if LV_USE_LINE
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_line_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_LINE_PART_MAIN, &style);
//
//    static lv_point_t p[] = {{10, 30}, {30, 50}, {100, 0}};
//    lv_line_set_points(obj, p, 3);
//
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//#endif
//}
