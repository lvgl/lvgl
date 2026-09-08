#if LV_BUILD_TEST

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

#if LV_USE_ST7796
/*ST7796 has no gamma curve support, so the setter is a no-op even with a
 *display; with a NULL one there is nothing to reject either*/
void test_lcd_args_st7796_set_gamma_curve_null_display(void)
{
    lv_st7796_set_gamma_curve(NULL, 0);
    TEST_PASS();
}
#else
void test_lcd_args_st7796_set_gamma_curve_null_display(void) { }
#endif /*LV_USE_ST7796*/

#endif /*LV_BUILD_TEST*/
