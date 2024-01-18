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

void test_draw_buf_stride_adjust(void)
{
#if LV_BIN_DECODER_RAM_LOAD == 1
    const char * img_src = "A:test_images/stride_align64/UNCOMPRESSED/test_ARGB8888.bin";
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_obj_center(img);
    lv_image_set_src(img, img_src);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/temp.o"); /*Generate the reference image, use .o so git ignore it*/

    lv_image_decoder_args_t args = {
        .no_cache = false,
        .premultiply = false,
        .stride_align = false,
        .use_indexed = false,
    };

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, img_src, &args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    const lv_image_header_t header = decoder_dsc.decoded->header;
    /*The test image must have aligned stride different with width * bpp*/
    TEST_ASSERT_NOT_EQUAL(header.w * 4, header.stride);

    lv_draw_buf_t * dup = lv_draw_buf_dup(decoder_dsc.decoded);
    TEST_ASSERT_NOT_NULL(dup);

    /*Close the decoder since we copied out the decoded draw buffer*/
    lv_image_decoder_close(&decoder_dsc);

    /* Shrink stride to below minimal stride(by -1 in code below) should fail */
    res = lv_draw_buf_adjust_stride(dup, header.w * 4 - 1);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

    res = lv_draw_buf_adjust_stride(dup, header.stride + 1);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

    /*Expand the stride should fail if stride is too large that buffer size overflow*/
    res = lv_draw_buf_adjust_stride(dup, header.stride + 1);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

    /* Expand the stride should work, use a proper stride value should succeed*/
    res = lv_draw_buf_adjust_stride(dup, (header.stride + header.w * 4) / 2);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    lv_image_set_src(img, dup);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/temp.o"); /*The image should still looks same*/

    /* Shrink stride to minimal stride should succeed */
    res = lv_draw_buf_adjust_stride(dup, header.w * 4);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    lv_image_set_src(img, dup);
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/temp.o"); /*Test against with above reference image*/

    lv_draw_buf_destroy(dup);
#endif
}

#endif
