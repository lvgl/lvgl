/**
 * @file lv_example_buttonmatrix_one_checked.c
 */

#include "../../lv_examples.h"
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

    /* Weekday selector with single-checked semantics */
    lv_obj_t * buttonmatrix = lv_buttonmatrix_create(screen);
    lv_obj_set_align(buttonmatrix, LV_ALIGN_CENTER);
    lv_obj_set_size(buttonmatrix, lv_pct(90), 80);
    lv_buttonmatrix_set_one_checked(buttonmatrix, true);
    static const char * buttonmatrix_map_0[] = {"Mon", "Tue", "Wed", "Thu", "Fri", NULL};
    lv_buttonmatrix_set_map(buttonmatrix, buttonmatrix_map_0);
    static const lv_buttonmatrix_ctrl_t buttonmatrix_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_CHECKABLE, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED), LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE, LV_BUTTONMATRIX_CTRL_CHECKABLE};
    lv_buttonmatrix_set_ctrl_map(buttonmatrix, buttonmatrix_ctrl_map_1);
}
#endif
