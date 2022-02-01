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
    void * ptr = NULL;
    size_t allocation_size = sizeof(lv_font_t);

    lv_res_t retval = lv_mem_realloc_safe(&ptr, allocation_size);
    TEST_ASSERT_EQUAL_UINT16(retval, LV_RES_OK);

    /* Don't leak memory */
    lv_mem_free(ptr);
}

/* a zero size with a non-null pointer will behave like free and return LV_RES_OK. */
void test_realloc_safe_behave_free_like(void)
{
    void * font = lv_mem_alloc(1 * sizeof(lv_font_t));

    lv_res_t retval = lv_mem_realloc_safe(&font, 0);
    TEST_ASSERT_EQUAL_UINT16(retval, LV_RES_OK);
}

#endif
