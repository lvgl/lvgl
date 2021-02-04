//#include "../../lv_examples.h"
//
//
///**
// * Create styles from scratch for buttons.
// */
//void lv_ex_get_started_2(void)
//{
//    static lv_style_t style_btn;
//    static lv_style_t style_btn_red;
//
//    /*Create a simple button style*/
//    lv_style_init(&style_btn);
//    lv_style_set_radius(&style_btn, LV_STATE_DEFAULT, 10);
//    lv_style_set_bg_opa(&style_btn, LV_STATE_DEFAULT, LV_OPA_COVER);
//    lv_style_set_bg_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_SILVER);
//    lv_style_set_bg_grad_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_GRAY);
//    lv_style_set_bg_grad_dir(&style_btn, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
//
//    /*Swap the colors in pressed state*/
//    lv_style_set_bg_color(&style_btn, LV_STATE_PRESSED, LV_COLOR_GRAY);
//    lv_style_set_bg_grad_color(&style_btn, LV_STATE_PRESSED, LV_COLOR_SILVER);
//
//    /*Add a border*/
//    lv_style_set_border_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//    lv_style_set_border_opa(&style_btn, LV_STATE_DEFAULT, LV_OPA_70);
//    lv_style_set_border_width(&style_btn, LV_STATE_DEFAULT, 2);
//
//    /*Different border color in focused state*/
//    lv_style_set_border_color(&style_btn, LV_STATE_FOCUSED, LV_COLOR_BLUE);
//    lv_style_set_border_color(&style_btn, LV_STATE_FOCUSED | LV_STATE_PRESSED, LV_COLOR_NAVY);
//
//    /*Set the text style*/
//    lv_style_set_text_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//
//    /*Make the button smaller when pressed*/
//    lv_style_set_transform_height(&style_btn, LV_STATE_PRESSED, -5);
//    lv_style_set_transform_width(&style_btn, LV_STATE_PRESSED, -10);
//#if LV_USE_ANIMATION
//    /*Add a transition to the size change*/
//    static lv_anim_path_t path;
//    lv_anim_path_init(&path);
//    lv_anim_path_set_cb(&path, lv_anim_path_overshoot);
//
//    lv_style_set_transition_prop_1(&style_btn, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_HEIGHT);
//    lv_style_set_transition_prop_2(&style_btn, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_WIDTH);
//    lv_style_set_transition_time(&style_btn, LV_STATE_DEFAULT, 300);
//    lv_style_set_transition_path(&style_btn, LV_STATE_DEFAULT, &path);
//#endif
//
//    /*Create a red style. Change only some colors.*/
//    lv_style_init(&style_btn_red);
//    lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
//    lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_MAROON);
//    lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_MAROON);
//    lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);
//    lv_style_set_text_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//#if LV_USE_BTN
//    /*Create buttons and use the new styles*/
//    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
//    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
//    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
//    lv_obj_reset_style_list(btn, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
//    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &style_btn);
//
//    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
//    lv_label_set_text(label, "Button");                     /*Set the labels text*/
//
//    /*Create a new button*/
//    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn);
//    lv_obj_set_pos(btn2, 10, 80);
//    lv_obj_set_size(btn2, 120, 50);                             /*Set its size*/
//    lv_obj_reset_style_list(btn2, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
//    lv_obj_add_style(btn2, LV_BTN_PART_MAIN, &style_btn);
//    lv_obj_add_style(btn2, LV_BTN_PART_MAIN, &style_btn_red);   /*Add the red style on top of the current */
//    lv_obj_set_style_local_radius(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE); /*Add a local style*/
//
//    label = lv_label_create(btn2, NULL);          /*Add a label to the button*/
//    lv_label_set_text(label, "Button 2");                     /*Set the labels text*/
//#endif
//}
//
//
