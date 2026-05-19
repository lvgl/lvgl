/**
 * @file lv_example_scale_range.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 20, 0);

    /* 💡 Edit either range; labels rescale automatically because tick count stays fixed. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Scale: value range");

    /* 0..100 — typical percent scale */
    lv_obj_t * lv_scale_0 = lv_scale_create(screen);
    lv_obj_set_size(lv_scale_0, 260, 60);
    lv_scale_set_mode(lv_scale_0, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(lv_scale_0, 11);
    lv_scale_set_major_tick_every(lv_scale_0, 2);
    lv_scale_set_label_show(lv_scale_0, true);
    lv_scale_set_min_value(lv_scale_0, 0);
    lv_scale_set_max_value(lv_scale_0, 100);

    /* -50..50 — a centred range, e.g. for offset values */
    lv_obj_t * lv_scale_1 = lv_scale_create(screen);
    lv_obj_set_size(lv_scale_1, 260, 60);
    lv_scale_set_mode(lv_scale_1, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_scale_set_total_tick_count(lv_scale_1, 11);
    lv_scale_set_major_tick_every(lv_scale_1, 2);
    lv_scale_set_label_show(lv_scale_1, true);
    lv_scale_set_min_value(lv_scale_1, -50);
    lv_scale_set_max_value(lv_scale_1, 50);
}
#endif
