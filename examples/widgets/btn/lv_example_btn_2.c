#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_BTN

/**
 * Style a button from scratch
 */
void lv_example_btn_2(void)
{

    static lv_style_t style;
    static lv_style_t style_pr;
    lv_style_init(&style);
    lv_style_init(&style_pr);

    /*Init the default style*/
    lv_style_set_radius(&style, 3);

    lv_style_set_bg_opa(&style, LV_OPA_70);
    lv_style_set_bg_color(&style, lv_color_blue());
    lv_style_set_bg_grad_color(&style, lv_color_blue_darken_4());
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_color_grey());

    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_color_grey());
    lv_style_set_shadow_ofs_x(&style, 8);
    lv_style_set_shadow_ofs_y(&style, 8);

    lv_style_set_text_color(&style, lv_color_white());

    /*Init the pressed style*/
    lv_style_set_shadow_ofs_x(&style_pr, 4);
    lv_style_set_shadow_ofs_y(&style_pr, 4);
    lv_style_set_color_filter_cb(&style_pr, lv_color_darken);   /*Darken every color*/
    lv_style_set_color_filter_opa(&style_pr, LV_OPA_30);

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_remove_style(btn1, LV_PART_ANY, LV_STATE_ANY, NULL);
    lv_obj_add_style(btn1, LV_PART_MAIN, LV_STATE_DEFAULT, &style);
    lv_obj_add_style(btn1, LV_PART_MAIN, LV_STATE_PRESSED, &style_pr);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

}
#endif
