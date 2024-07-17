#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

void lv_example_image_1(void)
{
    LV_IMAGE_DECLARE(img_cogwheel_argb);
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &img_cogwheel_argb);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_rotation(img1, 600, 0);
    lv_obj_set_style_transform_scale(img1, 700, 0);
    lv_obj_set_style_transform_pivot_x(img1, -100, 0);


    //    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    //    lv_image_set_src(img2, LV_SYMBOL_OK "Accept");
    //    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

#endif
