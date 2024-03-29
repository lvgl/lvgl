#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_bin_decoder_get_area_handles_width_change(void)
{
    size_t start_mem = lv_test_get_free_mem();

    lv_result_t res;
    lv_image_decoder_dsc_t dsc;
    lv_memzero(&dsc, sizeof(dsc));
    lv_image_decoder_t * decoder = NULL;
    do {
        decoder = lv_image_decoder_get_next(decoder);
        TEST_ASSERT_NOT_NULL(decoder);
    } while(decoder->info_cb != lv_bin_decoder_info);
    LV_IMAGE_DECLARE(test_I1_NONE_align1);

    res = lv_bin_decoder_info(decoder, &test_I1_NONE_align1, &dsc.header);
    TEST_ASSERT(res == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_MEMORY(&test_I1_NONE_align1.header, &dsc.header, sizeof(lv_image_header_t));
    dsc.src = &test_I1_NONE_align1;
    dsc.src_type = LV_IMAGE_SRC_VARIABLE;
    dsc.decoder = decoder;

    res = lv_bin_decoder_open(decoder, &dsc);
    TEST_ASSERT(res == LV_RESULT_OK);

    lv_area_t full_area = {0, 0, dsc.header.w - 1, dsc.header.h - 1};
    TEST_ASSERT(lv_area_get_width(&full_area) > 0);
    TEST_ASSERT(lv_area_get_height(&full_area) > 0);
    lv_area_t decoded_area;

    lv_area_set(&decoded_area, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MAX, LV_COORD_MAX);
    res = lv_bin_decoder_get_area(decoder, &dsc, &full_area, &decoded_area);
    TEST_ASSERT(res == LV_RESULT_OK);

    full_area.x2--;
    TEST_ASSERT(lv_area_get_width(&full_area) > 0);
    lv_area_set(&decoded_area, LV_COORD_MIN, LV_COORD_MIN, LV_COORD_MAX, LV_COORD_MAX);
    res = lv_bin_decoder_get_area(decoder, &dsc, &full_area, &decoded_area);
    TEST_ASSERT(res == LV_RESULT_OK);

    lv_bin_decoder_close(decoder, &dsc);

    TEST_ASSERT_MEM_LEAK_LESS_THAN(start_mem, 0);
}

#endif
