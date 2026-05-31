/**
 * @file lv_example_arc_set_angles.c
 */

#include "../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc with explicit indicator angles
 * @brief Drive the indicator directly via start_angle and end_angle.
 *
 * The left arc draws the indicator the usual way, from value, mode, and range. The right
 * arc sets start_angle and end_angle explicitly, which overrides the value-based drawing.
 * This is handy for static graphics or custom dial layouts where the indicator span is
 * not tied to a numeric value.
 */
void lv_example_arc_set_angles(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* Value-driven indicator arc */
    lv_obj_t * arc_1 = lv_arc_create(screen);
    lv_obj_set_align(arc_1, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(arc_1, 10);
    lv_obj_set_size(arc_1, 96, 96);
    lv_arc_set_bg_start_angle(arc_1, 135);
    lv_arc_set_bg_end_angle(arc_1, 45);
    lv_arc_set_min_value(arc_1, 100);
    lv_arc_set_max_value(arc_1, 200);
    lv_arc_set_value(arc_1, 145);
    lv_obj_t * label_1 = lv_label_create(arc_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "value");

    /* Indicator angle set explicitly; value/mode are ignored for indicator drawing */
    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_align(arc_2, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(arc_2, -10);
    lv_obj_set_size(arc_2, 96, 96);
    lv_arc_set_bg_start_angle(arc_2, 135);
    lv_arc_set_bg_end_angle(arc_2, 45);
    lv_arc_set_start_angle(arc_2, 210);
    lv_arc_set_end_angle(arc_2, 330);
    lv_obj_t * label_2 = lv_label_create(arc_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "angles");
}
#endif
