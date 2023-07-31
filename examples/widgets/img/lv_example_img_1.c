#include "../../lv_examples.h"
#if LV_USE_IMG && LV_BUILD_EXAMPLES


void lv_example_img_1(void)
{
    LV_IMG_DECLARE(test_img_cogwheel_rgb565);
    lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &test_img_cogwheel_rgb565);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, -20);
    lv_img_set_angle(img1, 300);
}

#endif
