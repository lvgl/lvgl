#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define PCT_MAX_VALUE 268435455

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_pct(void)
{
    int32_t pct_val;
    int32_t pct_coord;

    pct_val = 0;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = 1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = 100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = 111111111;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = PCT_MAX_VALUE;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -111111111;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    pct_val = -PCT_MAX_VALUE;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(pct_val, LV_COORD_GET_PCT(pct_coord));

    /**
     * Out of bounds behavior.
     * The pct value will be clamped to the max/min value if it's out of bounds.
    */

    pct_val = PCT_MAX_VALUE + 1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));

    pct_val = PCT_MAX_VALUE + 100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));

    pct_val = -PCT_MAX_VALUE - 1;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(-PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));

    pct_val = -PCT_MAX_VALUE - 100;
    pct_coord = lv_pct(pct_val);
    TEST_ASSERT_TRUE(LV_COORD_IS_PCT(pct_coord));
    TEST_ASSERT_FALSE(LV_COORD_IS_PX(pct_coord));
    TEST_ASSERT_TRUE(LV_COORD_IS_SPEC(pct_coord));
    TEST_ASSERT_EQUAL_INT32(-PCT_MAX_VALUE, LV_COORD_GET_PCT(pct_coord));
}

#endif
