#if LV_BUILD_TEST
#include "../lvgl.h"

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

#endif
