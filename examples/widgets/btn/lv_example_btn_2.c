#include "../../../lvgl.h"
#include <stdio.h>
#if LV_USE_BTN && LV_BUILD_EXAMPLES

/**
 * Style a button from scratch
 */
void lv_example_btn_2(void)
{
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, &lv_anim_path_def, 300, 0);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_color_blue());

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);
    lv_style_set_transition(&style_pr, &trans);

//
//    /*Init the default style*/
//    lv_style_set_radius(&style, 3);
//
//    lv_style_set_bg_opa(&style, LV_OPA_100);
//    lv_style_set_bg_color(&style, lv_color_blue());
//    lv_style_set_bg_grad_color(&style, lv_color_blue_darken_2());
//    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);
//
//    lv_style_set_border_opa(&style, LV_OPA_40);
//    lv_style_set_border_width(&style, 2);
//    lv_style_set_border_color(&style, lv_color_grey());
//
//    lv_style_set_shadow_width(&style, 8);
//    lv_style_set_shadow_color(&style, lv_color_grey());
//    lv_style_set_shadow_ofs_y(&style, 8);
//
//    lv_style_set_text_color(&style, lv_color_white());
//
//    lv_style_set_pad_all(&style, 10);
//    lv_style_set_pad_all(&style_pr, 40);

    /*Init the pressed style*/
    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_color_blue_darken_2());
    lv_style_set_bg_grad_color(&style_pr, lv_color_blue_darken_4());

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
//    lv_obj_remove_style(btn1, LV_PART_ANY, LV_STATE_ANY, NULL);
    lv_obj_add_style(btn1, LV_PART_MAIN, LV_STATE_DEFAULT, &style);
    lv_obj_add_style(btn1, LV_PART_MAIN, LV_STATE_PRESSED, &style_pr);
    lv_obj_set_size(btn1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn1);

    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);
}
#endif
