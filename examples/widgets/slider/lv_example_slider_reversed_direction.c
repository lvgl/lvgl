/**
 * @file lv_example_slider_reversed_direction.c
 */

#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

#define TEXT_COLOR lv_color_hex(0x013992)

/**
 * @title Slider reversed direction
 * @brief Flip the fill direction by swapping min and max.
 *
 * The two sliders are identical except that the second one declares min_value greater
 * than max_value. Swapping the two reverses the fill direction and the sign of motion
 * without touching orientation, styling, or layout.
 */
void lv_example_slider_reversed_direction(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Forward direction: range 0..100 */
    lv_obj_t * slider_1 = lv_slider_create(screen);
    lv_obj_set_size(slider_1, lv_pct(90), 16);
    lv_slider_set_min_value(slider_1, 0);
    lv_slider_set_max_value(slider_1, 100);
    lv_slider_set_value(slider_1, 30, false);
    lv_obj_t * label_1 = lv_label_create(slider_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "0..100");
    lv_obj_set_style_text_color(label_1, TEXT_COLOR, 0);

    /* Reverse direction: range 100..0 */
    lv_obj_t * slider_2 = lv_slider_create(screen);
    lv_obj_set_size(slider_2, lv_pct(90), 16);
    lv_slider_set_min_value(slider_2, 100);
    lv_slider_set_max_value(slider_2, 0);
    lv_slider_set_value(slider_2, 30, false);
    lv_obj_t * label_2 = lv_label_create(slider_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "100..0");
    lv_obj_set_style_text_color(label_2, TEXT_COLOR, 0);
}
#endif
