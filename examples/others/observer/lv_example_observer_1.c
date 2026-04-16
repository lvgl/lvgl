#include "../../lv_examples.h"
#if LV_USE_OBSERVER && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

static lv_subject_t temperature_subject;

/**
 * @title Slider and label bound to an int subject
 * @brief Move a slider and watch a label 30 px below it reformat live as the shared subject updates.
 *
 * `temperature_subject` is initialised with `lv_subject_init_int` to 28. A
 * centered slider binds to it with `lv_slider_bind_value`, and a label 30 px
 * below binds with `lv_label_bind_text` using a degree-Celsius format. Moving
 * the slider pushes the new value through the subject, which rewrites the
 * label text.
 */
void lv_example_observer_1(void)
{
    lv_subject_init_int(&temperature_subject, 28);

    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_screen_active());
    lv_obj_center(slider);
    lv_slider_bind_value(slider, &temperature_subject);

    /*Create a label below the slider*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 30);
    lv_label_bind_text(label, &temperature_subject, "%d °C");
}

#endif
