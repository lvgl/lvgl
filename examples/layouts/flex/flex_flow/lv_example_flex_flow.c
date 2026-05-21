/**
 * @file lv_example_flex_flow.c
 */

#include "../../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex flow
 * @brief Arrange children with a flex flow direction.
 *
 * A single row_wrap container holds enough buttons to overflow one line, so wrapping kicks
 * in. Try other flex_flow values (row, column, row_reverse, column_wrap_reverse) to see
 * how item order and wrap direction change with the same set of children.
 */
void lv_example_flex_flow(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);

    /* 💡 Adjust flex_flow (row, column, row_wrap, row_reverse) to see how item order and wrapping change. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Buttons Arranged in a row with automatic wrapping");

    /* Demo container with row wrapping */
    lv_obj_t * example_flex_flow = lv_obj_create(screen);
    lv_obj_set_flex_flow(example_flex_flow, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_size(example_flex_flow, lv_pct(100), lv_pct(50));
    /* Item sequence to visualize wrapping */
    lv_obj_t * lv_button_0 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, "First");

    lv_obj_t * lv_button_1 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_label_set_text(lv_label_2, "Second");

    lv_obj_t * lv_button_2 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_2);
    lv_label_set_text(lv_label_3, "Third");

    lv_obj_t * lv_button_3 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_3);
    lv_label_set_text(lv_label_4, "Forth");

    lv_obj_t * lv_button_4 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_4);
    lv_label_set_text(lv_label_5, "Fifth");

    lv_obj_t * lv_button_5 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_6 = lv_label_create(lv_button_5);
    lv_label_set_text(lv_label_6, "Sixth");

    lv_obj_t * lv_button_6 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_6);
    lv_label_set_text(lv_label_7, "Seventh");

    lv_obj_t * lv_button_7 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_8 = lv_label_create(lv_button_7);
    lv_label_set_text(lv_label_8, "Eighth");

    lv_obj_t * lv_button_8 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_9 = lv_label_create(lv_button_8);
    lv_label_set_text(lv_label_9, "Ninth");

    lv_obj_t * lv_button_9 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_10 = lv_label_create(lv_button_9);
    lv_label_set_text(lv_label_10, "Tenth");
}
#endif
