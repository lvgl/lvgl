#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_RLOTTIE

/**
 * Load an lottie animation from flash
 */
void lv_example_rlottie_1(void)
{
    extern const uint8_t lv_example_rlottie_approve[];
    extern const size_t lv_example_rlottie_approve_size;
    lv_obj_t * lottie = lv_img_create(lv_scr_act());
    lv_rlottie_from_raw(lottie, 100, 100, (const void *)lv_example_rlottie_approve, lv_example_rlottie_approve_size, 0);
    lv_obj_center(lottie);
}

#else
void lv_example_rlottie_1(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Rlottie is not installed");
    lv_obj_center(label);
}

#endif
#endif
