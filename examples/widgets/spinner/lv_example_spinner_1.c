#include "../../lv_examples.h"
#if LV_USE_SPINNER && LV_BUILD_EXAMPLES

/**
 * @title Centered busy spinner
 * @brief Place a 100 by 100 spinner on the active screen with custom timing.
 *
 * A spinner is created on the active screen, sized to 100 by 100,
 * centered, and tuned with `lv_spinner_set_anim_params` to a
 * 10000 ms loop time and a 200 degree arc length.
 */
void lv_example_spinner_1(void)
{
    /*Create a spinner*/
    lv_obj_t * spinner = lv_spinner_create(lv_screen_active());
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);
    lv_spinner_set_anim_params(spinner, 10000, 200);
}

#endif
