#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

/* The precision of an integer conversion is the minimum number of digits, the '-'
 * flag only pads the field on the right. Therefore the '-' flag must not remove the
 * leading zeros added by the precision. */
void test_sprintf_int_precision_with_left_align(void)
{
    char buf[32];

    lv_snprintf(buf, sizeof(buf), "%-8.5d", 42);
    TEST_ASSERT_EQUAL_STRING("00042   ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5d", -42);
    TEST_ASSERT_EQUAL_STRING("-00042  ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5d", 0);
    TEST_ASSERT_EQUAL_STRING("00000   ", buf);

    lv_snprintf(buf, sizeof(buf), "%-10.3d", 5);
    TEST_ASSERT_EQUAL_STRING("005       ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5u", 42);
    TEST_ASSERT_EQUAL_STRING("00042   ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5x", 0x2a);
    TEST_ASSERT_EQUAL_STRING("0002a   ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5X", 0x2a);
    TEST_ASSERT_EQUAL_STRING("0002A   ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5o", 042);
    TEST_ASSERT_EQUAL_STRING("00042   ", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.5b", 42);
    TEST_ASSERT_EQUAL_STRING("101010  ", buf);

    lv_snprintf(buf, sizeof(buf), "%-10.8b", 42);
    TEST_ASSERT_EQUAL_STRING("00101010  ", buf);

    lv_snprintf(buf, sizeof(buf), "%-#8.5x", 0x2a);
    TEST_ASSERT_EQUAL_STRING("0x0002a ", buf);

    lv_snprintf(buf, sizeof(buf), "%-12.5ld", 42L);
    TEST_ASSERT_EQUAL_STRING("00042       ", buf);

    lv_snprintf(buf, sizeof(buf), "%-12.5lld", 42LL);
    TEST_ASSERT_EQUAL_STRING("00042       ", buf);
}

void test_sprintf_int_precision_with_left_align_shorter_than_value(void)
{
    char buf[32];

    lv_snprintf(buf, sizeof(buf), "%-10.2d", 12345);
    TEST_ASSERT_EQUAL_STRING("12345     ", buf);

    lv_snprintf(buf, sizeof(buf), "%-6.2d", -12345);
    TEST_ASSERT_EQUAL_STRING("-12345", buf);

    lv_snprintf(buf, sizeof(buf), "%-4.3d", 12345);
    TEST_ASSERT_EQUAL_STRING("12345", buf);
}

void test_sprintf_int_precision_without_left_align(void)
{
    char buf[32];

    lv_snprintf(buf, sizeof(buf), "%.5d", 42);
    TEST_ASSERT_EQUAL_STRING("00042", buf);

    lv_snprintf(buf, sizeof(buf), "%8.5d", 42);
    TEST_ASSERT_EQUAL_STRING("   00042", buf);

    lv_snprintf(buf, sizeof(buf), "%05d", 42);
    TEST_ASSERT_EQUAL_STRING("00042", buf);

    lv_snprintf(buf, sizeof(buf), "%-05d", 42);
    TEST_ASSERT_EQUAL_STRING("42   ", buf);

    lv_snprintf(buf, sizeof(buf), "%.0d", 0);
    TEST_ASSERT_EQUAL_STRING("", buf);

    lv_snprintf(buf, sizeof(buf), "%-8.3s", "abcdef");
    TEST_ASSERT_EQUAL_STRING("abc     ", buf);
}

/* The '#' (alternate form) of an octal conversion adds a leading zero, but it must
 * not add a second one once the precision has already made the number start with a
 * zero. */
void test_sprintf_octal_alternate_form_with_precision(void)
{
    char buf[32];

    lv_snprintf(buf, sizeof(buf), "%#o", 042);
    TEST_ASSERT_EQUAL_STRING("042", buf);

    lv_snprintf(buf, sizeof(buf), "%#.3o", 042);
    TEST_ASSERT_EQUAL_STRING("042", buf);

    lv_snprintf(buf, sizeof(buf), "%#.5o", 042);
    TEST_ASSERT_EQUAL_STRING("00042", buf);

    lv_snprintf(buf, sizeof(buf), "%#8o", 042);
    TEST_ASSERT_EQUAL_STRING("     042", buf);

    lv_snprintf(buf, sizeof(buf), "%#-8.5o", 042);
    TEST_ASSERT_EQUAL_STRING("00042   ", buf);

    lv_snprintf(buf, sizeof(buf), "%#.5o", 0);
    TEST_ASSERT_EQUAL_STRING("00000", buf);
}

/* The sign and the alternate-form prefix are written after the precision padding, so
 * room for them must be kept. Otherwise a large precision fills the conversion buffer
 * and the sign disappears, turning a negative number into a positive looking one. */
void test_sprintf_precision_larger_than_conversion_buffer(void)
{
    char buf[40];

    lv_snprintf(buf, sizeof(buf), "%-20.20d", -42);
    TEST_ASSERT_EQUAL_STRING("-00000000000000000042", buf);

    lv_snprintf(buf, sizeof(buf), "%-31.31d", -42);
    TEST_ASSERT_EQUAL_STRING("-0000000000000000000000000000042", buf);

    /* The sign must survive a precision that reaches the end of the conversion
     * buffer. The number of leading zeros is backend dependent (the C library has no
     * fixed conversion buffer), so only the sign is required here. */
    lv_snprintf(buf, sizeof(buf), "%-32.32d", -42);
    TEST_ASSERT_EQUAL_CHAR('-', buf[0]);

    lv_snprintf(buf, sizeof(buf), "%#-32.32x", 0x2a);
    TEST_ASSERT_EQUAL_CHAR('0', buf[0]);
    TEST_ASSERT_EQUAL_CHAR('x', buf[1]);

#if LV_USE_STDLIB_SPRINTF == LV_STDLIB_BUILTIN
    /* The builtin reserves room for the sign and the prefix, so the digits are bounded
     * by PRINTF_NTOA_BUFFER_SIZE and the precision cannot exceed it. */
    lv_snprintf(buf, sizeof(buf), "%-32.32d", -42);
    TEST_ASSERT_EQUAL_STRING("-0000000000000000000000000000042", buf);

    lv_snprintf(buf, sizeof(buf), "%#-32.32x", 0x2a);
    TEST_ASSERT_EQUAL_STRING("0x00000000000000000000000000002a", buf);
#endif
}

#endif
