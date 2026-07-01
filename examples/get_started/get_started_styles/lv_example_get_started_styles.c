/**
 * @file lv_example_get_started_styles.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Styles from scratch for buttons
 * @brief Build named button styles, then add a pressed state and an accent variant.
 *
 * `style_button` is the shared base: a rounded vertical grey gradient with a thin
 * translucent border. The first button adds `style_button_pressed` on the
 * `pressed` selector so its gradient darkens while held. The second stacks
 * `style_button_red` to repaint only the gradient and sets a local circular
 * radius, showing how a small style plus one local property re-skins a button
 * without redefining the base.
 */
void lv_example_get_started_styles(void)
{
    static lv_style_t style_button;
    static lv_style_t style_button_pressed;
    static lv_style_t style_button_red;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_button);
        lv_style_set_radius(&style_button, 10);
        lv_style_set_bg_opa(&style_button, (255 * 100 / 100));
        lv_style_set_bg_color(&style_button, lv_color_hex(0xeeeeee));
        lv_style_set_bg_grad_color(&style_button, lv_color_hex(0x9e9e9e));
        lv_style_set_bg_grad_dir(&style_button, LV_GRAD_DIR_VER);
        lv_style_set_border_color(&style_button, lv_color_hex(0x000000));
        lv_style_set_border_opa(&style_button, (255 * 20 / 100));
        lv_style_set_border_width(&style_button, 2);
        lv_style_set_text_color(&style_button, lv_color_hex(0x000000));

        lv_style_init(&style_button_pressed);
        lv_style_set_bg_color(&style_button_pressed, lv_color_hex(0xbdbdbd));
        lv_style_set_bg_grad_color(&style_button_pressed, lv_color_hex(0x757575));

        lv_style_init(&style_button_red);
        lv_style_set_bg_color(&style_button_red, lv_color_hex(0xf44336));
        lv_style_set_bg_grad_color(&style_button_red, lv_color_hex(0xffcdd2));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* Base style + a pressed-state override */
    lv_obj_t * button_1 = lv_button_create(screen);
    lv_obj_set_align(button_1, LV_ALIGN_CENTER);
    lv_obj_set_y(button_1, -35);
    lv_obj_set_size(button_1, 120, 50);
    lv_obj_add_style(button_1, &style_button, 0);
    lv_obj_add_style(button_1, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Button");

    /* Base + accent variant + a local circular radius */
    lv_obj_t * button_2 = lv_button_create(screen);
    lv_obj_set_align(button_2, LV_ALIGN_CENTER);
    lv_obj_set_y(button_2, 35);
    lv_obj_set_size(button_2, 120, 50);
    lv_obj_set_style_radius(button_2, 25, 0);
    lv_obj_add_style(button_2, &style_button, 0);
    lv_obj_add_style(button_2, &style_button_red, 0);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Button 2");
}
#endif
