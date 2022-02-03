#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

/* Non-zero size with null pointer should behave like malloc */
void test_realloc_safe_behave_malloc_like(void)
{
    void * p = NULL;
    size_t s = sizeof(lv_font_t);

    lv_res_t retval = lv_mem_realloc_safe(&p, s);
    TEST_ASSERT_EQUAL_UINT16(retval, LV_RES_OK);

    /* Don't leak memory */
    lv_mem_free(p);
}

/* simply reallocate a pointer */
void test_realloc_safe_golden_path(void)
{
    size_t s = sizeof(lv_font_t);
    void * p = lv_mem_alloc(s);

    lv_res_t retval = lv_mem_realloc_safe(&p, 3 * s);
    TEST_ASSERT_EQUAL_UINT16(retval, LV_RES_OK);
    TEST_ASSERT_NOT_NULL(p);

    /* Don't leak memory */
    lv_mem_free(p);
}

/* for zero size return a not NULL pointer. */
void test_realloc_retun_not_null_on_zero_size(void)
{
    size_t s = sizeof(lv_font_t);
    void * p = lv_mem_alloc(s);

    lv_res_t retval = lv_mem_realloc_safe(&p, 0);
    TEST_ASSERT_EQUAL_UINT16(retval, LV_RES_OK);
    TEST_ASSERT_NOT_NULL(p);

    /* Don't leak memory */
    lv_mem_free(p);
}
#endif
