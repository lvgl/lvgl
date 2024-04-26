#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void test_config(void);

void test_config(void)
{
    TEST_ASSERT_EQUAL(130, LV_DPI_DEF);
    TEST_ASSERT_EQUAL(130, lv_display_get_dpi(NULL));
    TEST_ASSERT_EQUAL(800, LV_HOR_RES);
    TEST_ASSERT_EQUAL(800, lv_display_get_horizontal_resolution(NULL));
    TEST_ASSERT_EQUAL(480, LV_VER_RES);
    TEST_ASSERT_EQUAL(480, lv_display_get_vertical_resolution(NULL));
    TEST_ASSERT_EQUAL(32, LV_COLOR_DEPTH);
}

#endif
