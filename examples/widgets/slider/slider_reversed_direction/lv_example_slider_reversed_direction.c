/**
 * @file lv_example_slider_reversed_direction.c
 */

#include "../../../../lvgl.h"

/**
 * @title Slider reversed direction
 * @brief Flip the fill direction by swapping min and max.
 *
 * The two sliders are identical except that the second one declares min_value greater
 * than max_value. Swapping the two reverses the fill direction and the sign of motion
 * without touching orientation, styling, or layout.
 */
void lv_example_slider_reversed_direction_create(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Reverse min/max to flip slider direction without changing orientation. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Slider: reversed direction");

    /* Forward direction: range 0..100 */
    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_pos(lv_slider_0, 12, 42);
    lv_obj_set_size(lv_slider_0, 220, 16);
    lv_slider_set_min_value(lv_slider_0, 0);
    lv_slider_set_max_value(lv_slider_0, 100);
    lv_slider_set_value(lv_slider_0, 30, false);
    lv_obj_t * lv_label_1 = lv_label_create(lv_slider_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "0..100");

    /* Reverse direction: range 100..0 */
    lv_obj_t * lv_slider_1 = lv_slider_create(screen);
    lv_obj_set_pos(lv_slider_1, 12, 102);
    lv_obj_set_size(lv_slider_1, 220, 16);
    lv_slider_set_min_value(lv_slider_1, 100);
    lv_slider_set_max_value(lv_slider_1, 0);
    lv_slider_set_value(lv_slider_1, 30, false);
    lv_obj_t * lv_label_2 = lv_label_create(lv_slider_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "100..0");
}

