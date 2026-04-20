#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_DRAW_TRANSFORM_USE_MATRIX

static void timer_cb(lv_timer_t * timer)
{
    lv_obj_t * obj = (lv_obj_t *) lv_timer_get_user_data(timer);

    static float value = 0.1f;
    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_scale(&matrix, value, 1);
    lv_matrix_rotate(&matrix, value * 360);
    lv_obj_set_transform(obj, &matrix);

    value += 0.01f;

    if(value > 2.0f) {
        lv_obj_reset_transform(obj);
        value = 0.1f;
    }
}

/**
 * @title Animated matrix transform on a base object
 * @brief Continuously scale and rotate a centered object using `lv_obj_set_transform`.
 *
 * A centered base object is paired with a 20 ms `lv_timer` that
 * builds an identity `lv_matrix_t`, scales its x axis, rotates it by
 * 360 times the scale factor, and applies the result with
 * `lv_obj_set_transform`. The scale factor grows from 0.1 to 2.0 in
 * 0.01 steps; once it exceeds 2.0 the object is reset with
 * `lv_obj_reset_transform` and the loop restarts.
 */
void lv_example_obj_3(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_center(obj);

    lv_timer_create(timer_cb, 20, obj);
}

#else

void lv_example_obj_3(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text_static(label, "LV_DRAW_TRANSFORM_USE_MATRIX is not enabled");
    lv_obj_center(label);
}

#endif /*LV_DRAW_TRANSFORM_USE_MATRIX*/
#endif /*LV_BUILD_EXAMPLES*/
