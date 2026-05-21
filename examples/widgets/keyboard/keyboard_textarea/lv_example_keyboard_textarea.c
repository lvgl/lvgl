/**
 * @file lv_example_keyboard_textarea.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Keyboard: with text area");

    lv_obj_t * lv_textarea_0 = lv_textarea_create(screen);
    lv_obj_set_align(lv_textarea_0, LV_ALIGN_TOP_MID);
    lv_obj_set_y(lv_textarea_0, 40);
    lv_obj_set_size(lv_textarea_0, lv_pct(90), 80);
    lv_textarea_set_one_line(lv_textarea_0, true);
    lv_textarea_set_placeholder_text(lv_textarea_0, "Type here...");

    lv_obj_t * lv_keyboard_0 = lv_keyboard_create(screen);
    lv_obj_set_align(lv_keyboard_0, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(lv_keyboard_0, lv_pct(100), lv_pct(40));
    lv_keyboard_set_mode(lv_keyboard_0, LV_KEYBOARD_MODE_TEXT_LOWER);
}
#endif
