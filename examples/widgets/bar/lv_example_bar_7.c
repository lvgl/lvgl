#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * @title Vertical bar filling top to bottom
 * @brief A 20 x 180 vertical bar with a reversed 100-to-0 range at value 70.
 *
 * `lv_bar_set_range(bar_tob, 100, 0)` flips the indicator so it grows from
 * the top edge downward instead of bottom-up. `lv_bar_set_value` pins the
 * value at 70, and a label placed with `LV_ALIGN_OUT_TOP_MID` names the
 * direction above the bar.
 */
void lv_example_bar_7(void)
{
    lv_obj_t * bar_tob = lv_bar_create(lv_screen_active());
    lv_obj_set_size(bar_tob, 20, 180);
    lv_bar_set_range(bar_tob, 100, 0);
    lv_bar_set_value(bar_tob, 70, LV_ANIM_OFF);
    lv_obj_align(bar_tob, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "From top to bottom");
    lv_obj_align_to(label, bar_tob, LV_ALIGN_OUT_TOP_MID, 0, -5);
}

#endif
