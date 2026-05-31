/**
 * @file lv_example_spinbox_value.c
 */

#include "../../lv_examples.h"
#if LV_USE_SPINBOX && LV_BUILD_EXAMPLES

/**
 * @title Spinbox value, range, and step
 * @brief Pin the initial value, clamp it to a numeric range, and set the per-step delta.
 *
 * `value` is the starting number shown in the spinbox. `min_value`/`max_value`
 * clamp every adjustment — pressing the increment past the maximum stays at
 * the maximum. `step` is how much each key/press changes the value. With
 * `rollover="false"` (the default) the value sticks at the bounds; see
 * `spinbox_rollover` for the wraparound variant.
 */
void lv_example_spinbox_value(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit `step` to 5 or 10; each press now changes by that delta until a bound is hit. */
    lv_obj_t * spinbox = lv_spinbox_create(screen);
    lv_obj_set_width(spinbox, 160);
    lv_spinbox_set_digit_count(spinbox, 3);
    lv_spinbox_set_value(spinbox, 25);
    lv_spinbox_set_min_value(spinbox, 0);
    lv_spinbox_set_max_value(spinbox, 100);
    lv_spinbox_set_step(spinbox, 1);
}
#endif
