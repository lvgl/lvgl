#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_test_display_has_a_buffer_of_its_own(void)
{
    lv_display_t * first = lv_display_get_default();
    lv_draw_buf_t * first_buf = lv_display_get_buf_active(first);
    int32_t first_w = first_buf->header.w;

    lv_display_t * second = lv_test_display_create(50, 40);
    lv_draw_buf_t * second_buf = lv_display_get_buf_active(second);
    TEST_ASSERT_NOT_EQUAL(first_buf, second_buf);
    TEST_ASSERT_EQUAL_PTR(first_buf, lv_display_get_buf_active(first));
    TEST_ASSERT_EQUAL_INT32(first_w, first_buf->header.w);
    TEST_ASSERT_EQUAL_INT32(50, second_buf->header.w);
    TEST_ASSERT_EQUAL_INT32(40, second_buf->header.h);

    lv_display_delete(second);
}

/* Under ASan: drawing into a freed buffer, then freeing it twice at lv_deinit, would abort */
void test_deleting_a_second_test_display_keeps_the_first_drawing(void)
{
    lv_display_t * first = lv_display_get_default();
    lv_display_t * second = lv_test_display_create(50, 40);
    lv_display_delete(second);
    lv_display_set_default(first);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 100, 100);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x2060c0), 0);
    lv_refr_now(first);

    lv_draw_buf_t * buf = lv_display_get_buf_active(first);
    uint32_t px = ((uint32_t *)lv_draw_buf_goto_xy(buf, 50, 50))[0];
    TEST_ASSERT_EQUAL_HEX32(0x2060c0, px & 0xffffff);
}

void test_resizing_a_second_test_display_leaves_the_first(void)
{
    lv_display_t * first = lv_display_get_default();
    lv_draw_buf_t * first_buf = lv_display_get_buf_active(first);
    int32_t first_w = first_buf->header.w;

    lv_display_t * second = lv_test_display_create(50, 40);
    lv_display_set_resolution(second, 60, 30);
    TEST_ASSERT_EQUAL_PTR(first_buf, lv_display_get_buf_active(first));
    TEST_ASSERT_EQUAL_INT32(first_w, first_buf->header.w);
    TEST_ASSERT_EQUAL_INT32(60, lv_display_get_buf_active(second)->header.w);

    lv_display_delete(second);
    lv_display_set_default(first);
}

#endif
