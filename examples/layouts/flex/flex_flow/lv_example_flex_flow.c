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
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Demo container with row wrapping */
    lv_obj_t * example_flex_flow = lv_obj_create(screen);
    lv_obj_set_flex_flow(example_flex_flow, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_size(example_flex_flow, lv_pct(100), lv_pct(50));
    /* Item sequence to visualize wrapping */
    lv_obj_t * lv_button_0 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_0, "First");

    lv_obj_t * lv_button_1 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_1);
    lv_label_set_text(lv_label_1, "Second");

    lv_obj_t * lv_button_2 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_2);
    lv_label_set_text(lv_label_2, "Third");

    lv_obj_t * lv_button_3 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_3 = lv_label_create(lv_button_3);
    lv_label_set_text(lv_label_3, "Forth");

    lv_obj_t * lv_button_4 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_4 = lv_label_create(lv_button_4);
    lv_label_set_text(lv_label_4, "Fifth");

    lv_obj_t * lv_button_5 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_5 = lv_label_create(lv_button_5);
    lv_label_set_text(lv_label_5, "Sixth");

    lv_obj_t * lv_button_6 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_6 = lv_label_create(lv_button_6);
    lv_label_set_text(lv_label_6, "Seventh");

    lv_obj_t * lv_button_7 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_7 = lv_label_create(lv_button_7);
    lv_label_set_text(lv_label_7, "Eighth");

    lv_obj_t * lv_button_8 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_8 = lv_label_create(lv_button_8);
    lv_label_set_text(lv_label_8, "Ninth");

    lv_obj_t * lv_button_9 = lv_button_create(example_flex_flow);
    lv_obj_t * lv_label_9 = lv_label_create(lv_button_9);
    lv_label_set_text(lv_label_9, "Tenth");
}
#endif
