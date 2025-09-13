#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"


void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void create_images(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * img;
    lv_obj_t * label;

    LV_IMAGE_DECLARE(test_img_lvgl_logo_jpg);
    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_img_lvgl_logo_jpg);
    lv_obj_align(img, LV_ALIGN_CENTER, -100, -20);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Array");
    lv_obj_align(label, LV_ALIGN_CENTER, -100, 20);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 100, -20);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "File");
    lv_obj_align(label, LV_ALIGN_CENTER, 100, 20);
}

void test_tjpgd_1(void)
{
    /* Temporarily remove libjpeg_turbo decoder */
    lv_libjpeg_turbo_deinit();

    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_1.png");

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_1.png");

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 32);

    /* Re-add libjpeg_turbo decoder */
    lv_libjpeg_turbo_init();
}

static void create_image_2(void)
{
    LV_IMAGE_DECLARE(test_img_lvgl_logo_jpg);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_img_lvgl_logo_jpg);
    lv_obj_center(img);
    lv_obj_set_size(img, 300, 200);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
}

void test_jdpgd_align_tile(void)
{
    /* Temporarily remove libjpeg_turbo decoder */
    lv_libjpeg_turbo_deinit();

    create_image_2();
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_3.png");
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image_2();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_3.png");
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);

    /* Re-add libjpeg_turbo decoder */
    lv_libjpeg_turbo_init();
}

#endif
