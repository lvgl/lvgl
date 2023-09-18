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
    lv_obj_clean(lv_scr_act());

    lv_obj_t * img;
    lv_obj_t * label;

    LV_IMG_DECLARE(test_img_lvgl_logo_png);
    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &test_img_lvgl_logo_png);
    lv_obj_align(img, LV_ALIGN_CENTER, -100, -20);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Array");
    lv_obj_align(label, LV_ALIGN_CENTER, -100, 20);

    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "A:src/test_assets/test_img_lvgl_logo.png");
    lv_obj_align(img, LV_ALIGN_CENTER, 100, -20);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "File");
    lv_obj_align(label, LV_ALIGN_CENTER, 100, 20);
}

void test_png_1(void)
{
    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_1.png");


    uint32_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_scr_act());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_1.png");

    TEST_ASSERT_EQUAL(mem_before, lv_test_get_free_mem());

}

#endif
