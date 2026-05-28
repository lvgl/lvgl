/**
 * @file lv_example_spinbox_bind_value.c
 */

#include "../../lv_examples.h"
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

    /* 💡 Tap the spinbox arrows; the arc follows because both bind to `subject_value`. */
    lv_obj_t * arc = lv_arc_create(screen);
    lv_obj_set_size(arc, 160, 160);
    lv_arc_bind_value(arc, &subject_value);
    lv_obj_set_align(arc, LV_ALIGN_CENTER);
    lv_obj_t * spinbox = lv_spinbox_create(arc);
    lv_obj_set_width(spinbox, 80);
    lv_spinbox_set_digit_count(spinbox, 3);
    lv_spinbox_set_min_value(spinbox, 0);
    lv_spinbox_set_max_value(spinbox, 100);
    lv_spinbox_set_step(spinbox, 1);
    lv_spinbox_bind_value(spinbox, &subject_value);
    lv_obj_set_align(spinbox, LV_ALIGN_CENTER);
    lv_obj_set_y(spinbox, -20);

    lv_obj_t * button_1 = lv_button_create(arc);
    lv_obj_set_align(button_1, LV_ALIGN_CENTER);
    lv_obj_set_y(button_1, 20);
    lv_obj_set_x(button_1, -20);
    lv_obj_set_size(button_1, 30, 30);
    lv_obj_t * label_1 = lv_label_create(button_1);
    lv_label_set_text(label_1, "-");
    lv_obj_set_align(label_1, LV_ALIGN_CENTER);

    lv_obj_add_subject_increment_event(button_1, &subject_value, LV_EVENT_CLICKED, -1);
    lv_obj_add_subject_increment_event(button_1, &subject_value, LV_EVENT_LONG_PRESSED_REPEAT, -1);

    lv_obj_t * button_2 = lv_button_create(arc);
    lv_obj_set_align(button_2, LV_ALIGN_CENTER);
    lv_obj_set_y(button_2, 20);
    lv_obj_set_x(button_2, 20);
    lv_obj_set_size(button_2, 30, 30);
    lv_obj_t * label_2 = lv_label_create(button_2);
    lv_label_set_text(label_2, "+");
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);

    lv_obj_add_subject_increment_event(button_2, &subject_value, LV_EVENT_CLICKED, 1);
    lv_obj_add_subject_increment_event(button_2, &subject_value, LV_EVENT_LONG_PRESSED_REPEAT, 1);
}
#endif
