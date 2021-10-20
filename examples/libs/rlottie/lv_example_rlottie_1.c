#include "../../lv_examples.h"
#if LV_USE_RLOTTIE && LV_BUILD_EXAMPLES

/**
 * Load an lottie animation from flash
 */
void lv_example_rlottie_1(void)
{
    extern const uint8_t lv_rlottie_example_card[];
    lv_obj_t * lottie = lv_rlottie_create_from_raw(lv_scr_act(), 100, 100, (const void *)lv_rlottie_example_card);
    lv_obj_center(lottie);
}

#endif
