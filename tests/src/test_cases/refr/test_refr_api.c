/**
 * The entry points of the refreshing and the state they expose
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

void test_refr_now_refreshes_the_given_display(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_log_reset();

    lv_refr_now(refr_ctx.disp);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);

    /*Nothing left to do*/
    refr_log_reset();
    lv_refr_now(refr_ctx.disp);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

static uint32_t flush_count = 0;
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px)
{
    LV_UNUSED(disp);
    LV_UNUSED(px);
    LV_UNUSED(area);
    flush_count++;
}

void test_refr_now_refreshes_every_display(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_log_reset();

    /* dummy second display*/
    const size_t WIDTH = 64;
    const size_t HEIGHT = 64;
    size_t buf_size = 4 * (WIDTH + LV_DRAW_BUF_STRIDE_ALIGN - 1) * HEIGHT + LV_DRAW_BUF_ALIGN;
    uint8_t * buf = malloc(buf_size);
    uint8_t * aligned = lv_draw_buf_align(buf, LV_COLOR_FORMAT_XRGB8888);
    lv_display_t * disp = lv_display_create(WIDTH, HEIGHT);
    lv_display_set_buffers(disp, aligned, NULL, buf_size, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_obj_invalidate(lv_display_get_screen_active(disp));

    flush_count = 0;
    lv_refr_now(NULL);
    free(buf);
    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, flush_count);
}

void test_refr_now_does_not_refresh_without_refresh_timer(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_display_delete_refr_timer(refr_ctx.disp);
    TEST_ASSERT_NULL(lv_display_get_refr_timer(refr_ctx.disp));
    refr_log_reset();

    /*Without a refresh timer the display is never refreshed*/
    lv_refr_now(refr_ctx.disp);
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_timer_without_argument_uses_the_default_display(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();
    refr_log_reset();

    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(refr_ctx.disp);
    lv_obj_invalidate(refr_screen());
    lv_display_refr_timer(NULL);
    lv_display_set_default(disp_def);

    TEST_ASSERT_EQUAL_UINT32(1, refr_ctx.flush_cnt);
}

void test_refr_timer_without_display(void)
{
    lv_display_t * disp_def = lv_display_get_default();
    lv_display_set_default(NULL);
    /*Nothing to refresh, but it must not crash*/
    lv_display_refr_timer(NULL);
    lv_display_set_default(disp_def);
}

void test_refr_display_without_draw_buffer(void)
{
    lv_memzero(&refr_ctx, sizeof(refr_ctx));
    lv_display_t * disp = lv_display_create(64, 64);
    refr_ctx.disp = disp;
    lv_display_set_flush_cb(disp, refr_flush_cb);
    TEST_ASSERT_NULL(lv_display_get_buf_active(disp));

    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
}

void test_refr_get_and_set_disp_refreshing(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_frame();

    /*The last refreshed display is remembered*/
    TEST_ASSERT_EQUAL_PTR(refr_ctx.disp, lv_refr_get_disp_refreshing());

    lv_display_t * disp_def = lv_display_get_default();
    lv_refr_set_disp_refreshing(disp_def);
    TEST_ASSERT_EQUAL_PTR(disp_def, lv_refr_get_disp_refreshing());
}

static bool rendering_in_progress_while_drawing;

static lv_display_t * disp_refreshing_while_drawing;

static void draw_main_state_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_display_t * disp = lv_obj_get_display(obj);
    rendering_in_progress_while_drawing = disp->rendering_in_progress;
    disp_refreshing_while_drawing = lv_refr_get_disp_refreshing();
}

void test_refr_rendering_in_progress_flag(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * obj = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);

    rendering_in_progress_while_drawing = false;
    disp_refreshing_while_drawing = NULL;
    lv_obj_add_event_cb(obj, draw_main_state_cb, LV_EVENT_DRAW_MAIN, NULL);

    TEST_ASSERT_FALSE(refr_ctx.disp->rendering_in_progress);
    refr_frame();

    TEST_ASSERT_TRUE(rendering_in_progress_while_drawing);
    TEST_ASSERT_EQUAL_PTR(refr_ctx.disp, disp_refreshing_while_drawing);
    /*The flag is cleared when the rendering is over*/
    TEST_ASSERT_FALSE(refr_ctx.disp->rendering_in_progress);
}

static uint32_t refr_start_cnt;

static lv_event_code_t last_event_before_delete;

static void delete_on_refr_start_cb(lv_event_t * e)
{
    refr_start_cnt++;
    last_event_before_delete = lv_event_get_code(e);
    lv_display_delete(lv_event_get_current_target(e));
    refr_ctx.disp = NULL;
}

void test_refr_display_deleted_in_refr_start(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    refr_start_cnt = 0;
    lv_display_add_event_cb(refr_ctx.disp, delete_on_refr_start_cb, LV_EVENT_REFR_START, NULL);
    lv_display_add_event_cb(refr_ctx.disp, delete_on_refr_start_cb, LV_EVENT_REFR_READY, NULL);

    lv_display_t * disp = refr_ctx.disp;
    lv_obj_invalidate(lv_display_get_screen_active(disp));
    lv_display_refr_timer(lv_display_get_refr_timer(disp));

    /*The refresh is aborted, so LV_EVENT_REFR_READY is never sent*/
    TEST_ASSERT_EQUAL_UINT32(1, refr_start_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_REFR_START, last_event_before_delete);
    TEST_ASSERT_EQUAL_UINT32(0, refr_ctx.flush_cnt);
    TEST_ASSERT_NULL(refr_ctx.disp);
}

void test_refr_obj_redraw_invalid_arguments(void)
{
    refr_disp_create(64, 64, LV_COLOR_FORMAT_XRGB8888, LV_DISPLAY_RENDER_MODE_DIRECT, 1, 64);
    lv_layer_t layer;
    lv_obj_redraw(NULL, refr_screen());
    lv_obj_redraw(&layer, NULL);
    lv_obj_refr(NULL, refr_screen());
    lv_obj_refr(&layer, NULL);
}

#endif /*LV_BUILD_TEST*/
