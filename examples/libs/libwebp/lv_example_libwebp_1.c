#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES

#if LV_USE_LIBWEBP

/**
 * Load a WEBP image
 */
void lv_example_libwebp_1(void)
{
    lv_obj_t * img;

    img = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, "A:lvgl/examples/libs/libwebp/rose.webp");
    lv_obj_center(img);
}

#else

void lv_example_libwebp_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Libwebp is not installed");
    lv_obj_center(label);
}

#endif

#endif
