#include "../../lv_examples.h"
#if LV_USE_RLOTTIE && LV_BUILD_EXAMPLES

/**
 * Load an lottie animation from file
 */
void lv_example_rlottie_2(void)
{
    extern const char lv_rlottie_example_card[];
    lv_obj_t * lottie = lv_rlottie_create_from_file(lv_scr_act(), 100, 100,
            "A:lvgl/examples/libs/rlottie/lv_rlottie_example_fingerprint.json");
    lv_obj_center(lottie);
}

#endif
