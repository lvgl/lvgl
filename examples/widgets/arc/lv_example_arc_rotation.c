/**
 * @file lv_example_arc_rotation.c
 */

#include "../../lv_examples.h"
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
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Arc rotated by 45 degrees */
    lv_obj_t * arc_1 = lv_arc_create(screen);
    lv_obj_set_size(arc_1, 120, 120);
    lv_arc_set_bg_start_angle(arc_1, 0);
    lv_arc_set_bg_end_angle(arc_1, 270);
    lv_arc_set_rotation(arc_1, 45);
    lv_arc_set_value(arc_1, 60);
    lv_obj_t * label_1 = lv_label_create(arc_1);
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);
    lv_label_set_text(label_1, "rot=45");

    /* Arc rotated by 135 degrees */
    lv_obj_t * arc_2 = lv_arc_create(screen);
    lv_obj_set_size(arc_2, 120, 120);
    lv_arc_set_bg_start_angle(arc_2, 0);
    lv_arc_set_bg_end_angle(arc_2, 270);
    lv_arc_set_rotation(arc_2, 135);
    lv_arc_set_value(arc_2, 60);
    lv_obj_t * label_2 = lv_label_create(arc_2);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "rot=135");
}
#endif
