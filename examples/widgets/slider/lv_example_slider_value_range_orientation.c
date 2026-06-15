/**
 * @file lv_example_slider_value_range_orientation.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Horizontal slider with custom range */
    lv_obj_t * slider_1 = lv_slider_create(screen);
    lv_obj_set_size(slider_1, lv_pct(90), 20);
    lv_slider_set_min_value(slider_1, -20);
    lv_slider_set_max_value(slider_1, 80);
    lv_slider_set_value(slider_1, 35, false);
    lv_obj_t * label_1 = lv_label_create(slider_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Horizontal");
    lv_obj_set_style_text_color(label_1, TEXT_COLOR, 0);

    /* Vertical slider with explicit vertical orientation */
    lv_obj_t * slider_2 = lv_slider_create(screen);
    lv_obj_set_size(slider_2, 20, 130);
    lv_slider_set_orientation(slider_2, LV_SLIDER_ORIENTATION_VERTICAL);
    lv_slider_set_min_value(slider_2, 0);
    lv_slider_set_max_value(slider_2, 100);
    lv_slider_set_value(slider_2, 60, false);
    lv_obj_t * label_2 = lv_label_create(slider_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "V\ne\nr");
    lv_obj_set_style_text_color(label_2, TEXT_COLOR, 0);
    lv_obj_set_style_text_align(label_2, LV_TEXT_ALIGN_CENTER, 0);
}
#endif
