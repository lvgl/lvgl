//#include "../../lv_examples.h"
//
///**
// * Using the image style properties
// */
//void lv_example_style_9(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    /*Set a background color and a radius*/
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//    lv_style_set_border_width(&style, LV_STATE_DEFAULT, 2);
//    lv_style_set_border_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//
//    lv_style_set_pad_top(&style, LV_STATE_DEFAULT, 10);
//    lv_style_set_pad_bottom(&style, LV_STATE_DEFAULT, 10);
//    lv_style_set_pad_left(&style, LV_STATE_DEFAULT, 10);
//    lv_style_set_pad_right(&style, LV_STATE_DEFAULT, 10);
//
//    lv_style_set_image_recolor(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_image_recolor_opa(&style, LV_STATE_DEFAULT, LV_OPA_50);
//
//#if LV_USE_IMG
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_img_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_IMG_PART_MAIN, &style);
//    LV_IMG_DECLARE(img_cogwheel_argb);
//    lv_img_set_src(obj, &img_cogwheel_argb);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//#endif
//}
//
