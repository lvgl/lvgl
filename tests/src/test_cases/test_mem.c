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
    bool retval = true;
    size_t allocation_size = 1;

    void * ptr = NULL;
    void * data = &ptr;
    retval = lv_mem_realloc_safe(data, allocation_size);

    TEST_ASSERT_FALSE(retval);

    /* Don't leak memory */
    lv_mem_free(ptr);
}

/* a zero size with a non-null pointer will behave like free and return false. */
void test_realloc_safe_behave_free_like(void)
{
    bool retval = true;

    void * data = lv_mem_alloc(1);
    /* Free allocated memory inside realloc safe */
    retval = lv_mem_realloc_safe(&data, 0);

    TEST_ASSERT_FALSE(retval);
}

#endif
