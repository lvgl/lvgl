#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
}

void tearDown(void)
{
    active_screen = NULL;
}

void test_meter_default_settings(void)
{
    TEST_FAIL();
}

#endif
