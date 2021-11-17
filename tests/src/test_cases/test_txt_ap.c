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

void test_txt_ap_proc_detect_invalid_conjuction_with_fatha(void)
{
    char test_str[] = "لاحقًا";
    char * temp = (char *)lv_mem_alloc(sizeof(char) * (_lv_txt_get_encoded_length(test_str) + 1));
    _lv_txt_ap_proc(test_str, temp);

    TEST_ASSERT_EQUAL(temp[0], 0x0644);
    TEST_ASSERT_NOT_EQUAL(temp[0], 0x0644);

    lv_mem_free(temp);
}

#endif
