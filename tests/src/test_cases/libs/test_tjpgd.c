#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"


void setUp(void)
{
    /* Function run before every test */
    /* Temporarily remove other JPEG decoders to make sure tjpegd is used */
    lv_libjpeg_turbo_deinit();
#if LV_USE_FFMPEG
    lv_ffmpeg_deinit();
#endif
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

static lv_color_t get_first_pixel(const lv_draw_buf_t * decoded)
{
    const uint8_t * px = decoded->data;
    if(decoded->header.cf == LV_COLOR_FORMAT_RGB565) {
        uint16_t c16;
        lv_memcpy(&c16, px, sizeof(c16));
        return lv_color_make((uint8_t)((c16 >> 11) << 3),
                             (uint8_t)(((c16 >> 5) & 0x3F) << 2),
                             (uint8_t)((c16 & 0x1F) << 3));
    }

    /* RGB888 is stored in B, G, R byte order */
    return lv_color_make(px[2], px[1], px[0]);
}

void test_tjpgd_color_order(void)
{
    /* Temporarily remove libjpeg_turbo decoder */
    lv_libjpeg_turbo_deinit();

    /* 16x8 pixels, 4:4:4 sampling: the left 8x8 MCU is pure red, the right one pure blue */
    lv_image_decoder_dsc_t dsc;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&dsc, "A:src/test_assets/test_img_jpg_red_blue.jpg", NULL));
    TEST_ASSERT_EQUAL_STRING("TJPGD", dsc.decoder->name);

#if LV_TJPGD_OUTPUT_RGB565
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_RGB565, dsc.header.cf);
    TEST_ASSERT_EQUAL_UINT32(16 * 2, dsc.header.stride);
#else
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_RGB888, dsc.header.cf);
    TEST_ASSERT_EQUAL_UINT32(16 * 3, dsc.header.stride);
#endif

    const lv_color_t expected[2] = {lv_color_make(0xFF, 0x00, 0x00), lv_color_make(0x00, 0x00, 0xFF)};
    lv_area_t full_area = {0, 0, dsc.header.w - 1, dsc.header.h - 1};
    lv_area_t decoded_area;
    lv_area_set(&decoded_area, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MIN);

    for(uint32_t i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_get_area(&dsc, &full_area, &decoded_area));
        TEST_ASSERT_EQUAL_INT32(i * 8, decoded_area.x1);
        TEST_ASSERT_EQUAL(dsc.header.cf, dsc.decoded->header.cf);

        lv_color_t c = get_first_pixel(dsc.decoded);
        TEST_ASSERT_UINT8_WITHIN(8, expected[i].red, c.red);
        TEST_ASSERT_UINT8_WITHIN(8, expected[i].green, c.green);
        TEST_ASSERT_UINT8_WITHIN(8, expected[i].blue, c.blue);
    }

    lv_image_decoder_close(&dsc);

    /* Re-add libjpeg_turbo decoder */
    lv_libjpeg_turbo_init();
}

#endif
