#include "../../lv_examples.h"
#if LV_USE_TEXTAREA && LV_USE_KEYBOARD && LV_BUILD_EXAMPLES

/**
 * @title Password text area
 * @brief Mask typed characters with password mode.
 *
 * A one-line textarea has `lv_textarea_set_password_mode` enabled, so each
 * typed character shows briefly and then turns into a bullet. An
 * `lv_keyboard` docked at the bottom is bound to the textarea so the masking
 * can be tried interactively.
 */
void lv_example_textarea_password(void)
{
    lv_obj_t * pwd_ta = lv_textarea_create(lv_screen_active());
    lv_textarea_set_password_mode(pwd_ta, true);
    lv_textarea_set_one_line(pwd_ta, true);
    lv_textarea_set_placeholder_text(pwd_ta, "Password");
    lv_obj_set_width(pwd_ta, lv_pct(80));
    lv_obj_align(pwd_ta, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * kb = lv_keyboard_create(lv_screen_active());
    lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
    lv_keyboard_set_textarea(kb, pwd_ta);
}

#endif
