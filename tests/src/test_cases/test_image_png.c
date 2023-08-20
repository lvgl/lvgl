#if LV_BUILD_TEST
#include "../lvgl.h"

#if LV_USE_PNG

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_image_png_decode(void)
{
    lv_obj_t * img = lv_img_create(active_screen);
    lv_obj_center(img);
    lv_img_set_src(img, "A:src/test_files/image_png_1.png");

    TEST_ASSERT_EQUAL_SCREENSHOT("image_png_1.png");
}

#endif

#endif
