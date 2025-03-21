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

static void create_image(const void * src)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
}

static void bin_decoder(const void * src, const char * screenshot)
{
    create_image(src);
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image(src);

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
}

static void create_image_tile(const void * src)
{
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
    lv_obj_set_size(img, 275, 175);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
}

void bin_decoder_tile(const void * src, const char * screenshot)
{
    create_image_tile(src);
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());

    size_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        lv_obj_clean(lv_screen_active());
        create_image_tile(src);

        lv_obj_invalidate(lv_screen_active());
        lv_refr_now(NULL);
    }
    TEST_ASSERT_EQUAL_SCREENSHOT(screenshot);
    lv_obj_clean(lv_screen_active());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
}

void test_bin_decoder_i4(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    bin_decoder(&test_image_cogwheel_i4, "libs/bin_decoder_1.png");
}
void test_bin_decoder_i4_tile(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    bin_decoder_tile(&test_image_cogwheel_i4, "libs/bin_decoder_2.png");
}
void test_bin_decoder_argb8888(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    bin_decoder(&test_image_cogwheel_argb8888, "libs/bin_decoder_3.png");
}
void test_bin_decoder_argb8888_tile(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_argb8888);
    bin_decoder_tile(&test_image_cogwheel_argb8888, "libs/bin_decoder_4.png");
}

void test_bin_decoder_flush_cache(void)
{
#if LV_BIN_DECODER_RAM_LOAD == 1
    LV_IMAGE_DECLARE(test_I1_NONE_align64);
    LV_IMAGE_DECLARE(test_I2_NONE_align64);
    LV_IMAGE_DECLARE(test_I4_NONE_align64);
    LV_IMAGE_DECLARE(test_I8_NONE_align64);
    LV_IMAGE_DECLARE(test_A1_NONE_align64);
    LV_IMAGE_DECLARE(test_A2_NONE_align64);
    LV_IMAGE_DECLARE(test_A4_NONE_align64);
    LV_IMAGE_DECLARE(test_A8_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB565A8_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB565_NONE_align64);
    LV_IMAGE_DECLARE(test_RGB888_NONE_align64);
    LV_IMAGE_DECLARE(test_XRGB8888_NONE_align64);
    LV_IMAGE_DECLARE(test_ARGB8888_NONE_align64);

    const lv_image_dsc_t * img_dscs[] = {
        &test_I1_NONE_align64,
        &test_I2_NONE_align64,
        &test_I4_NONE_align64,
        &test_I8_NONE_align64,
        &test_A1_NONE_align64,
        &test_A2_NONE_align64,
        &test_A4_NONE_align64,
        &test_A8_NONE_align64,
        &test_RGB565A8_NONE_align64,
        &test_RGB565_NONE_align64,
        &test_RGB888_NONE_align64,
        &test_XRGB8888_NONE_align64,
        &test_ARGB8888_NONE_align64,
    };
    const lv_image_decoder_args_t args = {
        .no_cache = true,
        .premultiply = false,
        .stride_align = false,
        .use_indexed = true,
        .flush_cache = true,
    };

    for(unsigned long i = 0; i < sizeof(img_dscs) / sizeof(img_dscs[0]); i++) {
        lv_image_decoder_dsc_t decoder_dsc;
        lv_result_t res = lv_image_decoder_open(&decoder_dsc, img_dscs[i], &args);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_image_decoder_close(&decoder_dsc);
    }
#endif
}

#endif
