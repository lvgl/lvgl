/**
 * @file lv_example_checkbox_states.c
 */

#include "../../../lv_examples.h"
#if LV_USE_CHECKBOX && LV_BUILD_EXAMPLES

/**
 * @title Checkbox states
 * @brief Show unchecked, checked, and disabled tick boxes side by side.
 *
 * Each checkbox carries the same text but advertises a different state. State is
 * toggled at runtime with `lv_obj_add_state(cb, LV_STATE_CHECKED)` and friends; in
 * XML the same effect is achieved by setting the state attribute directly.
 */
void lv_example_checkbox_states(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* Default unchecked */
    lv_obj_t * lv_checkbox_0 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_0, "Unchecked");

    /* Checked */
    lv_obj_t * lv_checkbox_1 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_1, "Checked");
    lv_obj_set_state(lv_checkbox_1, LV_STATE_CHECKED, true);

    /* Disabled */
    lv_obj_t * lv_checkbox_2 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_2, "Disabled");
    lv_obj_set_state(lv_checkbox_2, LV_STATE_DISABLED, true);

    /* Disabled and Checked */
    lv_obj_t * lv_checkbox_3 = lv_checkbox_create(screen);
    lv_checkbox_set_text(lv_checkbox_3, "Disabled + Checked");
    lv_obj_set_state(lv_checkbox_3, LV_STATE_DISABLED, true);
    lv_obj_set_state(lv_checkbox_3, LV_STATE_CHECKED, true);
}
#endif
