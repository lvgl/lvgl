/**
 * @file lv_example_label_text_align.c
 */

#include "../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Label text alignment
 * @brief Align multi-line text inside a fixed-width label.
 *
 * Three labels of equal width hold the same two-line text but use different
 * style_text_align values: left, center, and right. Alignment only changes anything when
 * the label is wider than its text or contains line breaks, so each label is given an
 * explicit width that exceeds the longest line.
 */
void lv_example_label_text_align(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Left-aligned multi-line label */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_obj_set_width(label_1, 220);
    lv_obj_set_style_text_align(label_1, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(label_1, "Left aligned\nSecond line");
    lv_obj_set_style_border_width(label_1, 1, 0);

    /* Center-aligned multi-line label */
    lv_obj_t * label_2 = lv_label_create(screen);
    lv_obj_set_width(label_2, 220);
    lv_obj_set_style_text_align(label_2, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label_2, "Centered\nSecond line");
    lv_obj_set_style_border_width(label_2, 1, 0);

    /* Right-aligned multi-line label */
    lv_obj_t * label_3 = lv_label_create(screen);
    lv_obj_set_width(label_3, 220);
    lv_obj_set_style_text_align(label_3, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text(label_3, "Right aligned\nSecond line");
    lv_obj_set_style_border_width(label_3, 1, 0);
}
#endif
