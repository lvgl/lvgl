#include "../../lv_examples.h"
#if LV_USE_GIF && LV_BUILD_EXAMPLES

/**
 * @title Animated GIF from array and file
 * @brief Show the same bulb GIF decoded from a C array and from a file path.
 *
 * Two `lv_gif` widgets are created on the active screen with
 * `LV_COLOR_FORMAT_ARGB8888`. The left one binds to the embedded
 * `img_bulb_gif` descriptor via `lv_gif_set_src`; the right one reads
 * `A:lvgl/examples/libs/gif/bulb.gif` through the filesystem driver
 * registered under drive letter `A`.
 */
void lv_example_gif_1(void)
{
    LV_IMAGE_DECLARE(img_bulb_gif);
    lv_obj_t * img;

    img = lv_gif_create(lv_screen_active());
    lv_gif_set_color_format(img, LV_COLOR_FORMAT_ARGB8888);
    lv_gif_set_src(img, &img_bulb_gif);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 20, 0);

    img = lv_gif_create(lv_screen_active());
    lv_gif_set_color_format(img, LV_COLOR_FORMAT_ARGB8888);
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_gif_set_src(img, "A:lvgl/examples/libs/gif/bulb.gif");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -20, 0);
}

#endif
