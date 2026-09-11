/**
 * @file lv_example_switch_state.c
 */

#include "../../lv_examples.h"
#if LV_USE_SWITCH && LV_BUILD_EXAMPLES

/**
 * @title Switch states
 * @brief Pre-set the on/off state, and show what `disabled` looks like.
 *
 * A switch is a special toggle button: clicking it flips between unchecked
 * and checked. The `checked="true"` attribute sets the initial state at
 * creation; `disabled="true"` greys the widget out and blocks interaction.
 * To react to changes in code, attach an `LV_EVENT_VALUE_CHANGED` callback
 * (not shown here — see the docs for the C-side API).
 */
void lv_example_switch_state(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Toggle either of the first two switches by clicking; the disabled ones won't respond. */
    lv_switch_create(screen);

    lv_obj_t * sw_2 = lv_switch_create(screen);
    lv_obj_set_state(sw_2, LV_STATE_CHECKED, true);

    lv_obj_t * sw_3 = lv_switch_create(screen);
    lv_obj_set_state(sw_3, LV_STATE_DISABLED, true);

    lv_obj_t * sw_4 = lv_switch_create(screen);
    lv_obj_set_state(sw_4, LV_STATE_DISABLED, true);
    lv_obj_set_state(sw_4, LV_STATE_CHECKED, true);
}
#endif
