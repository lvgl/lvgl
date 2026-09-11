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

/** Every callback of the event API tests appends a character here so the call order
 * can be asserted */
static char call_log[64];
static uint32_t call_cnt;
static lv_indev_t * last_target;
static lv_indev_t * last_current_target;
static void * last_param;
static void * last_user_data;
static lv_event_code_t last_code;
static bool stop_in_preprocess;
static bool stop_in_normal;
static lv_indev_t * api_indev;

static void log_append(char c);
static void cb_a(lv_event_t * e);
static void cb_b(lv_event_t * e);
static void cb_pre_a(lv_event_t * e);
static void cb_pre_b(lv_event_t * e);
static void cb_record(lv_event_t * e);
static void cb_count(lv_event_t * e);

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

    call_log[0] = '\0';
    call_cnt = 0;
    last_target = NULL;
    last_current_target = NULL;
    last_param = NULL;
    last_user_data = NULL;
    last_code = LV_EVENT_ALL;
    stop_in_preprocess = false;
    stop_in_normal = false;

    api_indev = lv_indev_create();
    lv_indev_set_mode(api_indev, LV_INDEV_MODE_EVENT);
}

void tearDown(void)
{
    /* Function run after every test */
    if(api_indev) lv_indev_delete(api_indev);
    api_indev = NULL;
    lv_obj_clean(lv_screen_active());
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

static void log_append(char c)
{
    uint32_t len = lv_strlen(call_log);
    if(len + 1 >= sizeof(call_log)) return;
    call_log[len] = c;
    call_log[len + 1] = '\0';
}

static void cb_a(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('a');
}

static void cb_b(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('b');
}

static void cb_pre_a(lv_event_t * e)
{
    log_append('A');
    if(stop_in_preprocess) lv_event_stop_processing(e);
}

static void cb_pre_b(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('B');
}

static void cb_record(lv_event_t * e)
{
    call_cnt++;
    last_code = lv_event_get_code(e);
    last_target = lv_event_get_target(e);
    last_current_target = lv_event_get_current_target(e);
    last_param = lv_event_get_param(e);
    last_user_data = lv_event_get_user_data(e);
    if(stop_in_normal) lv_event_stop_processing(e);
}

static void cb_count(lv_event_t * e)
{
    LV_UNUSED(e);
    call_cnt++;
}

void test_indev_event_fresh_indev_event_list(void)
{
    const uint32_t expected = LV_USE_GESTURE_RECOGNITION ? 1 : 0;
    TEST_ASSERT_EQUAL(expected, lv_indev_get_event_count(api_indev));
}

void test_indev_event_add_and_query_dsc(void)
{
    const uint32_t base_cnt = lv_indev_get_event_count(api_indev);
    int user_data_a = 0;
    int user_data_b = 0;

    lv_event_dsc_t * dsc_a = lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, &user_data_a);
    lv_event_dsc_t * dsc_b = lv_indev_add_event_cb(api_indev, cb_b, LV_EVENT_ALL, &user_data_b);

    TEST_ASSERT_NOT_NULL(dsc_a);
    TEST_ASSERT_NOT_NULL(dsc_b);
    TEST_ASSERT_TRUE(dsc_a != dsc_b);
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 2, lv_indev_get_event_count(api_indev));

    TEST_ASSERT_EQUAL_PTR(dsc_a, lv_indev_get_event_dsc(api_indev, base_cnt));
    TEST_ASSERT_EQUAL_PTR(dsc_b, lv_indev_get_event_dsc(api_indev, base_cnt + 1));

    TEST_ASSERT_EQUAL_PTR(cb_a, lv_event_dsc_get_cb(dsc_a));
    TEST_ASSERT_EQUAL_PTR(cb_b, lv_event_dsc_get_cb(dsc_b));
    TEST_ASSERT_EQUAL_PTR(&user_data_a, lv_event_dsc_get_user_data(dsc_a));
    TEST_ASSERT_EQUAL_PTR(&user_data_b, lv_event_dsc_get_user_data(dsc_b));

    TEST_ASSERT_NULL(lv_indev_get_event_dsc(api_indev, base_cnt + 2));
}

void test_indev_event_filter(void)
{
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(api_indev, cb_b, LV_EVENT_ALL, NULL);

    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);
    TEST_ASSERT_EQUAL_STRING("ab", call_log);

    lv_indev_send_event(api_indev, LV_EVENT_RELEASED, NULL);
    TEST_ASSERT_EQUAL_STRING("abb", call_log);
}

void test_indev_event_target_param_and_user_data(void)
{
    int user_data = 0;
    int param = 0;

    lv_indev_add_event_cb(api_indev, cb_record, LV_EVENT_PRESSED, &user_data);
    lv_result_t res = lv_indev_send_event(api_indev, LV_EVENT_PRESSED, &param);

    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_PRESSED, last_code);
    TEST_ASSERT_EQUAL_PTR(api_indev, last_target);
    TEST_ASSERT_EQUAL_PTR(api_indev, last_current_target);
    TEST_ASSERT_EQUAL_PTR(&param, last_param);
    TEST_ASSERT_EQUAL_PTR(&user_data, last_user_data);
}

void test_indev_event_preprocess_runs_first(void)
{
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(api_indev, cb_pre_a, LV_EVENT_PRESSED | LV_EVENT_PREPROCESS, NULL);
    lv_indev_add_event_cb(api_indev, cb_b, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(api_indev, cb_pre_b, LV_EVENT_PRESSED | LV_EVENT_PREPROCESS, NULL);
    lv_indev_add_event_cb(api_indev, cb_record, LV_EVENT_PRESSED | LV_EVENT_PREPROCESS, NULL);

    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);

    TEST_ASSERT_EQUAL_STRING("ABab", call_log);
    /* The preprocess flag is not reported to the callback */
    TEST_ASSERT_EQUAL(LV_EVENT_PRESSED, last_code);
}

void test_indev_event_stop_processing_in_preprocess(void)
{
    lv_indev_add_event_cb(api_indev, cb_pre_a, LV_EVENT_PRESSED | LV_EVENT_PREPROCESS, NULL);
    lv_indev_add_event_cb(api_indev, cb_pre_b, LV_EVENT_PRESSED | LV_EVENT_PREPROCESS, NULL);
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);

    stop_in_preprocess = true;
    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);

    TEST_ASSERT_EQUAL_STRING("A", call_log);
}

void test_indev_event_stop_processing_in_normal(void)
{
    lv_indev_add_event_cb(api_indev, cb_record, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);

    stop_in_normal = true;
    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_STRING("", call_log);
}

void test_indev_event_remove_by_index(void)
{
    const uint32_t base_cnt = lv_indev_get_event_count(api_indev);

    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(api_indev, cb_b, LV_EVENT_PRESSED, NULL);

    TEST_ASSERT_TRUE(lv_indev_remove_event(api_indev, base_cnt));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_indev_get_event_count(api_indev));

    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);
    TEST_ASSERT_EQUAL_STRING("b", call_log);

    /* Removing a non-existing index must fail without touching the list */
    TEST_ASSERT_FALSE(lv_indev_remove_event(api_indev, base_cnt + 1));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_indev_get_event_count(api_indev));
}

void test_indev_event_remove_cb_with_user_data(void)
{
    const uint32_t base_cnt = lv_indev_get_event_count(api_indev);
    int user_data_1 = 0;
    int user_data_2 = 0;

    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, &user_data_1);
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_RELEASED, &user_data_1);
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, &user_data_2);
    lv_indev_add_event_cb(api_indev, cb_b, LV_EVENT_PRESSED, &user_data_1);

    TEST_ASSERT_EQUAL_UINT32(2, lv_indev_remove_event_cb_with_user_data(api_indev, cb_a, &user_data_1));
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 2, lv_indev_get_event_count(api_indev));

    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);
    TEST_ASSERT_EQUAL_STRING("ab", call_log);

    /* Nothing matches anymore */
    TEST_ASSERT_EQUAL_UINT32(0, lv_indev_remove_event_cb_with_user_data(api_indev, cb_a, &user_data_1));
}

void test_indev_event_custom_registered_id(void)
{
    lv_event_code_t custom_1 = (lv_event_code_t)lv_event_register_id();
    lv_event_code_t custom_2 = (lv_event_code_t)lv_event_register_id();

    TEST_ASSERT_NOT_EQUAL(custom_1, custom_2);
    TEST_ASSERT_GREATER_OR_EQUAL(LV_EVENT_LAST, custom_1);

    lv_indev_add_event_cb(api_indev, cb_record, custom_1, NULL);
    lv_indev_add_event_cb(api_indev, cb_a, custom_2, NULL);

    lv_indev_send_event(api_indev, custom_1, NULL);

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(custom_1, last_code);
    TEST_ASSERT_EQUAL_STRING("", call_log);
}

/* A callback may remove itself while the list is being traversed */
static void cb_remove_self(lv_event_t * e)
{
    lv_indev_t * indev = lv_event_get_current_target(e);
    call_cnt++;
    lv_indev_remove_event_cb_with_user_data(indev, cb_remove_self, NULL);
}

void test_indev_event_remove_own_cb_while_sending(void)
{
    const uint32_t base_cnt = lv_indev_get_event_count(api_indev);

    lv_indev_add_event_cb(api_indev, cb_remove_self, LV_EVENT_PRESSED, NULL);
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);

    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);

    /* The self-removing callback ran once, the one after it still ran */
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_STRING("a", call_log);
    TEST_ASSERT_EQUAL_UINT32(base_cnt + 1, lv_indev_get_event_count(api_indev));

    /* The removal really took effect */
    lv_indev_send_event(api_indev, LV_EVENT_PRESSED, NULL);
    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL_STRING("aa", call_log);
}

/* LV_EVENT_DELETE is sent while the indev is still usable */
static void cb_on_delete(lv_event_t * e)
{
    lv_indev_t * indev = lv_event_get_current_target(e);
    call_cnt++;
    last_code = lv_event_get_code(e);
    TEST_ASSERT_EQUAL(LV_INDEV_TYPE_BUTTON, lv_indev_get_type(indev));
}

void test_indev_event_delete(void)
{
    lv_indev_set_type(api_indev, LV_INDEV_TYPE_BUTTON);
    lv_indev_add_event_cb(api_indev, cb_on_delete, LV_EVENT_DELETE, NULL);
    lv_indev_add_event_cb(api_indev, cb_a, LV_EVENT_PRESSED, NULL);

    lv_indev_delete(api_indev);
    api_indev = NULL;

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_DELETE, last_code);
    TEST_ASSERT_EQUAL_STRING("", call_log);
}

/* The indev event list is notified about the click sequence, and the param is the
 * widget the indev is acting on */
static void cb_log_pointer_code(lv_event_t * e)
{
    lv_obj_t ** target_obj = lv_event_get_user_data(e);
    *target_obj = lv_event_get_param(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_PRESSED:
            log_append('P');
            break;
        case LV_EVENT_RELEASED:
            log_append('R');
            break;
        case LV_EVENT_SHORT_CLICKED:
            log_append('S');
            break;
        case LV_EVENT_CLICKED:
            log_append('C');
            break;
        default:
            break;
    }
}

void test_indev_event_pointer_click_sequence(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    TEST_ASSERT_NOT_NULL(indev);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_set_pos(btn, 0, 0);

    lv_obj_t * param_obj = NULL;
    lv_indev_add_event_cb(indev, cb_log_pointer_code, LV_EVENT_PRESSED, &param_obj);
    lv_indev_add_event_cb(indev, cb_log_pointer_code, LV_EVENT_RELEASED, &param_obj);
    lv_indev_add_event_cb(indev, cb_log_pointer_code, LV_EVENT_SHORT_CLICKED, &param_obj);
    lv_indev_add_event_cb(indev, cb_log_pointer_code, LV_EVENT_CLICKED, &param_obj);

    lv_test_mouse_click_at(20, 20);

    TEST_ASSERT_EQUAL_STRING("PRSC", call_log);
    TEST_ASSERT_EQUAL_PTR(btn, param_obj);

    lv_indev_remove_event_cb_with_user_data(indev, cb_log_pointer_code, &param_obj);
}

/* An indev event callback runs before the widget's one and can keep the event
 * from reaching the widget with lv_indev_stop_processing() */
static void cb_indev_stop_processing(lv_event_t * e)
{
    log_append('i');
    lv_indev_stop_processing(lv_event_get_current_target(e));
}

static void cb_widget_clicked(lv_event_t * e)
{
    LV_UNUSED(e);
    log_append('w');
}

void test_indev_event_stop_processing_blocks_widget(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    TEST_ASSERT_NOT_NULL(indev);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_add_event_cb(btn, cb_widget_clicked, LV_EVENT_CLICKED, NULL);

    /* Without the indev callback the widget is notified */
    lv_test_mouse_click_at(20, 20);
    TEST_ASSERT_EQUAL_STRING("w", call_log);

    call_log[0] = '\0';
    lv_indev_add_event_cb(indev, cb_indev_stop_processing, LV_EVENT_CLICKED, NULL);

    lv_test_mouse_click_at(20, 20);
    TEST_ASSERT_EQUAL_STRING("i", call_log);

    lv_indev_remove_event_cb_with_user_data(indev, cb_indev_stop_processing, NULL);

    /* The flag must not leak into the next click */
    call_log[0] = '\0';
    lv_test_mouse_click_at(20, 20);
    TEST_ASSERT_EQUAL_STRING("w", call_log);
}

/* Resetting a pressed indev notifies its event list with the widget it was acting on */
void test_indev_event_reset_sends_indev_reset(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    TEST_ASSERT_NOT_NULL(indev);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_set_pos(btn, 0, 0);

    lv_indev_add_event_cb(indev, cb_record, LV_EVENT_INDEV_RESET, NULL);

    /* Press and keep it pressed so the indev has an active widget */
    lv_test_mouse_move_to(20, 20);
    lv_test_mouse_press();
    lv_test_wait(50);
    TEST_ASSERT_EQUAL_UINT32(0, call_cnt);

    lv_indev_reset(indev, NULL);

    TEST_ASSERT_EQUAL_UINT32(1, call_cnt);
    TEST_ASSERT_EQUAL(LV_EVENT_INDEV_RESET, last_code);
    TEST_ASSERT_EQUAL_PTR(indev, last_target);
    TEST_ASSERT_EQUAL_PTR(btn, last_param);

    lv_indev_remove_event_cb_with_user_data(indev, cb_record, NULL);
    lv_test_mouse_release();
    lv_test_wait(50);
}

/* An indev that is disabled must not emit any event */
void test_indev_event_disabled_indev_is_silent(void)
{
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_POINTER);
    TEST_ASSERT_NOT_NULL(indev);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 50);
    lv_obj_set_pos(btn, 0, 0);

    lv_indev_add_event_cb(indev, cb_count, LV_EVENT_ALL, NULL);

    lv_indev_enable(indev, false);
    lv_test_mouse_click_at(20, 20);
    TEST_ASSERT_EQUAL_UINT32(0, call_cnt);

    lv_indev_enable(indev, true);
    lv_test_mouse_click_at(20, 20);
    TEST_ASSERT_GREATER_THAN_UINT32(0, call_cnt);

    lv_indev_remove_event_cb_with_user_data(indev, cb_count, NULL);
}

void test_indev_event_code_names(void)
{
    TEST_ASSERT_EQUAL_STRING("EVENT_PRESSED", lv_event_code_get_name(LV_EVENT_PRESSED));
    TEST_ASSERT_EQUAL_STRING("EVENT_PRESSING", lv_event_code_get_name(LV_EVENT_PRESSING));
    TEST_ASSERT_EQUAL_STRING("EVENT_PRESS_LOST", lv_event_code_get_name(LV_EVENT_PRESS_LOST));
    TEST_ASSERT_EQUAL_STRING("EVENT_SHORT_CLICKED", lv_event_code_get_name(LV_EVENT_SHORT_CLICKED));
    TEST_ASSERT_EQUAL_STRING("EVENT_SINGLE_CLICKED", lv_event_code_get_name(LV_EVENT_SINGLE_CLICKED));
    TEST_ASSERT_EQUAL_STRING("EVENT_DOUBLE_CLICKED", lv_event_code_get_name(LV_EVENT_DOUBLE_CLICKED));
    TEST_ASSERT_EQUAL_STRING("EVENT_TRIPLE_CLICKED", lv_event_code_get_name(LV_EVENT_TRIPLE_CLICKED));
    TEST_ASSERT_EQUAL_STRING("EVENT_LONG_PRESSED", lv_event_code_get_name(LV_EVENT_LONG_PRESSED));
    TEST_ASSERT_EQUAL_STRING("EVENT_LONG_PRESSED_REPEAT", lv_event_code_get_name(LV_EVENT_LONG_PRESSED_REPEAT));
    TEST_ASSERT_EQUAL_STRING("EVENT_CLICKED", lv_event_code_get_name(LV_EVENT_CLICKED));
    TEST_ASSERT_EQUAL_STRING("EVENT_RELEASED", lv_event_code_get_name(LV_EVENT_RELEASED));
    TEST_ASSERT_EQUAL_STRING("EVENT_GESTURE", lv_event_code_get_name(LV_EVENT_GESTURE));
    TEST_ASSERT_EQUAL_STRING("EVENT_KEY", lv_event_code_get_name(LV_EVENT_KEY));
    TEST_ASSERT_EQUAL_STRING("EVENT_ROTARY", lv_event_code_get_name(LV_EVENT_ROTARY));
    TEST_ASSERT_EQUAL_STRING("EVENT_INDEV_RESET", lv_event_code_get_name(LV_EVENT_INDEV_RESET));
    TEST_ASSERT_EQUAL_STRING("EVENT_HOVER_OVER", lv_event_code_get_name(LV_EVENT_HOVER_OVER));
    TEST_ASSERT_EQUAL_STRING("EVENT_HOVER_LEAVE", lv_event_code_get_name(LV_EVENT_HOVER_LEAVE));
}

#endif
