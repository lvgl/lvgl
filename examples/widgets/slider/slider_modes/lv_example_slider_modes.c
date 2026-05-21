/**
 * @file lv_example_slider_modes.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Switch mode and range values together: symmetrical needs negative+positive range, range mode uses left_value + value. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Slider: modes");

    /* Normal mode */
    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_0, lv_pct(90));
    lv_slider_set_mode(lv_slider_0, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_min_value(lv_slider_0, 0);
    lv_slider_set_max_value(lv_slider_0, 100);
    lv_slider_set_value(lv_slider_0, 70, false);
    lv_obj_t * lv_label_1 = lv_label_create(lv_slider_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "normal");
    lv_obj_set_style_text_color(lv_label_1, TEXT_COLOR, 0);

    /* Symmetrical mode around zero */
    lv_obj_t * lv_slider_1 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_1, lv_pct(90));
    lv_slider_set_mode(lv_slider_1, LV_SLIDER_MODE_SYMMETRICAL);
    lv_slider_set_min_value(lv_slider_1, -100);
    lv_slider_set_max_value(lv_slider_1, 100);
    lv_slider_set_value(lv_slider_1, 45, false);
    lv_obj_t * lv_label_2 = lv_label_create(lv_slider_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "symmetrical");
    lv_obj_set_style_text_color(lv_label_2, TEXT_COLOR, 0);

    /* Range mode with left and right values */
    lv_obj_t * lv_slider_2 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_2, lv_pct(90));
    lv_slider_set_mode(lv_slider_2, LV_SLIDER_MODE_RANGE);
    lv_slider_set_min_value(lv_slider_2, 0);
    lv_slider_set_max_value(lv_slider_2, 100);
    lv_slider_set_start_value(lv_slider_2, 25, false);
    lv_slider_set_value(lv_slider_2, 80, false);
    lv_obj_t * lv_label_3 = lv_label_create(lv_slider_2);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "range");
    lv_obj_set_style_text_color(lv_label_3, TEXT_COLOR, 0);
}
#endif
