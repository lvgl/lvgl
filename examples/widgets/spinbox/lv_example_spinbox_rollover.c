/**
 * @file lv_example_spinbox_rollover.c
 */

#include "../../lv_examples.h"
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

    /* 💡 Increment past 23; the value wraps to 0 because `rollover="true"`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_label_set_text(lv_label_0, "Use the arrows to change the value");
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_label_0, -50);

    lv_obj_t * spinbox = lv_spinbox_create(screen);
    lv_obj_set_width(spinbox, 160);
    lv_spinbox_set_digit_count(spinbox, 2);
    lv_spinbox_set_value(spinbox, 22);
    lv_spinbox_set_min_value(spinbox, 0);
    lv_spinbox_set_max_value(spinbox, 23);
    lv_spinbox_set_step(spinbox, 1);
    lv_obj_set_align(spinbox, LV_ALIGN_CENTER);
    lv_spinbox_set_rollover(spinbox, true);
}
#endif
