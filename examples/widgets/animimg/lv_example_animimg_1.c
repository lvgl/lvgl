#include "../../lv_examples.h"
#if LV_USE_ANIMIMG && LV_BUILD_EXAMPLES
LV_IMAGE_DECLARE(animimg001);
LV_IMAGE_DECLARE(animimg002);
LV_IMAGE_DECLARE(animimg003);

static const lv_image_dsc_t * anim_imgs[3] = {
    &animimg001,
    & animimg002,
    & animimg003,
};

/**
 * @title Three-frame animated image
 * @brief Cycle three frames on a centered animated image widget.
 *
 * An `lv_animimg` is centered on the active screen and receives an
 * array of three `lv_image_dsc_t` descriptors through
 * `lv_animimg_set_src`. `lv_animimg_set_duration` sets one full cycle
 * to 1000 ms, `lv_animimg_set_repeat_count` uses
 * `LV_ANIM_REPEAT_INFINITE`, and `lv_animimg_start` kicks the
 * animation off.
 */
void lv_example_animimg_1(void)
{
    lv_obj_t * animimg0 = lv_animimg_create(lv_screen_active());
    lv_obj_center(animimg0);
    lv_animimg_set_src(animimg0, (const void **) anim_imgs, 3);
    lv_animimg_set_duration(animimg0, 1000);
    lv_animimg_set_repeat_count(animimg0, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(animimg0);
}

#endif
