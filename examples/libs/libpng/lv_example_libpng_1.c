#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

#if LV_USE_LIBPNG

/**
 * Open a PNG image from a file
 */
void lv_example_libpng_1(void)
{
    lv_obj_t * img;

    img = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/libpng/png_demo.png");
    lv_obj_center(img);
}

#else

void lv_example_libpng_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "LibPNG is not installed");
    lv_obj_center(label);
}

#endif

#endif
