#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * line = NULL;

static const uint16_t default_point_num = 0U;

void setUp(void)
{
    active_screen = lv_scr_act();
    line = lv_line_create(active_screen);
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_line_should_have_valid_documented_defualt_values(void)
{
    lv_line_t * line_ptr = (lv_line_t *) line;
    TEST_ASSERT_EQUAL_UINT16(default_point_num, line_ptr->point_num);
    TEST_ASSERT_NULL(line_ptr->point_array);
    TEST_ASSERT_FALSE(lv_line_get_y_invert(line));
}

void test_line_should_return_valid_y_invert(void)
{
    lv_line_set_y_invert(line, true);
    TEST_ASSERT_TRUE(lv_line_get_y_invert(line));
}

#endif
