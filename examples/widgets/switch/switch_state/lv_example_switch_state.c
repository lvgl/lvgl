/**
 * @file lv_example_switch_state.c
 */

#include "../../../../lvgl.h"

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
void lv_example_switch_state_create(void)
{
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_row(screen, 16, 0);

    /* 💡 Toggle either of the first two switches by clicking; the disabled ones won't respond. */
    lv_obj_t * lv_label_0 = lv_label_create(screen);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_set_style_text_align(lv_label_0, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lv_label_0, "Switch: states");

    lv_switch_create(screen);

    lv_obj_t * lv_switch_1 = lv_switch_create(screen);
    lv_obj_set_state(lv_switch_1, LV_STATE_CHECKED, true);

    lv_obj_t * lv_switch_2 = lv_switch_create(screen);
    lv_obj_set_state(lv_switch_2, LV_STATE_DISABLED, true);

    lv_obj_t * lv_switch_3 = lv_switch_create(screen);
    lv_obj_set_state(lv_switch_3, LV_STATE_DISABLED, true);
    lv_obj_set_state(lv_switch_3, LV_STATE_CHECKED, true);
}

