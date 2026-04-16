#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

/**
 * @title Image from C array and symbol text
 * @brief Show a compiled-in cogwheel bitmap next to a text label rendered from a symbol font.
 *
 * Two `lv_image` widgets are placed on the active screen. The first
 * calls `lv_image_set_src` with the `img_cogwheel_argb` descriptor
 * declared through `LV_IMAGE_DECLARE` and is centered. The second
 * points at the string `LV_SYMBOL_OK "Accept"` and is aligned below
 * the first with `LV_ALIGN_OUT_BOTTOM_MID` and a 20 px offset.
 */
void lv_example_image_1(void)
{
    LV_IMAGE_DECLARE(img_cogwheel_argb);
    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &img_cogwheel_argb);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, LV_SYMBOL_OK "Accept");
    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

#endif
