/**
 * @file lv_example_keyboard_popovers.c
 */

#include "../../lv_examples.h"
#if LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

/**
 * @title Keyboard pop-overs
 * @brief Enlarged preview balloons appear above the pressed key while held.
 *
 * With `popovers="true"` LVGL renders a larger preview of the active key
 * above the keyboard while it's pressed, the same way mobile OS keyboards do.
 * The preview only renders for keys that produce a symbol (so space and
 * modifier keys stay quiet). Pop-overs in the top row can extend outside the
 * keyboard's bounds — leave free space above it (here the textarea provides
 * that headroom).
 */
void lv_example_keyboard_popovers(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Hold any letter key and watch the magnified pop-over appear above. */
    lv_obj_t * textarea = lv_textarea_create(screen);
    lv_obj_set_align(textarea, LV_ALIGN_TOP_MID);
    lv_obj_set_y(textarea, 10);
    lv_obj_set_width(textarea, lv_pct(90));
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_placeholder_text(textarea, "Press and hold a key");

    lv_obj_t * keyboard = lv_keyboard_create(screen);
    lv_obj_set_align(keyboard, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(keyboard, lv_pct(100), lv_pct(60));
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_keyboard_set_popovers(keyboard, true);
}
#endif
