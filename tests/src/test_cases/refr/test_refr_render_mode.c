/**
 * Splitting the invalidated areas in the partial, direct and full render modes
 */
#if LV_BUILD_TEST

#include "refr/lv_test_refr.h"

void setUp(void)
{
    refr_ctx_reset();
}

void tearDown(void)
{
    refr_disp_delete();
}

void test_refr_partial_mode_splits_the_area(void)
{
    const int32_t w = 100;
    const int32_t h = 100;
    const int32_t buf_h = 10;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, buf_h);
    refr_screen_set_color(REFR_COLOR_BLUE);
    refr_frame();
    refr_log_reset();

    uint32_t max_row = refr_max_row_of(w, h);
    TEST_ASSERT_LESS_THAN_UINT32((uint32_t)h, max_row);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*The screen does not fit into the buffer, so it is rendered in chunks*/
    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, max_row);
    LV_UNUSED(buf_h);

    /*The chunks together make up the whole screen*/
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(0, 0));
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(w - 1, h - 1));
    ASSERT_PX_EQ(REFR_COLOR_BLUE, refr_screen_px(w / 2, h / 2));
}

void test_refr_partial_mode_handles_the_remainder_rows(void)
{
    const int32_t w = 100;
    const int32_t h = 95;
    const int32_t buf_h = 10;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, buf_h);
    refr_screen_set_color(REFR_COLOR_GREEN);
    refr_frame();
    refr_log_reset();

    uint32_t max_row = refr_max_row_of(w, h);
    TEST_ASSERT_TRUE(max_row < (uint32_t)h);
    /*The last chunk is smaller than the others*/
    TEST_ASSERT_NOT_EQUAL(0, (uint32_t)h % max_row);

    lv_obj_invalidate(refr_screen());
    refr_frame();

    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, max_row);
    TEST_ASSERT_EQUAL_INT32((int32_t)((uint32_t)h % max_row),
                            lv_area_get_height(&refr_ctx.flush_area[refr_ctx.flush_cnt - 1]));

    ASSERT_PX_EQ(REFR_COLOR_GREEN, refr_screen_px(w - 1, h - 1));
    LV_UNUSED(buf_h);
}

void test_refr_partial_mode_small_area_is_not_split(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 10);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*An area that fits in the buffer is rendered in one go*/
    lv_area_t area = {10, 10, 59, 14};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&area, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(refr_ctx.flush_is_last[0]);
}

void test_refr_partial_mode_rounding_event_limits_the_rows(void)
{
    const int32_t w = 100;
    const int32_t h = 100;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 10);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    uint32_t max_row = refr_max_row_of(w, h);
    TEST_ASSERT_TRUE(max_row > 2);

    /*Drivers can round the rendered area. Rounding the height up to a multiple
     *that does not fit twice leaves exactly that many usable rows.*/
    uint32_t rounded_max_row = max_row - 1;
    refr_set_row_rounding((int32_t)rounded_max_row);
    lv_display_add_event_cb(refr_ctx.disp, refr_row_rounding_cb, LV_EVENT_INVALIDATE_AREA, NULL);
    refr_log_reset();

    lv_obj_invalidate(refr_screen());
    refr_frame();

    lv_area_t screen_area = {0, 0, w - 1, h - 1};
    refr_check_flushed_chunks(&screen_area, rounded_max_row);
}

void test_refr_partial_mode_narrow_area_gets_more_rows(void)
{
    const int32_t w = 100;
    const int32_t h = 200;
    refr_disp_create(w, h, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 10);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*A narrow area needs less memory per row, so more rows fit into the buffer*/
    const int32_t narrow_w = w / 4;
    lv_area_t narrow = {0, 0, narrow_w - 1, h - 1};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &narrow);
    refr_frame();

    uint32_t narrow_max_row = refr_max_row_of(narrow_w, h);
    uint32_t full_max_row = refr_max_row_of(w, h);
    TEST_ASSERT_TRUE(narrow_max_row > full_max_row);
    refr_check_flushed_chunks(&narrow, narrow_max_row);
}

void test_refr_direct_mode_renders_every_area_separately(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_area_t a1 = {0, 0, 9, 9};
    lv_area_t a2 = {90, 90, 99, 99};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    /*No splitting in direct mode, one flush per invalid area*/
    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(lv_area_is_equal(&a2, &refr_ctx.flush_area[1]));
}

void test_refr_full_mode_always_renders_the_whole_screen(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_FULL, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    refr_frame();

    lv_area_t full = {0, 0, 99, 99};
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&full, &refr_ctx.flush_area[0]));
    TEST_ASSERT_TRUE(refr_ctx.flush_is_last[0]);

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_screen_px(15, 15));
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_screen_px(90, 90));
}

#endif /*LV_BUILD_TEST*/
