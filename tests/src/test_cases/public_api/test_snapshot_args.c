#if LV_BUILD_TEST
#include "../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_snapshot_reshape_draw_buf_invalid_params(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 50, 40);

    /* Create initial draw buffer */
    lv_draw_buf_t * draw_buf = lv_draw_buf_create(50, 40, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(draw_buf);

    /* Only test NULL buffer, not NULL object (function asserts obj != NULL internally) */
    lv_result_t result = lv_snapshot_reshape_draw_buf(obj, NULL);
    TEST_ASSERT_EQUAL(LV_RESULT_INVALID, result);

    lv_draw_buf_destroy(draw_buf);
}

#endif /*LV_BUILD_TEST*/
