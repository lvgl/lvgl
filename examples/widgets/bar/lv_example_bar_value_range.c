/**
 * @file lv_example_bar_value_range.c
 */

#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

#define TEXT_COLOR lv_color_hex(0x013992)

/**
 * @title Bar value and custom range
 * @brief Drive the indicator with a custom min/max range.
 *
 * The first bar uses the default 0..100 range with value 30. The second bar shifts the
 * range to -20..80 with value 35 to demonstrate that ranges need not start at zero — the
 * indicator length is computed relative to wherever the range begins.
 */
void lv_example_bar_value_range(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Default 0..100 range */
    lv_obj_t * bar_1 = lv_bar_create(screen);
    lv_obj_set_size(bar_1, lv_pct(90), 16);
    lv_bar_set_min_value(bar_1, 0);
    lv_bar_set_max_value(bar_1, 100);
    lv_bar_set_value(bar_1, 30, false);
    lv_obj_t * label_1 = lv_label_create(bar_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "0..100");
    lv_obj_set_style_text_color(label_1, TEXT_COLOR, 0);

    /* Custom range that does not start at zero */
    lv_obj_t * bar_2 = lv_bar_create(screen);
    lv_obj_set_size(bar_2, lv_pct(90), 16);
    lv_bar_set_min_value(bar_2, -20);
    lv_bar_set_max_value(bar_2, 80);
    lv_bar_set_value(bar_2, 35, false);
    lv_obj_t * label_2 = lv_label_create(bar_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "-20..80");
    lv_obj_set_style_text_color(label_2, TEXT_COLOR, 0);
}
#endif
