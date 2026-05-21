/**
 * @file lv_example_label_text_align.c
 */

#include "../../../lv_examples.h"
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Adjust width and style_text_align to see where left, center, and right alignment become visible. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Label: text alignment");

    /* Left-aligned multi-line label */
    lv_obj_t * lv_label_1 = lv_label_create(screen);
    lv_obj_set_width(lv_label_1, 220);
    lv_obj_set_style_text_align(lv_label_1, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(lv_label_1, "Left aligned\nSecond line");

    /* Center-aligned multi-line label */
    lv_obj_t * lv_label_2 = lv_label_create(screen);
    lv_obj_set_width(lv_label_2, 220);
    lv_obj_set_style_text_align(lv_label_2, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_2, "Centered\nSecond line");

    /* Right-aligned multi-line label */
    lv_obj_t * lv_label_3 = lv_label_create(screen);
    lv_obj_set_width(lv_label_3, 220);
    lv_obj_set_style_text_align(lv_label_3, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text(lv_label_3, "Right aligned\nSecond line");
}
#endif
