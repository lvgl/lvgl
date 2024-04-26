#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "lv_test_indev.h"

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

#endif
