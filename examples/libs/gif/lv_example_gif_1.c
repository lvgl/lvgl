#include "../../lv_examples.h"
#if LV_USE_GIF && LV_BUILD_EXAMPLES

/**
 * Open a GIF image from a file and a variable
 */
void lv_example_gif_1(void)
{
    LV_RAW_IMG_DECLARE(img_bulb_gif); /* TODO: Remove this, and use the GIF buffer directly */
    lv_obj_t * img;

    img = lv_img_create(lv_scr_act());
    lv_img_set_src_data(img, (const uint8_t *)&img_bulb_gif.data, img_bulb_gif.data_size);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);

    img = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src_file(img, "A:lvgl/examples/libs/gif/bulb.gif");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
}

#endif
