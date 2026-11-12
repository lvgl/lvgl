#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#if LV_USE_FLOAT

static void check_format(const char * expected, const char * fmt, double value)
{
    char buf[32];
    lv_snprintf(buf, sizeof(buf), fmt, value);
    TEST_ASSERT_EQUAL_STRING(expected, buf);
}

void test_snprintf_exponent_of_zero(void)
{
    check_format("0.000000e+00", "%e", 0.0);
    check_format("+0.000E+00", "%+.3E", 0.0);
}

void test_snprintf_exponent_mantissa_below_ten(void)
{
    check_format("1.000000e+08", "%e", 1e8);
    check_format("1.000000e+01", "%e", 9.9999996);
    check_format("1.000e+09", "%.3e", 999999999.5);
    check_format("1e+06", "%.0e", 999999.5);
}

void test_snprintf_exponent_negative(void)
{
    check_format("9.913087E-06", "%E", 9.913086719800596e-06);
    check_format("-2.500000e-03", "%e", -0.0025);
    check_format("1.000000e-279", "%e", 1e-279);
    check_format("1.0000000e-45", "%.7e", 1e-45);
}

#endif

#endif
