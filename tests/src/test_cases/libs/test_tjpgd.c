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
    lv_obj_t * label;

    LV_IMG_DECLARE(test_img_lvgl_logo_jpg);
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

#endif
