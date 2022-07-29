#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * label;

void setUp(void)
{
    active_screen = lv_scr_act();
    label = lv_label_create(active_screen);
}

void tearDown(void)
{
    lv_obj_del(label);
}

void test_label_creation(void)
{
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), LV_LABEL_DEFAULT_TEXT);
    TEST_ASSERT_EQUAL(lv_label_get_long_mode(label), LV_LABEL_LONG_WRAP);
}

#endif
