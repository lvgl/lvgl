#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

void lv_example_image_1(void)
{
    LV_IMAGE_DECLARE(
        img_render_lvgl_logo_rgb565a8);
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &
                     img_render_lvgl_logo_rgb565a8);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_image_recolor(img1, lv_color_hex(0xff0000), 0);
    lv_obj_set_style_image_recolor_opa(img1, 200, 0);

    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, LV_SYMBOL_OK "Accept");
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

#endif
