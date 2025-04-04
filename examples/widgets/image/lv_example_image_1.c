#include "../../lv_examples.h"
#if LV_USE_IMAGE && LV_BUILD_EXAMPLES

void lv_example_image_1(void)
{
    static uint8_t buf[8] = {
        0xff, 0x00, 0x00, 0xff,
        0x80, 0x00, 0x00, 0x80,
    };

    static lv_image_dsc_t dsc = {};

    dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    dsc.header.cf = LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED;
    dsc.header.w = 2;
    dsc.header.h = 1;
    dsc.header.stride = 8;
    dsc.data_size = 8;
    dsc.data = buf;


    lv_obj_t * img1 = lv_image_create(lv_screen_active());
    lv_image_set_src(img1, &dsc);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

    //    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    //    lv_image_set_src(img2, LV_SYMBOL_OK "Accept");
    //    lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

#endif
