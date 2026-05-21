/**
 * @file lv_example_button_states.c
 */

#include "../../../lv_examples.h"
#if LV_USE_BUTTON && LV_BUILD_EXAMPLES

/**
 * @title Button states
 * @brief Show normal, checked, and disabled buttons side by side.
 *
 * Three buttons share the same shape but advertise different states via flags. The
 * second one is checkable and starts in the checked state, picking up the checked-style
 * colors. The third uses the disabled state so it ignores input and renders faded.
 */
void lv_example_button_states(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Default button */
    lv_obj_t * lv_button_0 = lv_button_create(screen);
    lv_obj_set_size(lv_button_0, 160, 40);
    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, "Normal");

    /* Checkable, starts checked */
    lv_obj_t * lv_button_1 = lv_button_create(screen);
    lv_obj_set_size(lv_button_1, 160, 40);
    lv_obj_set_flag(lv_button_1, LV_OBJ_FLAG_CHECKABLE, true);
    lv_obj_set_state(lv_button_1, LV_STATE_CHECKED, true);
    lv_obj_t * lv_label_1 = lv_label_create(lv_button_1);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_1, "Checked");

    /* Disabled */
    lv_obj_t * lv_button_2 = lv_button_create(screen);
    lv_obj_set_size(lv_button_2, 160, 40);
    lv_obj_set_state(lv_button_2, LV_STATE_DISABLED, true);
    lv_obj_t * lv_label_2 = lv_label_create(lv_button_2);
    lv_obj_set_align(lv_label_2, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_2, "Disabled");
}
#endif
