/**
 * @file lv_example_flex_align.c
 */

#include "../../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * @title Flex align
 * @brief Place items along the main and cross axes.
 *
 * style_flex_main_place distributes items along the row direction, while
 * style_flex_cross_place aligns items of different heights on the cross axis. The mix of a
 * tall button, a switch, and a checkbox makes cross-axis alignment easy to see.
 */
void lv_example_flex_align(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);

    /* 💡 Adjust style_flex_main_place and style_flex_cross_place to compare distribution and cross-axis alignment. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Space the items evenly horizontally, and to the center vertically");

    /* Demo container with explicit main and cross alignment */
    lv_obj_t * lv_obj_1 = lv_obj_create(screen);
    lv_obj_set_flex_flow(lv_obj_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(lv_obj_1, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(lv_obj_1, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_size(lv_obj_1, lv_pct(100), lv_pct(40));
    /* Tall item: a label with a large font */
    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_1);
    lv_obj_set_height(lv_button_0, 64);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, "64px");
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);

    lv_switch_create(lv_obj_1);

    lv_obj_t * lv_checkbox_0 = lv_checkbox_create(lv_obj_1);
    lv_checkbox_set_text(lv_checkbox_0, "Check");
}
#endif
