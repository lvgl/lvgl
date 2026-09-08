#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/*A NULL callback is nothing to call, so it is not added to the list*/
void test_event_args_add_event_cb_null_callback(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    uint32_t before = lv_obj_get_event_count(obj);

    lv_obj_add_event_cb(obj, NULL, LV_EVENT_ALL, NULL);
    TEST_ASSERT_EQUAL_UINT32(before, lv_obj_get_event_count(obj));

    /*Sending an event still works with the rejected callback absent*/
    lv_obj_send_event(obj, LV_EVENT_CLICKED, NULL);
    TEST_PASS();
}

#endif /*LV_BUILD_TEST*/
