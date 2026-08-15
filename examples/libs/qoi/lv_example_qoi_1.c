#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

#if LV_USE_QOI

/**
 * @title Open a QOI image from file and variable
 * @brief Load a QOI image from a file and from a variable source and display them side by side.
 *
 * An image widget is created on the active screen. The first image is loaded
 * from a variable (in-memory) source, and the second image is loaded from a
 * file through the LVGL filesystem. The file path assumes a filesystem driver
 * is attached to drive letter `A`, such as `LV_USE_FS_STDIO`.
 */
void lv_example_qoi_1(void)
{
    LV_IMAGE_DECLARE(img_qoi_demo);
    lv_obj_t * img;

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &img_qoi_demo);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 10, 0);

    img = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/qoi/qoi_demo.qoi");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -10, 0);
}

#else

void lv_example_qoi_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "QOI is not installed");
    lv_obj_center(label);
}

#endif

#endif
