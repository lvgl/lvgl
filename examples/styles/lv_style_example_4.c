//#include "../../lv_examples.h"
//
///**
// * Using the Shadow style properties
// */
//void lv_ex_style_4(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    /*Set a background color and a radius*/
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//
//    /*Add a shadow*/
//    lv_style_set_shadow_width(&style, LV_STATE_DEFAULT, 8);
//    lv_style_set_shadow_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_shadow_ofs_x(&style, LV_STATE_DEFAULT, 10);
//    lv_style_set_shadow_ofs_y(&style, LV_STATE_DEFAULT, 20);
//
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//}
//
