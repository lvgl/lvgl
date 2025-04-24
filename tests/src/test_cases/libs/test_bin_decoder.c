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
    lv_image_cache_drop(src);
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

static lv_image_dsc_t * get_image_dsc(void)
{
#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 2
    static LV_ATTRIBUTE_MEM_ALIGN uint8_t image_map[IMAGE_WIDTH * IMAGE_HEIGHT * sizeof(lv_color32_t)] = { 0 };
    static lv_image_dsc_t image_dsc = { 0 };

    image_dsc.header.magic = LV_IMAGE_HEADER_MAGIC,
    image_dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    image_dsc.header.w = IMAGE_WIDTH;
    image_dsc.header.h = IMAGE_HEIGHT;
    image_dsc.header.stride = IMAGE_WIDTH * sizeof(lv_color32_t);
    image_dsc.data_size = sizeof(image_map);
    image_dsc.data = image_map;

    return &image_dsc;
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
void test_bin_decoder_image_dsc_error_handling(void)
{
    lv_image_dsc_t * image_dsc = get_image_dsc();

    /* Valid image */
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid magic */
    image_dsc = get_image_dsc();
    image_dsc->header.magic = 0;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid NULL data */
    image_dsc = get_image_dsc();
    image_dsc->data = NULL;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid data_size */
    image_dsc = get_image_dsc();
    image_dsc->data_size = 0;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid stride */
    image_dsc = get_image_dsc();
    image_dsc->header.stride = 0;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid color format */
    image_dsc = get_image_dsc();
    image_dsc->header.cf = LV_COLOR_FORMAT_UNKNOWN;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid image size */
    image_dsc = get_image_dsc();
    image_dsc->header.w++;
    image_dsc->header.h++;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid unaligned data */
    image_dsc = get_image_dsc();
    image_dsc->data = image_dsc->data + 1;
    image_dsc->header.h = 1;
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test invalid flags */
    image_dsc = get_image_dsc();
    image_dsc->header.flags = (LV_IMAGE_FLAGS_ALLOCATED | LV_IMAGE_FLAGS_PREMULTIPLIED);
    bin_decoder(image_dsc, "libs/bin_decoder_empty_image.png");

    /* Test NULL image */
    bin_decoder(NULL, "libs/bin_decoder_empty_image.png");
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
