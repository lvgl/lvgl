#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_bugfix_for_2522(void);

void test_bugfix_for_2522(void)
{
    uint16_t expected_start_angle = 36;
    uint16_t expected_end_angle = 90;
    int16_t expected_value = 40;

    lv_obj_t *arcBlack;
    arcBlack = lv_arc_create(lv_scr_act());

    lv_arc_set_mode(arcBlack, LV_ARC_MODE_REVERSE);

    lv_arc_set_bg_angles(arcBlack, 0, 90);
    
    /* lv_arc_set_value calls value_update, where the fix is done */
    lv_arc_set_value(arcBlack, expected_value);

    TEST_ASSERT_EQUAL_UINT16(expected_start_angle, lv_arc_get_angle_start(arcBlack));
    TEST_ASSERT_EQUAL_UINT16(expected_end_angle, lv_arc_get_angle_end(arcBlack));
    TEST_ASSERT_EQUAL_INT16(expected_value, lv_arc_get_value(arcBlack));
}

#endif
