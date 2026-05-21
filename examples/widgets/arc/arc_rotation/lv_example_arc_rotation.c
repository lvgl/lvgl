/**
 * @file lv_example_arc_rotation.c
 */

#include "../../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc rotation offset
 * @brief Rotate the whole dial around its center.
 *
 * The rotation attribute shifts both background and indicator angles by the same amount,
 * keeping their relative span. The two arcs share the same bg_start_angle, bg_end_angle,
 * and value, so only the rotation offset distinguishes them visually.
 */
void lv_example_arc_rotation(void)
{
    lv_obj_t * screen = lv_screen_active();

    /* 💡 Adjust rotation while keeping bg_start_angle/bg_end_angle fixed to see the whole dial rotate around its center. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Arc: rotation offset");

    /* Arc rotated by 45 degrees */
    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_0, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(lv_arc_0, 10);
    lv_obj_set_size(lv_arc_0, 90, 90);
    lv_arc_set_bg_start_angle(lv_arc_0, 0);
    lv_arc_set_bg_end_angle(lv_arc_0, 270);
    lv_arc_set_rotation(lv_arc_0, 45);
    lv_arc_set_value(lv_arc_0, 60);
    lv_obj_t * lv_label_1 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "rot=45");

    /* Arc rotated by 135 degrees */
    lv_obj_t * lv_arc_1 = lv_arc_create(screen);
    lv_obj_set_align(lv_arc_1, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(lv_arc_1, -10);
    lv_obj_set_size(lv_arc_1, 90, 90);
    lv_arc_set_bg_start_angle(lv_arc_1, 0);
    lv_arc_set_bg_end_angle(lv_arc_1, 270);
    lv_arc_set_rotation(lv_arc_1, 135);
    lv_arc_set_value(lv_arc_1, 60);
    lv_obj_t * lv_label_2 = lv_label_create(lv_arc_1);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "rot=135");
}
#endif
