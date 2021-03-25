#include "../../lvgl.h"
#if LV_USE_BTN && LV_BUILD_EXAMPLES

/**
 * Create styles from scratch for buttons.
 */
void lv_example_get_started_2(void)
{
    static lv_style_t style_btn;
    static lv_style_t style_btn_red;
    static lv_style_t style_btn_pressed;

    /*Create a simple button style*/
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_color_grey_lighten_3());
    lv_style_set_bg_grad_color(&style_btn, lv_color_grey());
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    /*Add a border*/
    lv_style_set_border_color(&style_btn, lv_color_white());
    lv_style_set_border_opa(&style_btn, LV_OPA_70);
    lv_style_set_border_width(&style_btn, 2);

    /*Set the text style*/
    lv_style_set_text_color(&style_btn, lv_color_white());

    /*Create a red style. Change only some colors.*/
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_color_light_blue());
    lv_style_set_bg_grad_color(&style_btn_red, lv_color_light_blue_darken_3());

    /*Create a style for the pressed state. Add color filter to make every color darker*/
    lv_style_init(&style_btn_pressed);
    lv_style_set_bg_color(&style_btn_red, lv_color_blue());
    lv_style_set_bg_grad_color(&style_btn_red, lv_color_blue_darken_3());

    /*Create a button and use the new styles*/
    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_remove_style(btn, LV_PART_ANY, LV_STATE_ANY, NULL); /*Remove the styles coming from the theme*/
    lv_obj_add_style(btn, LV_PART_MAIN, LV_STATE_DEFAULT, &style_btn);
    lv_obj_add_style(btn, LV_PART_MAIN, LV_STATE_PRESSED, &style_btn_pressed);

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Create an other button and use the red style too*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn2, 10, 80);
    lv_obj_set_size(btn2, 120, 50);                            /*Set its size*/
    lv_obj_remove_style(btn2, LV_PART_ANY, LV_STATE_ANY, NULL); /*Remove the styles coming from the theme*/
    lv_obj_add_style(btn2, LV_PART_MAIN, LV_STATE_DEFAULT, &style_btn);
    lv_obj_add_style(btn2, LV_PART_MAIN, LV_STATE_DEFAULT, &style_btn_red);
    lv_obj_add_style(btn2, LV_PART_MAIN, LV_STATE_PRESSED, &style_btn_pressed);
    lv_obj_set_style_radius(btn2, LV_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE); /*Add a local style*/

    label = lv_label_create(btn2);          /*Add a label to the button*/
    lv_label_set_text(label, "Button 2");         /*Set the labels text*/
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

#endif
