#include "../../lv_examples.h"
#if LV_USE_TJPGD && LV_BUILD_EXAMPLES

/**
 * Load a JPG image
 */
void lv_example_tjpgd_1(void)
{
    lv_obj_t * wp;

    wp = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(wp, "A:lvgl/examples/libs/tjpgd/img_lvgl_logo.jpg");
    lv_obj_center(wp);
}

#endif
