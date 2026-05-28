/**
 * @file lv_example_switch_orientation.c
 */

#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

/**
 * @title Switch orientation
 * @brief Lay the switch out horizontally or vertically.
 *
 * `orientation="auto"` (the default) picks horizontal when the widget is
 * wider than it is tall, and vertical otherwise. Forcing the orientation with
 * `horizontal` or `vertical` overrides this — useful when the layout
 * dimensions don't match the orientation you want, e.g. a tall vertical
 * switch inside a wide row.
 */
void lv_example_switch_orientation(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_column(screen, 32, 0);

    /* 💡 Swap `orientation` on either switch to compare auto-derived vs explicit layouts. */
    lv_obj_t * sw_1 = lv_switch_create(screen);
    lv_obj_set_size(sw_1, 60, 30);
    lv_switch_set_orientation(sw_1, LV_SWITCH_ORIENTATION_HORIZONTAL);
    lv_obj_set_state(sw_1, LV_STATE_CHECKED, true);

    lv_obj_t * sw_2 = lv_switch_create(screen);
    lv_obj_set_size(sw_2, 30, 60);
    lv_switch_set_orientation(sw_2, LV_SWITCH_ORIENTATION_VERTICAL);
    lv_obj_set_state(sw_2, LV_STATE_CHECKED, true);
}
#endif
