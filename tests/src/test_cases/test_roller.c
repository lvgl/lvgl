#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * roller = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    roller = lv_roller_create(active_screen);
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_roller_init(void)
{
    TEST_FAIL();
}

#endif
