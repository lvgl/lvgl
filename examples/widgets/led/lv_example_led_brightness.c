/**
 * @file lv_example_led_brightness.c
 */

#include "../../lv_examples.h"
#if LV_USE_LED && LV_BUILD_EXAMPLES

/**
 * @title LED brightness
 * @brief Same hue, three different brightness values.
 *
 * The brightness attribute scales how lit each LED appears, in the 0..255 range. With
 * lower brightness the color darkens; with 255 it reaches the configured peak. The
 * three LEDs share one color so the effect of brightness alone is visible.
 */
void lv_example_led_brightness(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Dim */
    lv_obj_t * led_1 = lv_led_create(screen);
    lv_obj_set_size(led_1, 40, 40);
    lv_led_set_color(led_1, lv_color_hex(0xff8000));
    lv_led_set_brightness(led_1, 60);

    /* Mid */
    lv_obj_t * led_2 = lv_led_create(screen);
    lv_obj_set_size(led_2, 40, 40);
    lv_led_set_color(led_2, lv_color_hex(0xff8000));
    lv_led_set_brightness(led_2, 150);

    /* Full */
    lv_obj_t * led_3 = lv_led_create(screen);
    lv_obj_set_size(led_3, 40, 40);
    lv_led_set_color(led_3, lv_color_hex(0xff8000));
    lv_led_set_brightness(led_3, 255);
}
#endif
