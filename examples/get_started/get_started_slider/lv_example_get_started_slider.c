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
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Drag the slider; the label tracks it because both share `subject_value`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_label_bind_text(lv_label_0, &subject_value, "%d");

    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_0, lv_pct(90));
    lv_slider_bind_value(lv_slider_0, &subject_value);
}
#endif
