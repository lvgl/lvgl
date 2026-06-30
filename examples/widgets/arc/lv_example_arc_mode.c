/**
 * @file lv_example_arc_mode.c
 */

#include "../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc indicator modes
 * @brief Compare normal, reverse, and symmetrical mode side by side.
 *
 * Three arcs share the same min/max range and value but use different mode settings.
 * Normal grows clockwise from min_value, reverse grows counter-clockwise from max_value,
 * and symmetrical grows outward from the middle of the range — making it obvious where
 * each mode anchors the indicator.
 */
void lv_example_arc_mode(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Normal mode: clockwise from minimum to current value */
    lv_obj_t * arc_1 = lv_arc_create(screen);
    lv_obj_set_size(arc_1, 95, 95);
    lv_arc_set_mode(arc_1, LV_ARC_MODE_NORMAL);
    lv_arc_set_value(arc_1, 75);
    lv_obj_t * label_1 = lv_label_create(arc_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "normal");

    /* Reverse mode: counter-clockwise from maximum to current value */
    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_size(arc_2, 95, 95);
    lv_arc_set_mode(arc_2, LV_ARC_MODE_REVERSE);
    lv_arc_set_value(arc_2, 75);
    lv_obj_t * label_2 = lv_label_create(arc_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "rev.");

    /* Symmetrical mode: indicator grows from the middle */
    lv_obj_t * arc_3 = lv_arc_create(screen);
    lv_obj_set_size(arc_3, 95, 95);
    lv_arc_set_mode(arc_3, LV_ARC_MODE_SYMMETRICAL);
    lv_arc_set_value(arc_3, 75);
    lv_obj_t * label_3 = lv_label_create(arc_3);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_label_set_text(label_3, "sym.");
}
#endif
