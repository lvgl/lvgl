/**
 * @file lv_example_led_color.c
 */

#include "../../lv_examples.h"
#if LV_USE_LED && LV_BUILD_EXAMPLES

/**
 * @title LED color
 * @brief Three LEDs lit in different hues via the color attribute.
 *
 * The color attribute sets the LED's background, border, and shadow color together, so
 * a single value drives the whole lit-bulb look. Brightness stays at the default here
 * to isolate the effect of changing the color alone.
 */
void lv_example_led_color(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Red LED */
    lv_obj_t * led_1 = lv_led_create(screen);
    lv_obj_set_size(led_1, 40, 40);
    lv_led_set_color(led_1, lv_color_hex(0xff3030));

    /* Green LED */
    lv_obj_t * led_2 = lv_led_create(screen);
    lv_obj_set_size(led_2, 40, 40);
    lv_led_set_color(led_2, lv_color_hex(0x30c050));

    /* Blue LED */
    lv_obj_t * led_3 = lv_led_create(screen);
    lv_obj_set_size(led_3, 40, 40);
    lv_led_set_color(led_3, lv_color_hex(0x3080ff));
}
#endif
