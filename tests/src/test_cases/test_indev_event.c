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
static uint32_t event_cnt_key_pressed;
static uint32_t event_cnt_key_short_clicked;
static uint32_t event_cnt_key_long_clicked;
static uint32_t event_cnt_key_long_pressed;
static uint32_t event_cnt_key_long_pressed_repeat;
static uint32_t event_cnt_key_clicked;
static uint32_t event_cnt_key_released;
static uint32_t event_cnt_key_released_at_short_click;
static uint32_t event_cnt_key_short_clicked_at_click;
static uint32_t event_cnt_key_long_clicked_at_click;
static lv_key_t key_released_key;
static lv_key_t key_short_clicked_key;

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
    event_cnt_key_pressed = 0;
    event_cnt_key_short_clicked = 0;
    event_cnt_key_long_clicked = 0;
    event_cnt_key_long_pressed = 0;
    event_cnt_key_long_pressed_repeat = 0;
    event_cnt_key_clicked = 0;
    event_cnt_key_released = 0;
    event_cnt_key_released_at_short_click = 0;
    event_cnt_key_short_clicked_at_click = 0;
    event_cnt_key_long_clicked_at_click = 0;
    key_released_key = 0;
    key_short_clicked_key = 0;
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
        case LV_EVENT_KEY_PRESSED:
            event_cnt_key_pressed++;
            break;
        case LV_EVENT_KEY_SHORT_CLICKED:
            event_cnt_key_short_clicked++;
            event_cnt_key_released_at_short_click = event_cnt_key_released;
            key_short_clicked_key = lv_indev_get_key(lv_indev_active());
            break;
        case LV_EVENT_KEY_LONG_CLICKED:
            event_cnt_key_long_clicked++;
            break;
        case LV_EVENT_KEY_LONG_PRESSED:
            event_cnt_key_long_pressed++;
            break;
        case LV_EVENT_KEY_LONG_PRESSED_REPEAT:
            event_cnt_key_long_pressed_repeat++;
            break;
        case LV_EVENT_KEY_CLICKED:
            event_cnt_key_clicked++;
            event_cnt_key_short_clicked_at_click = event_cnt_key_short_clicked;
            event_cnt_key_long_clicked_at_click = event_cnt_key_long_clicked;
            break;
        case LV_EVENT_KEY_RELEASED:
            event_cnt_key_released++;
            key_released_key = lv_event_get_key(e);
            break;
        default:
            break;
    }
}

static void add_key_lifecycle_event_cbs(lv_indev_t * indev)
{
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_SHORT_CLICKED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_LONG_CLICKED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_LONG_PRESSED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_LONG_PRESSED_REPEAT, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_CLICKED, NULL);
    lv_indev_add_event_cb(indev, keypad_event_cb, LV_EVENT_KEY_RELEASED, NULL);
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

void test_indev_keypad_no_group_key_lifecycle_short_click(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    add_key_lifecycle_event_cbs(indev);

    /* Hit a key (press and release) */
    lv_test_key_hit('x');

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt_key_long_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt_key_long_pressed);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt_key_long_pressed_repeat);

    /* KEY_RELEASED is sent before KEY_SHORT_CLICKED, then KEY_CLICKED */
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_released_at_short_click);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_short_clicked_at_click);

    TEST_ASSERT_EQUAL_UINT32('x', key_released_key);
    TEST_ASSERT_EQUAL_UINT32('x', key_short_clicked_key);

    /* Cleanup */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

void test_indev_keypad_no_group_key_lifecycle_long_press(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);
    lv_indev_set_group(indev, NULL);

    add_key_lifecycle_event_cbs(indev);

    /* Make sure we start in a released state */
    lv_test_key_release();
    lv_test_wait(50);

    /* Press key and hold for long press time (default is 400ms) */
    lv_test_key_press('c');

    /* Hold key for a bit (longer than LV_INDEV_DEF_LONG_PRESS_TIME) */
    lv_test_wait(450);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_long_pressed);

    /* The first repeat only comes a full LV_INDEV_DEF_LONG_PRESS_REP_TIME
     * (default is 100ms) after the long press */
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt_key_long_pressed_repeat);

    /* Continue holding for repeats: one every repeat period, not every read cycle */
    lv_test_wait(400);

    TEST_ASSERT_GREATER_THAN_UINT32(1, event_cnt_key_long_pressed_repeat);
    TEST_ASSERT_LESS_THAN_UINT32(7, event_cnt_key_long_pressed_repeat);

    /* Release key */
    lv_test_key_release();
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt_key_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_long_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_clicked);

    /* KEY_LONG_CLICKED is sent before KEY_CLICKED */
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_long_clicked_at_click);

    TEST_ASSERT_EQUAL_UINT32('c', key_released_key);

    /* Cleanup */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
}

static uint32_t obj_cnt_pressed;
static uint32_t obj_cnt_key_pressed;
static uint32_t obj_cnt_key_short_clicked;
static uint32_t obj_cnt_key_released;
static lv_key_t obj_released_key;

static void group_obj_event_cb(lv_event_t * e)
{
    switch(lv_event_get_code(e)) {
        case LV_EVENT_PRESSED:
            obj_cnt_pressed++;
            break;
        case LV_EVENT_KEY_PRESSED:
            obj_cnt_key_pressed++;
            break;
        case LV_EVENT_KEY_SHORT_CLICKED:
            obj_cnt_key_short_clicked++;
            break;
        case LV_EVENT_KEY_RELEASED:
            obj_cnt_key_released++;
            obj_released_key = lv_event_get_key(e);
            break;
        default:
            break;
    }
}

void test_indev_keypad_group_key_lifecycle_any_key(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);

    obj_cnt_pressed = 0;
    obj_cnt_key_pressed = 0;
    obj_cnt_key_short_clicked = 0;
    obj_cnt_key_released = 0;
    obj_released_key = 0;

    lv_group_t * g = lv_group_create();
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_group_add_obj(g, obj);
    lv_group_focus_obj(obj);
    lv_indev_set_group(indev, g);

    add_key_lifecycle_event_cbs(indev);
    lv_obj_add_event_cb(obj, group_obj_event_cb, LV_EVENT_ALL, NULL);

    /* Hit a key that is not LV_KEY_ENTER */
    lv_test_key_hit('r');

    /* The lifecycle events are emitted on the indev for any key */
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_clicked);
    TEST_ASSERT_EQUAL_UINT32(0, event_cnt_key_long_clicked);

    /* And forwarded to the focused object */
    TEST_ASSERT_EQUAL_UINT32(1, obj_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, obj_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(1, obj_cnt_key_short_clicked);
    TEST_ASSERT_EQUAL_UINT32('r', obj_released_key);

    /* The legacy lifecycle events remain ENTER-only */
    TEST_ASSERT_EQUAL_UINT32(0, obj_cnt_pressed);

    /* Cleanup */
    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
    lv_indev_set_group(indev, NULL);
    lv_obj_delete(obj);
    lv_group_delete(g);
}

static uint32_t clearing_key_value;
static bool clearing_key_pressed;

static void clearing_keypad_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->key = clearing_key_pressed ? clearing_key_value : 0;
    data->state = clearing_key_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void test_indev_keypad_key_lifecycle_release_key_identity(void)
{
    /* Use a driver that clears the key when it is released */
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev, clearing_keypad_read_cb);

    add_key_lifecycle_event_cbs(indev);

    clearing_key_value = 'k';
    clearing_key_pressed = true;
    lv_test_wait(50);
    clearing_key_pressed = false;
    lv_test_wait(50);

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_clicked);

    /* lv_indev_get_key() returns the pressed key during the release events */
    TEST_ASSERT_EQUAL_UINT32('k', key_released_key);
    TEST_ASSERT_EQUAL_UINT32('k', key_short_clicked_key);

    lv_indev_delete(indev);
}

void test_indev_keypad_group_disabled_focused_key_lifecycle(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    TEST_ASSERT_NOT_NULL(indev);

    obj_cnt_pressed = 0;
    obj_cnt_key_pressed = 0;
    obj_cnt_key_short_clicked = 0;
    obj_cnt_key_released = 0;
    obj_released_key = 0;

    lv_group_t * g = lv_group_create();
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_group_add_obj(g, obj);
    lv_group_focus_obj(obj);
    lv_obj_add_state(obj, LV_STATE_DISABLED);
    lv_indev_set_group(indev, g);

    add_key_lifecycle_event_cbs(indev);
    lv_obj_add_event_cb(obj, group_obj_event_cb, LV_EVENT_ALL, NULL);

    lv_test_key_hit('r');

    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_short_clicked);
    TEST_ASSERT_EQUAL_UINT32(1, event_cnt_key_clicked);
    TEST_ASSERT_EQUAL_UINT32('r', key_released_key);

    TEST_ASSERT_EQUAL_UINT32(0, obj_cnt_key_pressed);
    TEST_ASSERT_EQUAL_UINT32(0, obj_cnt_key_released);
    TEST_ASSERT_EQUAL_UINT32(0, obj_cnt_key_short_clicked);

    lv_indev_remove_event_cb_with_user_data(indev, keypad_event_cb, NULL);
    lv_indev_set_group(indev, NULL);
    lv_obj_delete(obj);
    lv_group_delete(g);
}

#endif
