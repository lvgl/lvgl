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
    image_dsc.header.flags = 0;
    image_dsc.data_size = sizeof(image_map);
    image_dsc.data = image_map;

    return &image_dsc;
}

static lv_image_decoder_dsc_t * get_image_decoder_dsc(void)
{
    static lv_image_decoder_dsc_t decoder_dsc = { 0 };
    decoder_dsc.src_type = LV_IMAGE_SRC_FILE;
    decoder_dsc.src = NULL;
    decoder_dsc.header.flags = 0;
    return &decoder_dsc;
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
void test_bin_decoder_bin_file(void)
{
    bin_decoder("A:src/test_files/binimages/cogwheel.ARGB8888.bin", "libs/cogwheel.ARGB8888.png");
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

static void render_transformed_image(const void * src)
{
    lv_image_cache_drop(src);

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, src);
    lv_obj_center(img);
    lv_image_set_pivot(img, 0, 0);
    lv_image_set_rotation(img, 300);
    lv_image_set_scale(img, 400);
    lv_refr_now(NULL);

    lv_obj_clean(lv_screen_active());
    lv_image_cache_drop(src);
}

void test_bin_decoder_transformed_partial_decode(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_i4);
    render_transformed_image(&test_image_cogwheel_i4);
    render_transformed_image("A:src/test_files/binimages/cogwheel.I4.bin");
    render_transformed_image("A:src/test_files/binimages/cogwheel.RGB565.bin");
    render_transformed_image("A:src/test_files/binimages/cogwheel.RGB565A8.bin");
}

void test_bin_decoder_open_stride_zero_handling(void)
{
    /* Test open image with stride zero */
    lv_image_dsc_t * image_dsc = get_image_dsc();
    image_dsc->header.stride = 0;
    lv_image_decoder_dsc_t * decoder_dsc = get_image_decoder_dsc();
    decoder_dsc->src = image_dsc;
    decoder_dsc->src_type = LV_IMAGE_SRC_VARIABLE;
    decoder_dsc->header.stride = image_dsc->header.w * sizeof(lv_color32_t);

    lv_result_t result = lv_bin_decoder_open(NULL, decoder_dsc);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, result);

    /* Test open image with stride zero for indexed image */
    image_dsc = get_image_dsc();
    image_dsc->header.stride = 0;
    image_dsc->header.cf = LV_COLOR_FORMAT_I1;
    decoder_dsc = get_image_decoder_dsc();
    decoder_dsc->src = image_dsc;
    decoder_dsc->src_type = LV_IMAGE_SRC_VARIABLE;
    decoder_dsc->header.stride = image_dsc->header.w * sizeof(lv_color32_t);
    decoder_dsc->args.use_indexed = 1;
    result = lv_bin_decoder_open(NULL, decoder_dsc);
#if LV_BIN_DECODER_RAM_LOAD == 0
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);
#else
    TEST_ASSERT_EQUAL(LV_RESULT_OK, result);
#endif

    lv_bin_decoder_close(decoder_dsc->decoder, decoder_dsc);
}

#if LV_USE_LZ4 && LV_BIN_DECODER_RAM_LOAD

#define COMPRESSED_A8_LZ4 "A:test_images/stride_align1/LZ4/test_A8.bin"
#define COMPRESSED_A8_RLE "A:test_images/stride_align1/RLE/test_A8.bin"
#define COMPRESSED_A8_RAW "A:test_images/stride_align1/UNCOMPRESSED/test_A8.bin"
#define TMP_FILE_NAME     "A:tmp_bin_decoder.bin"

/*Read a whole file into an exactly sized heap buffer, so ASAN reports any access beyond it.*/
static uint8_t * read_file_exact(const char * path, uint32_t * size)
{
    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_open(&f, path, LV_FS_MODE_RD));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_seek(&f, 0, LV_FS_SEEK_END));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_tell(&f, size));
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_seek(&f, 0, LV_FS_SEEK_SET));
    uint8_t * buf = lv_malloc(*size);
    TEST_ASSERT_NOT_NULL(buf);
    uint32_t rn;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_read(&f, buf, *size, &rn));
    TEST_ASSERT_EQUAL_UINT32(*size, rn);
    lv_fs_close(&f);
    return buf;
}

static void write_tmp_file(const uint8_t * data, uint32_t size)
{
    lv_fs_file_t f;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_open(&f, TMP_FILE_NAME, LV_FS_MODE_WR | LV_FS_MODE_TRUNC));
    uint32_t wn;
    TEST_ASSERT_EQUAL(LV_FS_RES_OK, lv_fs_write(&f, data, size, &wn));
    TEST_ASSERT_EQUAL_UINT32(size, wn);
    lv_fs_close(&f);
}

/*Open `src` without cache and return the result. If it is OK, compare the pixels with the uncompressed image.*/
static lv_result_t open_and_check_a8(const void * src, const uint8_t * ref, uint32_t ref_size)
{
    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t args = { .no_cache = true };
    lv_result_t res = lv_image_decoder_open(&dsc, src, &args);
    if(res != LV_RESULT_OK) return res;

    TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_A8, dsc.header.cf);
    TEST_ASSERT_NOT_NULL(dsc.decoded);
    if(ref) {
        TEST_ASSERT_GREATER_OR_EQUAL_UINT32(ref_size, dsc.decoded->data_size);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(ref, dsc.decoded->data, ref_size);
    }
    lv_image_decoder_close(&dsc);
    return LV_RESULT_OK;
}

/*The source is a path. A short path must not be read as an `lv_image_dsc_t`.*/
static void test_short_path(const char * image_path)
{
    uint32_t ref_size;
    uint8_t * ref = read_file_exact(COMPRESSED_A8_RAW, &ref_size);
    uint32_t size;
    uint8_t * img = read_file_exact(image_path, &size);
    write_tmp_file(img, size);

    /*Exact sized heap copy of the path*/
    const char * path = TMP_FILE_NAME;
    char * short_path = lv_malloc(lv_strlen(path) + 1);
    lv_memcpy(short_path, path, lv_strlen(path) + 1);

    /*Skip the header of the uncompressed image, it has the same pixels.*/
    TEST_ASSERT_EQUAL(LV_RESULT_OK, open_and_check_a8(short_path, ref + sizeof(lv_image_header_t),
                                                      ref_size - sizeof(lv_image_header_t)));
    lv_free(short_path);
    lv_free(img);
    lv_free(ref);
}

void test_bin_decoder_compressed_file_short_path_lz4(void)
{
    test_short_path(COMPRESSED_A8_LZ4);
}

#if LV_USE_RLE
void test_bin_decoder_compressed_file_short_path_rle(void)
{
    test_short_path(COMPRESSED_A8_RLE);
}
#endif

/*Compressed file shorter than the header + compress header.*/
void test_bin_decoder_compressed_file_too_small(void)
{
    uint32_t size;
    uint8_t * img = read_file_exact(COMPRESSED_A8_LZ4, &size);
    const uint32_t lens[] = {sizeof(lv_image_header_t), sizeof(lv_image_header_t) + 1, sizeof(lv_image_header_t) + 11,
                             sizeof(lv_image_header_t) + 12
                            };
    for(uint32_t i = 0; i < sizeof(lens) / sizeof(lens[0]); i++) {
        write_tmp_file(img, lens[i]);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, open_and_check_a8(TMP_FILE_NAME, NULL, 0));
    }
    lv_free(img);
}

static lv_image_dsc_t * create_variable_dsc(uint8_t * file_data, uint32_t file_size, uint32_t data_size)
{
    lv_image_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_image_dsc_t));
    lv_memcpy(&dsc->header, file_data, sizeof(lv_image_header_t));
    dsc->data_size = data_size;
    /*Exact sized copy of the payload*/
    uint8_t * data = lv_malloc(data_size);
    lv_memcpy(data, file_data + sizeof(lv_image_header_t), data_size);
    dsc->data = data;
    LV_UNUSED(file_size);
    return dsc;
}

static void free_variable_dsc(lv_image_dsc_t * dsc)
{
    lv_free((void *)dsc->data);
    lv_free(dsc);
}

/*Variable image with less data than the compress header.*/
void test_bin_decoder_compressed_variable_too_small(void)
{
    uint32_t size;
    uint8_t * img = read_file_exact(COMPRESSED_A8_LZ4, &size);
    for(uint32_t n = 0; n < 12; n++) {
        lv_image_dsc_t * dsc = create_variable_dsc(img, size, n);
        TEST_ASSERT_EQUAL(LV_RESULT_INVALID, open_and_check_a8(dsc, NULL, 0));
        free_variable_dsc(dsc);
    }
    lv_free(img);
}

/*Variable image whose decompressed_size is larger than the draw buffer.*/
void test_bin_decoder_compressed_variable_decompressed_size_overflow(void)
{
    uint32_t size;
    uint8_t * img = read_file_exact(COMPRESSED_A8_LZ4, &size);
    lv_image_dsc_t * dsc = create_variable_dsc(img, size, size - sizeof(lv_image_header_t));

    uint32_t * decompressed_size = (uint32_t *)((uint8_t *)dsc->data + 8);
    *decompressed_size += 1024;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, open_and_check_a8(dsc, NULL, 0));

    free_variable_dsc(dsc);
    lv_free(img);
}

/*Variable image whose compressed_size does not match the data size.*/
void test_bin_decoder_compressed_variable_compressed_size_mismatch(void)
{
    uint32_t size;
    uint8_t * img = read_file_exact(COMPRESSED_A8_LZ4, &size);
    lv_image_dsc_t * dsc = create_variable_dsc(img, size, size - sizeof(lv_image_header_t));

    uint32_t * compressed_size = (uint32_t *)((uint8_t *)dsc->data + 4);
    *compressed_size += 1;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, open_and_check_a8(dsc, NULL, 0));

    free_variable_dsc(dsc);
    lv_free(img);
}

/*A valid variable image still decodes to the same pixels as the uncompressed one.*/
void test_bin_decoder_compressed_variable_ok(void)
{
    uint32_t ref_size;
    uint8_t * ref = read_file_exact(COMPRESSED_A8_RAW, &ref_size);
    uint32_t size;
    uint8_t * img = read_file_exact(COMPRESSED_A8_LZ4, &size);
    lv_image_dsc_t * dsc = create_variable_dsc(img, size, size - sizeof(lv_image_header_t));

    TEST_ASSERT_EQUAL(LV_RESULT_OK, open_and_check_a8(dsc, ref + sizeof(lv_image_header_t),
                                                      ref_size - sizeof(lv_image_header_t)));

    free_variable_dsc(dsc);
    lv_free(img);
    lv_free(ref);
}

#endif /*LV_USE_LZ4 && LV_BIN_DECODER_RAM_LOAD*/

#endif
