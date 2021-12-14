#include "../../lv_examples.h"
#if LV_USE_IMG && LV_BUILD_EXAMPLES


void lv_example_img_1(void)
{
    LV_IMG_DECLARE(img_cogwheel_argb);
    lv_obj_t * img1 = lv_img_create(lv_scr_act());
    lv_img_set_src(img1, &img_cogwheel_argb);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_size(img1, 200, 200);
    lv_obj_set_style_blend_mode(img1, LV_BLEND_MODE_MULTIPLY, 0);



    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_style_blend_mode(btn, LV_BLEND_MODE_MULTIPLY, 0);
//    lv_obj_set_style_opa(btn, 130, 0);
    lv_obj_set_size(btn, 100, 100);


    lv_obj_t * img2 = lv_img_create(lv_scr_act());
    lv_img_set_src(img2, LV_SYMBOL_OK "Accept");
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);


    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xff8888), 0);
}

#endif
