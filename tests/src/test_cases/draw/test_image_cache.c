#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static size_t initial_image_cache_max_size;
static size_t initial_free_mem;

void setUp(void)
{
    initial_image_cache_max_size = lv_image_cache_get_max_size();
    lv_image_cache_drop(NULL);
    initial_free_mem = lv_test_get_free_mem();
}

void tearDown(void)
{
    lv_image_cache_resize((uint32_t)initial_image_cache_max_size, true);
    lv_image_cache_drop(NULL);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(initial_free_mem, 64);
}

static size_t add_test_image_to_cache(void)
{
    static uint8_t image_src;
    lv_draw_buf_t * decoded = lv_draw_buf_create(4, 4, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    if(decoded == NULL) {
        TEST_FAIL_MESSAGE("Failed to create test image");
        return 0;
    }

    size_t payload_size = decoded->data_size;

    lv_image_cache_data_t search_key = {
        .slot.size = payload_size,
        .src = &image_src,
        .src_type = LV_IMAGE_SRC_VARIABLE,
    };
    lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(NULL, &search_key, decoded, NULL);
    if(entry == NULL) {
        lv_draw_buf_destroy(decoded);
        TEST_FAIL_MESSAGE("Failed to add test image to cache");
        return 0;
    }

    lv_cache_release(LV_GLOBAL_DEFAULT()->img_cache, entry, NULL);
    return payload_size;
}

void test_image_cache_metrics(void)
{
    const size_t max_size = 1024;
    lv_image_cache_resize(max_size, true);

    TEST_ASSERT_EQUAL_SIZE_T(max_size, lv_image_cache_get_max_size());
    TEST_ASSERT_EQUAL_SIZE_T(0, lv_image_cache_get_size());
    TEST_ASSERT_EQUAL_SIZE_T(max_size, lv_image_cache_get_free_size());

    size_t payload_size = add_test_image_to_cache();

    TEST_ASSERT_EQUAL_SIZE_T(payload_size, lv_image_cache_get_size());
    TEST_ASSERT_LESS_OR_EQUAL_SIZE_T(max_size, lv_image_cache_get_size());
    TEST_ASSERT_EQUAL_SIZE_T(max_size - payload_size, lv_image_cache_get_free_size());
}

void test_image_cache_metrics_resize_and_disabled(void)
{
    lv_image_cache_resize(128, true);
    TEST_ASSERT_TRUE(lv_image_cache_is_enabled());
    TEST_ASSERT_EQUAL_SIZE_T(128, lv_image_cache_get_max_size());
    TEST_ASSERT_EQUAL_SIZE_T(128, lv_image_cache_get_free_size());

    lv_image_cache_resize(0, true);
    TEST_ASSERT_FALSE(lv_image_cache_is_enabled());
    TEST_ASSERT_EQUAL_SIZE_T(0, lv_image_cache_get_max_size());
    TEST_ASSERT_EQUAL_SIZE_T(0, lv_image_cache_get_size());
    TEST_ASSERT_EQUAL_SIZE_T(0, lv_image_cache_get_free_size());
}

void test_image_cache_metrics_overcommit_saturates_free_size(void)
{
    lv_image_cache_resize(1024, true);
    size_t payload_size = add_test_image_to_cache();

    TEST_ASSERT_GREATER_THAN(1, payload_size);

    lv_image_cache_resize(1, false);
    TEST_ASSERT_EQUAL_SIZE_T(1, lv_image_cache_get_max_size());
    TEST_ASSERT_GREATER_THAN(lv_image_cache_get_max_size(), lv_image_cache_get_size());
    TEST_ASSERT_EQUAL_SIZE_T(0, lv_image_cache_get_free_size());
}

void test_image_cache_dump(void)
{
    /* Dump should not crash or memory leaks */
    lv_image_cache_dump();
    lv_image_header_cache_dump();
}

#endif
