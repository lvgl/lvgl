#if LV_BUILD_TEST || LV_BUILD_TEST_PERF
#include "lv_test_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../unity/unity.h"

#define HOR_RES 800
#define VER_RES 480

static void test_log_print_cb(lv_log_level_t level, const char * buf);

void lv_test_init(void)
{
    lv_init();

    lv_log_register_print_cb(test_log_print_cb);

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    /* Disable profiler, to reduce redundant profiler log printing  */
    lv_profiler_builtin_set_enable(false);
#endif

    lv_test_display_create(HOR_RES, VER_RES);
    lv_test_indev_create_all();

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
    lv_deinit();
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
