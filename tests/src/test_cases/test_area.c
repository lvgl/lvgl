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

void test_area_diff(void)
{
    lv_area_t expected[4] = {
        {0,  0,  100, 29},  /* wide top rectangle */
        {0,  91, 100, 100}, /* wide bottom rectangle */
        {0,  30, 39,  90},  /* left rectangle */
        {81, 30, 100, 90}   /* right rectangle */
    };
    lv_area_t outer = {0, 0, 100, 100};
    lv_area_t remove = {40, 30, 80, 90};
    lv_area_t actual[4];
    lv_memset(actual, 0, sizeof(actual));

    int8_t area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    TEST_PRINTF("%d %d %d %d", actual[0].x1, actual[0].y1, actual[0].x2, actual[0].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[0], &actual[0]));
    TEST_PRINTF("%d %d %d %d", actual[1].x1, actual[1].y1, actual[1].x2, actual[1].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[1], &actual[1]));
    TEST_PRINTF("%d %d %d %d", actual[2].x1, actual[2].y1, actual[2].x2, actual[2].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[2], &actual[2]));
    TEST_PRINTF("%d %d %d %d", actual[3].x1, actual[3].y1, actual[3].x2, actual[3].y2);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected[3], &actual[3]));


    /* no left edge */
    lv_area_set(&remove, 0, 10, 10, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 1, 10, 10, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    /* no right edge */
    lv_area_set(&remove, 90, 10, 100, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 90, 10, 99, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    /* no top edge */
    lv_area_set(&remove, 10, 0, 20, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 10, 1, 20, 20);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);

    /* no bottom edge */
    lv_area_set(&remove, 10, 90, 20, 100);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 3);

    lv_area_set(&remove, 10, 90, 20, 99);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 4);


    /* top and left edges missing */
    lv_area_set(&remove, -50, -50, 50, 50);
    area_count = lv_area_diff(actual, &outer, &remove);
    TEST_ASSERT_EQUAL_INT8(area_count, 2);
}

#endif
