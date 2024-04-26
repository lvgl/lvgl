#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_helpers.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void create_image(void)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.bmp");
    lv_obj_center(img);
}

void test_bmp(void)
{
    create_image();
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/bmp_1.png");
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/bmp_1.png");
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
}

static void create_image_tile(void)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.bmp");
    lv_obj_center(img);
    lv_obj_set_size(img, 300, 200);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
}

void test_bmp_align_tile(void)
{
    create_image_tile();
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/bmp_2.png");
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image_tile();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/bmp_2.png");
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
}

#endif
