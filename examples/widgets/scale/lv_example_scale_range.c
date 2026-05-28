/**
 * @file lv_example_scale_range.c
 */

#include "../../lv_examples.h"
#if LV_USE_SCALE && LV_BUILD_EXAMPLES

/**
 * @title Scale value range
 * @brief `min_value` / `max_value` control the numeric domain the scale spans.
 *
 * The two scales below share the same physical width and tick count but cover
 * different numeric ranges. The major tick labels are computed from
 * `min_value`/`max_value` divided across `total_tick_count`. Setting an
 * inverted range (min > max) flips the label direction without changing the
 * layout.
 */
void lv_example_scale_range(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* 0..100 — typical percent scale */
    lv_obj_t * scale_1 = lv_scale_create(screen);
    lv_obj_set_size(scale_1, 260, 60);
    lv_scale_set_mode(scale_1, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale_1, 11);
    lv_scale_set_major_tick_every(scale_1, 2);
    lv_scale_set_label_show(scale_1, true);
    lv_scale_set_min_value(scale_1, 0);
    lv_scale_set_max_value(scale_1, 100);

    /* -50..50 — a centred range, e.g. for offset values */
    lv_obj_t * scale_2 = lv_scale_create(screen);
    lv_obj_set_size(scale_2, 260, 60);
    lv_scale_set_mode(scale_2, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(scale_2, 11);
    lv_scale_set_major_tick_every(scale_2, 2);
    lv_scale_set_label_show(scale_2, true);
    lv_scale_set_min_value(scale_2, -50);
    lv_scale_set_max_value(scale_2, 50);
}
#endif
