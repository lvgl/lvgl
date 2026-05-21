/**
 * @file lv_example_label_set_text_newline.c
 */

#include "../../../lv_examples.h"
#if LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * @title Label set text with newlines
 * @brief Insert explicit line breaks into a label's text.
 *
 * In XML attributes the &#10; entity produces a newline; using two of them in a row leaves
 * a blank line between paragraphs. The second label demonstrates that the text attribute
 * also accepts plain text with no escaping for normal characters.
 */
void lv_example_label_set_text_newline(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Adjust text and line breaks (&#10;) to test multi-line layout and spacing. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Label: set text and newline");

    /* Label with explicit newline characters */
    lv_obj_t * lv_label_1 = lv_label_create(screen);
    lv_obj_set_y(lv_label_1, 26);
    lv_label_set_text(lv_label_1, "First line\nSecond line\n\nFourth line");

    /* Plain label for comparison */
    lv_obj_t * lv_label_2 = lv_label_create(screen);
    lv_obj_set_y(lv_label_2, 140);
    lv_label_set_text(lv_label_2, "Text is set directly with the text attribute.");
}
#endif
