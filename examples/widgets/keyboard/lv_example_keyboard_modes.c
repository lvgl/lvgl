/**
 * @file lv_example_keyboard_modes.c
 */

#include "../../lv_examples.h"
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
    lv_obj_t * textarea = lv_textarea_create(screen);
    lv_obj_set_align(textarea, LV_ALIGN_TOP_MID);
    lv_obj_set_y(textarea, 10);
    lv_obj_set_size(textarea, lv_pct(90), 60);
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_placeholder_text(textarea, "Numbers only");

    lv_obj_t * keyboard = lv_keyboard_create(screen);
    lv_obj_set_align(keyboard, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(keyboard, lv_pct(100), lv_pct(50));
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
}
#endif
