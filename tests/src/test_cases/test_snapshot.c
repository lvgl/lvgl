#if LV_BUILD_TEST
#include "../lvgl.h"

#if LV_USE_SNAPSHOT

#include "unity/unity.h"

#define NUM_SNAPSHOTS 1

void test_snapshot_should_not_leak_memory(void)
{
    uint32_t idx = 0;
    uint32_t initial_available_memory = 0;
    uint32_t final_available_memory = 0;
    lv_mem_monitor_t monitor;

    lv_img_dsc_t * snapshots[NUM_SNAPSHOTS] = {NULL};

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        snapshots[idx] = lv_snapshot_take(lv_scr_act(), LV_IMG_CF_TRUE_COLOR_ALPHA);
        TEST_ASSERT_NOT_NULL(snapshots[idx]);
    }

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        lv_snapshot_free(snapshots[idx]);
    }

    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;

    TEST_ASSERT_EQUAL(initial_available_memory, final_available_memory);
}

#else /*LV_USE_SNAPSHOT*/

void test_snapshot_should_not_leak_memory(void)
{

}

#endif

#endif
