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
    lv_obj_clean(lv_screen_active());
}

#if LV_USE_QOI

void test_qoi_decoder_registration(void)
{
    /* Verify QOI decoder is available and registered */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_sample.qoi";
    lv_image_cache_drop(image_path);

    /* Try to get decoder info - this validates decoder registration */
    lv_image_header_t header;
    lv_result_t res = lv_image_decoder_get_info(image_path, &header);

    /* Should succeed if decoder is registered */
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    /* Try to open the image */
    res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_NOT_NULL(decoder_dsc.decoded);

    /* Clean up */
    lv_image_decoder_close(&decoder_dsc);
}

void test_qoi_get_info(void)
{
    /* Test image information retrieval */
    lv_image_header_t header;
    const char * image_path = "A:src/test_assets/test_qoi_sample.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_get_info(image_path, &header);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    /* Verify expected dimensions (100x100 RGB sample image) */
    TEST_ASSERT_EQUAL(100, header.w);
    TEST_ASSERT_EQUAL(100, header.h);

    /* Verify color format is set correctly */
    TEST_ASSERT_NOT_EQUAL(LV_COLOR_FORMAT_UNKNOWN, header.cf);
}

void test_qoi_valid_decode(void)
{
    /* Test valid QOI image decode */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_sample.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_NOT_NULL(decoder_dsc.decoded);
    TEST_ASSERT_EQUAL(100, decoder_dsc.header.w);
    TEST_ASSERT_EQUAL(100, decoder_dsc.header.h);

    lv_image_decoder_close(&decoder_dsc);
}

void test_qoi_rendering(void)
{
    /* Test QOI image rendering with screenshot comparison */
    lv_obj_clean(lv_screen_active());

    lv_image_cache_drop("A:src/test_assets/test_qoi_sample.qoi");

    lv_obj_t * img = lv_image_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(img);

    lv_image_set_src(img, "A:src/test_assets/test_qoi_sample.qoi");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qoi_sample.png");
}

void test_qoi_alpha_channel(void)
{
    /* Test QOI image with alpha channel */
    lv_obj_clean(lv_screen_active());

    /* Create a background to verify alpha blending */
    lv_obj_t * bg = lv_obj_create(lv_screen_active());
    lv_obj_set_size(bg, 120, 120);
    lv_obj_center(bg);
    lv_obj_set_style_bg_color(bg, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);

    lv_image_cache_drop("A:src/test_assets/test_qoi_alpha.qoi");

    /* Create image with alpha */
    lv_obj_t * img = lv_image_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(img);

    lv_image_set_src(img, "A:src/test_assets/test_qoi_alpha.qoi");
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qoi_alpha.png");
}

void test_qoi_small_image(void)
{
    /* Test 1x1 pixel QOI image */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_1x1.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL(1, decoder_dsc.header.w);
    TEST_ASSERT_EQUAL(1, decoder_dsc.header.h);
    TEST_ASSERT_NOT_NULL(decoder_dsc.decoded);

    lv_image_decoder_close(&decoder_dsc);

    /* Also test rendering by checking the pixel color directly. */
    /* Single-buffered display: active buffer holds the fresh frame. */
    /* Pixel readback needs 32-bit XRGB8888/ARGB8888 layout. */
#if LV_COLOR_DEPTH == 32
    lv_obj_clean(lv_screen_active());

    lv_obj_t * img = lv_image_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(img);

    lv_image_set_src(img, image_path);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);

    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(NULL);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Verify buffer format is 32-bit so pixel reads are valid. */
    lv_color_format_t cf = draw_buf->header.cf;
    TEST_ASSERT_TRUE(cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_ARGB8888);

    /* The 1x1 image is centered on the 800x480 test display */
    int32_t cx = lv_display_get_horizontal_resolution(NULL) / 2;
    int32_t cy = lv_display_get_vertical_resolution(NULL) / 2;

    /* Use bounds-checked lv_draw_buf_goto_xy() instead of raw pointer math. */
    lv_color32_t * px_center = (lv_color32_t *)lv_draw_buf_goto_xy(draw_buf, (uint32_t)cx, (uint32_t)cy);
    lv_color32_t * px_left   = (lv_color32_t *)lv_draw_buf_goto_xy(draw_buf, (uint32_t)(cx - 1), (uint32_t)cy);
    lv_color32_t * px_right  = (lv_color32_t *)lv_draw_buf_goto_xy(draw_buf, (uint32_t)(cx + 1), (uint32_t)cy);
    lv_color32_t * px_up     = (lv_color32_t *)lv_draw_buf_goto_xy(draw_buf, (uint32_t)cx, (uint32_t)(cy - 1));
    lv_color32_t * px_down   = (lv_color32_t *)lv_draw_buf_goto_xy(draw_buf, (uint32_t)cx, (uint32_t)(cy + 1));

    TEST_ASSERT_NOT_NULL(px_center);
    TEST_ASSERT_NOT_NULL(px_left);
    TEST_ASSERT_NOT_NULL(px_right);
    TEST_ASSERT_NOT_NULL(px_up);
    TEST_ASSERT_NOT_NULL(px_down);

    /* 1x1 QOI image is red; allow renderer color-conversion tolerance. */
    TEST_ASSERT_UINT8_WITHIN(32, 0xFF, px_center->red);
    TEST_ASSERT_UINT8_WITHIN(32, 0x00, px_center->green);
    TEST_ASSERT_UINT8_WITHIN(32, 0x00, px_center->blue);

    /* Neighbors are background: green/blue > 100 distinguishes from red image. */
    TEST_ASSERT_TRUE(px_left->green > 100);
    TEST_ASSERT_TRUE(px_left->blue > 100);

    TEST_ASSERT_TRUE(px_right->green > 100);
    TEST_ASSERT_TRUE(px_right->blue > 100);

    TEST_ASSERT_TRUE(px_up->green > 100);
    TEST_ASSERT_TRUE(px_up->blue > 100);

    TEST_ASSERT_TRUE(px_down->green > 100);
    TEST_ASSERT_TRUE(px_down->blue > 100);
#else
    TEST_IGNORE_MESSAGE("Pixel readback requires LV_COLOR_DEPTH 32");
#endif /* LV_COLOR_DEPTH == 32 */
}

void test_qoi_large_image(void)
{
    /* Test large QOI image (512x512) */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_large.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL(512, decoder_dsc.header.w);
    TEST_ASSERT_EQUAL(512, decoder_dsc.header.h);
    TEST_ASSERT_NOT_NULL(decoder_dsc.decoded);

    lv_image_decoder_close(&decoder_dsc);

    /* Also test rendering */
    lv_obj_clean(lv_screen_active());

    lv_obj_t * img = lv_image_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(img);

    lv_image_set_src(img, image_path);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qoi_large.png");
}

void test_qoi_invalid_header(void)
{
    /* Test QOI file with invalid header */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_invalid_header.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);

    /* Should fail with invalid header */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
    TEST_ASSERT_NULL(decoder_dsc.decoded);
}

void test_qoi_corrupted_data(void)
{
    /* Test QOI file with corrupted data */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_corrupted.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);

    /* Should fail gracefully without crash */
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);
    TEST_ASSERT_NULL(decoder_dsc.decoded);
}

void test_qoi_repeated_open_close(void)
{
    /* Test repeated open/close to detect resource leaks */
    const char * image_path = "A:src/test_assets/test_qoi_sample.qoi";
    lv_image_cache_drop(NULL);
    lv_image_cache_drop(image_path);
    size_t mem_before = lv_test_get_free_mem();

    for(uint32_t i = 0; i < 100; i++) {
        lv_image_decoder_dsc_t decoder_dsc;

        lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

        lv_image_decoder_close(&decoder_dsc);
    }

    /* Drop image cache to free the retained decoded frame before measuring */
    lv_image_cache_drop(NULL);

    /* Check for memory leaks */
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 128);
}

void test_qoi_memory_leak_rendering(void)
{
    /* Test memory leak during repeated rendering */
    lv_image_cache_drop(NULL);
    lv_image_cache_drop("A:src/test_assets/test_qoi_sample.qoi");
    size_t mem_before = lv_test_get_free_mem();

    for(uint32_t i = 0; i < 40; i++) {
        lv_obj_clean(lv_screen_active());

        lv_obj_t * img = lv_image_create(lv_screen_active());
        lv_image_set_src(img, "A:src/test_assets/test_qoi_sample.qoi");
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qoi_sample.png");

    /* Clean up the screen objects before measuring memory */
    lv_obj_clean(lv_screen_active());

    /* Drop image cache to free the retained decoded frame before measuring */
    lv_image_cache_drop(NULL);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 128);
}

void test_qoi_multiple_formats(void)
{
    /* Test QOI images with different channel configurations */
    lv_obj_clean(lv_screen_active());

    lv_image_cache_drop("A:src/test_assets/test_qoi_rgb.qoi");
    lv_image_cache_drop("A:src/test_assets/test_qoi_rgba.qoi");

    /* RGB image */
    lv_obj_t * img_rgb = lv_image_create(lv_screen_active());
    lv_image_set_src(img_rgb, "A:src/test_assets/test_qoi_rgb.qoi");
    lv_obj_align(img_rgb, LV_ALIGN_CENTER, -80, 0);

    /* RGBA image */
    lv_obj_t * img_rgba = lv_image_create(lv_screen_active());
    lv_image_set_src(img_rgba, "A:src/test_assets/test_qoi_rgba.qoi");
    lv_obj_align(img_rgba, LV_ALIGN_CENTER, 80, 0);

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/qoi_formats.png");
}

void test_qoi_colorspace_linear(void)
{
    /* Test QOI with linear colorspace */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_linear.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_NOT_NULL(decoder_dsc.decoded);
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_ARGB8888, decoder_dsc.header.cf);

    lv_image_decoder_close(&decoder_dsc);
}

void test_qoi_colorspace_srgb(void)
{
    /* Test QOI with sRGB colorspace */
    lv_image_decoder_dsc_t decoder_dsc;
    const char * image_path = "A:src/test_assets/test_qoi_srgb.qoi";
    lv_image_cache_drop(image_path);

    lv_result_t res = lv_image_decoder_open(&decoder_dsc, image_path, NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_NOT_NULL(decoder_dsc.decoded);
    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_ARGB8888, decoder_dsc.header.cf);

    lv_image_decoder_close(&decoder_dsc);
}

#else /* LV_USE_QOI */

void test_qoi_decoder_registration(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_get_info(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_valid_decode(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_rendering(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_alpha_channel(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_small_image(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_large_image(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_invalid_header(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_corrupted_data(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_repeated_open_close(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_memory_leak_rendering(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_multiple_formats(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_colorspace_linear(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

void test_qoi_colorspace_srgb(void)
{
    TEST_PASS_MESSAGE("QOI decoder not enabled (LV_USE_QOI == 0)");
}

#endif /* LV_USE_QOI */

#endif /* LV_BUILD_TEST */
