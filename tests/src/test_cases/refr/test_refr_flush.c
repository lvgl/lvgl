/**
 * Flushing the rendered areas and the events around it
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

void test_refr_without_active_screen(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_obj_t * scr = refr_screen();
    lv_obj_invalidate(scr);
    TEST_ASSERT_TRUE(refr_ctx.disp->inv_p > 0);

    /*Without an active screen there is nothing to render*/
    refr_ctx.disp->act_scr = NULL;
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.disp->inv_p);

    refr_ctx.disp->act_scr = scr;
}

void test_refr_nothing_to_do(void)
{
    refr_disp_create(100, 100, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 100);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    refr_log_events(refr_ctx.disp);
    refr_log_reset();

    /*Without an invalid area nothing is rendered, but the refresh events are sent*/
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_REFR_START));
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_REFR_READY));
    TEST_ASSERT_FALSE(refr_event_is_seen(LV_EVENT_RENDER_START));
    TEST_ASSERT_FALSE(refr_event_is_seen(LV_EVENT_RENDER_READY));
}

void test_refr_refreshed_area_is_stored(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 16);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    lv_area_t area = {8, 8, 23, 23};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_TRUE(lv_area_is_equal(&area, &refr_ctx.disp->refreshed_area));
}

void test_refr_flush_area_carries_the_display_offset(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_physical_resolution(refr_ctx.disp, 128, 128);
    lv_display_set_offset(refr_ctx.disp, 5, 7);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_area_t area = {10, 20, 29, 39};
    lv_area_t expected = {15, 27, 34, 46};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.flush_area[0]));
}

void test_refr_event_order(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    refr_log_events(refr_ctx.disp);
    refr_log_reset();

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_event_get_count(LV_EVENT_REFR_START));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_get_count(LV_EVENT_RENDER_START));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_get_count(LV_EVENT_FLUSH_START));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_get_count(LV_EVENT_FLUSH_FINISH));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_get_count(LV_EVENT_RENDER_READY));
    TEST_ASSERT_EQUAL_UINT32(1, refr_event_get_count(LV_EVENT_REFR_READY));

    TEST_ASSERT_TRUE(refr_event_get_index(LV_EVENT_REFR_START) < refr_event_get_index(LV_EVENT_RENDER_START));
    TEST_ASSERT_TRUE(refr_event_get_index(LV_EVENT_RENDER_START) < refr_event_get_index(LV_EVENT_FLUSH_START));
    TEST_ASSERT_TRUE(refr_event_get_index(LV_EVENT_FLUSH_START) < refr_event_get_index(LV_EVENT_FLUSH_FINISH));
    TEST_ASSERT_TRUE(refr_event_get_index(LV_EVENT_FLUSH_FINISH) < refr_event_get_index(LV_EVENT_RENDER_READY));
    TEST_ASSERT_TRUE(refr_event_get_index(LV_EVENT_RENDER_READY) < refr_event_get_index(LV_EVENT_REFR_READY));
}

void test_refr_flush_wait_events(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);
    refr_ctx.defer_flush_ready = true;
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    refr_log_events(refr_ctx.disp);
    refr_log_reset();

    lv_obj_invalidate(refr_screen());
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_wait_cnt);
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_FLUSH_WAIT_START));
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_FLUSH_WAIT_FINISH));
    /*The frame is complete, so the driver was told that this was the last flush*/
    TEST_ASSERT_EQUAL(1, refr_ctx.disp->flushing_last);
}

void test_refr_flush_wait_is_skipped_when_not_flushing(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*The flush callback reports readiness right away, so there is nothing to wait for*/
    lv_obj_invalidate(refr_screen());
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_wait_cnt);
}

void test_refr_without_flush_cb(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_display_set_flush_cb(refr_ctx.disp, NULL);
    lv_display_set_flush_wait_cb(refr_ctx.disp, refr_flush_wait_cb);

    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Rendering still happens, only the flushing is skipped*/
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_buf_px(refr_ctx.buf[0], 30, 30));
}

#endif /*LV_BUILD_TEST*/
