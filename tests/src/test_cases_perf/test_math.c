#if LV_BUILD_TEST_PERF
#include "unity/unity.h"

void test_cubic_bezier(void)
{
    TEST_ASSERT_MAX_TIME_ITER(lv_cubic_bezier, 10, 60000, 1, 2, 3, 4,
                              5);
}
#endif
