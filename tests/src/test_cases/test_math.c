#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#define ERROR_THRESHOLD         5 /*5 in 1024, 0.5% max error allowed*/
#define NEWTON_ITERATIONS       8

static float do_cubic_bezier_f(float t, float a, float b, float c)
{
    /*a*t^3 + b*t^2 + c*t*/
    return ((a * t + b) * t + c) * t;
}

/**
 * Calculate the y value of cubic-bezier(x1, y1, x2, y2) function as specified x.
 * @param x time in range of [0..1]
 * @param x1 x of control point 1 in range of [0..1]
 * @param y1 y of control point 1 in range of [0..1]
 * @param x2 x of control point 2 in range of [0..1]
 * @param y2 y of control point 2 in range of [0..1]
 * @return the value calculated
 */
static float lv_cubic_bezier_f(float x, float x1, float y1, float x2, float y2)
{
    float ax, bx, cx, ay, by, cy;
    float tl, tr, t;  /*t in cubic-bezier function, used for bisection */
    float xs;  /*x sampled on curve */
    float d; /*slope value at specified t*/

    if(x == 0 || x == 1) return x;

    cx = 3.f * x1;
    bx = 3.f * (x2 - x1) - cx;
    ax = 1.f - cx - bx;

    cy = 3.f * y1;
    by = 3.f * (y2 - y1) - cy;
    ay = 1.f - cy - by;

    /*Try Newton's method firstly */
    t = x; /*Make a guess*/
    for(int i = 0; i < NEWTON_ITERATIONS; i++) {
        xs = do_cubic_bezier_f(t, ax, bx, cx);
        xs -= x;
        if(LV_ABS(xs) < 1e-6f) goto found;

        d = (3.f * ax * t + 2.f * bx) * t + cx;
        if(LV_ABS(d) < 1e-6f) break;
        t -= xs / d;
    }

    /*Fallback to bisection method for reliability*/
    tl = 0.f, tr = 1.f, t = x;

    if(t < tl) {
        t = tl;
        goto found;
    }

    if(t > tr) {
        t = tr;
        goto found;
    }

    while(tl < tr) {
        xs = do_cubic_bezier_f(t, ax, bx, cx);
        if(LV_ABS(xs - x) < 1e-6f) goto found;
        x > xs ? (tl = t) : (tr = t);
        t = (tr - tl) * .5f + tl;
    }

found:
    return do_cubic_bezier_f(t, ay, by, cy);
}


static int test_cubic_bezier_ease_functions(float fx1, float fy1, float fx2, float fy2)
{
    int x1, y1, x2, y2, y;
    float t, t_step, fy;

    t_step = .001f;
    x1 = LV_BEZIER_VAL_FLOAT(fx1);
    y1 = LV_BEZIER_VAL_FLOAT(fy1);
    x2 = LV_BEZIER_VAL_FLOAT(fx2);
    y2 = LV_BEZIER_VAL_FLOAT(fy2);

    for(t = 0; t <= 1; t += t_step) {
        fy = lv_cubic_bezier_f(t, fx1, fy1, fx2, fy2);
        y = lv_cubic_bezier(LV_BEZIER_VAL_FLOAT(t), x1, y1, x2, y2);
        if(LV_ABS(LV_BEZIER_VAL_FLOAT(fy) - y) >= ERROR_THRESHOLD) {
            return 0;
        }
    }

    return 1;
}

static uint32_t lv_bezier3_legacy(uint32_t t, uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3)
{
    uint32_t t_rem  = 1024 - t;
    uint32_t t_rem2 = (t_rem * t_rem) >> 10;
    uint32_t t_rem3 = (t_rem2 * t_rem) >> 10;
    uint32_t t2     = (t * t) >> 10;
    uint32_t t3     = (t2 * t) >> 10;

    uint32_t v1 = (t_rem3 * u0) >> 10;
    uint32_t v2 = (3 * t_rem2 * t * u1) >> 20;
    uint32_t v3 = (3 * t_rem * t2 * u2) >> 20;
    uint32_t v4 = (t3 * u3) >> 10;

    return v1 + v2 + v3 + v4;
}

void test_math_cubic_bezier_result_should_be_precise(void)
{
    /*ease-in-out function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(.42f, 0, .58f, 1));

    /*ease-out function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(0, 0, .58f, 1));

    /*ease-in function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(.42f, 0, 1, 1));

    /*ease function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(.25f, .1f, .25f, 1));

    int32_t u0 = 0, u1 = 50, u2 = 952, u3 = LV_BEZIER_VAL_MAX;
    for(int32_t i = 0; i <= 1024; i++) {
        int32_t legacy = lv_bezier3_legacy(i, u0, u1, u2, u3);
        int32_t cubic_bezier = lv_bezier3(i, u0, u1, u2, u3);

        TEST_ASSERT_TRUE(LV_ABS(legacy - cubic_bezier) <= 5);
    }
}

void test_math_align_up_down(void)
{
    int32_t values[] = { 0, 1, 3, 6, 23, 54, 55, 345, 5342, 662, 456, 234, 123, 12, 44, 55, 66, 77, 88, 99, 100, 101, 102, 103, 104, 105, 106, 107 };
    int32_t values_align_up_4[] = { 0, 4, 4, 8, 24, 56, 56, 348, 5344, 664, 456, 236, 124, 12, 44, 56, 68, 80, 88, 100, 100, 104, 104, 104, 104, 108, 108, 108 };
    int32_t values_align_up_8[] = { 0, 8, 8, 8, 24, 56, 56, 352, 5344, 664, 456, 240, 128, 16, 48, 56, 72, 80, 88, 104, 104, 104, 104, 104, 104, 112, 112, 112 };
    int32_t values_align_up_16[] = { 0, 16, 16, 16, 32, 64, 64, 352, 5344, 672, 464, 240, 128, 16, 48, 64, 80, 80, 96, 112, 112, 112, 112, 112, 112, 112, 112, 112 };
    int32_t values_align_up_64[] = { 0, 64, 64, 64, 64, 64, 64, 384, 5376, 704, 512, 256, 128, 64, 64, 64, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 };

    int32_t values_align_down_4[] = { 0, 0, 0, 4, 20, 52, 52, 344, 5340, 660, 456, 232, 120, 12, 44, 52, 64, 76, 88, 96, 100, 100, 100, 100, 104, 104, 104, 104 };
    int32_t values_align_down_8[] = { 0, 0, 0, 0, 16, 48, 48, 344, 5336, 656, 456, 232, 120, 8, 40, 48, 64, 72, 88, 96, 96, 96, 96, 96, 104, 104, 104, 104 };
    int32_t values_align_down_16[] = { 0, 0, 0, 0, 16, 48, 48, 336, 5328, 656, 448, 224, 112, 0, 32, 48, 64, 64, 80, 96, 96, 96, 96, 96, 96, 96, 96, 96 };
    int32_t values_align_down_64[] = { 0, 0, 0, 0, 0, 0, 0, 320, 5312, 640, 448, 192, 64, 0, 0, 0, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64 };

    for(uint32_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        TEST_ASSERT_EQUAL_INT32(values_align_up_4[i], lv_align_up(values[i], 4));
        TEST_ASSERT_EQUAL_INT32(values_align_up_8[i], lv_align_up(values[i], 8));
        TEST_ASSERT_EQUAL_INT32(values_align_up_16[i], lv_align_up(values[i], 16));
        TEST_ASSERT_EQUAL_INT32(values_align_up_64[i], lv_align_up(values[i], 64));

        TEST_ASSERT_EQUAL_INT32(values_align_down_4[i], lv_align_down(values[i], 4));
        TEST_ASSERT_EQUAL_INT32(values_align_down_8[i], lv_align_down(values[i], 8));
        TEST_ASSERT_EQUAL_INT32(values_align_down_16[i], lv_align_down(values[i], 16));
        TEST_ASSERT_EQUAL_INT32(values_align_down_64[i], lv_align_down(values[i], 64));
    }
}

#endif
