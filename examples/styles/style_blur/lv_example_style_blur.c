/**
 * @file lv_example_style_blur.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Backdrop blur
 * @brief Frost the text sitting behind a translucent card.
 *
 * A paragraph fills the screen and the glass card is centered over its
 * middle. The card's `blur_backdrop="true"` with `blur_radius="14"` blurs
 * only what is behind it, and `bg_opa="40%"` keeps the card translucent so
 * the blurred text shows through — sharp text around it, soft text under it.
 */
void lv_example_style_blur(void)
{
    static lv_style_t style_glass;

    static bool inited = false;

    if(!inited) {
        lv_style_init(&style_glass);
        lv_style_set_radius(&style_glass, 40);
        lv_style_set_bg_opa(&style_glass, (255 * 40 / 100));
        lv_style_set_bg_color(&style_glass, lv_color_hex(0xffffff));
        lv_style_set_border_width(&style_glass, 1);
        lv_style_set_border_color(&style_glass, lv_color_hex(0xffffff));
        lv_style_set_border_opa(&style_glass, (255 * 60 / 100));
        lv_style_set_blur_backdrop(&style_glass, true);
        lv_style_set_blur_radius(&style_glass, 18);
        lv_style_set_text_color(&style_glass, lv_color_hex(0xffffff));

        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_opa(screen, (255 * 100 / 100), 0);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x6366f1), 0);
    lv_obj_set_style_bg_grad_color(screen, lv_color_hex(0xec4899), 0);
    lv_obj_set_style_bg_grad_dir(screen, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_pad_all(screen, 10, 0);

    /* 💡 The text under the card is blurred; lower the card's `bg_opa` or raise `blur_radius` for a stronger frost. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0,
                      "Hey there, I'm a little text in the background which is blurred. You might seem that some of parts are sharp, but other under the glass cannot be read. ");

    lv_obj_t * lv_button_0 = lv_button_create(screen);
    lv_obj_set_align(lv_button_0, LV_ALIGN_CENTER);
    lv_obj_set_style_pad_all(lv_button_0, 16, 0);
    lv_obj_set_y(lv_button_0, 55);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, "Click me");

    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_size(lv_obj_1, 210, 110);
    lv_obj_set_align(lv_obj_1, LV_ALIGN_CENTER);
    lv_obj_add_style(lv_obj_1, &style_glass, 0);
    lv_obj_t * lv_label_2 = lv_label_create(lv_obj_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Frosted glass");
}
#endif
