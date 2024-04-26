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

void test_draw_buf_stride_adjust(void)
{
#if LV_BIN_DECODER_RAM_LOAD == 1
    const char * color_formats[] = {
        "I1",
        "I2",
        "I4",
        "I8",
#if 0   /* Decoder will convert them to A8 anyway.*/
        "A1",
        "A2",
        "A4",
#endif
        "A8",
        "RGB565",
#if 0   /* RGB565 with alpha is not supported*/
        "RGB565A8",
        "ARGB8565",
#endif
        "RGB888",
        "ARGB8888",
        "XRGB8888",
    };

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_obj_center(img);

    const lv_image_decoder_args_t args = {
        .no_cache = true,
        .premultiply = false,
        .stride_align = false,
        .use_indexed = true,
    };

    for(unsigned long i = 0; i < sizeof(color_formats) / sizeof(color_formats[0]); i++) {
        char img_src[256];
        char ref_image[256];
        snprintf(img_src, sizeof(img_src), "A:test_images/stride_align1/UNCOMPRESSED/test_%s.bin", color_formats[i]);
        snprintf(ref_image, sizeof(ref_image), "draw/temp_%s.o", color_formats[i]); /*Use .o file name so git ignores it.*/

        lv_image_set_src(img, img_src);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*Generate the reference image, use .o so git ignore it*/

        lv_image_cache_drop(img_src); /* Image could be added to cache during lv_image_set_src*/

        lv_image_decoder_dsc_t decoder_dsc;
        lv_result_t res = lv_image_decoder_open(&decoder_dsc, img_src, &args);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_draw_buf_t * decoded = lv_draw_buf_dup(decoder_dsc.decoded);
        TEST_ASSERT_NOT_NULL(decoded);

        const lv_image_header_t header = decoder_dsc.decoded->header;
        uint32_t image_width = header.w;
        uint32_t image_height = header.h;
        uint32_t image_stride = header.stride;
        uint32_t min_stride = (image_width * lv_color_format_get_bpp(header.cf) + 7) >> 3;

        /*Close the decoder since we copied out the decoded draw buffer*/
        lv_image_decoder_close(&decoder_dsc);

        /* Shrink stride to below minimal stride(by -1 in code below) should fail */
        res = lv_draw_buf_adjust_stride(decoded, min_stride - 1);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

        /*Expand the stride should fail if stride is too large that buffer size overflow*/
        res = lv_draw_buf_adjust_stride(decoded, image_stride + 1);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res);

        /*Create a larger draw buffer*/
        lv_draw_buf_t * larger = lv_draw_buf_create(image_width, image_height, header.cf, min_stride + 100);

        /*Copy draw buffer, it should look same.*/
        lv_draw_buf_copy(larger, NULL, decoded, NULL);
        lv_image_cache_drop(larger);
        lv_image_set_src(img, larger);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*The image should still looks same*/

        /* Shrink stride to minimal stride should succeed */
        res = lv_draw_buf_adjust_stride(larger, min_stride);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_image_cache_drop(larger);
        lv_image_set_src(img, larger);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*Test against with above reference image*/

        /* Expand the stride should work, use a proper stride value should succeed*/
        res = lv_draw_buf_adjust_stride(larger, min_stride + 20);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        lv_image_cache_drop(larger);
        lv_image_set_src(img, larger);
        TEST_ASSERT_EQUAL_SCREENSHOT(ref_image); /*The image should still look same*/

        lv_draw_buf_destroy(larger);
        lv_draw_buf_destroy(decoded);
    }

    lv_obj_delete(img);
#endif
}

#endif
