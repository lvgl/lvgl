/**
 * @file lv_example_keyboard_modes.c
 */

#include "../../../lv_examples.h"
#if LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

/**
 * @title Keyboard modes
 * @brief Switch between text and number layouts via the `mode` attribute.
 *
 * `mode` selects the keymap LVGL shows. `text_lower`/`text_upper` give the
 * letter layouts with case shift keys; `special` swaps in punctuation;
 * `number` shows the calculator-style numeric pad. The mode persists until
 * changed — pressing the in-layout `ABC`/`abc`/`1#` keys switches the keymap
 * at runtime, while this attribute pins the initial layout.
 */
void lv_example_keyboard_modes(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Change `mode` from `number` to `text_lower`, `text_upper`, or `special` to see each keymap. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Keyboard: modes");

    lv_obj_t * lv_textarea_0 = lv_textarea_create(screen);
    lv_obj_set_align(lv_textarea_0, LV_ALIGN_TOP_MID);
    lv_obj_set_y(lv_textarea_0, 40);
    lv_obj_set_size(lv_textarea_0, lv_pct(90), 60);
    lv_textarea_set_one_line(lv_textarea_0, true);
    lv_textarea_set_placeholder_text(lv_textarea_0, "Numbers only");

    lv_obj_t * lv_keyboard_0 = lv_keyboard_create(screen);
    lv_obj_set_align(lv_keyboard_0, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(lv_keyboard_0, lv_pct(100), lv_pct(50));
    lv_keyboard_set_mode(lv_keyboard_0, LV_KEYBOARD_MODE_NUMBER);
}
#endif
