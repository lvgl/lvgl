#include "../../lv_examples.h"
#if LV_USE_LODEPNG && LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * @title PNG from array and file with LodePNG
 * @brief Show the same PNG decoded from an embedded array and from a file path.
 *
 * Two image widgets are created on the active screen. The left one binds
 * to the `img_wink_png` descriptor declared with `LV_IMAGE_DECLARE`, and
 * the right one reads `A:lvgl/examples/libs/lodepng/wink.png` through
 * the filesystem driver registered under drive letter `A`. Both routes
 * go through the LodePNG image decoder.
 */
void lv_example_lodepng_1(void)
{
    LV_IMAGE_DECLARE(img_wink_png);
    lv_obj_t * img;

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &img_wink_png);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);

    img = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/lodepng/wink.png");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
}

#endif
