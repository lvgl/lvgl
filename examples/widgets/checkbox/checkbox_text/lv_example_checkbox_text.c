/**
 * @file lv_example_checkbox_text.c
 */

#include "../../../../lvgl.h"

/**
 * @title Checkbox text
 * @brief Set the label that appears next to the tick box.
 *
 * Three checkboxes share a layout but each carries a different text: a short word, a
 * longer single-line sentence, and a multi-line label using `&#10;` for the line break.
 * The text attribute is what `lv_checkbox_set_text` would set in code, and the gap
 * between tick box and label is controlled by `pad_column` on the main part.
 */
void lv_example_checkbox_text_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Edit each text attribute to see the label update next to the tick box. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Checkbox: text");

    /* Short label */
    lv_obj_t * lv_checkbox_0 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_0, "Enable");

    /* Longer label */
    lv_obj_t * lv_checkbox_1 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_1, "Send anonymous usage data");

    /* Multi-line label via &amp;#10; */
    lv_obj_t * lv_checkbox_2 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_2, "I agree to the terms\nand the privacy policy");
}

