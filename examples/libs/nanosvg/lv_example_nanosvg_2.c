#include "../../lv_examples.h"
#if LV_USE_NANOSVG && LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * Open a SVG image from a file and a variable
 */
void lv_example_nanosvg_2(void)
{
    LV_IMAGE_DECLARE(lv_example_img_nanosvg);
    lv_obj_t * img;

    img = lv_image_create(lv_screen_active());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_image_set_src(img, &lv_example_img_nanosvg);
    lv_obj_center(img);
}

#else

void lv_example_nanosvg_2(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "NanoSvg is not enabled");
    lv_obj_center(label);
}

#endif
