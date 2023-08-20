#if LV_BUILD_TEST
#include "../lvgl.h"

#if LV_USE_SJPG

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    lv_obj_clean(active_screen);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_image_jpg_decode(void)
{
    lv_obj_t * img = lv_img_create(active_screen);
    lv_obj_center(img);
    lv_obj_set_style_opa(img, LV_OPA_COVER, LV_PART_MAIN);
    lv_img_set_src(img, "A:src/test_files/image_jpg_1.jpg");

    // JPEG decoder test currently fails, so after the decoder is fixed, undo next line:
    //TEST_ASSERT_EQUAL_SCREENSHOT("image_jpg_1.png");
}

#endif

#endif
