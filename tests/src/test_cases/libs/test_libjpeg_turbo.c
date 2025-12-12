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

static void create_image_unit(const char * img_src, uint32_t image_pos_x, uint32_t image_pos_y, const char * label_text,
                              uint32_t label_pos_x, uint32_t label_pos_y)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, img_src);
    lv_obj_align(img, LV_ALIGN_CENTER, image_pos_x, image_pos_y);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, label_text);
    lv_obj_align(label, LV_ALIGN_CENTER, label_pos_x, label_pos_y);
}

static void create_images(void)
{
    lv_obj_clean(lv_screen_active());

    /* Test images with exif orientation 0 */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_0.jpg", -150, -150,
                      "jpeg with exif orientation 0", -150, -110);
    /* Test images with exif orientation 180 */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_180.jpg", 150, -150,
                      "jpeg with exif orientation 180", 150, -110);
    /* Test images with exif orientation hflip */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_hflip.jpg", -150, -60,
                      "jpeg with exif orientation hflip", -150, -20);
    /* Test images with exif orientation vflip */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_vflip.jpg", 150, -60,
                      "jpeg with exif orientation vflip", 150, -20);
    /* Test images with exif orientation 90 */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_90.jpg", -280, 70, "orientation 90", -280,
                      150);
    /* Test images with exif orientation 270 */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_270.jpg", -100, 70, "orientation 270",
                      -100, 150);
    /* Test images with exif orientation transpose */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_transpose.jpg", 100, 70,
                      "orientation transpose", 100, 150);
    /* Test images with exif orientation transverse */
    create_image_unit("A:src/test_assets/test_img_lvgl_logo_with_exif_orientation_transverse.jpg", 300, 70,
                      "orientation transverse", 300, 150);
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

void test_jpg_sign_error(void)
{
    /* Temporarily remove tjpgd decoder */
    lv_tjpgd_deinit();

    lv_obj_clean(lv_screen_active());
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:src/test_assets/test_img_lvgl_logo_with_sign_error.jpg");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_sign_error.png");

    /* Re-add tjpgd decoder */
    lv_tjpgd_init();
}

void test_jpg_decode_failed(void)
{
    /* Temporarily remove tjpgd decoder */
    lv_tjpgd_deinit();

    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_img_lvgl_logo_with_decode_failed.jpg";

    /* Try to decode the image */
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);

    /* Should fail when decoder is removed */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

    /* Re-add tjpgd decoder */
    lv_tjpgd_init();
}

#endif
