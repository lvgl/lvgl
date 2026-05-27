/**
 * @file lv_example_scale_round_modes.c
 */

#include "../../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Scale round modes
 * @brief Two round scales: ticks pointing inward and outward.
 *
 * `round_inner` places the ticks on the inside of the arc — useful when the
 * scale wraps content like a gauge needle. `round_outer` puts the ticks on
 * the outside — typical for dials and clocks. Both modes pick up
 * `angle_range` (the arc's angular sweep) and `rotation` (where the first
 * tick sits, measured clockwise from 3 o'clock).
 */
void lv_example_scale_round_modes(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 24, 0);

    /* Ticks inside: gauge-style */
    lv_obj_t * scale_1 = lv_scale_create(screen);
    lv_obj_set_size(scale_1, 180, 180);
    lv_scale_set_mode(scale_1, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_total_tick_count(scale_1, 11);
    lv_scale_set_major_tick_every(scale_1, 2);
    lv_scale_set_label_show(scale_1, true);
    lv_scale_set_angle_range(scale_1, 270);
    lv_scale_set_rotation(scale_1, 135);
    lv_scale_set_min_value(scale_1, 0);
    lv_scale_set_max_value(scale_1, 100);

    /* Ticks outside: clock face / dial */
    lv_obj_t * scale_2 = lv_scale_create(screen);
    lv_obj_set_size(scale_2, 180, 180);
    lv_scale_set_mode(scale_2, LV_SCALE_MODE_ROUND_OUTER);
    lv_scale_set_total_tick_count(scale_2, 11);
    lv_scale_set_major_tick_every(scale_2, 2);
    lv_scale_set_label_show(scale_2, true);
    lv_scale_set_angle_range(scale_2, 270);
    lv_scale_set_rotation(scale_2, 135);
    lv_scale_set_min_value(scale_2, 0);
    lv_scale_set_max_value(scale_2, 100);
}
#endif
