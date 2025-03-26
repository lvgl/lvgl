#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    uint32_t * pressed_count = lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_PRESSED:
            (*pressed_count)++;
            lv_indev_wait_release(lv_indev_active());
            break;
        case LV_EVENT_PRESS_LOST:
            lv_indev_reset(lv_indev_active(), obj);
            break;
        default:
            break;
    }
}

void test_indev_wait_release(void)
{
    uint32_t pressed_count = 0;
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_ALL, &pressed_count);

    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(50, 50);

    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);

    lv_test_mouse_press();
    lv_test_wait(50);
    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(2, pressed_count);
}

#endif
