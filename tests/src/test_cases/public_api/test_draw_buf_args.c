#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/*A coordinate outside the buffer has no address to report*/
void test_draw_buf_args_goto_xy_out_of_range(void)
{
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 100, 100, LV_COLOR_FORMAT_RGB565);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

    TEST_ASSERT_NULL(lv_draw_buf_goto_xy(&draw_buf, 100, 100));
    TEST_ASSERT_NULL(lv_draw_buf_goto_xy(&draw_buf, -10, -10));
}

#endif /*LV_BUILD_TEST*/
