#include "../../lv_examples.h"
#if LV_USE_RLOTTIE && LV_BUILD_EXAMPLES

/**
 * Load an lottie animation from file
 */
void lv_example_rlottie_2(void)
{
    /*The rlottie library uses STDIO file API, so there is no drievr letter for LVGL*/
    lv_obj_t * lottie = lv_rlottie_create_from_file(lv_scr_act(), 100, 100,
            "lvgl/examples/libs/rlottie/lv_example_rlottie_approve.json");
    lv_obj_center(lottie);
}

#endif
