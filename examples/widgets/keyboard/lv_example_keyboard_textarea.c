/**
 * @file lv_example_keyboard_textarea.c
 */

#include "../../lv_examples.h"
#if LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

/**
 * @title Keyboard with text area
 * @brief Pair a keyboard with a textarea so typed keys land in the field.
 *
 * The keyboard and the textarea sit on the same screen; LVGL's default focus
 * group routes key presses to whichever input widget currently holds focus
 * (here the textarea is the only one). In runtime code you'd usually call
 * `lv_keyboard_set_textarea(kb, ta)` to pin the link explicitly, but for a
 * single textarea on a screen the default group is sufficient.
 */
void lv_example_keyboard_textarea(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Tap the textarea to focus it, then type — the keyboard writes into the focused input. */
    lv_obj_t * textarea = lv_textarea_create(screen);
    lv_obj_set_align(textarea, LV_ALIGN_TOP_MID);
    lv_obj_set_y(textarea, 10);
    lv_obj_set_width(textarea, lv_pct(90));
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_placeholder_text(textarea, "Type here...");

    lv_obj_t * keyboard = lv_keyboard_create(screen);
    lv_obj_set_align(keyboard, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(keyboard, lv_pct(100), lv_pct(60));
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
}
#endif
