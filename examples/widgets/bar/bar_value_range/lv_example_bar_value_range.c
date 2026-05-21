/**
 * @file lv_example_bar_value_range.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit min_value / max_value / value to see how the indicator scales within the chosen range. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Bar: value and range");

    /* Default 0..100 range */
    lv_obj_t * lv_bar_0 = lv_bar_create(screen);
    lv_obj_set_size(lv_bar_0, lv_pct(90), 16);
    lv_bar_set_min_value(lv_bar_0, 0);
    lv_bar_set_max_value(lv_bar_0, 100);
    lv_bar_set_value(lv_bar_0, 30, false);
    lv_obj_t * lv_label_1 = lv_label_create(lv_bar_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "0..100");
    lv_obj_set_style_text_color(lv_label_1, TEXT_COLOR, 0);

    /* Custom range that does not start at zero */
    lv_obj_t * lv_bar_1 = lv_bar_create(screen);
    lv_obj_set_size(lv_bar_1, lv_pct(90), 16);
    lv_bar_set_min_value(lv_bar_1, -20);
    lv_bar_set_max_value(lv_bar_1, 80);
    lv_bar_set_value(lv_bar_1, 35, false);
    lv_obj_t * lv_label_2 = lv_label_create(lv_bar_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "-20..80");
    lv_obj_set_style_text_color(lv_label_2, TEXT_COLOR, 0);
}
#endif
