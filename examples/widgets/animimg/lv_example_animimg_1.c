#include "../../lv_examples.h"
#if LV_USE_ANIMIMG && LV_BUILD_EXAMPLES
LV_IMG_DECLARE(animimg001)
LV_IMG_DECLARE(animimg002)
LV_IMG_DECLARE(animimg003)



void lv_example_animimg_1(void)
{
    lv_obj_t * animimg0 = lv_animimg_create(lv_scr_act());
    lv_img_src_t * anim_imgs[3] = {
        lv_img_src_from_raw(&animimg001),
        lv_img_src_from_raw(&animimg002),
        lv_img_src_from_raw(&animimg003),
    };
    lv_obj_center(animimg0);
    lv_animimg_set_src(animimg0, lv_img_src_from_srcs(anim_imgs, 3));
    lv_animimg_set_duration(animimg0, 1000);
    lv_animimg_set_repeat_count(animimg0, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg0);
}

#endif
