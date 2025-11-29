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
    lv_obj_align(label, LV_ALIGN_CENTER, -150, -110);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_180.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 150, -150);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation 180");
    lv_obj_align(label, LV_ALIGN_CENTER, 150, -110);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_hflip.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, -150, -60);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation hflip");
    lv_obj_align(label, LV_ALIGN_CENTER, -150, -20);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_vflip.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 150, -60);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "jpeg with exif orientation vflip");
    lv_obj_align(label, LV_ALIGN_CENTER, 150, -20);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_90.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, -280, 70);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "orientation 90");
    lv_obj_align(label, LV_ALIGN_CENTER, -280, 150);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_270.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, -100, 70);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "orientation 270");
    lv_obj_align(label, LV_ALIGN_CENTER, -100, 150);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_transpose.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 100, 70);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "orientation transpose");
    lv_obj_align(label, LV_ALIGN_CENTER, 70, 150);

    img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_transverse.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 300, 70);

    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "orientation transverse");
    lv_obj_align(label, LV_ALIGN_CENTER, 280, 150);
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

void test_jpg_cmyk(void)
{
    /* Temporarily remove tjpgd decoder */
    lv_tjpgd_deinit();

    lv_obj_clean(lv_screen_active());
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_cmyk.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_cmyk.png");

    /* Re-add tjpgd decoder */
    lv_tjpgd_init();
}

#endif
