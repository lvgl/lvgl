/**
 * @file lv_example_arc_set_angles.c
 */

#include "../../../lv_examples.h"
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

    /* 💡 Edit start_angle/end_angle directly to draw any indicator span independent of value and mode. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Arc: set indicator angles");

    /* Value-driven indicator arc */
    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_0, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(lv_arc_0, 10);
    lv_obj_set_size(lv_arc_0, 96, 96);
    lv_arc_set_bg_start_angle(lv_arc_0, 135);
    lv_arc_set_bg_end_angle(lv_arc_0, 45);
    lv_arc_set_min_value(lv_arc_0, 100);
    lv_arc_set_max_value(lv_arc_0, 200);
    lv_arc_set_value(lv_arc_0, 145);
    lv_obj_t * lv_label_1 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "value");

    /* Indicator angle set explicitly; value/mode are ignored for indicator drawing */
    lv_obj_t * lv_arc_1 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_1, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(lv_arc_1, -10);
    lv_obj_set_size(lv_arc_1, 96, 96);
    lv_arc_set_bg_start_angle(lv_arc_1, 135);
    lv_arc_set_bg_end_angle(lv_arc_1, 45);
    lv_arc_set_start_angle(lv_arc_1, 210);
    lv_arc_set_end_angle(lv_arc_1, 330);
    lv_obj_t * lv_label_2 = lv_label_create(lv_arc_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "angles");
}
#endif
