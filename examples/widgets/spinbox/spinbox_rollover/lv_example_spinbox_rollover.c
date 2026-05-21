/**
 * @file lv_example_spinbox_rollover.c
 */

#include "../../../lv_examples.h"
#if LV_USE_SPINBOX && LV_BUILD_EXAMPLES

/**
 * @title Spinbox rollover
 * @brief Wrap the value around when it hits a bound instead of sticking.
 *
 * `rollover="true"` makes the spinbox treat its range as circular: pressing
 * past `max_value` jumps to `min_value` and vice versa. This is convenient
 * for cyclic quantities like hours (0..23) or angles (0..359), where the user
 * should be able to step from 23 → 0 without re-entering the value.
 */
void lv_example_spinbox_rollover(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Increment past 23; the value wraps to 0 because `rollover="true"`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Spinbox: rollover");

    lv_obj_t * lv_spinbox_0 = lv_spinbox_create(screen);
    lv_obj_set_width(lv_spinbox_0, 160);
    lv_spinbox_set_digit_count(lv_spinbox_0, 2);
    lv_spinbox_set_value(lv_spinbox_0, 22);
    lv_spinbox_set_min_value(lv_spinbox_0, 0);
    lv_spinbox_set_max_value(lv_spinbox_0, 23);
    lv_spinbox_set_step(lv_spinbox_0, 1);
    lv_spinbox_set_rollover(lv_spinbox_0, true);
}
#endif
