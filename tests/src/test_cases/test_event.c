#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static void event_object_deletion_cb(const lv_obj_class_t * cls, lv_event_t * e)
{
    LV_UNUSED(cls);
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_delete(lv_event_get_target(e));
    }
}

static const lv_obj_class_t event_object_deletion_class = {
    .event_cb = event_object_deletion_cb,
    .base_class = &lv_obj_class
};

/* Checks for memory leaks/invalid memory accesses on deleted objects */
void test_event_object_deletion(void)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&event_object_deletion_class, lv_screen_active());
    lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

/* Add and then remove event should not memory leak */
void test_event_should_not_memory_lean(void)
{
    lv_mem_monitor_t monitor;
    lv_mem_monitor(&monitor);
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    size_t initial_free_size = monitor.free_size;

    for(int i = 0; i < 10; i++) {
        lv_obj_add_event_cb(obj, NULL, LV_EVENT_ALL, NULL);
    }

    lv_obj_delete(obj);

    lv_mem_monitor_t m2;
    lv_mem_monitor(&m2);
    TEST_ASSERT_LESS_OR_EQUAL_CHAR(initial_free_size, m2.free_size);
}

static uint32_t pre_cnt_1;
static uint32_t pre_cnt_2;
static uint32_t post_cnt_1;
static uint32_t post_cnt_2;
static bool pre_stop_1;
static bool post_stop_1;

static void event_pre_1_cb(lv_event_t * e)
{
    pre_cnt_1++;
    if(pre_stop_1) lv_event_stop_processing(e);
}

static void event_pre_2_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    pre_cnt_2++;
}

static void event_post_1_cb(lv_event_t * e)
{
    post_cnt_1++;
    if(post_stop_1) lv_event_stop_processing(e);
}

static void event_post_2_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    post_cnt_2++;
}

/* Add and then remove event should not memory leak */
void test_event_stop_processing(void)
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 200, 100);
    lv_obj_add_event_cb(btn, event_pre_1_cb, LV_EVENT_CLICKED | LV_EVENT_PREPROCESS, NULL);
    lv_obj_add_event_cb(btn, event_pre_2_cb, LV_EVENT_CLICKED | LV_EVENT_PREPROCESS, NULL);
    lv_obj_add_event_cb(btn, event_post_1_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn, event_post_2_cb, LV_EVENT_CLICKED, NULL);

    pre_cnt_1 = 0;
    pre_cnt_2 = 0;
    post_cnt_1 = 0;
    post_cnt_2 = 0;
    pre_stop_1 = false;
    post_stop_1 = false;
    lv_test_mouse_click_at(30, 30);
    TEST_ASSERT_EQUAL(pre_cnt_1, 1);
    TEST_ASSERT_EQUAL(pre_cnt_2, 1);
    TEST_ASSERT_EQUAL(post_cnt_1, 1);
    TEST_ASSERT_EQUAL(post_cnt_2, 1);

    pre_cnt_1 = 0;
    pre_cnt_2 = 0;
    post_cnt_1 = 0;
    post_cnt_2 = 0;
    pre_stop_1 = true;
    post_stop_1 = false;
    lv_test_mouse_click_at(30, 30);
    TEST_ASSERT_EQUAL(pre_cnt_1, 1);
    TEST_ASSERT_EQUAL(pre_cnt_2, 0);
    TEST_ASSERT_EQUAL(post_cnt_1, 0);
    TEST_ASSERT_EQUAL(post_cnt_2, 0);

    pre_cnt_1 = 0;
    pre_cnt_2 = 0;
    post_cnt_1 = 0;
    post_cnt_2 = 0;
    pre_stop_1 = false;
    post_stop_1 = true;
    lv_test_mouse_click_at(30, 30);
    TEST_ASSERT_EQUAL(pre_cnt_1, 1);
    TEST_ASSERT_EQUAL(pre_cnt_2, 1);
    TEST_ASSERT_EQUAL(post_cnt_1, 1);
    TEST_ASSERT_EQUAL(post_cnt_2, 0);
}

static uint32_t click_count = 0;
static void event_click_to_delete_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    click_count++;

    if(click_count      == 10) lv_obj_remove_event(obj, 0);
    else if(click_count == 15) lv_obj_delete(obj);
    else                       lv_obj_send_event(obj, LV_EVENT_CLICKED, NULL);
}

void test_event_delete_obj_in_recursive_event_call(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, 200, 100);
    lv_obj_add_event_cb(obj, event_click_to_delete_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(obj, NULL, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(obj, event_click_to_delete_cb, LV_EVENT_CLICKED, NULL);
    lv_test_mouse_click_at(30, 30);
}

// Test event callback function
static void test_event_cb_1(lv_event_t * e)
{
    LV_UNUSED(e);
}

void test_event_remove_event_cb(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());

    // Register the same callback function twice with different event types
    lv_obj_add_event_cb(obj, test_event_cb_1, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(obj, test_event_cb_1, LV_EVENT_PRESSED, NULL);

    // Check event count after adding
    uint32_t event_count_after_add = lv_obj_get_event_count(obj);

    // Verify that 2 events were added
    TEST_ASSERT_EQUAL_UINT32(2, event_count_after_add);

    // Remove all events with test_event_cb_1 callback
    uint32_t removed_count = lv_obj_remove_event_cb(obj, test_event_cb_1);

    // Verify that 2 events were removed
    TEST_ASSERT_EQUAL_UINT32(2, removed_count);

    // Check event count after removal
    uint32_t event_count_after_remove = lv_obj_get_event_count(obj);

    // Verify that all events were removed
    TEST_ASSERT_EQUAL_UINT32(0, event_count_after_remove);

    lv_obj_delete(obj);
}

#endif
