#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * line = NULL;

void setUp(void)
{
    active_screen = lv_scr_act();
    line = lv_line_create(active_screen);
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_line_should_return_valid_y_invert(void)
{
    lv_line_set_y_invert(line, true);
    TEST_ASSERT_TRUE(lv_line_get_y_invert(line));
}

#endif
