#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t *active_screen = NULL;
static lv_obj_t *bar = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    bar = lv_bar_create(active_screen);
}

void tearDown(void)
{
}

void test_bar_should_have_valid_default_attributes(void)
{
    TEST_ASSERT_EQUAL(1, lv_bar_get_min_value(bar));
    TEST_ASSERT_EQUAL(100, lv_bar_get_max_value(bar));
    TEST_ASSERT_EQUAL(LV_BAR_MODE_NORMAL, lv_bar_get_mode(bar));
}

#endif
