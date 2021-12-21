#include "../../lv_examples.h"
#if LV_USE_PNG && LV_USE_IMG && LV_BUILD_EXAMPLES

/**
 * Open a PNG image from a file and a variable
 */
void lv_example_png_1(void)
{
    LV_IMG_DECLARE(img_wink_png);
    lv_obj_t * img;

    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &img_wink_png);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);

    img = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img, "A:lvgl/examples/libs/png/wink.png");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
}

#endif
