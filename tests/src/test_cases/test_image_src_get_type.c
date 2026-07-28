#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_image_src_get_type_null(void)
{
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_UNKNOWN, lv_image_src_get_type(NULL));
}

void test_image_src_get_type_file(void)
{
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_FILE, lv_image_src_get_type("A:test.png"));
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_FILE, lv_image_src_get_type("/path/to/image.bin"));
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_FILE, lv_image_src_get_type("S:logo.bmp"));
}

void test_image_src_get_type_symbol(void)
{
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_SYMBOL, lv_image_src_get_type(LV_SYMBOL_OK));
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_SYMBOL, lv_image_src_get_type(LV_SYMBOL_CLOSE));
}

void test_image_src_get_type_valid_draw_buf(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_VARIABLE, lv_image_src_get_type(buf));
    lv_draw_buf_destroy(buf);
}

void test_image_src_get_type_legacy_no_magic(void)
{
    /*Legacy image resources with LV_IMAGE_HEADER_LEGACY should still be accepted*/
    lv_image_dsc_t fake_dsc;
    lv_memzero(&fake_dsc, sizeof(fake_dsc));
    fake_dsc.header.magic = LV_IMAGE_HEADER_LEGACY;
    fake_dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    fake_dsc.header.w = 10;
    fake_dsc.header.h = 10;
    fake_dsc.data_size = 10 * 10 * 4;
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_VARIABLE, lv_image_src_get_type(&fake_dsc));
}

void test_image_src_get_type_deadbeef_magic(void)
{
    lv_image_dsc_t fake_dsc;
    lv_memzero(&fake_dsc, sizeof(fake_dsc));
    fake_dsc.header.magic = LV_IMAGE_HEADER_DEADBEEF;
    fake_dsc.header.cf = LV_COLOR_FORMAT_ARGB8888;
    fake_dsc.header.w = 10;
    fake_dsc.header.h = 10;
    fake_dsc.data_size = 10 * 10 * 4;
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_UNKNOWN, lv_image_src_get_type(&fake_dsc));
}

void test_image_src_get_type_destroyed_draw_buf(void)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_VARIABLE, lv_image_src_get_type(buf));

    /*Simulate the poisoned state*/
    lv_draw_buf_t buf_copy = *buf;
    lv_draw_buf_destroy(buf);

    buf_copy.header.magic = LV_IMAGE_HEADER_DEADBEEF;
    TEST_ASSERT_EQUAL(LV_IMAGE_SRC_UNKNOWN, lv_image_src_get_type(&buf_copy));
}

#endif
