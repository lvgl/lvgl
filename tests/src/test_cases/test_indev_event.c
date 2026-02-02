#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void test_event_cb_1(lv_event_t * e)
{
    LV_UNUSED(e);
}

static void test_event_cb_2(lv_event_t * e)
{
    LV_UNUSED(e);
}

void test_indev_remove_event_all(void)
{
    lv_indev_t * indev = lv_indev_create();
    TEST_ASSERT_NOT_NULL(indev);

    /* Add several event callbacks */
    lv_indev_add_event_cb(indev, test_event_cb_1, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(indev, test_event_cb_1, LV_EVENT_RELEASED, NULL);
    lv_indev_add_event_cb(indev, test_event_cb_2, LV_EVENT_CLICKED, NULL);

    /* Verify that 3 events were added */
    uint32_t event_count_after_add = lv_indev_get_event_count(indev);
    TEST_ASSERT_EQUAL_UINT32(3, event_count_after_add);

    /* Remove all events */
    lv_indev_remove_event_all(indev);

    /* Verify that all events were removed */
    uint32_t event_count_after_remove = lv_indev_get_event_count(indev);
    TEST_ASSERT_EQUAL_UINT32(0, event_count_after_remove);

    lv_indev_delete(indev);
}

void test_indev_remove_event_all_null(void)
{
    /* Should not crash when called with NULL */
    lv_indev_remove_event_all(NULL);
}

void test_indev_remove_event_all_no_events(void)
{
    lv_indev_t * indev = lv_indev_create();
    TEST_ASSERT_NOT_NULL(indev);

    /* Verify indev starts with no events */
    uint32_t event_count_before = lv_indev_get_event_count(indev);
    TEST_ASSERT_EQUAL_UINT32(0, event_count_before);

    /* Should not crash when no events are registered */
    lv_indev_remove_event_all(indev);

    /* Verify still no events */
    uint32_t event_count_after = lv_indev_get_event_count(indev);
    TEST_ASSERT_EQUAL_UINT32(0, event_count_after);

    lv_indev_delete(indev);
}

#endif
