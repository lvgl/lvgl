#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

void lv_example_image_5(void)
{
    LV_IMAGE_DECLARE(img_svg_img);
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_obj_set_size(img1, lv_pct(100), lv_pct(50));
    lv_obj_set_style_outline_width(img1, 5, 0);
    lv_image_set_src(img1, &img_svg_img);

    lv_obj_set_style_transform_rotation(img1, 450, 0);
    lv_obj_set_style_transform_scale(img1, 128, 0);
    lv_image_set_scale(img1, 128);
    lv_image_set_rotation(img1, 450);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
}

#endif
