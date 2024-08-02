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
    /* Function run after every test */
}

static void create_images(void)
{
    lv_obj_clean(lv_screen_active());

    lv_obj_t * img;
    lv_obj_t * label;

    /* PNG array */
    LV_IMAGE_DECLARE(test_img_lvgl_logo_png);
    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_img_lvgl_logo_png);
    lv_obj_align(img, LV_ALIGN_CENTER, -100, -20);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Array");
    lv_obj_align(label, LV_ALIGN_CENTER, -100, 20);

    /* 32 bit PNG file */
    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.png");
    lv_obj_align(img, LV_ALIGN_CENTER, 100, -100);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "File (32 bit)");
    lv_obj_align(label, LV_ALIGN_CENTER, 100, -60);

    /* 8 bit palette PNG file */
    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_8bit_palette.png");
    lv_obj_align(img, LV_ALIGN_CENTER, 100, 60);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "File (8 bit palette)");
    lv_obj_align(label, LV_ALIGN_CENTER, 100, 100);
}

void test_libpng_1(void)
{
    /* Temporarily remove lodepng decoder */
    lv_lodepng_deinit();

    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/png_2.png");

    size_t mem_before = lv_test_get_free_mem();
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
