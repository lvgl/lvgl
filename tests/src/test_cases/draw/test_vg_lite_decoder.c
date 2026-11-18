#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "src/draw/vg_lite/lv_vg_lite_utils.h"

#include "unity/unity.h"

#if LV_USE_DRAW_VG_LITE && LV_BIN_DECODER_RAM_LOAD

#define DECLARE_SET(cf, align) \
    LV_IMAGE_DECLARE(test_##cf##_NONE_align##align); \
    LV_IMAGE_DECLARE(test_##cf##_RLE_align##align); \
    LV_IMAGE_DECLARE(test_##cf##_LZ4_align##align)

#define DECLARE_FORMAT(cf) \
    DECLARE_SET(cf, 1); \
    DECLARE_SET(cf, 64)

#define IMG_SET(cf, align) { \
        &test_##cf##_NONE_align##align, \
        &test_##cf##_RLE_align##align, \
        &test_##cf##_LZ4_align##align, \
    }

#define IMG_SET_ALL(cf) { IMG_SET(cf, 1), IMG_SET(cf, 64) }

#define COMPRESSION_CNT 3 /* index 0 is the uncompressed reference */

/* The formats the VG-Lite decoder converts, the bin files exist for the first 7 only */
#define VARIABLE_FORMAT_CNT 9
#define FILE_FORMAT_CNT 7

DECLARE_FORMAT(I1);
DECLARE_FORMAT(I2);
DECLARE_FORMAT(I4);
DECLARE_FORMAT(A1);
DECLARE_FORMAT(A2);
DECLARE_FORMAT(RGB565A8);
DECLARE_FORMAT(RGB888);
DECLARE_FORMAT(ARGB8565);
DECLARE_FORMAT(RGB565_SWAPPED);

static const char * const color_formats[VARIABLE_FORMAT_CNT] = {
    "I1", "I2", "I4", "A1", "A2", "RGB565A8", "RGB888", "ARGB8565", "RGB565_SWAPPED"
};
static const char * const compression_dirs[COMPRESSION_CNT] = { "UNCOMPRESSED", "RLE", "LZ4" };
static const int stride_aligns[] = { 1, 64 };

static const lv_image_dsc_t * const images[VARIABLE_FORMAT_CNT][2][COMPRESSION_CNT] = {
    IMG_SET_ALL(I1),
    IMG_SET_ALL(I2),
    IMG_SET_ALL(I4),
    IMG_SET_ALL(A1),
    IMG_SET_ALL(A2),
    IMG_SET_ALL(RGB565A8),
    IMG_SET_ALL(RGB888),
    IMG_SET_ALL(ARGB8565),
    IMG_SET_ALL(RGB565_SWAPPED),
};

/* The format the VG-Lite decoder must output, UNKNOWN when the bin decoder handles the format */
static lv_color_format_t get_converted_format(unsigned cf)
{
    const bool has_24bit = vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT);

    switch(cf) {
        case 0: /*I1*/
        case 1: /*I2*/
        case 2: /*I4*/
            return LV_COLOR_FORMAT_I8;
        case 3: /*A1*/
        case 4: /*A2*/
            return LV_COLOR_FORMAT_A8;
        case 5: /*RGB565A8*/
            return LV_COLOR_FORMAT_ARGB8888;
        case 6: /*RGB888*/
            return has_24bit ? LV_COLOR_FORMAT_UNKNOWN : LV_COLOR_FORMAT_XRGB8888;
        case 7: /*ARGB8565*/
            return has_24bit ? LV_COLOR_FORMAT_UNKNOWN : LV_COLOR_FORMAT_ARGB8888;
        default: /*RGB565_SWAPPED*/
            return LV_COLOR_FORMAT_RGB565;
    }
}

/* Open both with the same args, the decoded images of the compressed one must be identical to the reference */
static void assert_same_decoded(const void * ref_src, const void * comp_src, bool premultiply,
                                lv_color_format_t converted_cf, const char * desc)
{
    lv_image_decoder_args_t args = {0};
    args.use_indexed = true;
    args.stride_align = true;
    args.no_cache = true;
    args.premultiply = premultiply;

    lv_image_decoder_dsc_t ref;
    lv_image_decoder_dsc_t comp;
    TEST_ASSERT_EQUAL_MESSAGE(LV_RESULT_OK, lv_image_decoder_open(&ref, ref_src, &args), desc);
    TEST_ASSERT_EQUAL_MESSAGE(LV_RESULT_OK, lv_image_decoder_open(&comp, comp_src, &args), desc);
    TEST_ASSERT_NOT_NULL(ref.decoded);
    TEST_ASSERT_NOT_NULL(comp.decoded);

    if(converted_cf != LV_COLOR_FORMAT_UNKNOWN) {
        /* The VG-Lite decoder, not the bin decoder, must have produced both */
        TEST_ASSERT_EQUAL_MESSAGE(converted_cf, ref.decoded->header.cf, desc);
        TEST_ASSERT_EQUAL_MESSAGE(converted_cf, comp.decoded->header.cf, desc);
    }
    TEST_ASSERT_EQUAL_MESSAGE(ref.decoded->header.cf, comp.decoded->header.cf, desc);
    TEST_ASSERT_EQUAL_MESSAGE(ref.decoded->header.w, comp.decoded->header.w, desc);
    TEST_ASSERT_EQUAL_MESSAGE(ref.decoded->header.h, comp.decoded->header.h, desc);

    /* The bin decoder may pad the decompressed data differently, the layout is only defined for the VG-Lite output */
    if(converted_cf != LV_COLOR_FORMAT_UNKNOWN) {
        TEST_ASSERT_EQUAL_MESSAGE(ref.decoded->header.stride, comp.decoded->header.stride, desc);
        TEST_ASSERT_EQUAL_MESSAGE(ref.decoded->data_size, comp.decoded->data_size, desc);
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(ref.decoded->data, comp.decoded->data, ref.decoded->data_size, desc);
        TEST_ASSERT_EQUAL_MESSAGE(
            (bool)(ref.decoded->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED),
            (bool)(comp.decoded->header.flags & LV_IMAGE_FLAGS_PREMULTIPLIED), desc);
    }

    lv_image_decoder_close(&comp);
    lv_image_decoder_close(&ref);
}

#endif /*LV_USE_DRAW_VG_LITE && LV_BIN_DECODER_RAM_LOAD*/

void setUp(void)
{
}

void tearDown(void)
{
    lv_image_cache_drop(NULL);
}

void test_vg_lite_decoder_compressed_variable(void)
{
#if LV_USE_DRAW_VG_LITE && LV_BIN_DECODER_RAM_LOAD
    size_t mem_before = lv_test_get_free_mem();
    char desc[96];

    for(unsigned cf = 0; cf < VARIABLE_FORMAT_CNT; cf++) {
        for(unsigned align = 0; align < sizeof(stride_aligns) / sizeof(stride_aligns[0]); align++) {
            for(unsigned comp = 1; comp < COMPRESSION_CNT; comp++) {
                for(int premultiply = 0; premultiply <= 1; premultiply++) {
                    lv_snprintf(desc, sizeof(desc), "%s align%d %s premultiply %d", color_formats[cf],
                                stride_aligns[align], compression_dirs[comp], premultiply);
                    assert_same_decoded(images[cf][align][0], images[cf][align][comp], premultiply,
                                        get_converted_format(cf), desc);
                }
            }
        }
    }

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
#endif
}

void test_vg_lite_decoder_compressed_file(void)
{
#if LV_USE_DRAW_VG_LITE && LV_BIN_DECODER_RAM_LOAD
    size_t mem_before = lv_test_get_free_mem();
    char ref_path[128];
    char comp_path[128];
    char desc[128];

    for(unsigned cf = 0; cf < FILE_FORMAT_CNT; cf++) {
        for(unsigned align = 0; align < sizeof(stride_aligns) / sizeof(stride_aligns[0]); align++) {
            lv_snprintf(ref_path, sizeof(ref_path), "A:test_images/stride_align%d/%s/test_%s.bin",
                        stride_aligns[align], compression_dirs[0], color_formats[cf]);
            for(unsigned comp = 1; comp < COMPRESSION_CNT; comp++) {
                lv_snprintf(comp_path, sizeof(comp_path), "A:test_images/stride_align%d/%s/test_%s.bin",
                            stride_aligns[align], compression_dirs[comp], color_formats[cf]);
                for(int premultiply = 0; premultiply <= 1; premultiply++) {
                    lv_snprintf(desc, sizeof(desc), "%s premultiply %d", comp_path, premultiply);
                    assert_same_decoded(ref_path, comp_path, premultiply, get_converted_format(cf), desc);
                }
            }
        }
    }

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
#endif
}

/* The cached result must be the converted I8 image, not the decompressed index4 data */
void test_vg_lite_decoder_compressed_cache(void)
{
#if LV_USE_DRAW_VG_LITE && LV_BIN_DECODER_RAM_LOAD
    lv_image_decoder_args_t args = {0};
    args.use_indexed = true;
    args.stride_align = true;

    for(int i = 0; i < 2; i++) {
        lv_image_decoder_dsc_t dsc;
        TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&dsc, &test_I4_LZ4_align64, &args));
        TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_I8, dsc.decoded->header.cf);
        lv_image_decoder_close(&dsc);
    }
#endif
}

void test_vg_lite_decoder_compressed_corrupted(void)
{
#if LV_USE_DRAW_VG_LITE && LV_BIN_DECODER_RAM_LOAD && LV_USE_LZ4
    /* 12-byte compression header (method | compressed_size | decompressed_size) + invalid LZ4 payload */
    static const uint8_t data[] = {
        LV_IMAGE_COMPRESS_LZ4, 0, 0, 0,
        8, 0, 0, 0,
        72, 0, 0, 0,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    lv_image_dsc_t image_dsc = {0};
    image_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
    image_dsc.header.cf = LV_COLOR_FORMAT_I4;
    image_dsc.header.w = 4;
    image_dsc.header.h = 4;
    image_dsc.header.stride = 2;
    image_dsc.header.flags = LV_IMAGE_FLAGS_COMPRESSED;
    image_dsc.data_size = sizeof(data);
    image_dsc.data = data;

    size_t mem_before = lv_test_get_free_mem();

    lv_image_decoder_dsc_t dsc;
    lv_image_decoder_args_t args = {0};
    args.use_indexed = true;
    args.no_cache = true;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_image_decoder_open(&dsc, &image_dsc, &args));

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem_before, 0);
#endif
}

#endif /*LV_BUILD_TEST*/
