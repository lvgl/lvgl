#if LV_BUILD_TEST || LV_BUILD_TEST_PERF
#include "lv_test_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../unity/unity.h"

static void test_log_print_cb(lv_log_level_t level, const char * buf);

void lv_test_init(void)
{
    lv_init();

    lv_log_register_print_cb(test_log_print_cb);

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    /* Disable profiler, to reduce redundant profiler log printing  */
    lv_profiler_builtin_set_enable(false);
#endif

#if defined(LV_USE_DRAW_NANOVG) && LV_USE_DRAW_NANOVG && defined(LV_USE_NANOVG_TEST_HEADLESS) && LV_USE_NANOVG_TEST_HEADLESS
    lv_display_t * egl_disp = lv_test_display_egl_create(LV_TEST_DISPLAY_HOR_RES, LV_TEST_DISPLAY_VER_RES);
    /* LV_ASSERT_MSG is always compiled in (unlike assert() under NDEBUG) and
     * triggers LV_ASSERT_HANDLER, so a fatal setup failure is reported reliably. */
    LV_ASSERT_MSG(egl_disp != NULL, "EGL headless display creation failed");
#else
    lv_test_display_create(LV_TEST_DISPLAY_HOR_RES, LV_TEST_DISPLAY_VER_RES);
#endif
    lv_test_indev_create_all();
    lv_test_fs_init();

#if LV_USE_GESTURE_RECOGNITION
    lv_test_indev_gesture_create();
#endif

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
#if LV_USE_GESTURE_RECOGNITION
    lv_test_indev_gesture_delete();
#endif
    lv_test_indev_delete_all();

#if defined(LV_USE_DRAW_NANOVG) && LV_USE_DRAW_NANOVG && defined(LV_USE_NANOVG_TEST_HEADLESS) && LV_USE_NANOVG_TEST_HEADLESS
    /* Capture the EGL context before lv_deinit() deletes the display, then release
     * the EGL/GL resources after lv_deinit() has destroyed the NanoVG draw unit. */
    void * egl_ctx = lv_display_get_driver_data(lv_display_get_default());
    lv_deinit();
    lv_test_display_egl_cleanup(egl_ctx);
#else
    lv_deinit();
#endif
}

static void test_log_print_cb(lv_log_level_t level, const char * buf)
{
    if(level < LV_LOG_LEVEL_WARN) {
        return;
    }

    TEST_PRINTF(buf);
}

void lv_test_assert_fail(void)
{
    /*Flush the output*/
    fflush(stdout);

    /*Handle error on test*/
    assert(false);
}

#endif
