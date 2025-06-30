#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t event_count = 0;
static lv_obj_t * last_target = NULL;

static void test_event_cb(lv_event_t * e)
{
    event_count++;
    last_target = lv_event_get_current_target_obj(e);
}

void test_event_trickle_basic(void)
{
    /*Create a parent container*/
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_add_flag(parent, LV_OBJ_FLAG_EVENT_TRICKLE);
    lv_obj_add_event_cb(parent, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Create children*/
    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_add_event_cb(child1, test_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * child2 = lv_obj_create(parent);
    lv_obj_add_event_cb(child2, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Reset counters*/
    event_count = 0;
    last_target = NULL;

    /*Send event to parent - should trickle down to children*/
    lv_obj_send_event(parent, LV_EVENT_CLICKED, NULL);

    /*Should have received 3 events: parent + 2 children*/
    TEST_ASSERT_EQUAL(3, event_count);

    /*Clean up*/
    lv_obj_delete(parent);
}

void test_event_trickle_stop(void)
{
    /*Create a parent container*/
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_add_flag(parent, LV_OBJ_FLAG_EVENT_TRICKLE);

    /*Add event handler that stops trickle down*/
    lv_obj_add_event_cb(parent, test_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(parent, (lv_event_cb_t)lv_event_stop_trickling, LV_EVENT_CLICKED, NULL);

    /*Create children*/
    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_add_event_cb(child1, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Reset counters*/
    event_count = 0;
    last_target = NULL;

    /*Send event to parent - should NOT trickle down due to stop*/
    lv_obj_send_event(parent, LV_EVENT_CLICKED, NULL);

    /*Should have received only 1 event: parent only*/
    TEST_ASSERT_EQUAL(1, event_count); /* parent event handler + stop handler */

    /*Clean up*/
    lv_obj_delete(parent);
}

void test_event_trickle_disabled(void)
{
    /*Create a parent container WITHOUT trickle down flag*/
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_add_event_cb(parent, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Create children*/
    lv_obj_t * child1 = lv_obj_create(parent);
    lv_obj_add_event_cb(child1, test_event_cb, LV_EVENT_CLICKED, NULL);

    /*Reset counters*/
    event_count = 0;
    last_target = NULL;

    /*Send event to parent - should NOT trickle down*/
    lv_obj_send_event(parent, LV_EVENT_CLICKED, NULL);

    /*Should have received only 1 event: parent only*/
    TEST_ASSERT_EQUAL(1, event_count);

    /*Clean up*/
    lv_obj_delete(parent);
}

#endif