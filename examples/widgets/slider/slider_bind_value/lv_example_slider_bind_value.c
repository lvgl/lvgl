/**
 * @file lv_example_slider_bind_value.c
 */

#include "../../../lv_examples.h"
#if LV_USE_SLIDER && LV_BUILD_EXAMPLES

/**
 * @title Slider bind value
 * @brief Two-way bind a slider to a shared int subject; a label mirrors the live value.
 *
 * `subject_value` lives in `examples/xml_project/globals.xml` (range 0..100, default 50).
 * The slider's `bind_value` reads and writes the subject: dragging it pushes the new
 * value out so anything else bound to `subject_value` (here, the label) updates
 * immediately. `bind_text-fmt` lets a label render a numeric subject through a
 * printf-style format.
 */
void lv_example_slider_bind_value(void)
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

    /* 💡 Drag the slider; the label re-renders because both widgets share `subject_value`. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Slider: bind to subject");

    lv_obj_t * lv_slider_0 = lv_slider_create(screen);
    lv_obj_set_width(lv_slider_0, lv_pct(90));
    lv_slider_bind_value(lv_slider_0, &subject_value);

    lv_obj_t * lv_label_1 = lv_label_create(screen);
    lv_label_bind_text(lv_label_1, &subject_value, "Value: %d/100");
}
#endif
