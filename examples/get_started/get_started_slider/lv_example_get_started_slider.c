/**
 * @file lv_example_get_started_slider.c
 */

#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

/**
 * @title Slider with live value
 * @brief Mirror a slider's value into a label through a shared subject.
 *
 * The slider writes its position to `subject_value` with `bind_value`, and the
 * label reads the same subject with `bind_text` + `bind_text-fmt="%d"`. Because
 * both refer to one subject, dragging the slider updates the label with no event
 * callback — the binding keeps the two in sync.
 */
void lv_example_get_started_slider(void)
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

    /* 💡 Drag the slider; the label tracks it because both share `subject_value`. */
    lv_obj_t * label = lv_label_create(screen);
    lv_obj_set_align(label, LV_ALIGN_CENTER);
    lv_obj_set_y(label, -20);
    lv_label_bind_text(label, &subject_value, "%d");

    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_align(slider, LV_ALIGN_CENTER);
    lv_obj_set_y(slider, 20);
    lv_obj_set_width(slider, lv_pct(90));
    lv_slider_bind_value(slider, &subject_value);
}
#endif
