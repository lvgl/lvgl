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

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, -150, -150);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation 0");
    lv_obj_align(label, LV_ALIGN_CENTER, -150, -100);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_180.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 150, -150);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation 180");
    lv_obj_align(label, LV_ALIGN_CENTER, 150, -100);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_90.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, -150, 40);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation 90");
    lv_obj_align(label, LV_ALIGN_CENTER, -150, 150);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_270.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 150, 40);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation 270");
    lv_obj_align(label, LV_ALIGN_CENTER, 150, 150);
}

void test_jpg_2(void)
{
    /* Temporarily remove tjpgd decoder */
    lv_tjpgd_deinit();

    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_2.png");

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_2.png");

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 64);

    /* Re-add tjpgd decoder */
    lv_tjpgd_init();
}

#endif
