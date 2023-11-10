#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_helpers.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void create_images(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * img;

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.png");
    lv_obj_center(img);
}

void test_libpng_1(void)
{
    /* Temporarily remove lodepng decoder */
    lv_lodepng_deinit();

    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_2.png");

    uint32_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_2.png");

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 32);

    /* Re-add lodepng decoder */
    lv_lodepng_init();
}

#endif
