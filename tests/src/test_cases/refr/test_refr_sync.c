/**
 * Keeping the draw buffers in sync in direct mode
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

void test_refr_sync_areas_of_double_buffered_direct_mode(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Render a red rectangle into one of the buffers only*/
    const lv_draw_buf_t * rendered_buf = refr_ctx.disp->buf_act;
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t obj_area = {0, 0, 19, 19};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &obj_area);
    refr_frame();
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(rendered_buf, 10, 10));

    const lv_draw_buf_t * next_buf = refr_ctx.disp->buf_act;
    TEST_ASSERT_NOT_EQUAL(rendered_buf, next_buf);
    ASSERT_PX_EQ(REFR_COLOR_BLACK, refr_buf_px(next_buf, 10, 10));

    /*Refresh a different area. Before rendering, the area updated in the previous
     *frame is copied to the buffer that is now being rendered.*/
    lv_area_t other = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &other);
    refr_frame();

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(next_buf, 10, 10));
}

void test_refr_sync_area_is_dropped_when_redrawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();
    refr_log_reset();

    /*The previous frame covered the whole screen. Everything around the newly
     *rendered area still has to be synchronized.*/
    lv_area_t area = {10, 10, 29, 29};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(4, refr_ctx.sync_cnt);
    uint32_t i;
    for(i = 0; i < refr_ctx.sync_cnt; i++) {
        lv_area_t common;
        TEST_ASSERT_FALSE(lv_area_intersect(&common, &refr_ctx.sync_area[i], &area));
    }

    refr_log_reset();
    /*Redraw exactly the same area: nothing is left to synchronize*/
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.sync_cnt);
}

void test_refr_sync_area_is_split_when_partially_redrawn(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t big = {10, 10, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &big);
    refr_frame();
    refr_log_reset();

    /*Cut a stripe out of the previous area, the rest still has to be synchronized*/
    lv_area_t stripe = {10, 20, 49, 29};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &stripe);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(2, refr_ctx.sync_cnt);
    uint32_t i;
    for(i = 0; i < refr_ctx.sync_cnt; i++) {
        /*The synchronized parts are inside the old area but outside the new one*/
        TEST_ASSERT_TRUE(lv_area_is_in(&refr_ctx.sync_area[i], &big, 0));
        lv_area_t common;
        TEST_ASSERT_FALSE(lv_area_intersect(&common, &refr_ctx.sync_area[i], &stripe));
    }
}

void test_refr_joined_areas_are_synced_once(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    /*These two areas are joined before rendering, so only the joined area is
     *remembered for the next synchronization*/
    lv_area_t a1 = {10, 10, 39, 39};
    lv_area_t a2 = {20, 20, 49, 49};
    lv_area_t joined = {10, 10, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();
    refr_log_reset();

    lv_area_t far_area = {55, 0, 63, 5};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &far_area);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&joined, &refr_ctx.sync_area[0]));
}

void test_refr_sync_area_outside_the_display_is_skipped(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_area_t area = {40, 40, 59, 59};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &area);
    refr_frame();
    refr_log_reset();

    /*The display got smaller, the remembered area is not on it anymore*/
    lv_display_set_resolution(refr_ctx.disp, 32, 32);
    refr_ctx.hor_res = 32;
    refr_ctx.ver_res = 32;

    lv_area_t small = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &small);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.sync_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
}

void test_refr_sync_cb_with_single_buffer(void)
{
    /*A sync callback is used even without double buffering*/
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_PARTIAL, 1, 16);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    lv_display_set_sync_wait_cb(refr_ctx.disp, refr_sync_wait_cb);
    refr_ctx.defer_sync_ready = true;
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t a1 = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    refr_frame();
    refr_log_reset();

    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_cnt);
    TEST_ASSERT_TRUE(lv_area_is_equal(&a1, &refr_ctx.sync_area[0]));
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_wait_cnt);
    TEST_ASSERT_EQUAL(0, refr_ctx.disp->syncing_last);
}

void test_refr_sync_events_and_offset(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 2, 64);
    lv_display_set_physical_resolution(refr_ctx.disp, 128, 128);
    lv_display_set_offset(refr_ctx.disp, 3, 4);
    lv_display_set_sync_cb(refr_ctx.disp, refr_sync_cb);
    refr_log_events(refr_ctx.disp);

    lv_area_t a1 = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    refr_frame();
    refr_log_reset();

    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.sync_cnt);
    lv_area_t expected = {3, 4, 12, 13};
    TEST_ASSERT_TRUE(lv_area_is_equal(&expected, &refr_ctx.sync_area[0]));
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_SYNC_START));
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_SYNC_FINISH));
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_SYNC_WAIT_START));
    TEST_ASSERT_TRUE(refr_event_is_seen(LV_EVENT_SYNC_WAIT_FINISH));
}

void test_refr_no_sync_without_double_buffer_or_sync_cb(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_log_events(refr_ctx.disp);
    refr_screen_set_color(REFR_COLOR_BLACK);

    lv_area_t a1 = {0, 0, 9, 9};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a1);
    refr_frame();
    refr_log_reset();

    lv_area_t a2 = {40, 40, 49, 49};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &a2);
    refr_frame();

    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.sync_cnt);
    TEST_ASSERT_FALSE(refr_event_is_seen(LV_EVENT_SYNC_START));
}

void test_refr_triple_buffer_sync(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 3, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_invalidate(refr_screen());
    refr_frame();

    /*Render a rectangle into one buffer only*/
    refr_rect_create(refr_screen(), 0, 0, 20, 20, REFR_COLOR_RED);
    lv_obj_update_layout(refr_screen());
    lv_area_t obj_area = {0, 0, 19, 19};
    refr_ctx.disp->inv_p = 0;
    lv_inv_area(refr_ctx.disp, &obj_area);
    refr_frame();

    /*Two more frames, each of them refreshing an unrelated area. The rectangle
     *has to be propagated to all of the three buffers.*/
    uint32_t i;
    for(i = 0; i < 2; i++) {
        lv_area_t other = {40, 40, 59, 59};
        refr_ctx.disp->inv_p = 0;
        lv_inv_area(refr_ctx.disp, &other);
        refr_frame();
    }

    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[0], 10, 10));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[1], 10, 10));
    ASSERT_PX_EQ(REFR_COLOR_RED, refr_buf_px(refr_ctx.buf[2], 10, 10));
}

#endif /*LV_BUILD_TEST*/
