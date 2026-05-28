/**
 * @file lv_example_scale_ticks.c
 */

#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Scale tick configuration
 * @brief Tune tick density and which ticks count as "major" (drawn longer with labels).
 *
 * `total_tick_count` is the *total* number of ticks rendered — both minor and
 * major. `major_tick_every` picks every Nth tick to be drawn longer and
 * (when `label_show="true"`) carry a numeric label. The three scales below
 * share the same physical width but vary their tick density.
 */
void lv_example_scale_ticks(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* Sparse: 5 ticks total, every second one major */
    lv_obj_t * scale_1 = lv_scale_create(screen);
    lv_obj_set_size(scale_1, 260, 60);
    lv_scale_set_mode(scale_1, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale_1, 5);
    lv_scale_set_major_tick_every(scale_1, 2);
    lv_scale_set_label_show(scale_1, true);
    lv_scale_set_min_value(scale_1, 0);
    lv_scale_set_max_value(scale_1, 100);

    /* Medium: 11 ticks, every 5th major */
    lv_obj_t * scale_2 = lv_scale_create(screen);
    lv_obj_set_size(scale_2, 260, 60);
    lv_scale_set_mode(scale_2, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale_2, 11);
    lv_scale_set_major_tick_every(scale_2, 5);
    lv_scale_set_label_show(scale_2, true);
    lv_scale_set_min_value(scale_2, 0);
    lv_scale_set_max_value(scale_2, 100);

    /* Dense: 21 ticks, every 4th major — minor ticks become visible */
    lv_obj_t * scale_3 = lv_scale_create(screen);
    lv_obj_set_size(scale_3, 260, 60);
    lv_scale_set_mode(scale_3, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale_3, 21);
    lv_scale_set_major_tick_every(scale_3, 4);
    lv_scale_set_label_show(scale_3, true);
    lv_scale_set_min_value(scale_3, 0);
    lv_scale_set_max_value(scale_3, 100);
}
#endif
