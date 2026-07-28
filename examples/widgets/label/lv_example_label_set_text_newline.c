/**
 * @file lv_example_label_set_text_newline.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Label with explicit newline characters */
    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_set_text(label_1, "First line\nSecond line\n\nFourth line");

    /* Plain label for comparison */
    lv_obj_t * label_2 = lv_label_create(screen);
    lv_label_set_text(label_2, "Just a normal single line text.");
}
#endif
