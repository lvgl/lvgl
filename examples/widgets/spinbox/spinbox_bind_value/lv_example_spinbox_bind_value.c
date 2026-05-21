/**
 * @file lv_example_spinbox_bind_value.c
 */

#include "../../../lv_examples.h"
#if LV_USE_SPINBOX && LV_BUILD_EXAMPLES

/**
 * @title Spinbox bind value
 * @brief Spinbox edits a subject; a bound arc visualises the same number.
 *
 * The spinbox lets the user nudge `subject_value` with precision. The arc reads
 * the same subject through `bind_value`, so the visualisation tracks every step
 * without any explicit redraw call. Use this pattern whenever a numeric editor
 * and a graphical indicator both refer to the same underlying quantity.
 */
void lv_example_spinbox_bind_value(void)
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

    /* 💡 Tap the spinbox arrows; the arc follows because both bind to `subject_value`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Spinbox: edit, arc displays");

    lv_obj_t * lv_arc_0 = lv_arc_create(screen);
    lv_obj_set_size(lv_arc_0, 160, 160);
    lv_arc_bind_value(lv_arc_0, &subject_value);
    lv_obj_t * lv_spinbox_0 = lv_spinbox_create(lv_arc_0);
    lv_obj_set_width(lv_spinbox_0, 80);
    lv_spinbox_set_digit_count(lv_spinbox_0, 3);
    lv_spinbox_set_min_value(lv_spinbox_0, 0);
    lv_spinbox_set_max_value(lv_spinbox_0, 100);
    lv_spinbox_set_step(lv_spinbox_0, 1);
    lv_spinbox_bind_value(lv_spinbox_0, &subject_value);
    lv_obj_set_align(lv_spinbox_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_spinbox_0, -20);

    lv_obj_t * lv_button_0 = lv_button_create(lv_arc_0);
    lv_obj_set_align(lv_button_0, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_button_0, 20);
    lv_obj_set_x(lv_button_0, -20);
    lv_obj_set_size(lv_button_0, 30, 30);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, "-");
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);

    lv_obj_add_subject_increment_event(lv_button_0, &subject_value, LV_EVENT_CLICKED, -1);
    lv_obj_add_subject_increment_event(lv_button_0, &subject_value, LV_EVENT_LONG_PRESSED_REPEAT, -1);

    lv_obj_t * lv_button_1 = lv_button_create(lv_arc_0);
    lv_obj_set_align(lv_button_1, LV_ALIGN_CENTER);
    lv_obj_set_y(lv_button_1, 20);
    lv_obj_set_x(lv_button_1, 20);
    lv_obj_set_size(lv_button_1, 30, 30);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_1);
    lv_label_set_text(lv_label_2, "+");
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);

    lv_obj_add_subject_increment_event(lv_button_1, &subject_value, LV_EVENT_CLICKED, 1);
    lv_obj_add_subject_increment_event(lv_button_1, &subject_value, LV_EVENT_LONG_PRESSED_REPEAT, 1);
}
#endif
