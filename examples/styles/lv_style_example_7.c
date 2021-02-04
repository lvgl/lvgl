//#include "../../lv_examples.h"
//
///**
// * Using the text style properties
// */
//void lv_ex_style_7(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
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
//    lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
//    lv_style_set_text_letter_space(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_text_line_space(&style, LV_STATE_DEFAULT, 20);
//    lv_style_set_text_decor(&style, LV_STATE_DEFAULT, LV_TEXT_DECOR_UNDERLINE);
//
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_label_create(lv_scr_act(), NULL);
//    lv_obj_add_style(obj, LV_LABEL_PART_MAIN, &style);
//    lv_label_set_text(obj, "Text of\n"
//                            "a label");
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//}
//
