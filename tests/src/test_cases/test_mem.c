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

void test_malloc(void)
{
    uint32_t mem = lv_test_get_free_mem();
    void * buf = lv_malloc(32);
    TEST_ASSERT_NOT_NULL(buf);
    lv_free(buf);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
}

static void check_zero_mem(const void * data, size_t size)
{
    const uint8_t * p = data;
    for(size_t i = 0; i < size; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, p[i]);
    }
}

void test_calloc(void)
{
    uint32_t mem = lv_test_get_free_mem();
    void * buf = lv_calloc(4, 8);
    TEST_ASSERT_NOT_NULL(buf);

    check_zero_mem(buf, 32);

    lv_free(buf);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
}

void test_zalloc(void)
{
    uint32_t mem = lv_test_get_free_mem();
    void * buf = lv_zalloc(32);
    TEST_ASSERT_NOT_NULL(buf);

    check_zero_mem(buf, 32);

    lv_free(buf);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
}

void test_realloc(void)
{
    uint32_t mem = lv_test_get_free_mem();
    void * buf = lv_malloc(16);
    TEST_ASSERT_NOT_NULL(buf);

    buf = lv_realloc(buf, 32);
    TEST_ASSERT_NOT_NULL(buf);

    buf = lv_realloc(buf, 8);
    TEST_ASSERT_NOT_NULL(buf);

    lv_free(buf);

    /* Should behave like malloc */
    buf = lv_realloc(NULL, 16);
    TEST_ASSERT_NOT_NULL(buf);
    lv_free(buf);

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
}

/* #3324 */
void test_realloc_failed(void)
{
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    uint32_t mem = lv_test_get_free_mem();

    void * buf1 = lv_malloc(20);

    void * buf2 = lv_realloc(buf1, LV_MEM_SIZE + 1);
    TEST_ASSERT_NULL(buf2);

    /* Realloc failed, but should free buf1 */
    void * buf3 = lv_reallocf(buf1, LV_MEM_SIZE + 1);
    TEST_ASSERT_NULL(buf3);

    void * buf4 = lv_reallocf(NULL, 30);
    TEST_ASSERT_NOT_NULL(buf4);
    lv_free(buf4);

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
#endif
}

void test_malloc_failed(void)
{
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    uint32_t mem = lv_test_get_free_mem();
    TEST_ASSERT_NULL(lv_malloc(LV_MEM_SIZE + 1));
    TEST_ASSERT_NULL(lv_malloc_zeroed(LV_MEM_SIZE + 1));
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
#endif
}

void test_malloc_size_overflow(void)
{
#ifdef LVGL_CI_USING_DEF_HEAP
    uint32_t mem = lv_test_get_free_mem();

    /* Aligning these up wraps around, which must not turn them into a tiny request */
    for(size_t i = 0; i < 2 * lv_tlsf_align_size(); i++) {
        TEST_ASSERT_NULL(lv_malloc(SIZE_MAX - i));
        TEST_ASSERT_NULL(lv_malloc_zeroed(SIZE_MAX - i));
    }

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_mem_test());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
#endif
}

void test_realloc_size_overflow(void)
{
#ifdef LVGL_CI_USING_DEF_HEAP
    const size_t len = 100;
    uint32_t mem = lv_test_get_free_mem();

    uint8_t * buf = lv_malloc(len);
    TEST_ASSERT_NOT_NULL(buf);
    lv_memset(buf, 0xA5, len);

    for(size_t i = 0; i < 2 * lv_tlsf_align_size(); i++) {
        TEST_ASSERT_NULL(lv_realloc(buf, SIZE_MAX - i));
    }

    /* Shrinking buf instead of failing would write a block header over its payload */
    for(size_t i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL_UINT8(0xA5, buf[i]);
    }

    /* ...and would hand the trailing part of buf out to the next allocation */
    uint8_t * other = lv_malloc(32);
    TEST_ASSERT_NOT_NULL(other);
    lv_memset(other, 0x5A, 32);
    for(size_t i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL_UINT8(0xA5, buf[i]);
    }

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_mem_test());

    lv_free(other);
    lv_free(buf);
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
#endif
}

void test_mem_add_pool_size_limits(void)
{
#ifdef LVGL_CI_USING_DEF_HEAP
    const size_t overhead = lv_tlsf_pool_overhead();
    const size_t block_max = lv_tlsf_block_size_max();
    uint32_t mem = lv_test_get_free_mem();

    void * pool_mem = malloc(overhead + block_max);
    TEST_ASSERT_NOT_NULL(pool_mem);

    /* A pool of exactly block_size_max would index sl_bitmap out of bounds */
    TEST_ASSERT_NULL(lv_mem_add_pool(pool_mem, overhead + block_max));
    TEST_ASSERT_NULL(lv_mem_add_pool(pool_mem, SIZE_MAX));
    TEST_ASSERT_NULL(lv_mem_add_pool(pool_mem, overhead + lv_tlsf_block_size_min() - 1));
    TEST_ASSERT_NULL(lv_mem_add_pool(pool_mem, 0));

    /* One word less is the largest pool the range check accepts */
    lv_mem_pool_t pool = lv_mem_add_pool(pool_mem, overhead + block_max - lv_tlsf_align_size());
    TEST_ASSERT_NOT_NULL(pool);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_mem_test());

    lv_mem_remove_pool(pool);
    free(pool_mem);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_mem_test());
    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
#endif
}

void test_mem_test(void)
{
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    uint32_t mem = lv_test_get_free_mem();
    uint32_t * zero_mem = lv_malloc_zeroed(0);
    TEST_ASSERT_NOT_NULL(zero_mem);

    /* Test magic value */
    TEST_ASSERT_EQUAL_UINT32(ZERO_MEM_SENTINEL, *zero_mem);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_mem_test());

    /* Test wrong memory, test should fail */
    *zero_mem = 0;
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, lv_mem_test());

    /* Restore magic value */
    *zero_mem = ZERO_MEM_SENTINEL;
    lv_free(zero_mem);

    /* Re-verify zero memory */
    uint32_t * new_zero_mem = lv_malloc_zeroed(0);
    TEST_ASSERT_EQUAL_UINT32(ZERO_MEM_SENTINEL, *new_zero_mem);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_mem_test());
    lv_free(new_zero_mem);

    TEST_ASSERT_MEM_LEAK_LESS_THAN(mem, 0);
#endif
}

/* #7573: Test memcpy with unaligned addresses */
void test_memcpy_unaligned(void)
{
    union {
        uint8_t u8[20];
        uint32_t dummy_u32;
    } source, destination;

    /* Initialize source with some values */
    for(int i = 0; i < 20; i++) {
        source.u8[i] = (uint8_t)(i + 1);
    }

    /* Ensure unaligned addresses */
    uint8_t * unaligned_source = source.u8 + 1;
    uint8_t * unaligned_destination = destination.u8 + 1;

    /* Perform lv_memcpy */
    lv_memcpy(unaligned_destination, unaligned_source, 19);

    /* Verify that the copied values match the source */
    for(int i = 0; i < 19; i++) {
        TEST_ASSERT_EQUAL_UINT8(unaligned_source[i], unaligned_destination[i]);
    }
}

#endif
