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
    static lv_subject_t subject_value;

    static bool inited = false;

    if(!inited) {
        lv_subject_init_int(&subject_value, 50);
        lv_subject_set_min_value_int(&subject_value, 0);
        lv_subject_set_max_value_int(&subject_value, 100);
        inited = true;
    }

    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Arc with a custom range showing the same concept at different numeric scale */
    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_size(arc_2, 120, 120);
    lv_arc_set_min_value(arc_2, -20);
    lv_arc_set_max_value(arc_2, 220);
    lv_arc_set_value(arc_2, 140);
    lv_arc_set_bg_start_angle(arc_2, 135);
    lv_arc_set_bg_end_angle(arc_2, 45);
    lv_arc_bind_value(arc_2, &subject_value);
    lv_obj_t * label_4 = lv_label_create(arc_2);
    lv_obj_set_align(label_4, LV_ALIGN_CENTER);
    lv_obj_set_y(label_4, -10);
    lv_label_bind_text(label_4, &subject_value, NULL);

    lv_obj_t * label_3 = lv_label_create(arc_2);
    lv_obj_set_align(label_3, LV_ALIGN_CENTER);
    lv_obj_set_y(label_3, 10);
    lv_label_set_text(label_3, "[-20..220]");
}
#endif
