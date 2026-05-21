/**
 * @file lv_example_slider_value_range_orientation.c
 */

#include "../../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

#define TEXT_COLOR lv_color_hex(0x013992)

/**
 * @title Slider value, range, and orientation
 * @brief Set a custom numeric range and choose horizontal or vertical layout.
 *
 * The horizontal slider uses min_value=-20 and max_value=80 to show that ranges need not
 * start at zero. The vertical slider sets orientation="vertical" explicitly and uses a
 * tall, narrow size so the knob travels upward instead of left-to-right.
 */
void lv_example_slider_value_range_orientation(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit min_value/max_value/value and orientation to see how fill direction and knob travel change. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Slider: value, range, orientation");

    /* Horizontal slider with custom range */
    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_size(lv_slider_0, lv_pct(90), 16);
    lv_slider_set_min_value(lv_slider_0, -20);
    lv_slider_set_max_value(lv_slider_0, 80);
    lv_slider_set_value(lv_slider_0, 35, false);
    lv_obj_t * lv_label_1 = lv_label_create(lv_slider_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "-20..80");
    lv_obj_set_style_text_color(lv_label_1, TEXT_COLOR, 0);

    /* Vertical slider with explicit vertical orientation */
    lv_obj_t * lv_slider_1 = lv_slider_create(screen);
    lv_obj_set_size(lv_slider_1, 18, 130);
    lv_slider_set_orientation(lv_slider_1, LV_SLIDER_ORIENTATION_VERTICAL);
    lv_slider_set_min_value(lv_slider_1, 0);
    lv_slider_set_max_value(lv_slider_1, 100);
    lv_slider_set_value(lv_slider_1, 60, false);
    lv_obj_t * lv_label_2 = lv_label_create(lv_slider_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "V\ne\nr");
    lv_obj_set_style_text_color(lv_label_2, TEXT_COLOR, 0);
}
#endif
