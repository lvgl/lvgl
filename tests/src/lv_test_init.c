#if LV_BUILD_TEST
#include "lv_test_init.h"
#include "lv_test_indev.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../unity/unity.h"

#define HOR_RES 800
#define VER_RES 480

static void hal_init(void);
static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static void test_log_print_cb(lv_log_level_t level, const char * buf);

uint8_t * last_flushed_buf;
lv_indev_t * lv_test_mouse_indev;
lv_indev_t * lv_test_keypad_indev;
lv_indev_t * lv_test_encoder_indev;

void lv_test_init(void)
{
    lv_init();

    lv_log_register_print_cb(test_log_print_cb);

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    /* Disable profiler, to reduce redundant profiler log printing  */
    lv_profiler_builtin_set_enable(false);
#endif

    hal_init();
#if LV_USE_SYSMON
#if LV_USE_MEM_MONITOR
    lv_sysmon_hide_memory(NULL);
#endif
#if LV_USE_PERF_MONITOR
    lv_sysmon_hide_performance(NULL);
#endif
#endif
}

void lv_test_deinit(void)
{
    lv_mem_deinit();
}

static void color_format_changled_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(NULL);

    lv_display_set_buffers(disp, lv_draw_buf_align(draw_buf->unaligned_data, cf), NULL, draw_buf->data_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);

}

static void hal_init(void)
{

    static lv_color32_t test_fb[(HOR_RES + LV_DRAW_BUF_STRIDE_ALIGN - 1) * VER_RES + LV_DRAW_BUF_ALIGN];
    lv_display_t * disp = lv_display_create(HOR_RES, VER_RES);
    lv_display_set_buffers(disp, lv_draw_buf_align(test_fb, LV_COLOR_FORMAT_ARGB8888), NULL, HOR_RES * VER_RES * 4,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, dummy_flush_cb);
    lv_display_add_event_cb(disp, color_format_changled_event_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    lv_test_mouse_indev = lv_indev_create();
    lv_indev_set_type(lv_test_mouse_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lv_test_mouse_indev,  lv_test_mouse_read_cb);

    lv_test_keypad_indev = lv_indev_create();
    lv_indev_set_type(lv_test_keypad_indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(lv_test_keypad_indev,  lv_test_keypad_read_cb);

    lv_test_encoder_indev = lv_indev_create();
    lv_indev_set_type(lv_test_encoder_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(lv_test_encoder_indev,  lv_test_encoder_read_cb);
}

static void dummy_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);
    last_flushed_buf = color_p;
    lv_display_flush_ready(disp);
}

static void test_log_print_cb(lv_log_level_t level, const char * buf)
{
    if(level < LV_LOG_LEVEL_WARN) {
        return;
    }

    TEST_PRINTF("%s", buf);
}

void lv_test_assert_fail(void)
{
    /*Flush the output*/
    fflush(stdout);

    /*Handle error on test*/
    assert(false);
}

#endif
