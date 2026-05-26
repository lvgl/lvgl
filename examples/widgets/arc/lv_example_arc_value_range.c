/**
 * @file lv_example_arc_value_range.c
 */

#include "../../lv_examples.h"
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

    /* Arc with a symmetric custom range */
    lv_obj_t * arc_1 = lv_arc_create(screen);
    lv_obj_set_align(arc_1, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(arc_1, 10);
    lv_obj_set_size(arc_1, 96, 96);
    lv_arc_set_min_value(arc_1, -50);
    lv_arc_set_max_value(arc_1, 50);
    lv_arc_set_value(arc_1, 20);
    lv_arc_set_bg_start_angle(arc_1, 135);
    lv_arc_set_bg_end_angle(arc_1, 45);
    lv_obj_t * label_1 = lv_label_create(arc_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "-50..50");

    /* Arc with a custom range showing the same concept at different numeric scale */
    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_align(arc_2, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(arc_2, -10);
    lv_obj_set_size(arc_2, 96, 96);
    lv_arc_set_min_value(arc_2, 20);
    lv_arc_set_max_value(arc_2, 220);
    lv_arc_set_value(arc_2, 140);
    lv_arc_set_bg_start_angle(arc_2, 135);
    lv_arc_set_bg_end_angle(arc_2, 45);
    lv_obj_t * label_2 = lv_label_create(arc_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "20..220");
}
#endif
