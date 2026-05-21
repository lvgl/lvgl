/**
 * @file lv_example_buttonmatrix_popover.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

/**
 * @title Button matrix popover preview
 * @brief Show a magnified label above a button while it is pressed.
 *
 * The `popover` flag mirrors the on-screen keyboard convention: while a button is
 * held down, its text floats up in a small popover above the finger so the user
 * can still read what they are pressing. The entire top row carries `popover` in
 * `ctrl_map`, which is how a keyboard layer typically enables the feature.
 */
void lv_example_buttonmatrix_popover(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Press and hold any letter: a popover appears above the pressed key while the press is active. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button matrix: popover preview on press");

    /* Keyboard-style row with popover enabled on every button */
    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(screen);
    lv_obj_set_align(lv_buttonmatrix_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_buttonmatrix_0, 40);
    lv_obj_set_size(lv_buttonmatrix_0, lv_pct(90), 60);
    static const char * lv_buttonmatrix_0_map_0[] = {"Q", "W", "E", "R", "T", "Y", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_0, lv_buttonmatrix_0_map_0);
    static const lv_buttonmatrix_ctrl_t lv_buttonmatrix_0_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_POPOVER, LV_BUTTONMATRIX_CTRL_POPOVER, LV_BUTTONMATRIX_CTRL_POPOVER, LV_BUTTONMATRIX_CTRL_POPOVER, LV_BUTTONMATRIX_CTRL_POPOVER, LV_BUTTONMATRIX_CTRL_POPOVER};
    lv_buttonmatrix_set_ctrl_map(lv_buttonmatrix_0, lv_buttonmatrix_0_ctrl_map_1);
}
#endif
