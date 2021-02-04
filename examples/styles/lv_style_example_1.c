//#include "../../lv_examples.h"
//
///**
// * Using the background style properties
// */
//void lv_ex_style_1(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
//
//    /*Make a gradient*/
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//    lv_style_set_bg_grad_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_bg_grad_dir(&style, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
//
//    /*Shift the gradient to the bottom*/
//    lv_style_set_bg_main_stop(&style, LV_STATE_DEFAULT, 128);
//    lv_style_set_bg_grad_stop(&style, LV_STATE_DEFAULT, 192);
//
//
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//}
//
