/**
 * @file lv_example_arc_change_rate.c
 */

#include "../../../lv_examples.h"
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

    /* 💡 Drag each arc quickly: lower change_rate responds more slowly, higher change_rate tracks your pointer faster. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Arc: change rate");

    /* Slower response arc */
    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_0, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(lv_arc_0, 10);
    lv_obj_set_size(lv_arc_0, 96, 96);
    lv_arc_set_bg_start_angle(lv_arc_0, 135);
    lv_arc_set_bg_end_angle(lv_arc_0, 45);
    lv_arc_set_change_rate(lv_arc_0, 40);
    lv_arc_set_value(lv_arc_0, 35);
    lv_obj_t * lv_label_1 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "rate=40");

    /* Faster response arc */
    lv_obj_t * lv_arc_1 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_1, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(lv_arc_1, -10);
    lv_obj_set_size(lv_arc_1, 96, 96);
    lv_arc_set_bg_start_angle(lv_arc_1, 135);
    lv_arc_set_bg_end_angle(lv_arc_1, 45);
    lv_arc_set_change_rate(lv_arc_1, 360);
    lv_arc_set_value(lv_arc_1, 35);
    lv_obj_t * lv_label_2 = lv_label_create(lv_arc_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "rate=360");
}
#endif
