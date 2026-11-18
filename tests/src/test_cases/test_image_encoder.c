#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

#define TEST_ENCODER_PATH(path) "A:src/test_files/encoder_" path

static bool file_exists(const char * path)
{
    lv_fs_file_t f;
    if(lv_fs_open(&f, path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;
    lv_fs_close(&f);
    return true;
}

static lv_draw_buf_t * create_test_buf(uint32_t w, uint32_t h, lv_color_format_t cf)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(w, h, cf, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf);
    lv_memset(buf->data, 0xAB, buf->data_size);
    return buf;
}

static void fill_solid_argb8888(lv_draw_buf_t * buf, uint8_t b, uint8_t g, uint8_t r, uint8_t a)
{
    uint32_t stride = buf->header.stride;
    for(uint32_t y = 0; y < buf->header.h; y++) {
        uint8_t * row = buf->data + y * stride;
        for(uint32_t x = 0; x < buf->header.w; x++) {
            row[x * 4 + 0] = b;
            row[x * 4 + 1] = g;
            row[x * 4 + 2] = r;
            row[x * 4 + 3] = a;
        }
    }
}

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_image_encoder_save_invalid_draw_buf(void)
{
    lv_draw_buf_t draw_buf;
    /*lv_draw_buf_init() refuses a missing buffer, so fill the struct by hand*/
    lv_memzero(&draw_buf, sizeof(draw_buf));
    draw_buf.header.w = 10;
    draw_buf.header.h = 10;
    draw_buf.header.cf = LV_COLOR_FORMAT_ARGB8888;
    draw_buf.header.stride = 40;
    draw_buf.data_size = 100;

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(&draw_buf, TEST_ENCODER_PATH("null_data.bin")));
}

void test_image_encoder_save_null_args(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);

    lv_result_t res = lv_image_encoder_save(buf, TEST_ENCODER_PATH("null_args.bin"), NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_exists(TEST_ENCODER_PATH("null_args.bin")));

    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK,
                      lv_fs_open(&f, TEST_ENCODER_PATH("null_args.bin"), LV_FS_MODE_RD));
    lv_image_header_t header;
    uint32_t br;
    lv_fs_read(&f, &header, sizeof(header), &br);
    TEST_ASSERT_EQUAL_UINT32(sizeof(header), br);
    TEST_ASSERT_EQUAL_UINT16(4, header.w);
    TEST_ASSERT_EQUAL_UINT16(4, header.h);
    lv_fs_close(&f);

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_save_invalid_path(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, "Z:no_such_driver/test.bin"));
    lv_draw_buf_destroy(buf);
}

void test_image_encoder_save_invalid_header(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);
    uint32_t saved_w = buf->header.w;
    uint32_t saved_h = buf->header.h;
    uint32_t saved_stride = buf->header.stride;

    /* w = 0 */
    buf->header.w = 0;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("zero_w.bin")));
    buf->header.w = saved_w;

    /* h = 0 */
    buf->header.h = 0;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("zero_h.bin")));
    buf->header.h = saved_h;

    /* stride = 0 */
    buf->header.stride = 0;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("zero_stride.bin")));
    buf->header.stride = saved_stride;

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_save_zero_data_size(void)
{
    lv_draw_buf_t draw_buf;
    lv_draw_buf_init(&draw_buf, 0, 0, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO, NULL, 0);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(&draw_buf, TEST_ENCODER_PATH("zero_size.bin")));
}

void test_image_encoder_save_data_too_small(void)
{
    lv_draw_buf_t * buf = create_test_buf(8, 8, LV_COLOR_FORMAT_ARGB8888);

    /*Claim more rows than the data holds*/
    buf->header.h = buf->data_size / buf->header.stride + 1;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("small_data.bin")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("small_data.bin")));

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_save_stride_too_small(void)
{
    lv_draw_buf_t * buf = create_test_buf(8, 8, LV_COLOR_FORMAT_ARGB8888);

    /*A row needs 32 bytes*/
    buf->header.stride = 16;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("small_stride.bin")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("small_stride.bin")));

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_save_indexed_data_too_small(void)
{
    lv_draw_buf_t * buf = create_test_buf(8, 8, LV_COLOR_FORMAT_I8);

    /*Claim all the data is pixels, there is no room left for the palette*/
    buf->header.h = buf->data_size / buf->header.stride;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("small_indexed.bin")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("small_indexed.bin")));

    lv_draw_buf_destroy(buf);
}

static lv_result_t custom_encoder_save_cb(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                          const char * path, const lv_image_encoder_args_t * args)
{
    LV_UNUSED(draw_buf);
    LV_UNUSED(args);

    if(lv_strcmp(lv_fs_get_ext(path), "xyz") != 0) return LV_RESULT_INVALID;

    int32_t * counter = lv_image_encoder_get_user_data(encoder);
    (*counter)++;
    return LV_RESULT_OK;
}

void test_image_encoder_custom_encoder_user_data(void)
{
    int32_t counter = 0;

    lv_image_encoder_t * enc = lv_image_encoder_create();
    TEST_ASSERT_NOT_NULL(enc);
    TEST_ASSERT_NULL(lv_image_encoder_get_user_data(enc));
    lv_image_encoder_set_save_cb(enc, custom_encoder_save_cb);
    lv_image_encoder_set_user_data(enc, &counter);
    TEST_ASSERT_EQUAL_PTR(&counter, lv_image_encoder_get_user_data(enc));

    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("custom.xyz")));
    TEST_ASSERT_EQUAL_INT32(1, counter);

    /*Other extensions are not claimed by the custom encoder*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("custom.bin")));
    TEST_ASSERT_EQUAL_INT32(1, counter);

    lv_draw_buf_destroy(buf);
    lv_image_encoder_delete(enc);
}

void test_image_encoder_delete_null(void)
{
    lv_image_encoder_t * first = lv_image_encoder_get_next(NULL);

    lv_image_encoder_delete(NULL);

    TEST_ASSERT_EQUAL_PTR(first, lv_image_encoder_get_next(NULL));
}

void test_image_encoder_args_init(void)
{
    lv_image_encoder_args_t args;
    lv_image_encoder_args_init(&args);
    TEST_ASSERT_EQUAL_INT32(85, args.quality);
    TEST_ASSERT_EQUAL_INT32(6, args.compression_level);

    lv_draw_buf_t * buf = create_test_buf(8, 8, LV_COLOR_FORMAT_ARGB8888);

    lv_result_t res = lv_image_encoder_save(buf, TEST_ENCODER_PATH("encoder_args.bin"), &args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_exists(TEST_ENCODER_PATH("encoder_args.bin")));

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_bin(void)
{
    lv_draw_buf_t * buf = create_test_buf(10, 10, LV_COLOR_FORMAT_ARGB8888);

    lv_result_t res = lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("encoder_bin.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_exists(TEST_ENCODER_PATH("encoder_bin.bin")));

    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK,
                      lv_fs_open(&f, TEST_ENCODER_PATH("encoder_bin.bin"), LV_FS_MODE_RD));

    lv_image_header_t header;
    uint32_t br;
    lv_fs_read(&f, &header, sizeof(header), &br);
    TEST_ASSERT_EQUAL_UINT32(sizeof(header), br);
    TEST_ASSERT_EQUAL_UINT16(10, header.w);
    TEST_ASSERT_EQUAL_UINT16(10, header.h);
    TEST_ASSERT_EQUAL_UINT8(LV_COLOR_FORMAT_ARGB8888, header.cf);

    uint8_t * data = lv_malloc(buf->data_size);
    lv_fs_read(&f, data, buf->data_size, &br);
    TEST_ASSERT_EQUAL_UINT32(buf->data_size, br);
    TEST_ASSERT_EQUAL_UINT8(0xAB, data[0]);
    TEST_ASSERT_EQUAL_UINT8(0xAB, data[buf->data_size - 1]);

    lv_fs_close(&f);
    lv_free(data);
    lv_draw_buf_destroy(buf);
}

void test_image_encoder_bin_rejects_other_extensions(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("other.raw")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("other.raw")));

    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("noext")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("noext")));

    /*A failed PNG/JPEG encode must not end up as raw LVGL data under a PNG/JPEG name*/
    lv_draw_buf_destroy(buf);
    buf = create_test_buf(4, 4, LV_COLOR_FORMAT_A8);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("unsupported_cf.png")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("unsupported_cf.png")));
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("unsupported_cf.jpg")));
    TEST_ASSERT_FALSE(file_exists(TEST_ENCODER_PATH("unsupported_cf.jpg")));

    lv_draw_buf_destroy(buf);
}

/* Saving over a longer existing file must not leave a tail (the posix driver doesn't truncate by default) */
void test_image_encoder_bin_truncates_existing_file(void)
{
    const char * path = "B:src/test_files/encoder_trunc.bin";
    const uint32_t filler_len = 4096;

    uint8_t * filler = lv_malloc(filler_len);
    TEST_ASSERT_NOT_NULL(filler);
    lv_memset(filler, 0xCD, filler_len);

    lv_fs_file_t f;
    uint32_t bw = 0;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_open(&f, path, LV_FS_MODE_WR | LV_FS_MODE_TRUNC));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_write(&f, filler, filler_len, &bw));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_close(&f));
    lv_free(filler);

    lv_draw_buf_t * buf = create_test_buf(10, 10, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_draw_buf_save_to_file(buf, path));

    uint32_t size = 0;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_path_get_size(path, &size));
    TEST_ASSERT_EQUAL_UINT32(sizeof(lv_image_header_t) + buf->data_size, size);

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_bin_open_fail(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, "Z:invalid/path.bin"));
    lv_draw_buf_destroy(buf);
}

void test_image_encoder_bin_and_decode(void)
{
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(10, 10, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);
    fill_solid_argb8888(draw_buf, 0x00, 0x00, 0xFF, 0xFF);

    lv_result_t res = lv_draw_buf_save_to_file(draw_buf, TEST_ENCODER_PATH("decode_XRGB8888.bin"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_exists(TEST_ENCODER_PATH("decode_XRGB8888.bin")));

    lv_obj_t * image = lv_image_create(lv_screen_active());
    lv_image_set_src(image, TEST_ENCODER_PATH("decode_XRGB8888.bin"));
    lv_obj_center(image);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw_buf_save_to_file_decode_XRGB8888.png");

    lv_draw_buf_destroy(draw_buf);
}

#if LV_USE_LIBPNG_ENCODER

static bool file_starts_with_png_magic(const char * path)
{
    static const uint8_t png_magic[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    lv_fs_file_t f;
    if(lv_fs_open(&f, path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;
    uint8_t magic[8];
    uint32_t br;
    lv_fs_read(&f, magic, 8, &br);
    lv_fs_close(&f);
    if(br != 8) return false;
    return lv_memcmp(magic, png_magic, 8) == 0;
}

void test_image_encoder_png_argb8888(void)
{
    lv_draw_buf_t * buf = create_test_buf(16, 16, LV_COLOR_FORMAT_ARGB8888);
    fill_solid_argb8888(buf, 0x00, 0x00, 0xFF, 0xFF);

    lv_result_t res = lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("out.png"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_starts_with_png_magic(TEST_ENCODER_PATH("out.png")));

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.no_cache = true;

    res = lv_image_decoder_open(&dsc, TEST_ENCODER_PATH("out.png"), &dec_args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(16, dsc.decoded->header.w);

    const uint8_t * px = dsc.decoded->data;
    TEST_ASSERT_EQUAL_UINT8(0x00, px[0]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, px[2]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, px[3]);

    lv_image_decoder_close(&dsc);
    lv_draw_buf_destroy(buf);
}

void test_image_encoder_png_xrgb8888(void)
{
    lv_draw_buf_t * buf = create_test_buf(8, 8, LV_COLOR_FORMAT_XRGB8888);

    lv_result_t res = lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("xrgb.png"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_starts_with_png_magic(TEST_ENCODER_PATH("xrgb.png")));

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_png_xrgb8888_is_opaque(void)
{
    /*The X byte is unused, it must not be written as alpha*/
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_XRGB8888);
    fill_solid_argb8888(buf, 0x10, 0x20, 0x30, 0x00);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("xrgb_opaque.png")));

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.no_cache = true;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&dsc, TEST_ENCODER_PATH("xrgb_opaque.png"), &dec_args));
    const uint8_t * px = dsc.decoded->data;
    TEST_ASSERT_EQUAL_UINT8(0x10, px[0]);
    TEST_ASSERT_EQUAL_UINT8(0x20, px[1]);
    TEST_ASSERT_EQUAL_UINT8(0x30, px[2]);
    TEST_ASSERT_EQUAL_UINT8(0xFF, px[3]);
    lv_image_decoder_close(&dsc);

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_png_rgb565(void)
{
    lv_draw_buf_t * buf = create_test_buf(3, 2, LV_COLOR_FORMAT_RGB565);
    const uint16_t colors[3] = {0xF800, 0x07E0, 0x001F};
    for(uint32_t y = 0; y < 2; y++) {
        uint16_t * row = (uint16_t *)(void *)(buf->data + y * buf->header.stride);
        for(uint32_t x = 0; x < 3; x++) row[x] = colors[x];
    }

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("565.png")));
    TEST_ASSERT_TRUE(file_starts_with_png_magic(TEST_ENCODER_PATH("565.png")));

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.no_cache = true;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&dsc, TEST_ENCODER_PATH("565.png"), &dec_args));
    TEST_ASSERT_EQUAL_UINT16(3, dsc.decoded->header.w);
    TEST_ASSERT_EQUAL_UINT16(2, dsc.decoded->header.h);

    /*Decoded as B, G, R, A*/
    for(uint32_t y = 0; y < 2; y++) {
        const uint8_t * row = dsc.decoded->data + y * dsc.decoded->header.stride;
        TEST_ASSERT_EQUAL_UINT8(0x00, row[0]);
        TEST_ASSERT_EQUAL_UINT8(0x00, row[1]);
        TEST_ASSERT_EQUAL_UINT8(0xFF, row[2]);
        TEST_ASSERT_EQUAL_UINT8(0x00, row[4]);
        TEST_ASSERT_EQUAL_UINT8(0xFF, row[5]);
        TEST_ASSERT_EQUAL_UINT8(0x00, row[6]);
        TEST_ASSERT_EQUAL_UINT8(0xFF, row[8]);
        TEST_ASSERT_EQUAL_UINT8(0x00, row[9]);
        TEST_ASSERT_EQUAL_UINT8(0x00, row[10]);
    }
    lv_image_decoder_close(&dsc);

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_png_compression_level(void)
{
    lv_draw_buf_t * buf = create_test_buf(64, 64, LV_COLOR_FORMAT_ARGB8888);
    fill_solid_argb8888(buf, 0x10, 0x20, 0x30, 0xFF);

    lv_image_encoder_args_t args;
    lv_image_encoder_args_init(&args);

    args.compression_level = 0;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_encoder_save(buf, TEST_ENCODER_PATH("level0.png"), &args));
    args.compression_level = 9;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_encoder_save(buf, TEST_ENCODER_PATH("level9.png"), &args));
    /*Out of range values are clamped*/
    args.compression_level = 100;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_encoder_save(buf, TEST_ENCODER_PATH("level100.png"), &args));

    uint32_t size0 = 0;
    uint32_t size9 = 0;
    uint32_t size100 = 0;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_path_get_size(TEST_ENCODER_PATH("level0.png"), &size0));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_path_get_size(TEST_ENCODER_PATH("level9.png"), &size9));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_path_get_size(TEST_ENCODER_PATH("level100.png"), &size100));
    TEST_ASSERT_GREATER_THAN_UINT32(size9, size0);
    TEST_ASSERT_EQUAL_UINT32(size9, size100);

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_png_roundtrip(void)
{
    const uint32_t w = 4, h = 4;
    lv_draw_buf_t * buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf);

    uint32_t stride = buf->header.stride;
    for(uint32_t y = 0; y < h; y++) {
        uint8_t * row = buf->data + y * stride;
        for(uint32_t x = 0; x < w; x++) {
            row[x * 4 + 0] = (uint8_t)(x * 60);
            row[x * 4 + 1] = (uint8_t)(y * 60);
            row[x * 4 + 2] = (uint8_t)((x + y) * 30);
            row[x * 4 + 3] = 0xFF;
        }
    }

    lv_result_t res = lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("roundtrip.png"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.no_cache = true;

    res = lv_image_decoder_open(&dsc, TEST_ENCODER_PATH("roundtrip.png"), &dec_args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT16(w, dsc.decoded->header.w);
    TEST_ASSERT_EQUAL_UINT16(h, dsc.decoded->header.h);

    for(uint32_t y = 0; y < h; y++) {
        const uint8_t * orig_row = buf->data + y * stride;
        const uint8_t * dec_row = dsc.decoded->data + y * dsc.decoded->header.stride;
        for(uint32_t x = 0; x < w; x++) {
            TEST_ASSERT_EQUAL_UINT8(orig_row[x * 4 + 0], dec_row[x * 4 + 0]);
            TEST_ASSERT_EQUAL_UINT8(orig_row[x * 4 + 1], dec_row[x * 4 + 1]);
            TEST_ASSERT_EQUAL_UINT8(orig_row[x * 4 + 2], dec_row[x * 4 + 2]);
            TEST_ASSERT_EQUAL_UINT8(orig_row[x * 4 + 3], dec_row[x * 4 + 3]);
        }
    }

    lv_image_decoder_close(&dsc);
    lv_draw_buf_destroy(buf);
}

void test_image_encoder_png_open_fail(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, "Z:invalid/path.png"));
    lv_draw_buf_destroy(buf);
}

#endif /*LV_USE_LIBPNG_ENCODER*/

#if LV_USE_LIBJPEG_TURBO_ENCODER

static bool file_starts_with_jpeg_magic(const char * path)
{
    static const uint8_t jpeg_magic[] = {0xFF, 0xD8, 0xFF};
    lv_fs_file_t f;
    if(lv_fs_open(&f, path, LV_FS_MODE_RD) != LV_FS_RES_OK) return false;
    uint8_t magic[3];
    uint32_t br;
    lv_fs_read(&f, magic, 3, &br);
    lv_fs_close(&f);
    if(br != 3) return false;
    return lv_memcmp(magic, jpeg_magic, 3) == 0;
}

void test_image_encoder_jpeg_argb8888(void)
{
    lv_draw_buf_t * buf = create_test_buf(16, 16, LV_COLOR_FORMAT_ARGB8888);
    fill_solid_argb8888(buf, 0x00, 0x00, 0xFF, 0xFF);

    lv_result_t res = lv_draw_buf_save_to_file(buf, TEST_ENCODER_PATH("out.jpg"));
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_starts_with_jpeg_magic(TEST_ENCODER_PATH("out.jpg")));

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_jpeg_with_quality(void)
{
    lv_draw_buf_t * buf = create_test_buf(16, 16, LV_COLOR_FORMAT_ARGB8888);

    lv_image_encoder_args_t args;
    lv_image_encoder_args_init(&args);
    args.quality = 50;

    lv_result_t res = lv_image_encoder_save(buf, TEST_ENCODER_PATH("q50.jpg"), &args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_TRUE(file_starts_with_jpeg_magic(TEST_ENCODER_PATH("q50.jpg")));

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_jpeg_rgb565(void)
{
    lv_draw_buf_t * buf = create_test_buf(8, 8, LV_COLOR_FORMAT_RGB565);
    for(uint32_t y = 0; y < 8; y++) {
        uint16_t * row = (uint16_t *)(void *)(buf->data + y * buf->header.stride);
        for(uint32_t x = 0; x < 8; x++) row[x] = 0x8410; /*Gray*/
    }

    lv_image_encoder_args_t args;
    lv_image_encoder_args_init(&args);
    args.quality = 100;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_encoder_save(buf, TEST_ENCODER_PATH("565.jpg"), &args));
    TEST_ASSERT_TRUE(file_starts_with_jpeg_magic(TEST_ENCODER_PATH("565.jpg")));

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.no_cache = true;
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&dsc, TEST_ENCODER_PATH("565.jpg"), &dec_args));
    TEST_ASSERT_EQUAL_UINT16(8, dsc.decoded->header.w);
    TEST_ASSERT_EQUAL_UINT16(8, dsc.decoded->header.h);
    const uint8_t * px = dsc.decoded->data;
    TEST_ASSERT_UINT8_WITHIN(5, 0x84, px[0]);
    TEST_ASSERT_UINT8_WITHIN(5, 0x82, px[1]);
    TEST_ASSERT_UINT8_WITHIN(5, 0x84, px[2]);
    lv_image_decoder_close(&dsc);

    lv_draw_buf_destroy(buf);
}

void test_image_encoder_jpeg_roundtrip(void)
{
    const uint32_t w = 8, h = 8;
    lv_draw_buf_t * buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf);
    fill_solid_argb8888(buf, 0x80, 0x80, 0x80, 0xFF);

    lv_image_encoder_args_t enc_args;
    lv_image_encoder_args_init(&enc_args);
    enc_args.quality = 100;

    lv_result_t res = lv_image_encoder_save(buf, TEST_ENCODER_PATH("roundtrip.jpg"), &enc_args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t dec_args;
    lv_memzero(&dec_args, sizeof(dec_args));
    dec_args.no_cache = true;

    res = lv_image_decoder_open(&dsc, TEST_ENCODER_PATH("roundtrip.jpg"), &dec_args);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT16(w, dsc.decoded->header.w);
    TEST_ASSERT_EQUAL_UINT16(h, dsc.decoded->header.h);

    const uint8_t * px = dsc.decoded->data;
    TEST_ASSERT_UINT8_WITHIN(5, 0x80, px[0]);
    TEST_ASSERT_UINT8_WITHIN(5, 0x80, px[1]);
    TEST_ASSERT_UINT8_WITHIN(5, 0x80, px[2]);

    lv_image_decoder_close(&dsc);
    lv_draw_buf_destroy(buf);
}

void test_image_encoder_jpeg_open_fail(void)
{
    lv_draw_buf_t * buf = create_test_buf(4, 4, LV_COLOR_FORMAT_ARGB8888);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_draw_buf_save_to_file(buf, "Z:invalid/path.jpg"));
    lv_draw_buf_destroy(buf);
}

#endif /*LV_USE_LIBJPEG_TURBO_ENCODER*/

#endif
