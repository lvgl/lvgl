/**
 * @file lv_example_led_styling.c
 */

#include "../../../lv_examples.h"
#if LV_USE_LED && LV_BUILD_EXAMPLES

/**
 * @title LED styling
 * @brief Push the LED past its color attribute with border, outline, shadow, and shape.
 *
 * The color attribute drives the base lit look, but the LED is still a base widget so
 * the border / outline / shadow / radius stack all apply. The first LED keeps the round
 * default with a ring outline + glow; the second uses a small radius to render as a
 * square indicator with a hard border instead.
 */
void lv_example_led_styling(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_pad_top(screen, 56, 0);

    /* 💡 Combine color + border + outline + shadow to design status indicators. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "LED: styling");

    /* Round LED with a tight ring outline and a soft outer glow */
    lv_obj_t * lv_led_0 = lv_led_create(screen);
    lv_obj_set_size(lv_led_0, 50, 50);
    lv_led_set_color(lv_led_0, lv_color_hex(0x22c55e));
    lv_obj_set_style_border_color(lv_led_0, lv_color_hex(0x14532d), 0);
    lv_obj_set_style_border_width(lv_led_0, 2, 0);
    lv_obj_set_style_outline_color(lv_led_0, lv_color_hex(0x22c55e), 0);
    lv_obj_set_style_outline_width(lv_led_0, 6, 0);
    lv_obj_set_style_outline_pad(lv_led_0, 4, 0);
    lv_obj_set_style_outline_opa(lv_led_0, 80, 0);
    lv_obj_set_style_shadow_color(lv_led_0, lv_color_hex(0x22c55e), 0);
    lv_obj_set_style_shadow_width(lv_led_0, 20, 0);
    lv_obj_set_style_shadow_opa(lv_led_0, 200, 0);

    /* Square LED via small radius + hard border, no shadow */
    lv_obj_t * lv_led_1 = lv_led_create(screen);
    lv_obj_set_size(lv_led_1, 50, 50);
    lv_led_set_color(lv_led_1, lv_color_hex(0xef4444));
    lv_obj_set_style_radius(lv_led_1, 6, 0);
    lv_obj_set_style_border_color(lv_led_1, lv_color_hex(0x7f1d1d), 0);
    lv_obj_set_style_border_width(lv_led_1, 3, 0);
    lv_obj_set_style_shadow_width(lv_led_1, 0, 0);

    /* Dimmed LED with a heavy outline ring as a "pending" indicator */
    lv_obj_t * lv_led_2 = lv_led_create(screen);
    lv_obj_set_size(lv_led_2, 50, 50);
    lv_led_set_color(lv_led_2, lv_color_hex(0xf59e0b));
    lv_led_set_brightness(lv_led_2, 120);
    lv_obj_set_style_outline_color(lv_led_2, lv_color_hex(0xf59e0b), 0);
    lv_obj_set_style_outline_width(lv_led_2, 3, 0);
    lv_obj_set_style_outline_pad(lv_led_2, 6, 0);
    lv_obj_set_style_outline_opa(lv_led_2, 180, 0);
    lv_obj_set_style_shadow_width(lv_led_2, 0, 0);
}
#endif
