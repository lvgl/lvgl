#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_math_cubic_bezier_result_should_be_precise(void)
{
    int32_t x, x1, y1, x2, y2, y;
    int i = 0;
    while(i++ < 10000) {
        x1 = lv_rand(0, 1024);
        x2 = lv_rand(0, 1024);
        y1 = lv_rand(0, 1024);
        y2 = lv_rand(0, 1024);
        float fx1 = x1 / 1024.f;
        float fx2 = x2 / 1024.f;
        float fy1 = y1 / 1024.f;
        float fy2 = y2 / 1024.f;
        float fx, fy;

        int j = 0;
        while(j++ < 100000) {
            x = lv_rand(0, 1024);
            y = lv_cubic_bezier(x, x1, y1, x2, y2);
            fx = x / 1024.f;
            fy = lv_cubic_bezier_f(fx, fx1, fy1, fx2, fy2);
#if 1
            if (LV_ABS(fy * 1024 - y) > 250) {
                printf("x, x1, y1, x2, y2: %d,%d,%d,%d,%d\n", (int)x, (int)x1, (int)y1, (int)x2, (int)y2);
            }
#endif
            TEST_ASSERT_LESS_OR_EQUAL(250, LV_ABS(fy * 1024 - y));
        }
    }
}

#endif
