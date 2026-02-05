#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t event_cnt_pressed;
static uint32_t event_cnt_pressing;
static uint32_t event_cnt_released;
static uint32_t event_cnt_long_pressed;
static uint32_t event_cnt_long_pressed_repeat;
static uint32_t event_cnt_key;
static lv_key_t last_key;

void setUp(void)
{
    /* Function run before every test */
    event_cnt_pressed = 0;
    event_cnt_pressing = 0;
    event_cnt_released = 0;
    event_cnt_long_pressed = 0;
    event_cnt_long_pressed_repeat = 0;
    event_cnt_key = 0;
    last_key = 0;
}

void tearDown(void)
{
    /* Function run after every test */
}

static void keypad_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    switch(code) {
        case LV_EVENT_PRESSED:
            event_cnt_pressed++;
            break;
        case LV_EVENT_PRESSING:
            event_cnt_pressing++;
            break;
        case LV_EVENT_RELEASED:
            event_cnt_released++;
            break;
        case LV_EVENT_LONG_PRESSED:
            event_cnt_long_pressed++;
            break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            event_cnt_long_pressed_repeat++;
            break;
        case LV_EVENT_KEY:
            event_cnt_key++;
            last_key = lv_indev_get_key(lv_indev_active());
            break;
        default:
            break;
    }
}

void test_indev_keypad_no_group_key_event(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    /* Add event callback to the indev */
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY, NULL);

    /* Hit a key */
    lv_test_key_hit('x');

    /* KEY event should be emitted */
    TEST_ASSERT_GREATER_THAN_UINT32(0, event_cnt_key);
    TEST_ASSERT_EQUAL_UINT32('x', last_key);

    /* Cleanup */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

void test_indev_keypad_no_group_press_release(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    /* Add event callbacks to the indev */
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_RELEASED, NULL);

    /* Hit a key (press and release) */
    lv_test_key_hit('a');

    /* PRESSED and RELEASED events should be emitted */
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_released);

    /* Cleanup */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

void test_indev_keypad_no_group_long_press(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    /* Add event callbacks to the indev */
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_RELEASED, NULL);

    /* Make sure we start in a released state */
    lv_test_key_release();
    lv_test_wait(50);

    /* Press key and hold for long press time (default is 400ms) */
    lv_test_key_press('c');

    /* Hold key for a bit (longer than LV_INDEV_DEF_LONG_PRESS_TIME) */
    lv_test_wait(450);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_long_pressed);

    /* Continue holding for repeat */
    lv_test_wait(200);

    TEST_ASSERT_GREATER_THAN_UINT32(0, event_cnt_long_pressed_repeat);

    /* Release key */
    lv_test_key_release();
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_released);

    /* Cleanup */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

#endif
