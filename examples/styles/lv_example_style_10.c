typedef int _keep_pedantic_happy;
//#include "../../lv_examples.h"
//
///**
// * Using the transitions style properties
// */
//void lv_example_style_10(void)
//{
//    static lv_style_t style;
//    lv_style_init(&style);
//
//    /*Set a background color and a radius*/
//    lv_style_set_radius(&style, LV_STATE_DEFAULT, 5);
//    lv_style_set_bg_opa(&style, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, lv_color_grey_lighten_3());
//
//    /*Set different background color in pressed state*/
//    lv_style_set_bg_color(&style, LV_STATE_PRESSED, lv_palette_main(LV_PALETTE_GREY));
//
//    /*Set different transition time in default and pressed state
//     *fast press, slower revert to default*/
//    lv_style_set_transition_time(&style, LV_STATE_DEFAULT, 500);
//    lv_style_set_transition_time(&style, LV_STATE_PRESSED, 200);
//
//    /*Small delay to make transition more visible*/
//    lv_style_set_transition_delay(&style, LV_STATE_DEFAULT, 100);
//
//    /*Add `bg_color` to transitioned properties*/
//    lv_style_set_transition_prop_1(&style, LV_STATE_DEFAULT, LV_STYLE_BG_COLOR);
//
//    /*Create an object with the new style*/
//    lv_obj_t * obj = lv_obj_create(lv_scr_act());
//    lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style);
//    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
//}
//
