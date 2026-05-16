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

        /* Expand the stride should fail if the new stride is so large that the buffer
         * would overflow. Use a stride that's guaranteed to push the required size past
         * the aligned allocation (lv_draw_buf_create rounds the allocation up to
         * LV_DRAW_BUF_ALIGN for PPA/cache-line reasons, so a +1 bump can fit in the
         * padding for small images). Doubling the stride always overflows. */
        res = lv_draw_buf_adjust_stride(decoded, image_stride * 2);
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

/* Verify that the draw buffer allocation size reported in `data_size`
 * is rounded up to a multiple of LV_DRAW_BUF_ALIGN, as required by back-ends
 * (e.g. the ESP32 PPA) that need the allocation length to match the cache-line
 * boundary rather than the raw image content size. */
void test_draw_buf_size_alignment(void)
{
    /* A set of (w, h, cf, stride) cases chosen so that the raw image size
     * (stride*h [+ alpha/palette]) is deliberately not a multiple of
     * LV_DRAW_BUF_ALIGN, so the round-up behavior is observable. */
    struct {
        uint32_t w;
        uint32_t h;
        lv_color_format_t cf;
        uint32_t stride; /* 0 => let LVGL choose via lv_draw_buf_width_to_stride */
    } cases[] = {
        /* Small indexed: stride=1, h=1, palette=8 => raw=9 (not a multiple of ALIGN) */
        { 5, 1, LV_COLOR_FORMAT_I1, 1 },
        /* A8 with odd raw size: 1*3 = 3 */
        { 1, 3, LV_COLOR_FORMAT_A8, 1 },
        /* RGB565 with an intentionally non-aligned raw size: 6*1 = 6 */
        { 3, 1, LV_COLOR_FORMAT_RGB565, 6 },
        /* RGB888 with raw = 9*1 = 9 */
        { 3, 1, LV_COLOR_FORMAT_RGB888, 9 },
        /* RGB565A8: rgb=4*1=4, a8=(4/2)*1=2 => raw=6 */
        { 2, 1, LV_COLOR_FORMAT_RGB565A8, 4 },
        /* Larger ARGB8888 with raw already aligned: 10*10*4 = 400 (rounding should be a no-op) */
        { 10, 10, LV_COLOR_FORMAT_ARGB8888, 40 },
        /* Stride auto-computed (0) path */
        { 7, 5, LV_COLOR_FORMAT_RGB565, 0 },
    };

    for(unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        uint32_t w = cases[i].w;
        uint32_t h = cases[i].h;
        lv_color_format_t cf = cases[i].cf;
        uint32_t stride = cases[i].stride;

        lv_draw_buf_t * buf = lv_draw_buf_create(w, h, cf, stride);
        TEST_ASSERT_NOT_NULL(buf);

        /* Recompute the stride the same way `_calculate_draw_buf_size` would
         * if the caller passed 0, so we can derive the expected raw size. */
        uint32_t effective_stride = stride ? stride : lv_draw_buf_width_to_stride(w, cf);
        uint32_t raw_size = effective_stride * h;
        if(cf == LV_COLOR_FORMAT_RGB565A8) {
            raw_size += (effective_stride / 2) * h;
        }
        else if(LV_COLOR_FORMAT_IS_INDEXED(cf)) {
            raw_size += LV_COLOR_INDEXED_PALETTE_SIZE(cf) * 4;
        }

        uint32_t expected = LV_ROUND_UP(raw_size, LV_DRAW_BUF_ALIGN);

        /* data_size must be rounded up to LV_DRAW_BUF_ALIGN ... */
        TEST_ASSERT_EQUAL_UINT32(0, buf->data_size % LV_DRAW_BUF_ALIGN);
        /* ... must cover at least the raw image content size ... */
        TEST_ASSERT_TRUE(buf->data_size >= raw_size);
        /* ... and must match the explicit LV_ROUND_UP value. */
        TEST_ASSERT_EQUAL_UINT32(expected, buf->data_size);

        lv_draw_buf_destroy(buf);
    }
}

void test_draw_buf_xy_access(void)
{
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

    uint8_t * ret = lv_draw_buf_goto_xy(&draw_buf, 50, 50);
    TEST_ASSERT_NOT_NULL(ret);
    ret = lv_draw_buf_goto_xy(&draw_buf, 100, 100);
    TEST_ASSERT_NULL(ret);
    ret = lv_draw_buf_goto_xy(&draw_buf, -10, -10);
    TEST_ASSERT_NULL(ret);
}


void test_draw_buf_premultiply(void)
{
    lv_draw_buf_t * draw_buf;
    lv_result_t res;
    int i;
    /* Test ARGB8888 format */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_ARGB8888, 2 * 4);

        /* Fill with test data: white with 50% alpha */
        lv_color32_t * pixel = (lv_color32_t *)draw_buf->data;
        for(i = 0; i < 4; i++) {
            pixel[i] = (lv_color32_t) {
                .alpha = 128, .red = 255, .green = 255, .blue = 255
            };
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Verify premultiplied result: color should be 127 = (255 * 128) >> 8, alpha should be 128 */
        for(i = 0; i < 4; i++) {
            TEST_ASSERT_EQUAL_UINT8(127, pixel[i].red);
            TEST_ASSERT_EQUAL_UINT8(127, pixel[i].green);
            TEST_ASSERT_EQUAL_UINT8(127, pixel[i].blue);
            TEST_ASSERT_EQUAL_UINT8(128, pixel[i].alpha); /* Alpha should remain unchanged */
        }

        lv_draw_buf_destroy(draw_buf);
    }

    /* Test XRGB8888 format */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_XRGB8888, 2 * 4);

        /* Fill with test data: white with 50% alpha */
        lv_color32_t * pixel = (lv_color32_t *)draw_buf->data;
        for(i = 0; i < 4; i++) {
            pixel[i] = (lv_color32_t) {
                .alpha = 255, .red = 255, .green = 255, .blue = 255
            };
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, res); /* XRGB8888 is not supported */

        lv_draw_buf_destroy(draw_buf);
    }

    /* Test RGB565A8 format */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_RGB565A8, 2 * 2);

        /* Fill with test data */
        uint16_t * pixel = (uint16_t *)draw_buf->data;
        lv_opa_t * alpha = (lv_opa_t *)(draw_buf->data + 2 * 2 * 2); /* RGB565 data size */
        for(i = 0; i < 4; i++) {
            pixel[i] = lv_color_to_u16(lv_color_white()); /* White: 0xFFFF in RGB565 */
            alpha[i] = 128; /* 50% alpha */
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Verify premultiplied result: white (0xFFFF) with 50% alpha should become gray */
        for(i = 0; i < 4; i++) {
            /* RGB565: 5-6-5 bits, white premultiplied by 0.5 should be approximately half intensity */
            TEST_ASSERT_EQUAL_UINT16(0x7BEF, pixel[i]); /* Approximate half of white in RGB565 */
        }

        lv_draw_buf_destroy(draw_buf);
    }

    /* Test ARGB8565 format */
    {
        draw_buf = lv_draw_buf_create(2, 2, LV_COLOR_FORMAT_ARGB8565, 2 * 3);

        /* Fill with test data */
        uint8_t * pixel = draw_buf->data;
        for(i = 0; i < 4; i++) {
            uint16_t c = lv_color_to_u16(lv_color_white()); /* White: 0xFFFF */
            pixel[0] = c & 0xFF;         /* Low byte of RGB565 */
            pixel[1] = (c >> 8) & 0xFF;  /* High byte of RGB565 */
            pixel[2] = 128;                   /* Alpha */
            pixel += 3;
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Verify premultiplied result */
        pixel = draw_buf->data;
        for(i = 0; i < 4; i++) {
            uint16_t c = (pixel[1] << 8) | pixel[0]; /* Reconstruct RGB565 */
            /* White premultiplied by 0.5 should be approximately half intensity */
            TEST_ASSERT_EQUAL_UINT16(0x7BEF, c); /* Approximate half of white in RGB565 */
            TEST_ASSERT_EQUAL_UINT8(128, pixel[2]); /* Alpha should remain unchanged */
            pixel += 3;
        }

        lv_draw_buf_destroy(draw_buf);
    }

    /* Test indexed format (I1/I2/I4/I8) */
    lv_color_format_t color_formats[] = {
        LV_COLOR_FORMAT_I1,
        LV_COLOR_FORMAT_I2,
        LV_COLOR_FORMAT_I4,
        LV_COLOR_FORMAT_I8,
    };

    for(unsigned int fmt_i = 0; fmt_i < sizeof(color_formats) / sizeof(color_formats[0]); fmt_i++) {
        draw_buf = lv_draw_buf_create(2, 2, color_formats[fmt_i], 0);

        /* Fill palette with test data */
        lv_color32_t * palette = (lv_color32_t *)draw_buf->data;
        int palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(color_formats[fmt_i]);
        for(i = 0; i < palette_size; i++) {
            palette[i] = (lv_color32_t) {
                .alpha = 128, .red = 255, .green = 255, .blue = 255
            };
        }

        res = lv_draw_buf_premultiply(draw_buf);
        TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
        TEST_ASSERT(draw_buf->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED);

        /* Verify palette was premultiplied */
        for(i = 0; i < palette_size; i++) {
            TEST_ASSERT_EQUAL_UINT8(127, palette[i].red);
            TEST_ASSERT_EQUAL_UINT8(127, palette[i].green);
            TEST_ASSERT_EQUAL_UINT8(127, palette[i].blue);
            TEST_ASSERT_EQUAL_UINT8(128, palette[i].alpha);
        }

        lv_draw_buf_destroy(draw_buf);
    }
}
#endif
