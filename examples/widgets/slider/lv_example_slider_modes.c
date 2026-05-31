/**
 * @file lv_example_slider_modes.c
 */

#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

#define TEXT_COLOR lv_color_hex(0x013992)

/**
 * @title Slider modes
 * @brief Compare normal, symmetrical, and range modes.
 *
 * Normal fills from min_value to value. Symmetrical needs a range that crosses zero and
 * fills outward from the middle, so positive and negative values pull the indicator in
 * opposite directions. Range mode draws a band between left_value and value, useful for
 * picking an interval rather than a single number.
 */
void lv_example_slider_modes(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Normal mode */
    lv_obj_t * slider_1 = lv_slider_create(screen);
    lv_obj_set_width(slider_1, lv_pct(90));
    lv_slider_set_mode(slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_min_value(slider_1, 0);
    lv_slider_set_max_value(slider_1, 100);
    lv_slider_set_value(slider_1, 70, false);
    lv_obj_t * label_1 = lv_label_create(slider_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "normal");
    lv_obj_set_style_text_color(label_1, TEXT_COLOR, 0);

    /* Symmetrical mode around zero */
    lv_obj_t * slider_2 = lv_slider_create(screen);
    lv_obj_set_width(slider_2, lv_pct(90));
    lv_slider_set_mode(slider_2, LV_SLIDER_MODE_SYMMETRICAL);
    lv_slider_set_min_value(slider_2, -100);
    lv_slider_set_max_value(slider_2, 100);
    lv_slider_set_value(slider_2, 45, false);
    lv_obj_t * label_2 = lv_label_create(slider_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "symmetrical");
    lv_obj_set_style_text_color(label_2, TEXT_COLOR, 0);

    /* Range mode with left and right values */
    lv_obj_t * slider_3 = lv_slider_create(screen);
    lv_obj_set_width(slider_3, lv_pct(90));
    lv_slider_set_mode(slider_3, LV_SLIDER_MODE_RANGE);
    lv_slider_set_min_value(slider_3, 0);
    lv_slider_set_max_value(slider_3, 100);
    lv_slider_set_start_value(slider_3, 25, false);
    lv_slider_set_value(slider_3, 80, false);
    lv_obj_t * label_3 = lv_label_create(slider_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "range");
    lv_obj_set_style_text_color(label_3, TEXT_COLOR, 0);
}
#endif
