#include "../../lv_examples.h"
#if LV_USE_BAR && LV_BUILD_EXAMPLES

/**
 * @title Simple bar at 70 percent
 * @brief A default 200 x 20 bar centered on the screen with its value set to 70.
 *
 * `lv_bar_create` produces a themed progress bar that is sized to
 * 200 x 20 px and centered on the active screen. `lv_bar_set_value` with
 * `LV_ANIM_OFF` snaps the indicator to 70 on the default 0 to 100 range
 * without animating.
 */
void lv_example_bar_1(void)
{
    lv_obj_t * bar1 = lv_bar_create(lv_screen_active());
    lv_obj_set_size(bar1, 200, 20);
    lv_obj_center(bar1);
    lv_bar_set_value(bar1, 70, LV_ANIM_OFF);
}

#endif
