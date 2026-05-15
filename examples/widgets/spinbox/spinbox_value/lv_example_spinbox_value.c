/**
 * @file lv_example_spinbox_value.c
 */

#include "../../../../lvgl.h"

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
void lv_example_spinbox_value_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit `step` to 5 or 10; each press now changes by that delta until a bound is hit. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Spinbox: value / range / step");

    lv_obj_t * lv_spinbox_0 = lv_spinbox_create(screen);
    lv_obj_set_width(lv_spinbox_0, 160);
    lv_spinbox_set_digit_count(lv_spinbox_0, 3);
    lv_spinbox_set_value(lv_spinbox_0, 25);
    lv_spinbox_set_min_value(lv_spinbox_0, 0);
    lv_spinbox_set_max_value(lv_spinbox_0, 100);
    lv_spinbox_set_step(lv_spinbox_0, 1);
}

