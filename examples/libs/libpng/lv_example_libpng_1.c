#include "../../lv_examples.h"
#if LV_USE_LIBPNG && LV_USE_IMG && LV_BUILD_EXAMPLES

/**
 * Open a PNG image from a file
 */
void lv_example_libpng_1(void)
{
    lv_obj_t * img;

    img = lv_image_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/libpng/png_demo.png");
    lv_obj_center(img);
}

#endif
