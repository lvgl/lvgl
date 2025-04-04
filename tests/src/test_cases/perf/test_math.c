#if LV_BUILD_TEST_PERF
#include "unity/unity.h"

void test_cubic_bezier(void)
{
	LV_PERF_ASSERT_MAX_TIME(lv_cubic_bezier, 10, 1, 2, 3, 4, 5);
}
#endif
