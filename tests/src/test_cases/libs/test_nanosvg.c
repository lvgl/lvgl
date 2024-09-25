#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_helpers.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    /* Function run before every test */
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(active_screen);
}

void test_nanosvg_1(void)
{
    lv_obj_t * img;

    img = lv_image_create(active_screen);
    lv_image_set_src(img, "A:src/test_assets/test_img_svg.svg");
    lv_obj_center(img);

#ifndef NON_AMD64_BUILD
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/svg_1.lp64.png");
#else
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/svg_1.lp32.png");
#endif
}

#endif
