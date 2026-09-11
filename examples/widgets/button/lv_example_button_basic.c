/**
 * @file lv_example_button_basic.c
 */

#include "../../lv_examples.h"
#if LV_USE_BUTTON && LV_BUILD_EXAMPLES

/**
 * @title Button basics
 * @brief Two buttons showing default content-sized sizing and an explicit width.
 *
 * The first button uses default sizing so it wraps tightly around its label. The second
 * button sets an explicit width and height, illustrating that a button is just a
 * clickable base widget with a label child.
 */
void lv_example_button_basic(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Default size driven by label content */
    lv_obj_t * button_1 = lv_button_create(screen);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "Click me");

    /* Explicit width and height */
    lv_obj_t * button_2 = lv_button_create(screen);
    lv_obj_set_size(button_2, 160, 48);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Wide button");
}
#endif
