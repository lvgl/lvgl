//#include "../../lv_examples.h"
//
///**
// * Using the border style properties
// */
//void lv_example_style_2(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    /*Set a background color and a radius*/
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 20);
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//
//    /*Add border to the bottom+right*/
//    lv_style_set_border_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_border_width(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_border_opa(&style, LV_STATE_DEFAULT, LV_OPA_50);
//    lv_style_set_border_side(&style, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_RIGHT);
//
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//}
//
