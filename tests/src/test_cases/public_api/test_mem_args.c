#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
void test_mem_add_pool_size_limits(void)
{
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
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

#endif /*LV_BUILD_TEST*/
