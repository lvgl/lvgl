#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

#define MOCK_REFRESH_TIME 10
#define MOCK_RENDER_TIME 20

static lv_sysmon_perf_t * basic;

static void mock_refresh(void)
{
    lv_display_send_event(lv_display_get_default(), LV_EVENT_REFR_START, NULL);
    lv_tick_inc(MOCK_REFRESH_TIME);
    lv_display_send_event(lv_display_get_default(), LV_EVENT_REFR_READY, NULL);
}

static void mock_render(void)
{
    lv_display_send_event(lv_display_get_default(), LV_EVENT_REFR_START, NULL);
    lv_display_send_event(lv_display_get_default(), LV_EVENT_RENDER_START, NULL);
    lv_tick_inc(MOCK_RENDER_TIME);
    lv_display_send_event(lv_display_get_default(), LV_EVENT_RENDER_READY, NULL);
    lv_display_send_event(lv_display_get_default(), LV_EVENT_REFR_READY, NULL);
}

static void mock_event(lv_event_code_t code)
{
    lv_event_t e;
    e.code = code;
    lv_sysmon_perf_event(lv_display_get_default(), &e);
}

void setUp(void)
{
    basic = lv_sysmon_perf_create(lv_display_get_default(), "test-basic", 0, 0);
    TEST_ASSERT_NOT_NULL(basic);
}

void tearDown(void)
{
    lv_sysmon_perf_destroy(basic);
}

void test_perf_monitor_start_stop(void)
{
    lv_result_t res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_stop(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(0, data->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(0, data->overall.measured.render_cnt);
    TEST_ASSERT_EQUAL(0, data->overall.calculated.duration);
}

void test_perf_monitor_refresh(void)
{
    lv_result_t res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    mock_refresh();

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_stop(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(1, data->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(MOCK_REFRESH_TIME, data->overall.measured.refr_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_REFRESH_TIME, data->overall.calculated.duration);
    TEST_ASSERT_EQUAL(MOCK_REFRESH_TIME, data->overall.calculated.refr_avg_time);
}

void test_perf_monitor_render(void)
{
    lv_result_t res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    mock_render();

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_stop(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(1, data->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(1, data->overall.measured.render_cnt);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.measured.render_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.measured.refr_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.calculated.duration);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.calculated.render_avg_time);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.calculated.refr_avg_time);
}

void test_perf_monitor_real_refresh(void)
{
    lv_result_t res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    lv_refr_now(NULL);

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_stop(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(1, data->overall.measured.refr_cnt);
}

void test_perf_monitor_fps(void)
{
    lv_result_t res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    mock_refresh();
    mock_render();
    lv_tick_inc(100 - MOCK_REFRESH_TIME - MOCK_RENDER_TIME);

    mock_refresh();
    mock_render();
    lv_tick_inc(100 - MOCK_REFRESH_TIME - MOCK_RENDER_TIME);

    /* Now we have 4 refreshes and 2 renders over 200 ticks */

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_stop(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(20, data->overall.calculated.fps);
    TEST_ASSERT_EQUAL(200, data->overall.calculated.duration);
    TEST_ASSERT_EQUAL((lv_value_precise_t)(MOCK_REFRESH_TIME + MOCK_RENDER_TIME) / 2,
                      data->overall.calculated.refr_avg_time);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.calculated.render_avg_time);
    TEST_ASSERT_EQUAL(4, data->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(2, data->overall.measured.render_cnt);
}

void test_perf_monitor_reset_data(void)
{
    lv_result_t res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    mock_refresh();
    mock_render();
    lv_tick_inc(100 - MOCK_REFRESH_TIME - MOCK_RENDER_TIME);

    const lv_sysmon_perf_data_t * data = lv_sysmon_perf_get_data(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(2, data->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(1, data->overall.measured.render_cnt);
    TEST_ASSERT_EQUAL(MOCK_REFRESH_TIME + MOCK_RENDER_TIME, data->overall.measured.refr_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.measured.render_elaps_sum);
    TEST_ASSERT_EQUAL(100, data->overall.calculated.duration);
    TEST_ASSERT_EQUAL((lv_value_precise_t)(MOCK_REFRESH_TIME + MOCK_RENDER_TIME) / 2,
                      data->overall.calculated.refr_avg_time);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data->overall.calculated.render_avg_time);
    TEST_ASSERT_EQUAL(20, data->overall.calculated.fps);

    lv_sysmon_perf_reset_data(basic, LV_SYSMON_PERF_TYPE_OVERALL);
    data = lv_sysmon_perf_stop(basic);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(0, data->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(0, data->overall.measured.render_cnt);
    TEST_ASSERT_EQUAL(0, data->overall.calculated.duration);
    TEST_ASSERT_EQUAL(0, data->overall.calculated.refr_avg_time);
    TEST_ASSERT_EQUAL(0, data->overall.calculated.render_avg_time);
    TEST_ASSERT_EQUAL(0, data->overall.calculated.fps);
}

void test_perf_monitor_scroll(void)
{
    lv_sysmon_perf_t * scroll = lv_sysmon_perf_create(lv_display_get_default(), "test-scroll", 0, 1);
    TEST_ASSERT_NOT_NULL(scroll);
    lv_result_t res = lv_sysmon_perf_start(scroll);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    res = lv_sysmon_perf_start(basic);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);

    mock_refresh();
    lv_tick_inc(100 - MOCK_REFRESH_TIME);

    mock_event(LV_EVENT_SCROLL_BEGIN); /* Scroll begin */

    mock_render();
    lv_tick_inc(100 - MOCK_RENDER_TIME);

    mock_event(LV_EVENT_SCROLL_END); /* Scroll end */

    /* Check overall data */

    const lv_sysmon_perf_data_t * data_basic = lv_sysmon_perf_stop(basic);
    const lv_sysmon_perf_data_t * data_scroll = lv_sysmon_perf_stop(scroll);
    TEST_ASSERT_NOT_NULL(data_basic);
    TEST_ASSERT_NOT_NULL(data_scroll);
    TEST_ASSERT_EQUAL(2, data_basic->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(2, data_scroll->overall.measured.refr_cnt);
    TEST_ASSERT_EQUAL(MOCK_REFRESH_TIME + MOCK_RENDER_TIME, data_basic->overall.measured.refr_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_REFRESH_TIME + MOCK_RENDER_TIME, data_scroll->overall.measured.refr_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data_basic->overall.measured.render_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data_scroll->overall.measured.render_elaps_sum);
    TEST_ASSERT_EQUAL(200, data_basic->overall.calculated.duration);
    TEST_ASSERT_EQUAL(200, data_scroll->overall.calculated.duration);
    TEST_ASSERT_EQUAL((lv_value_precise_t)(MOCK_REFRESH_TIME + MOCK_RENDER_TIME) / 2,
                      data_basic->overall.calculated.refr_avg_time);
    TEST_ASSERT_EQUAL((lv_value_precise_t)(MOCK_REFRESH_TIME + MOCK_RENDER_TIME) / 2,
                      data_scroll->overall.calculated.refr_avg_time);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data_basic->overall.calculated.render_avg_time);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, data_scroll->overall.calculated.render_avg_time);
    TEST_ASSERT_EQUAL(10, data_basic->overall.calculated.fps);
    TEST_ASSERT_EQUAL(10, data_scroll->overall.calculated.fps);

    /* Check scroll data */

    TEST_ASSERT_NULL(data_basic->scrolls);
    TEST_ASSERT_NOT_NULL(data_scroll->scrolls);
    TEST_ASSERT_EQUAL(1, lv_circle_buf_size(data_scroll->scrolls));
    lv_sysmon_perf_info_t * info = lv_circle_buf_head(data_scroll->scrolls);
    TEST_ASSERT_NOT_NULL(info);
    TEST_ASSERT_EQUAL(1, info->measured.refr_cnt);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, info->measured.refr_elaps_sum);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, info->measured.render_elaps_sum);
    TEST_ASSERT_EQUAL(100, info->calculated.duration);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, info->calculated.refr_avg_time);
    TEST_ASSERT_EQUAL(MOCK_RENDER_TIME, info->calculated.render_avg_time);
    TEST_ASSERT_EQUAL(10, info->calculated.fps);

    /* Done */

    lv_sysmon_perf_destroy(scroll);
}

#endif /* LV_BUILD_TEST */
