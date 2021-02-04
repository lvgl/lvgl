//#include "../../lv_examples.h"
//
///**
// * Using the value style properties
// */
//void lv_ex_style_6(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    /*Set a background color and a radius*/
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//
//    /*Add a value text properties*/
//    lv_style_set_value_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_value_align(&style, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_RIGHT);
//    lv_style_set_value_ofs_x(&style, LV_STATE_DEFAULT, 10);
//    lv_style_set_value_ofs_y(&style, LV_STATE_DEFAULT, 10);
//
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//
//    /*Add a value text to the local style. This way every object can have different text*/
//    lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "Text");
//}
//
