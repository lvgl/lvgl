#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_display_t * disp_def;

void setUp(void)
{
    disp_def = lv_display_get_default();
    lv_display_set_default(NULL);
}

void tearDown(void)
{
    lv_display_set_default(disp_def);
    lv_obj_clean(lv_screen_active());
}

void test_display_args_resolution_without_default_display(void)
{
    lv_display_set_resolution(lv_display_get_default(), 2, 3);
    lv_display_set_physical_resolution(lv_display_get_default(), 4, 5);
    lv_display_set_offset(lv_display_get_default(), 6, 7);

    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_horizontal_resolution(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_vertical_resolution(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_original_horizontal_resolution(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_original_vertical_resolution(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_physical_horizontal_resolution(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_physical_vertical_resolution(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_offset_x(lv_display_get_default()));
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_offset_y(lv_display_get_default()));
    TEST_ASSERT_EQUAL(LV_DISPLAY_ROTATION_0, lv_display_get_rotation(lv_display_get_default()));
}

void test_display_args_dpi_tile_cnt_antialiasing_without_default_display(void)
{
    lv_display_set_dpi(lv_display_get_default(), 200);
    TEST_ASSERT_EQUAL_INT32(LV_DPI_DEF, lv_display_get_dpi(lv_display_get_default()));

    lv_display_set_antialiasing(lv_display_get_default(), false);
    TEST_ASSERT_FALSE(lv_display_get_antialiasing(lv_display_get_default()));
    lv_display_set_antialiasing(lv_display_get_default(), true);
    TEST_ASSERT_FALSE(lv_display_get_antialiasing(lv_display_get_default()));

    lv_display_set_tile_cnt(lv_display_get_default(), 20);
    TEST_ASSERT_EQUAL_INT32(0, lv_display_get_tile_cnt(lv_display_get_default()));
}

void test_display_args_layers_without_default_display(void)
{
    TEST_ASSERT_NULL(lv_display_get_screen_active(lv_display_get_default()));
    TEST_ASSERT_NULL(lv_display_get_screen_prev(lv_display_get_default()));
    TEST_ASSERT_NULL(lv_display_get_layer_top(lv_display_get_default()));
    TEST_ASSERT_NULL(lv_display_get_layer_sys(lv_display_get_default()));
    TEST_ASSERT_NULL(lv_display_get_layer_bottom(lv_display_get_default()));
}

void test_display_args_activity_without_default_display(void)
{
    /*Triggering activity on nothing must not disturb the display that does exist*/
    lv_display_trigger_activity(disp_def);
    lv_tick_inc(1000);

    lv_display_trigger_activity(lv_display_get_default());
    lv_tick_inc(1000);
    TEST_ASSERT_EQUAL_UINT32(2000, lv_display_get_inactive_time(disp_def));
}

#endif /*LV_BUILD_TEST*/
