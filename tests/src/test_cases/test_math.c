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
    x1 = fx1 * 1024;
    y1 = fy1 * 1024;
    x2 = fx2 * 1024;
    y2 = fy2 * 1024;

    for(t = 0; t <= 1; t += t_step) {
        fy = lv_cubic_bezier_f(t, fx1, fy1, fx2, fy2);
        y = lv_cubic_bezier(t * 1024, x1, y1, x2, y2);
        if(LV_ABS(fy * 1024 - y) >= ERROR_THRESHOLD) {
            return 0;
        }
    }

    return 1;
}

void test_math_cubic_bezier_result_should_be_precise(void)
{
    /*ease-in-out function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(.42, 0, .58, 1));

    /*ease-out function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(0, 0, .58, 1));

    /*ease-in function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(.42, 0, 1, 1));

    /*ease function*/
    TEST_ASSERT_TRUE(test_cubic_bezier_ease_functions(.25, .1, .25, 1));
}

#endif
