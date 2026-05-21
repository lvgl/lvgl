/**
 * @file lv_example_bar_orientation.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

#define TEXT_COLOR lv_color_hex(0x013992)

/**
 * @title Bar orientation and size
 * @brief Compare horizontal and explicit vertical bars.
 *
 * The horizontal bar uses the default orientation derived from a wide-and-short size. The
 * vertical one sets orientation="vertical" explicitly and uses a tall, narrow size so the
 * indicator grows upward instead of left-to-right.
 */
void lv_example_bar_orientation(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Swap width/height or set orientation explicitly to flip the indicator's travel direction. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Bar: orientation and size");

    /* Horizontal bar (auto orientation from width &gt; height) */
    lv_obj_t * lv_bar_0 = lv_bar_create(screen);
    lv_obj_set_size(lv_bar_0, lv_pct(90), 16);
    lv_bar_set_min_value(lv_bar_0, 0);
    lv_bar_set_max_value(lv_bar_0, 100);
    lv_bar_set_value(lv_bar_0, 60, false);
    lv_obj_t * lv_label_1 = lv_label_create(lv_bar_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "horizontal");
    lv_obj_set_style_text_color(lv_label_1, TEXT_COLOR, 0);

    /* Explicit vertical orientation */
    lv_obj_t * lv_bar_1 = lv_bar_create(screen);
    lv_obj_set_size(lv_bar_1, 18, 130);
    lv_bar_set_orientation(lv_bar_1, LV_BAR_ORIENTATION_VERTICAL);
    lv_bar_set_min_value(lv_bar_1, 0);
    lv_bar_set_max_value(lv_bar_1, 100);
    lv_bar_set_value(lv_bar_1, 60, false);
    lv_obj_t * lv_label_2 = lv_label_create(lv_bar_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "V\ne\nr");
    lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lv_label_2, TEXT_COLOR, 0);
}
#endif
