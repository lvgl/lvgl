#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

#if LV_USE_LIBJPEG_TURBO

/**
 * @title Decode JPEG with libjpeg-turbo
 * @brief Load a JPEG file through the libjpeg-turbo decoder and center it on the screen.
 *
 * An image widget is created on the active screen with its source set to
 * `A:lvgl/examples/libs/libjpeg_turbo/flower.jpg`, so the libjpeg-turbo
 * image decoder reads the file through the filesystem driver registered
 * under drive letter `A`.
 */
void lv_example_libjpeg_turbo_1(void)
{
    lv_obj_t * wp;

    wp = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(wp, "A:lvgl/examples/libs/libjpeg_turbo/flower.jpg");
    lv_obj_center(wp);
}

#else

void lv_example_libjpeg_turbo_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "LibJPEG-Turbo is not installed");
    lv_obj_center(label);
}

#endif

#endif
