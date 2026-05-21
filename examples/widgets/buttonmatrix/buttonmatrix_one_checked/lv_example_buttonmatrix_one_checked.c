/**
 * @file lv_example_buttonmatrix_one_checked.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

/**
 * @title Button matrix one-checked mode
 * @brief Make the matrix behave like a radio group with a single checked button.
 *
 * With `one_checked="true"`, the buttons that carry `checkable` in `ctrl_map` form
 * a mutually exclusive group: pressing one un-checks the previously checked button.
 * Every weekday here is `checkable`, and Tue starts as `checkable|checked` so the
 * example doubles as a small weekday selector.
 */
void lv_example_buttonmatrix_one_checked(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Tap any day: it becomes checked and the previously checked button clears automatically. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button matrix: one-checked selection");

    /* Weekday selector with single-checked semantics */
    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(screen);
    lv_obj_set_align(lv_buttonmatrix_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_buttonmatrix_0, 30);
    lv_obj_set_size(lv_buttonmatrix_0, lv_pct(90), 60);
    lv_buttonmatrix_set_one_checked(lv_buttonmatrix_0, true);
    static const char * lv_buttonmatrix_0_map_0[] = {"Mon", "Tue", "Wed", "Thu", "Fri", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_0, lv_buttonmatrix_0_map_0);
    static const lv_buttonmatrix_ctrl_t lv_buttonmatrix_0_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_CHECKABLE, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED), LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE};
    lv_buttonmatrix_set_ctrl_map(lv_buttonmatrix_0, lv_buttonmatrix_0_ctrl_map_1);
}
#endif
