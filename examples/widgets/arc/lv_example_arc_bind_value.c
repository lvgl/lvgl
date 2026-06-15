/**
 * @file lv_example_arc_bind_value.c
 */

#include "../../lv_examples.h"
#if LV_USE_ARC && LV_BUILD_EXAMPLES

/**
 * @title Arc bind value
 * @brief An arc and a slider co-bound to one subject; moving the slider rotates the arc.
 *
 * Both widgets target `subject_value` via `bind_value`. The slider is the editor;
 * the arc is a read-only visualizer. Because the binding is two-way at the widget
 * level, the arc would also push updates back if it were interactive — but here
 * we lock it with `clickable="false"` so it stays purely indicative.
 */
void lv_example_arc_bind_value(void)
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
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Slide to change `subject_value`; the arc follows because both bind to it. */
    lv_obj_t * arc = lv_arc_create(screen);
    lv_obj_set_size(arc, 160, 160);
    lv_arc_bind_value(arc, &subject_value);
    lv_obj_set_flag(arc, LV_OBJ_FLAG_CLICKABLE, false);
    lv_obj_set_style_bg_opa(arc, (255 * 0 / 100), LV_PART_KNOB);
    lv_obj_t * lv_label_0 = lv_label_create(arc);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_bind_text(lv_label_0, &subject_value, "%d %%");

    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_width(slider, 220);
    lv_slider_bind_value(slider, &subject_value);
}
#endif
