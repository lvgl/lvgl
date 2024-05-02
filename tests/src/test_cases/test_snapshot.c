#if LV_BUILD_TEST
#include "../lvgl.h"

#if LV_USE_SNAPSHOT

#include "unity/unity.h"

#define NUM_SNAPSHOTS 1

void test_snapshot_should_not_leak_memory(void)
{
    uint32_t idx = 0;
    size_t initial_available_memory = 0;
    size_t final_available_memory = 0;
    lv_mem_monitor_t monitor;

    lv_draw_buf_t * snapshots[NUM_SNAPSHOTS] = {NULL};

    lv_mem_monitor(&monitor);
    initial_available_memory = monitor.free_size;

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        snapshots[idx] = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_NATIVE_WITH_ALPHA);
        TEST_ASSERT_NOT_NULL(snapshots[idx]);
    }

    for(idx = 0; idx < NUM_SNAPSHOTS; idx++) {
        lv_draw_buf_destroy(snapshots[idx]);
    }

    lv_mem_monitor(&monitor);
    final_available_memory = monitor.free_size;

    TEST_ASSERT_EQUAL(initial_available_memory, final_available_memory);
}

void test_snapshot_take_snapshot_immidiately_after_obj_create(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
    lv_label_set_text(label, "Wubba lubba dub dub!");

    lv_draw_buf_t * draw_dsc = lv_snapshot_take(label, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_t * img_obj = lv_image_create(lv_screen_active());
    lv_image_set_src(img_obj, draw_dsc);

    lv_obj_delete(label);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_0.png");

    lv_obj_center(img_obj);
    lv_image_set_inner_align(img_obj, LV_IMAGE_ALIGN_CENTER);
    lv_image_set_rotation(img_obj, 450);

    TEST_ASSERT_EQUAL_SCREENSHOT("snapshot_1.png");
}

#else /*LV_USE_SNAPSHOT*/

void test_snapshot_should_not_leak_memory(void)
{

}

#endif

#endif
