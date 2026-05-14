/**
 * @file lv_example_arc_mode.c
 */

#include "../../../../lvgl.h"

/**
 * @title Arc indicator modes
 * @brief Compare normal, reverse, and symmetrical mode side by side.
 *
 * Three arcs share the same min/max range and value but use different mode settings.
 * Normal grows clockwise from min_value, reverse grows counter-clockwise from max_value,
 * and symmetrical grows outward from the middle of the range — making it obvious where
 * each mode anchors the indicator.
 */
void lv_example_arc_mode_create(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Switch mode between normal, reverse, and symmetrical to compare where the indicator grows from. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Arc: indicator modes");

    /* Normal mode: clockwise from minimum to current value */
    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_0, LV_ALIGN_LEFT_MID);
    lv_obj_set_size(lv_arc_0, 90, 90);
    lv_obj_set_x(lv_arc_0, 10);
    lv_arc_set_mode(lv_arc_0, LV_ARC_MODE_NORMAL);
    lv_arc_set_value(lv_arc_0, 75);
    lv_obj_t * lv_label_1 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "normal");

    /* Reverse mode: counter-clockwise from maximum to current value */
    lv_obj_t * lv_arc_1 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_1, LV_ALIGN_CENTER);
    lv_obj_set_size(lv_arc_1, 90, 90);
    lv_arc_set_mode(lv_arc_1, LV_ARC_MODE_REVERSE);
    lv_arc_set_value(lv_arc_1, 75);
    lv_obj_t * lv_label_2 = lv_label_create(lv_arc_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "rev.");

    /* Symmetrical mode: indicator grows from the middle */
    lv_obj_t * lv_arc_2 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_2, LV_ALIGN_RIGHT_MID);
    lv_obj_set_size(lv_arc_2, 90, 90);
    lv_obj_set_x(lv_arc_2, -10);
    lv_arc_set_mode(lv_arc_2, LV_ARC_MODE_SYMMETRICAL);
    lv_arc_set_value(lv_arc_2, 75);
    lv_obj_t * lv_label_3 = lv_label_create(lv_arc_2);
    lv_obj_set_align(lv_label_3, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_3, "sym.");
}

