/**
 * @file lv_example_arc_value_range.c
 */

#include "../../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc value and range
 * @brief Show how min_value/max_value map onto the same background arc.
 *
 * Two arcs with different numeric ranges share the same visible background span. The
 * indicator length depends on (value - min_value) / (max_value - min_value), so any range
 * — including one crossing zero — produces the same relative fill.
 */
void lv_example_arc_value_range(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Change min_value, max_value, and value together to see how the indicator span maps onto the same background arc. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Arc: value and range");

    /* Arc with a symmetric custom range */
    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_0, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(lv_arc_0, 10);
    lv_obj_set_size(lv_arc_0, 96, 96);
    lv_arc_set_min_value(lv_arc_0, -50);
    lv_arc_set_max_value(lv_arc_0, 50);
    lv_arc_set_value(lv_arc_0, 20);
    lv_arc_set_bg_start_angle(lv_arc_0, 135);
    lv_arc_set_bg_end_angle(lv_arc_0, 45);
    lv_obj_t * lv_label_1 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "-50..50");

    /* Arc with a custom range showing the same concept at different numeric scale */
    lv_obj_t * lv_arc_1 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_1, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(lv_arc_1, -10);
    lv_obj_set_size(lv_arc_1, 96, 96);
    lv_arc_set_min_value(lv_arc_1, 20);
    lv_arc_set_max_value(lv_arc_1, 220);
    lv_arc_set_value(lv_arc_1, 140);
    lv_arc_set_bg_start_angle(lv_arc_1, 135);
    lv_arc_set_bg_end_angle(lv_arc_1, 45);
    lv_obj_t * lv_label_2 = lv_label_create(lv_arc_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "20..220");
}
#endif
