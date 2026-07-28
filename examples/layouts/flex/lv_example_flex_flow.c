/**
 * @file lv_example_flex_flow.c
 */

#include "../../lv_examples.h"
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

    /* Demo container with row wrapping */
    lv_obj_t * example_flex_flow = lv_obj_create(screen);
    lv_obj_set_align(example_flex_flow, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(example_flex_flow, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_size(example_flex_flow, lv_pct(90), lv_pct(50));
    /* Item sequence to visualize wrapping */
    lv_obj_t * button_1 = lv_button_create(example_flex_flow);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_label_set_text(label_1, "First");

    lv_obj_t * button_2 = lv_button_create(example_flex_flow);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_label_set_text(label_2, "Second");

    lv_obj_t * button_3 = lv_button_create(example_flex_flow);
    lv_obj_t * label_3 = lv_label_create(button_3);
    lv_label_set_text(label_3, "Third");

    lv_obj_t * button_4 = lv_button_create(example_flex_flow);
    lv_obj_t * label_4 = lv_label_create(button_4);
    lv_label_set_text(label_4, "Forth");

    lv_obj_t * button_5 = lv_button_create(example_flex_flow);
    lv_obj_t * label_5 = lv_label_create(button_5);
    lv_label_set_text(label_5, "Fifth");

    lv_obj_t * button_6 = lv_button_create(example_flex_flow);
    lv_obj_t * label_6 = lv_label_create(button_6);
    lv_label_set_text(label_6, "Sixth");

    lv_obj_t * button_7 = lv_button_create(example_flex_flow);
    lv_obj_t * label_7 = lv_label_create(button_7);
    lv_label_set_text(label_7, "Seventh");

    lv_obj_t * button_8 = lv_button_create(example_flex_flow);
    lv_obj_t * label_8 = lv_label_create(button_8);
    lv_label_set_text(label_8, "Eighth");

    lv_obj_t * button_9 = lv_button_create(example_flex_flow);
    lv_obj_t * label_9 = lv_label_create(button_9);
    lv_label_set_text(label_9, "Ninth");

    lv_obj_t * button_10 = lv_button_create(example_flex_flow);
    lv_obj_t * label_10 = lv_label_create(button_10);
    lv_label_set_text(label_10, "Tenth");
}
#endif
