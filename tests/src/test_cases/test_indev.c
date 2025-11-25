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

static void indev_reset_event_cb(lv_event_t * e)
{
    lv_obj_t * scroll_obj = lv_event_get_target(e);
    lv_obj_t * act_obj = lv_obj_get_child(scroll_obj, 0);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SCROLL && act_obj) {
        lv_obj_delete(act_obj);
    }
    else if(code == LV_EVENT_INDEV_RESET) {
        uint32_t * indev_reset_count = lv_event_get_user_data(e);
        (*indev_reset_count)++;
    }
}

void test_indev_obj_delete_reset(void)
{
    lv_obj_t * scroll_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(scroll_obj, 300, 300);
    lv_obj_align(scroll_obj, LV_ALIGN_LEFT_MID, 0, 0);

    uint32_t indev_reset_count = 0;
    lv_obj_add_event_cb(scroll_obj, indev_reset_event_cb, LV_EVENT_ALL, &indev_reset_count);

    lv_obj_t * act_obj = lv_obj_create(scroll_obj);
    lv_obj_set_size(act_obj, 400, 200);
    lv_obj_align(act_obj, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_update_layout(scroll_obj);

    lv_test_mouse_move_to(200, 200);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(-20, 0);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_move_by(-20, 0);
    lv_test_mouse_press();
    lv_test_wait(50);

    lv_test_mouse_release();

    TEST_ASSERT_EQUAL_UINT32(1, indev_reset_count);
}

static void indev_wait_release_event_cb(lv_event_t * e)
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
    lv_obj_add_event_cb(btn, indev_wait_release_event_cb, LV_EVENT_ALL, &pressed_count);

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

static void indev_long_pressed_event_cb(lv_event_t * e)
{
    uint32_t * long_pressed_cnt = lv_event_get_user_data(e);
    (*long_pressed_cnt)++;
}

void test_indev_long_pressed(void)
{
    uint32_t long_pressed_cnt = 0;
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, indev_long_pressed_event_cb, LV_EVENT_LONG_PRESSED, &long_pressed_cnt);
    lv_test_mouse_release();
    lv_test_wait(50);
    lv_test_mouse_move_to(50, 50);
    lv_test_mouse_press();
    lv_test_wait(500);
    lv_test_mouse_release();
    lv_test_wait(50);
    TEST_ASSERT_EQUAL_UINT32(1, long_pressed_cnt);
}

#endif
