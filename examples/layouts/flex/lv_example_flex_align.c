/**
 * @file lv_example_flex_align.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Demo container with explicit main and cross alignment */
    lv_obj_t * container = lv_obj_create(screen);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(container, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(container, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_size(container, lv_pct(100), lv_pct(40));
    /* Tall item: a label with a large font */
    lv_obj_t * button = lv_button_create(container);
    lv_obj_set_height(button, 64);
    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, "64px");
    lv_obj_set_align(label, LV_ALIGN_CENTER);

    /* Medium item: a toggle switch */
    lv_switch_create(container);

    /* Another medium item: a checkbox */
    lv_obj_t * checkbox = lv_checkbox_create(container);
    lv_checkbox_set_text(checkbox, "Check");
}
#endif
