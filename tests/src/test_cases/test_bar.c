#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "lv_test_indev.h"

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
    TEST_ASSERT_EQUAL(0, lv_bar_get_min_value(bar));
    TEST_ASSERT_EQUAL(100, lv_bar_get_max_value(bar));
    TEST_ASSERT_EQUAL(LV_BAR_MODE_NORMAL, lv_bar_get_mode(bar));
}

void test_bar_indicator_width_should_track_bar_value(void)
{
    lv_bar_t * bar_ptr = (lv_bar_t *) bar;

    /* By default the bar is horizontal, so we get the indicator width */
    int32_t indicator_width = lv_area_get_width(&bar_ptr->indic_area);

    /* If x1 == x2 then width == 1 */
    TEST_ASSERT_EQUAL_INT32(1, indicator_width);

    /* Set a new value */
    lv_bar_set_value(bar, 10, LV_ANIM_OFF);
    lv_test_indev_wait(50);

    indicator_width = lv_area_get_width(&bar_ptr->indic_area);
    TEST_ASSERT_EQUAL_INT32(27 /* Value found by debugging */, indicator_width);
}
#endif
