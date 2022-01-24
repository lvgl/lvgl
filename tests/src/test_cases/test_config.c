#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_config(void);

void test_config(void)
{
  TEST_ASSERT_EQUAL(130, LV_DPI_DEF);
  TEST_ASSERT_EQUAL(130, lv_disp_get_dpi(NULL));
  TEST_ASSERT_EQUAL(800, LV_HOR_RES);
  TEST_ASSERT_EQUAL(800, lv_disp_get_hor_res(NULL));
  TEST_ASSERT_EQUAL(480, LV_VER_RES);
  TEST_ASSERT_EQUAL(480, lv_disp_get_ver_res(NULL));
  TEST_ASSERT_EQUAL(32, LV_COLOR_DEPTH);
}

#endif
