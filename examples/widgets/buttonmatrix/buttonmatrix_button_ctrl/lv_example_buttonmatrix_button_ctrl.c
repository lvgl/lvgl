/**
 * @file lv_example_buttonmatrix_button_ctrl.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BUTTONMATRIX && LV_BUILD_EXAMPLES

/**
 * @title Button matrix per-button control flags
 * @brief Apply checkable, checked, disabled, and hidden flags via `ctrl_map`.
 *
 * Each token in `ctrl_map` is a `|`-separated list of flag names that applies to
 * the matching button in `map`. The four buttons here show a plain button, a
 * checkable button that starts checked (`checkable|checked`), a `disabled` button,
 * and a `hidden` button which still reserves its slot in the layout.
 */
void lv_example_buttonmatrix_button_ctrl(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Edit individual `ctrl_map` tokens to see how each button state is rendered and which still receives input. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Button matrix: per-button control flags");

    /* One row with four differently-flagged buttons */
    lv_obj_t * lv_buttonmatrix_0 = lv_buttonmatrix_create(screen);
    lv_obj_set_align(lv_buttonmatrix_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_buttonmatrix_0, 30);
    lv_obj_set_size(lv_buttonmatrix_0, lv_pct(90), 60);
    static const char * lv_buttonmatrix_0_map_0[] = {"Normal", "Checked", "Disabled", "Hidden", NULL};
    lv_buttonmatrix_set_map(lv_buttonmatrix_0, lv_buttonmatrix_0_map_0);
    static const lv_buttonmatrix_ctrl_t lv_buttonmatrix_0_ctrl_map_1[] = {LV_BUTTONMATRIX_CTRL_NONE, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_CHECKABLE | LV_BUTTONMATRIX_CTRL_CHECKED), LV_BUTTONMATRIX_CTRL_DISABLED, LV_BUTTONMATRIX_CTRL_HIDDEN};
    lv_buttonmatrix_set_ctrl_map(lv_buttonmatrix_0, lv_buttonmatrix_0_ctrl_map_1);
}
#endif
