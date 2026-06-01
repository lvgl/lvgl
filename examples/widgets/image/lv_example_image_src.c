/**
 * @file lv_example_image_src.c
 */

#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * @title Image source
 * @brief Display an image registered globally for the project.
 *
 * `lv_image` paints whatever is set as its `src`.
 * In C the image can be a file or a C array.
 * In XML the source needs to b set in globals.xml.
 */
void lv_example_image_src(void)
{
    LV_IMAGE_DECLARE(img_example_lvgl_logo);

    lv_obj_t * screen = lv_screen_active();

    /* 💡 Register another image in `globals.xml` and swap `src` to its name to see a different bitmap. */
    lv_obj_t * image = lv_image_create(screen);
    lv_image_set_src(image, &img_example_lvgl_logo);
    lv_obj_set_align(image, LV_ALIGN_CENTER);
}
#endif
