/**
 * @file lv_example_get_started_hello_world.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Hello world label
 * @brief Paint the screen background and center a label on it.
 *
 * The view sets its own `style_bg_color` to a dark teal and `style_text_color`
 * to white. The label sets no color of its own — it inherits white from the
 * view — and `align="center"` places it in the middle of the display.
 */
void lv_example_get_started_hello_world(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x003a57), 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xffffff), 0);

    lv_obj_t * label = lv_label_create(screen);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_label_set_text(label, "Hello world");
}
#endif
