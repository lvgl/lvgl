#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static void event_object_deletion_cb(const lv_obj_class_t * cls, lv_event_t * e)
{
    LV_UNUSED(cls);
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_del(lv_event_get_current_target(e));
    }
}

static const lv_obj_class_t event_object_deletion_class = {
    .event_cb = event_object_deletion_cb,
    .base_class = &lv_obj_class
};


/* Checks for memory leaks/invalid memory accesses on deleted objects */
void test_event_object_deletion(void)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&event_object_deletion_class, lv_scr_act());
    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

#endif
