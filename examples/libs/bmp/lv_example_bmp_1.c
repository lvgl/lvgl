#include "../../lv_examples.h"
#if LV_USE_BMP && LV_BUILD_EXAMPLES

/**
 * @title Decode BMP from filesystem
 * @brief Load a 32-bit BMP file through the LVGL filesystem and center it on the screen.
 *
 * An image widget is created on the active screen and its source is set to
 * `A:lvgl/examples/libs/bmp/example_32bit.bmp` so the BMP decoder reads the
 * file through an attached filesystem driver registered under drive letter
 * `A`, such as `LV_USE_FS_STDIO`.
 */
void lv_example_bmp_1(void)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/bmp/example_32bit.bmp");
    lv_obj_center(img);

}

#endif
