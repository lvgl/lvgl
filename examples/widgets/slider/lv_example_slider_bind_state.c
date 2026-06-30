/**
 * @file lv_example_slider_bind_state.c
 */

#include "../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

/**
 * @title Slider drives another widget's state
 * @brief Bind a button's `disabled` state to a subject driven by a slider's value.
 *
 * The slider edits `subject_value` (0..100). The "Submit" button uses
 * `bind_state_if_gt` to apply `disabled` whenever `subject_value > 80` — modelling
 * a guard like "block submit when the value is out of range". Slide past 80 to
 * watch the button grey out.
 */
void lv_example_slider_bind_state(void)
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

    /* 💡 Drag past 80: the button becomes disabled; drop below 80 to re-enable it. */
    lv_obj_t * slider = lv_slider_create(screen);
    lv_obj_set_width(slider, lv_pct(90));
    lv_slider_bind_value(slider, &subject_value);

    lv_obj_t * label_1 = lv_label_create(screen);
    lv_label_bind_text(label_1, &subject_value, "Current: %d (disabled above 80)");

    lv_obj_t * button = lv_button_create(screen);
    lv_obj_t * label_2 = lv_label_create(button);
    lv_obj_set_align(label_2, LV_ALIGN_CENTER);
    lv_label_set_text(label_2, "Submit");

    lv_obj_bind_state_if_gt(button, &subject_value, LV_STATE_DISABLED, 80);
}
#endif
