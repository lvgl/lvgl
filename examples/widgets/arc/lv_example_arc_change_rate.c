/**
 * @file lv_example_arc_change_rate.c
 */

#include "../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc change rate
 * @brief Limit how fast the value can change while dragging.
 *
 * change_rate caps the maximum value change per second triggered by user input. The lower
 * setting responds sluggishly to fast pointer gestures, while the higher setting tracks
 * the pointer almost instantly — useful for fine-tuning versus quick-jump knobs.
 */
void lv_example_arc_change_rate(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Slower response arc */
    lv_obj_t * arc_1 = lv_arc_create(screen);
    lv_obj_set_size(arc_1, 120, 120);
    lv_arc_set_bg_start_angle(arc_1, 135);
    lv_arc_set_bg_end_angle(arc_1, 45);
    lv_arc_set_change_rate(arc_1, 40);
    lv_arc_set_value(arc_1, 35);
    lv_obj_t * label_1 = lv_label_create(arc_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "rate=40");

    /* Faster response arc */
    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_size(arc_2, 120, 120);
    lv_arc_set_bg_start_angle(arc_2, 135);
    lv_arc_set_bg_end_angle(arc_2, 45);
    lv_arc_set_change_rate(arc_2, 360);
    lv_arc_set_value(arc_2, 35);
    lv_obj_t * label_2 = lv_label_create(arc_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "rate=360");
}
#endif
