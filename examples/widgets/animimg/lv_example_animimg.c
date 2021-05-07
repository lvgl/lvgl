#include "../../lv_examples.h"
#if LV_USE_ANIMIMG && LV_BUILD_EXAMPLES
LV_IMG_DECLARE(anim001)
LV_IMG_DECLARE(anim002)
LV_IMG_DECLARE(anim003)

static const lv_img_dsc_t* anim_imgs[3] = {
	&anim001,
	&anim002,
	&anim003,
};

void lv_example_animimg(void)
{
    lv_obj_t * animimg0 = lv_animimg_create(lv_scr_act());
    lv_obj_set_pos(animimg0, 189, 79);
    lv_animimg_set_src(animimg0, (lv_img_dsc_t**) anim_imgs, 3);
    lv_animimg_set_duration(animimg0, 1000);
    lv_animimg_set_repeat_count(animimg0, 3000);
    lv_animimg_start(animimg0);
}

#endif
