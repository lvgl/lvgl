#include "../../lv_examples.h"
#if LV_USE_GIF && LV_BUILD_EXAMPLES

/**
 * Open a GIF image from a file and a variable
 */
void lv_example_gif_1(void)
{
    LV_IMG_DECLARE(img_bulb_gif);
    lv_obj_t * img;

    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, lv_img_src_from_data(img_bulb_gif.data, img_bulb_gif.data_size, 0));
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);

    img = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img, lv_img_src_from_file("A:lvgl/examples/libs/gif/bulb.gif", 0));
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
}

#endif
