/**
 * @file lv_example_style_background.c
 */

#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Background gradient
 * @brief Blend two colors vertically and shift where the blend happens.
 *
 * `bg_color` and `bg_grad_color` are the two stops; `bg_grad_dir="ver"`
 * blends top-to-bottom. `bg_main_stop="80"` keeps the indigo solid only
 * briefly and `bg_grad_stop="220"` stretches the blend almost to the
 * bottom, so most of the card is transition rather than flat color.
 */
void lv_example_style_background(void)
{
    static lv_style_t style_grad;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_grad);
        lv_style_set_radius(&style_grad, 20);
        lv_style_set_bg_opa(&style_grad, (255 * 100 / 100));
        lv_style_set_bg_color(&style_grad, lv_color_hex(0x6366f1));
        lv_style_set_bg_grad_color(&style_grad, lv_color_hex(0xec4899));
        lv_style_set_bg_grad_dir(&style_grad, LV_GRAD_DIR_VER);
        lv_style_set_bg_main_stop(&style_grad, 80);
        lv_style_set_bg_grad_stop(&style_grad, 220);
        lv_style_set_shadow_color(&style_grad, lv_color_hex(0x6366f1));
        lv_style_set_shadow_width(&style_grad, 26);
        lv_style_set_shadow_offset_y(&style_grad, 10);
        lv_style_set_shadow_opa(&style_grad, 70);
        lv_style_set_text_color(&style_grad, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Slide `bg_main_stop`/`bg_grad_stop` (0..255) to move where the indigo→pink blend starts and ends. */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_size(container, 210, 150);
    lv_obj_set_align(container, LV_ALIGN_CENTER);
    lv_obj_add_style(container, &style_grad, 0);
    lv_obj_t * label = lv_label_create(container);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Gradient");
}
#endif
