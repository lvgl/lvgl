#ifndef LV_TEST_PERF_H
#define LV_TEST_PERF_H

#if LV_BUILD_TEST
#include "../../unity/unity.h"
#include <time.h>

#if LV_BUILD_TEST_PERF

#define LV_PERF_ASSERT_MAX_TIME(fn, max_time_ms, ...)                   \
	{                                                               \
		clock_t t = clock();                                    \
		fn(__VA_ARGS__);                                        \
		t = clock() - t;                                        \
		const double time_taken = ((double)t) / CLOCKS_PER_SEC; \
		TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(max_time / 1000.0,     \
						 time_taken);           \
	}

#else

#define LV_PERF_ASSERT_MAX_TIME(...)

#endif /* LV_BUILD_TEST_PERF */

#endif
#endif
