/**
 * @file lv_example_arc_bind_value.c
 */

#include "../../../../lvgl.h"

/**
 * @title Arc bind value
 * @brief An arc and a slider co-bound to one subject; moving the slider rotates the arc.
 *
 * Both widgets target `subject_value` via `bind_value`. The slider is the editor;
 * the arc is a read-only visualizer. Because the binding is two-way at the widget
 * level, the arc would also push updates back if it were interactive — but here
 * we lock it with `clickable="false"` so it stays purely indicative.
 */
void lv_example_arc_bind_value_create(void)
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
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Slide to change `subject_value`; the arc follows because both bind to it. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Arc: bind to subject");

    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_size(lv_arc_0, 160, 160);
    lv_arc_bind_value(lv_arc_0, &subject_value);
    lv_obj_set_flag(lv_arc_0, LV_OBJ_FLAG_CLICKABLE, false);

    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_0, 220);
    lv_slider_bind_value(lv_slider_0, &subject_value);
}

