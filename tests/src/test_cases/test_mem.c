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

/* #3324 */
void test_mem_buf_realloc(void)
{
#ifdef LVGL_CI_USING_DEF_HEAP
    void * buf1 = lv_malloc(20);
    void * buf2 = lv_realloc(buf1, LV_MEM_SIZE + 16384);
    TEST_ASSERT_NULL(buf2);
#endif
}

/* #7573: Test memcpy with unaligned addresses */
void test_memcpy_unaligned(void)
{
    uint8_t source[20];
    uint8_t destination[20];

    /* Initialize source with some values */
    for (int i = 0; i < 20; i++) {
        source[i] = (uint8_t)(i + 1);
    }

    /* Use unaligned addresses by offsetting by 1 */
    uint8_t *unaligned_source = source + 1;
    uint8_t *unaligned_destination = destination + 1;

    /* Perform memcpy */
    lv_memcpy(unaligned_destination, unaligned_source, 18);

    /* Verify that the copied values match the source */
    for (int i = 0; i < 18; i++) {
        TEST_ASSERT_EQUAL_UINT8(unaligned_source[i], unaligned_destination[i]);
    }
}

#endif
